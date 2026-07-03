from typing import Dict, List, Tuple

import numpy as np
from scipy.spatial.transform import Rotation as R

from bvh_loader import BVHMotion, build_loop_motion
from physics_warpper import PhysicsInfo


FPS = 60.0
Y_AXIS = np.array([0.0, 1.0, 0.0], dtype=np.float64)
TURN_INPUT_SIGN = 1.0


def _normalize_quat(q: np.ndarray) -> np.ndarray:
    return q / np.linalg.norm(q, axis=-1, keepdims=True)


def _wrap_angle(x: np.ndarray) -> np.ndarray:
    return (x + np.pi) % (2.0 * np.pi) - np.pi


def _yaw_from_quat(q: np.ndarray) -> np.ndarray:
    q = np.asarray(q)
    return R.from_quat(q.reshape(-1, 4)).as_euler("YXZ")[:, 0].reshape(q.shape[:-1])


def _yaw_quat(yaw: float) -> np.ndarray:
    return R.from_rotvec(float(yaw) * Y_AXIS).as_quat()


def _quat_nlerp(a: np.ndarray, b: np.ndarray, alpha: float) -> np.ndarray:
    b = b.copy()
    sign = np.sum(a * b, axis=-1, keepdims=True)
    b = np.where(sign < 0.0, -b, b)
    q = (1.0 - alpha) * a + alpha * b
    return _normalize_quat(q)


def _smoothstep(t: float) -> float:
    t = float(np.clip(t, 0.0, 1.0))
    return t * t * (3.0 - 2.0 * t)


def _rotate_xz(v_xz: np.ndarray, yaw: np.ndarray) -> np.ndarray:
    c = np.cos(yaw)
    s = np.sin(yaw)
    x = c * v_xz[..., 0] + s * v_xz[..., 1]
    z = -s * v_xz[..., 0] + c * v_xz[..., 1]
    return np.stack([x, z], axis=-1)


def _motion_root_velocity(root_pos: np.ndarray) -> np.ndarray:
    vel = np.zeros_like(root_pos)
    vel[1:-1] = (root_pos[2:] - root_pos[:-2]) * (0.5 * FPS)
    vel[0] = (root_pos[1] - root_pos[0]) * FPS
    vel[-1] = (root_pos[-1] - root_pos[-2]) * FPS
    return vel


def _motion_root_acceleration(root_vel: np.ndarray) -> np.ndarray:
    acc = np.zeros_like(root_vel)
    acc[1:-1] = (root_vel[2:] - root_vel[:-2]) * (0.5 * FPS)
    acc[0] = (root_vel[1] - root_vel[0]) * FPS
    acc[-1] = (root_vel[-1] - root_vel[-2]) * FPS
    return acc


def _motion_yaw_velocity(yaw: np.ndarray) -> np.ndarray:
    vel = np.zeros_like(yaw)
    vel[1:-1] = _wrap_angle(yaw[2:] - yaw[:-2]) * (0.5 * FPS)
    vel[0] = _wrap_angle(yaw[1] - yaw[0]) * FPS
    vel[-1] = _wrap_angle(yaw[-1] - yaw[-2]) * FPS
    return vel


def _motion_yaw_acceleration(yaw_vel: np.ndarray) -> np.ndarray:
    acc = np.zeros_like(yaw_vel)
    acc[1:-1] = (yaw_vel[2:] - yaw_vel[:-2]) * (0.5 * FPS)
    acc[0] = (yaw_vel[1] - yaw_vel[0]) * FPS
    acc[-1] = (yaw_vel[-1] - yaw_vel[-2]) * FPS
    return acc


def _future_derivative(values: np.ndarray, offsets: np.ndarray) -> np.ndarray:
    times = offsets[: values.shape[0]].astype(np.float64) / FPS
    if len(times) <= 1:
        return np.zeros_like(values)
    if len(times) == 2:
        delta = (values[1] - values[0]) / max(times[1] - times[0], 1e-6)
        return np.stack([delta, delta], axis=0)
    return np.gradient(values, times, axis=0, edge_order=1)


def _decay_spring_pos(pos: np.ndarray, vel: np.ndarray, halflife: float, dt: float):
    d = 2.0 * np.log(2.0) / halflife
    j = vel + d * pos
    e = np.exp(-d * dt)
    return e * (pos + j * dt), e * (vel - j * dt * d)


def _decay_spring_rot(rotvec: np.ndarray, avel: np.ndarray, halflife: float, dt: float):
    d = 2.0 * np.log(2.0) / halflife
    j = avel + d * rotvec
    e = np.exp(-d * dt)
    return e * (rotvec + j * dt), e * (avel - j * dt * d)


class MotionSource:
    def __init__(self, name: str, path: str, kind: str, viewer_joint_names: List[str]):
        self.name = name
        self.kind = kind
        self.motion = BVHMotion(path)
        self.motion.adjust_joint_name(viewer_joint_names)
        if kind == "idle":
            self.motion = build_loop_motion(self.motion)
        if name in ("walk", "run"):
            self.motion = build_loop_motion(self.motion)

        self._normalize_root_coordinate()
        self.joint_name = self.motion.joint_name
        self.joint_translation, self.joint_orientation = self.motion.batch_forward_kinematics()
        self.root_pos = self.joint_translation[:, 0].copy()
        self.root_rot = _normalize_quat(self.joint_orientation[:, 0].copy())
        self.root_yaw = _yaw_from_quat(self.root_rot)
        self.root_vel = _motion_root_velocity(self.root_pos)
        self.root_acc = _motion_root_acceleration(self.root_vel)
        self.root_avel_y = _motion_yaw_velocity(self.root_yaw)
        self.root_aacc_y = _motion_yaw_acceleration(self.root_avel_y)
        self.num_frames = self.motion.motion_length
        self.is_looping = kind == "idle" or name in ("walk", "run")
        self.cycle_len = max(1, self.num_frames - 1) if self.is_looping else self.num_frames
        if self.is_looping and self.cycle_len < self.num_frames:
            self.cycle_root_delta = self.root_pos[self.cycle_len] - self.root_pos[0]
            self.cycle_yaw_delta = float(_wrap_angle(self.root_yaw[self.cycle_len] - self.root_yaw[0]))
            if kind == "idle":
                self.cycle_root_delta = np.zeros(3, dtype=np.float64)
                self.cycle_yaw_delta = 0.0
        else:
            self.cycle_root_delta = np.zeros(3, dtype=np.float64)
            self.cycle_yaw_delta = 0.0

    def _normalize_root_coordinate(self):
        joint_translation, joint_orientation = self.motion.batch_forward_kinematics()
        root_yaw0 = float(_yaw_from_quat(joint_orientation[0, 0]))
        root_align = R.from_rotvec(-root_yaw0 * Y_AXIS)

        root_pos = self.motion.joint_position[:, 0].copy()
        root_base = root_pos[0].copy()
        root_delta = root_pos - root_base
        self.motion.joint_position[:, 0] = root_base + root_align.apply(root_delta)
        self.motion.joint_rotation[:, 0] = (
            root_align * R.from_quat(self.motion.joint_rotation[:, 0])
        ).as_quat()


class MotionDatabase:
    OFFSETS = np.array([0, 20, 40, 60, 80, 100], dtype=np.int64)

    def __init__(self, sources: List[MotionSource]):
        self.sources = sources
        self.source_ids = []
        self.frame_ids = []
        self.kind_ids = []
        self.pos_offsets = []
        self.vel_local = []
        self.acc_local = []
        self.yaw_offsets = []
        self.avel_y = []
        self.aacc_y = []
        self._build()

    @staticmethod
    def _kind_id(kind: str) -> int:
        return {
            "idle": 0,
            "walk": 1,
            "run": 2,
            "turn_left": 3,
            "turn_right": 4,
        }.get(kind, 1)

    def _candidate_stride(self, source: MotionSource) -> int:
        if source.kind in ("turn_left", "turn_right"):
            return 1
        if source.kind == "idle":
            return 2
        return 3

    def _build(self):
        for source_id, source in enumerate(self.sources):
            last = source.cycle_len if source.is_looping else source.num_frames - 2
            if last <= 1:
                continue

            frames = np.arange(0, last, self._candidate_stride(source), dtype=np.int64)
            base_pos = source.root_pos[frames]
            base_yaw = source.root_yaw[frames]
            future_abs = frames[:, None] + self.OFFSETS[None, :]
            if source.is_looping:
                cycle_count = future_abs // source.cycle_len
                future_frames = future_abs % source.cycle_len
            else:
                cycle_count = np.zeros_like(future_abs)
                future_frames = np.minimum(future_abs, source.num_frames - 1)

            future_pos = source.root_pos[future_frames] + cycle_count[..., None] * source.cycle_root_delta
            pos_delta = future_pos - base_pos[:, None, :]
            pos_delta_xz = pos_delta[..., [0, 2]]
            pos_local = _rotate_xz(pos_delta_xz, -base_yaw[:, None])

            vel_xz = source.root_vel[future_frames][..., [0, 2]]
            vel_local = _rotate_xz(vel_xz, -base_yaw[:, None])

            acc_xz = source.root_acc[future_frames][..., [0, 2]]
            acc_local = _rotate_xz(acc_xz, -base_yaw[:, None])

            future_yaw = source.root_yaw[future_frames] + cycle_count * source.cycle_yaw_delta
            yaw_offsets = _wrap_angle(future_yaw - base_yaw[:, None])
            avel_y = source.root_avel_y[future_frames]
            aacc_y = source.root_aacc_y[future_frames]

            self.source_ids.append(np.full(len(frames), source_id, dtype=np.int64))
            self.frame_ids.append(frames)
            self.kind_ids.append(np.full(len(frames), self._kind_id(source.kind), dtype=np.int64))
            self.pos_offsets.append(pos_local)
            self.vel_local.append(vel_local)
            self.acc_local.append(acc_local)
            self.yaw_offsets.append(yaw_offsets)
            self.avel_y.append(avel_y)
            self.aacc_y.append(aacc_y)

        self.source_ids = np.concatenate(self.source_ids)
        self.frame_ids = np.concatenate(self.frame_ids)
        self.kind_ids = np.concatenate(self.kind_ids)
        self.pos_offsets = np.concatenate(self.pos_offsets, axis=0)
        self.vel_local = np.concatenate(self.vel_local, axis=0)
        self.acc_local = np.concatenate(self.acc_local, axis=0)
        self.yaw_offsets = np.concatenate(self.yaw_offsets, axis=0)
        self.avel_y = np.concatenate(self.avel_y, axis=0)
        self.aacc_y = np.concatenate(self.aacc_y, axis=0)

    def __len__(self) -> int:
        return len(self.frame_ids)


class PDController:
    def __init__(self, viewer) -> None:
        self.viewer = viewer
        self.physics_info = PhysicsInfo(viewer)
        self.cnt = 0
        self.get_pose = None

    def apply_pd_torque(self):
        return

    def apply_root_force_and_torque(self):
        return

    def apply_static_torque(self):
        return


class CharacterController:
    SEARCH_INTERVAL = 30
    TURN_SEARCH_INTERVAL = 12
    MIN_SWITCH_FRAMES = 24
    TURN_MIN_SWITCH_FRAMES = 12
    TURN_COOLDOWN_FRAMES = 45
    TURN_ENTRY_FRAME = 20
    BLEND_FRAMES = 12
    CURRENT_MARGIN = 0.45
    CROSS_SOURCE_MARGIN = 2.5
    SAME_KIND_SWITCH_MARGIN = 4.0
    STOP_SPEED = 0.12
    RUN_SPEED = 1.65
    TURN_ENTER_YAW = 0.18
    TURN_EXIT_YAW = 0.12

    W_POS = 2.2
    W_VEL = 3.2
    W_SPEED = 2.0
    W_FORWARD = 4.0
    W_ACC = 0.18
    W_YAW = 4.8
    W_AVEL = 0.55
    W_AACC = 0.04
    W_TURN_DIR = 3.0
    W_TURN_IDLE = 1.2
    W_TURN_MATCH = 0.9
    W_NON_TURN_YAW = 16.0
    W_NON_TURN_ABS_YAW = 40.0
    W_SHORT_CLIP = 4.0
    FORWARD_SWITCH_WINDOW = 90
    ROOT_VELOCITY_BLEND_MIN = 0.15
    ROOT_VELOCITY_BLEND_MAX = 0.85
    INERTIAL_HALFLIFE = 0.12

    def __init__(self, viewer, controller, pd_controller) -> None:
        self.viewer = viewer
        self.controller = controller
        self.pd_controller = pd_controller

        specs = [
            ("idle", "./motion_material/idle.bvh", "idle"),
            ("walk", "./motion_material/walk.bvh", "walk"),
            ("run", "./motion_material/run.bvh", "run"),
            ("long_walk", "./motion_material/long_motion/long_walk.bvh", "walk"),
            ("long_walk_mirror", "./motion_material/long_motion/long_walk_mirror.bvh", "walk"),
            ("long_run", "./motion_material/long_motion/long_run.bvh", "run"),
            ("long_run_mirror", "./motion_material/long_motion/long_run_mirror.bvh", "run"),
            ("turn_left", "./motion_material/turn_left.bvh", "turn_left"),
            ("turn_right", "./motion_material/turn_right.bvh", "turn_right"),
        ]
        self.sources = [
            MotionSource(name, path, kind, self.viewer.joint_name)
            for name, path, kind in specs
        ]
        self.source_by_kind: Dict[str, int] = {}
        for source_id, source in enumerate(self.sources):
            self.source_by_kind.setdefault(source.kind, source_id)
        self.motions = [source.motion for source in self.sources]
        self.database = MotionDatabase(self.sources)

        self.cur_source_id = 0
        self.cur_frame = 0
        self.anchor_frame = 0
        self.anchor_pos = np.asarray(self.viewer.root_pos, dtype=np.float64).copy()
        self.base_root_y = float(self.anchor_pos[1])
        self.anchor_yaw = 0.0
        self.align_rot = R.from_rotvec(-float(self.sources[0].root_yaw[0]) * Y_AXIS)

        self.cur_root_pos = self.anchor_pos.copy()
        self.cur_root_rot = (self.align_rot * R.from_quat(self.sources[0].root_rot[0])).as_quat()
        self.last_joint_translation = None
        self.last_joint_orientation = None

        self.search_countdown = 0
        self.frames_since_switch = self.MIN_SWITCH_FRAMES
        self.blend_left = 0
        self.blend_total = self.BLEND_FRAMES
        self.inertial_pos_offset = None
        self.inertial_vel_offset = None
        self.inertial_rot_offset = None
        self.inertial_avel_offset = None
        self.turn_cooldown = 0
        self.last_local_pos = None
        self.last_local_rot = None

    def _source(self) -> MotionSource:
        return self.sources[self.cur_source_id]

    def _wrap_current_loop(self):
        source = self._source()
        if not source.is_looping or self.cur_frame < source.cycle_len:
            return

        cycles = self.cur_frame // source.cycle_len
        self.cur_frame = self.cur_frame % source.cycle_len
        self.anchor_frame = self.anchor_frame % source.cycle_len

        if source.kind == "idle":
            self.anchor_pos = self.cur_root_pos.copy()
            self.anchor_pos[1] = self.base_root_y
            self.anchor_yaw = float(_yaw_from_quat(self.cur_root_rot))
            self.align_rot = R.from_rotvec(
                _wrap_angle(self.anchor_yaw - float(source.root_yaw[self.anchor_frame])) * Y_AXIS
            )
            return

        cycle_delta = source.cycle_root_delta * cycles
        self.anchor_pos = self.anchor_pos + self.align_rot.apply(cycle_delta)
        self.anchor_pos[1] = self.base_root_y
        self.anchor_yaw = float(_wrap_angle(self.anchor_yaw + source.cycle_yaw_delta * cycles))
        self.align_rot = R.from_rotvec(
            _wrap_angle(self.anchor_yaw - float(source.root_yaw[self.anchor_frame])) * Y_AXIS
        )

    def _aligned_local_pose(
        self,
        source_id: int,
        frame: int,
        anchor_frame: int = None,
        anchor_pos: np.ndarray = None,
        anchor_yaw: float = None,
        align_rot: R = None,
    ) -> Tuple[List[str], np.ndarray, np.ndarray]:
        source = self.sources[source_id]
        if source.is_looping:
            frame = int(frame) % source.cycle_len
        else:
            frame = int(np.clip(frame, 0, source.num_frames - 1))
        anchor_frame = self.anchor_frame if anchor_frame is None else int(anchor_frame)
        anchor_pos = self.anchor_pos if anchor_pos is None else anchor_pos
        anchor_yaw = self.anchor_yaw if anchor_yaw is None else float(anchor_yaw)
        align_rot = self.align_rot if align_rot is None else align_rot

        root_delta = source.root_pos[frame] - source.root_pos[anchor_frame]
        root_pos = align_rot.apply(root_delta) + anchor_pos
        root_y_delta = source.root_pos[frame, 1] - source.root_pos[anchor_frame, 1]
        root_pos[1] = self.base_root_y + float(np.clip(root_y_delta, -0.08, 0.08))

        local_pos = source.motion.joint_position[frame].copy()
        local_rot = source.motion.joint_rotation[frame].copy()
        local_pos[0] = root_pos

        if source.kind in ("walk", "run", "idle"):
            yaw_delta = _wrap_angle(source.root_yaw[frame] - source.root_yaw[anchor_frame])
            if source.kind == "idle":
                yaw_delta = 0.0
            local_rot[0] = _yaw_quat(anchor_yaw + yaw_delta)
        else:
            local_rot[0] = (align_rot * R.from_quat(local_rot[0])).as_quat()

        return source.joint_name, local_pos, local_rot

    def _fk_from_local(self, source_id: int, local_pos: np.ndarray, local_rot: np.ndarray):
        source = self.sources[source_id]
        joint_translation, joint_orientation = source.motion.batch_forward_kinematics(
            joint_position=local_pos[None, :, :],
            joint_rotation=local_rot[None, :, :],
        )
        return joint_translation[0], joint_orientation[0]

    def _begin_inertialization(
        self,
        source_id: int,
        frame: int,
        anchor_yaw: float = None,
        align_rot: R = None,
    ):
        if self.last_local_pos is None:
            return
        _, dst_pos, dst_rot = self._aligned_local_pose(
            source_id,
            frame,
            anchor_frame=frame,
            anchor_pos=self.cur_root_pos.copy(),
            anchor_yaw=anchor_yaw,
            align_rot=align_rot,
        )
        self.inertial_pos_offset = self.last_local_pos - dst_pos
        self.inertial_pos_offset[0] = 0.0
        self.inertial_vel_offset = np.zeros_like(self.inertial_pos_offset)

        src_rot = R.from_quat(self.last_local_rot.reshape(-1, 4))
        dst_rot_r = R.from_quat(dst_rot.reshape(-1, 4))
        self.inertial_rot_offset = (src_rot * dst_rot_r.inv()).as_rotvec().reshape(dst_rot.shape[:-1] + (3,))
        self.inertial_rot_offset[0] = 0.0
        self.inertial_avel_offset = np.zeros_like(self.inertial_rot_offset)
        self.blend_left = self.BLEND_FRAMES
        self.blend_total = self.BLEND_FRAMES

    def _apply_inertialization(self, local_pos: np.ndarray, local_rot: np.ndarray):
        if self.blend_left <= 0 or self.inertial_pos_offset is None:
            return local_pos, local_rot

        self.inertial_pos_offset, self.inertial_vel_offset = _decay_spring_pos(
            self.inertial_pos_offset,
            self.inertial_vel_offset,
            self.INERTIAL_HALFLIFE,
            1.0 / FPS,
        )
        self.inertial_rot_offset, self.inertial_avel_offset = _decay_spring_rot(
            self.inertial_rot_offset,
            self.inertial_avel_offset,
            self.INERTIAL_HALFLIFE,
            1.0 / FPS,
        )

        local_pos = local_pos.copy()
        local_rot = local_rot.copy()
        local_pos += self.inertial_pos_offset
        local_pos[0] -= self.inertial_pos_offset[0]
        offset_rot = R.from_rotvec(self.inertial_rot_offset.reshape(-1, 3))
        local_rot = (offset_rot * R.from_quat(local_rot.reshape(-1, 4))).as_quat().reshape(local_rot.shape)
        self.blend_left -= 1
        if self.blend_left <= 0:
            self.inertial_pos_offset = None
            self.inertial_vel_offset = None
            self.inertial_rot_offset = None
            self.inertial_avel_offset = None
        return local_pos, local_rot

    def _install_match(self, source_id: int, frame: int):
        source = self.sources[source_id]
        frame_limit = source.cycle_len - 1 if source.is_looping else source.num_frames - 2
        frame = int(np.clip(frame, 0, frame_limit))
        current_yaw = float(_yaw_from_quat(self.cur_root_rot))
        target_yaw = float(source.root_yaw[frame])
        new_align_rot = R.from_rotvec(_wrap_angle(current_yaw - target_yaw) * Y_AXIS)
        self._begin_inertialization(
            source_id,
            frame,
            anchor_yaw=current_yaw,
            align_rot=new_align_rot,
        )

        self.cur_source_id = source_id
        self.cur_frame = frame + 1
        self.anchor_frame = frame
        self.anchor_pos = self.cur_root_pos.copy()
        self.anchor_pos[1] = self.base_root_y
        self.anchor_yaw = current_yaw
        self.align_rot = new_align_rot
        self.search_countdown = (
            self.TURN_SEARCH_INTERVAL
            if source.kind in ("turn_left", "turn_right")
            else self.SEARCH_INTERVAL
        )
        self.frames_since_switch = 0

    def _desired_features(self, desired_pos_list, desired_rot_list, desired_vel_list, desired_avel_list):
        k = min(
            len(MotionDatabase.OFFSETS),
            desired_pos_list.shape[0],
            desired_rot_list.shape[0],
            desired_vel_list.shape[0],
            desired_avel_list.shape[0],
        )
        cur_yaw = float(_yaw_from_quat(self.cur_root_rot))
        base_pos = desired_pos_list[0]

        pos_delta = desired_pos_list[:k] - base_pos[None, :]
        pos_local = _rotate_xz(pos_delta[:, [0, 2]], -cur_yaw)

        vel_local = _rotate_xz(desired_vel_list[:k, [0, 2]], -cur_yaw)
        acc_local = _future_derivative(vel_local, MotionDatabase.OFFSETS)
        desired_yaw = _yaw_from_quat(desired_rot_list[:k])
        yaw_offsets = _wrap_angle(desired_yaw - cur_yaw)
        avel_y = desired_avel_list[:k, 1]
        aacc_y = _future_derivative(avel_y, MotionDatabase.OFFSETS)
        return pos_local, vel_local, acc_local, yaw_offsets, avel_y, aacc_y

    def _target_kind(self, desired_vel_local: np.ndarray, desired_yaw_offsets: np.ndarray) -> str:
        speed = float(np.linalg.norm(desired_vel_local[0]))
        gait = int(getattr(self.controller, "gait", 0))
        yaw_path = np.unwrap(desired_yaw_offsets)
        max_yaw = float(np.max(np.abs(yaw_path[1:]))) if len(yaw_path) > 1 else 0.0
        end_yaw = float(yaw_path[-1]) if len(yaw_path) > 0 else 0.0

        if speed < self.STOP_SPEED and max_yaw < 0.20:
            return "idle"
        turn_threshold = (
            self.TURN_EXIT_YAW
            if self._source().kind in ("turn_left", "turn_right")
            else self.TURN_ENTER_YAW
        )
        if max_yaw > turn_threshold:
            return "turn_left" if end_yaw > 0.0 else "turn_right"
        if gait == 1 or speed > self.RUN_SPEED:
            return "run"
        return "walk"

    def _score_candidates(
        self,
        desired_pos_local,
        desired_vel_local,
        desired_acc_local,
        desired_yaw_offsets,
        desired_avel_y,
        desired_aacc_y,
        forced_target: str = None,
    ):
        db = self.database
        k = desired_pos_local.shape[0]

        pos_err = db.pos_offsets[:, :k] - desired_pos_local[None, :, :]
        vel_err = db.vel_local[:, :k] - desired_vel_local[None, :, :]
        acc_err = db.acc_local[:, :k] - desired_acc_local[None, :, :]
        yaw_err = _wrap_angle(db.yaw_offsets[:, :k] - desired_yaw_offsets[None, :])
        avel_err = db.avel_y[:, :k] - desired_avel_y[None, :]
        aacc_err = db.aacc_y[:, :k] - desired_aacc_y[None, :]

        motion_speed = np.linalg.norm(db.vel_local[:, :k], axis=-1)
        desired_speed = np.linalg.norm(desired_vel_local, axis=-1)
        speed_err = motion_speed - desired_speed[None, :]
        forward_shortfall = np.maximum(
            0.0,
            0.35 * desired_vel_local[None, :, 1] - db.vel_local[:, :k, 1],
        )
        desired_turn = float(desired_yaw_offsets[-1])
        candidate_turn = db.yaw_offsets[:, k - 1]
        turn_mask = (db.kind_ids == 3) | (db.kind_ids == 4)
        non_turn_mask = (db.kind_ids == 1) | (db.kind_ids == 2)
        wrong_turn = np.maximum(0.0, -desired_turn * candidate_turn)
        matched_turn = np.maximum(0.0, desired_turn * candidate_turn)
        quiet_turn = np.exp(-2.0 * abs(desired_turn))

        scores = (
            self.W_POS * np.mean(np.sum(pos_err * pos_err, axis=-1), axis=1)
            + self.W_VEL * np.mean(np.sum(vel_err * vel_err, axis=-1), axis=1)
            + self.W_SPEED * np.mean(speed_err * speed_err, axis=1)
            + self.W_FORWARD * np.mean(forward_shortfall * forward_shortfall, axis=1)
            + self.W_ACC * np.mean(np.sum(acc_err * acc_err, axis=-1), axis=1)
            + self.W_YAW * np.mean(yaw_err * yaw_err, axis=1)
            + self.W_AVEL * np.mean(avel_err * avel_err, axis=1)
            + self.W_AACC * np.mean(aacc_err * aacc_err, axis=1)
            + np.where(
                turn_mask,
                self.W_TURN_DIR * wrong_turn + self.W_TURN_IDLE * quiet_turn - self.W_TURN_MATCH * matched_turn,
                0.0,
            )
            + np.where(non_turn_mask, self.W_NON_TURN_YAW * abs(desired_turn) * np.abs(candidate_turn), 0.0)
        )

        target = forced_target or self._target_kind(desired_vel_local, desired_yaw_offsets)
        kind = db.kind_ids
        allowed = np.ones_like(scores, dtype=bool)
        if target == "idle":
            allowed = kind == 0
            scores += np.where(kind == 0, -0.5, 8.0)
        elif target == "walk":
            allowed = (kind == 1) | (kind == 3) | (kind == 4)
            scores += np.where(kind == 1, -0.4, 0.0)
            scores += np.where(kind == 2, 8.0, 0.0)
            scores += np.where(kind == 0, 5.0, 0.0)
        elif target == "run":
            allowed = (kind == 2) | (kind == 3) | (kind == 4)
            scores += np.where(kind == 2, -0.7, 0.0)
            scores += np.where(kind == 1, 8.0, 0.0)
            scores += np.where(kind == 0, 8.0, 0.0)
        elif target == "turn_left":
            allowed = kind == 3
            scores += np.where(kind == 3, -1.2, 0.0)
            scores += np.where(kind == 4, 6.0, 0.0)
            scores += np.where(kind == 0, 8.0, 0.0)
        elif target == "turn_right":
            allowed = kind == 4
            scores += np.where(kind == 4, -1.2, 0.0)
            scores += np.where(kind == 3, 6.0, 0.0)
            scores += np.where(kind == 0, 8.0, 0.0)

        for source_id, source in enumerate(self.sources):
            if source.name in ("walk", "run"):
                scores += np.where(db.source_ids == source_id, -self.W_SHORT_CLIP, 0.0)

        scores += np.where(
            ((kind == 1) | (kind == 2)) & (np.abs(candidate_turn) > 0.35),
            self.W_NON_TURN_ABS_YAW * (np.abs(candidate_turn) - 0.35) ** 2,
            0.0,
        )
        scores = np.where(allowed, scores, scores + 1000.0)
        return scores

    @staticmethod
    def _target_score_bias(target: str) -> float:
        if target == "idle":
            return -0.5
        if target == "walk":
            return -0.4
        if target == "run":
            return -0.7
        if target in ("turn_left", "turn_right"):
            return -1.2
        return 0.0

    def _current_score(
        self,
        desired_pos_local,
        desired_vel_local,
        desired_acc_local,
        desired_yaw_offsets,
        desired_avel_y,
        desired_aacc_y,
        forced_target: str = None,
    ) -> float:
        target = forced_target or self._target_kind(desired_vel_local, desired_yaw_offsets)
        current_kind = self._source().kind
        if target != current_kind:
            return float("inf")

        db = self.database
        source_mask = db.source_ids == self.cur_source_id
        if not np.any(source_mask):
            return float("inf")
        frame_delta = np.abs(db.frame_ids[source_mask] - self.cur_frame)
        idx_local = int(np.argmin(frame_delta))
        if frame_delta[idx_local] > 6:
            return float("inf")

        idx = np.flatnonzero(source_mask)[idx_local]
        k = desired_pos_local.shape[0]
        pos_err = db.pos_offsets[idx, :k] - desired_pos_local
        vel_err = db.vel_local[idx, :k] - desired_vel_local
        acc_err = db.acc_local[idx, :k] - desired_acc_local
        yaw_err = _wrap_angle(db.yaw_offsets[idx, :k] - desired_yaw_offsets)
        avel_err = db.avel_y[idx, :k] - desired_avel_y
        aacc_err = db.aacc_y[idx, :k] - desired_aacc_y
        speed_err = np.linalg.norm(db.vel_local[idx, :k], axis=-1) - np.linalg.norm(
            desired_vel_local,
            axis=-1,
        )
        forward_shortfall = np.maximum(0.0, 0.35 * desired_vel_local[:, 1] - db.vel_local[idx, :k, 1])
        desired_turn = float(desired_yaw_offsets[-1])
        candidate_turn = float(db.yaw_offsets[idx, k - 1])
        non_turn_penalty = 0.0
        turn_penalty = 0.0
        if current_kind in ("turn_left", "turn_right"):
            turn_penalty = (
                self.W_TURN_DIR * max(0.0, -desired_turn * candidate_turn)
                + self.W_TURN_IDLE * np.exp(-2.0 * abs(desired_turn))
                - self.W_TURN_MATCH * max(0.0, desired_turn * candidate_turn)
            )
        elif current_kind in ("walk", "run"):
            non_turn_penalty = self.W_NON_TURN_YAW * abs(desired_turn) * abs(candidate_turn)
            if abs(candidate_turn) > 0.35:
                non_turn_penalty += self.W_NON_TURN_ABS_YAW * (abs(candidate_turn) - 0.35) ** 2

        score = (
            self.W_POS * np.mean(np.sum(pos_err * pos_err, axis=-1))
            + self.W_VEL * np.mean(np.sum(vel_err * vel_err, axis=-1))
            + self.W_SPEED * np.mean(speed_err * speed_err)
            + self.W_FORWARD * np.mean(forward_shortfall * forward_shortfall)
            + self.W_ACC * np.mean(np.sum(acc_err * acc_err, axis=-1))
            + self.W_YAW * np.mean(yaw_err * yaw_err)
            + self.W_AVEL * np.mean(avel_err * avel_err)
            + self.W_AACC * np.mean(aacc_err * aacc_err)
            + turn_penalty
            + non_turn_penalty
        )
        return float(score + self._target_score_bias(target))

    def _maybe_search(self, desired_pos_list, desired_rot_list, desired_vel_list, desired_avel_list):
        source = self._source()
        near_end = (not source.is_looping) and self.cur_frame >= source.num_frames - 3
        self.search_countdown -= 1

        desired = self._desired_features(
            desired_pos_list,
            desired_rot_list,
            desired_vel_list,
            desired_avel_list,
        )
        target = self._target_kind(desired[1], desired[3])
        turn_target = target in ("turn_left", "turn_right")

        if (
            source.kind in ("turn_left", "turn_right")
            and not near_end
            and self.frames_since_switch < self.TURN_MIN_SWITCH_FRAMES
        ):
            return

        forced_target = None
        if source.kind == target and source.kind == "idle" and not near_end:
            return

        if not near_end and self.search_countdown > 0:
            if turn_target:
                self.search_countdown = min(self.search_countdown, self.TURN_SEARCH_INTERVAL)
            return

        scores = self._score_candidates(*desired, forced_target=forced_target)
        best_idx = int(np.argmin(scores))
        current_score = self._current_score(*desired, forced_target=forced_target)

        if not near_end and current_score <= scores[best_idx] + self.CURRENT_MARGIN:
            self.search_countdown = self.TURN_SEARCH_INTERVAL if turn_target else self.SEARCH_INTERVAL
            return

        same_source = int(self.database.source_ids[best_idx]) == self.cur_source_id
        best_frame = int(self.database.frame_ids[best_idx])
        best_source_id = int(self.database.source_ids[best_idx])
        best_source = self.sources[best_source_id]
        frame_advance = best_frame - self.cur_frame
        same_neighborhood = same_source and abs(frame_advance) < 20
        if not near_end and source.kind in ("turn_left", "turn_right") and same_source:
            self.search_countdown = self.TURN_SEARCH_INTERVAL
            return
        if not near_end and source.kind in ("walk", "run") and same_source:
            self.search_countdown = self.TURN_SEARCH_INTERVAL if turn_target else self.SEARCH_INTERVAL
            return
        if (
            not near_end
            and source.kind in ("walk", "run")
            and best_source.kind == source.kind
        ):
            self.search_countdown = self.TURN_SEARCH_INTERVAL if turn_target else self.SEARCH_INTERVAL
            return
        if not near_end and not same_source:
            required_margin = self.CROSS_SOURCE_MARGIN
            if source.kind == best_source.kind:
                required_margin = self.SAME_KIND_SWITCH_MARGIN
            if current_score <= scores[best_idx] + required_margin:
                self.search_countdown = self.TURN_SEARCH_INTERVAL if turn_target else self.SEARCH_INTERVAL
                return

        if near_end or not same_neighborhood:
            self._install_desired_vel = desired_vel_list[0]
            self._install_match(
                best_source_id,
                best_frame,
            )
            self._install_desired_vel = None
        else:
            self.search_countdown = (
                self.TURN_SEARCH_INTERVAL
                if source.kind in ("turn_left", "turn_right") or turn_target
                else self.SEARCH_INTERVAL
            )

    def update_state(
        self,
        desired_pos_list,
        desired_rot_list,
        desired_vel_list,
        desired_avel_list,
    ):
        desired_pos_list = np.asarray(desired_pos_list, dtype=np.float64)
        desired_rot_list = np.asarray(desired_rot_list, dtype=np.float64)
        desired_vel_list = np.asarray(desired_vel_list, dtype=np.float64)
        desired_avel_list = np.asarray(desired_avel_list, dtype=np.float64)

        if self.last_joint_translation is not None:
            self._maybe_search(desired_pos_list, desired_rot_list, desired_vel_list, desired_avel_list)

        source = self._source()
        self._wrap_current_loop()
        source = self._source()
        if not source.is_looping and self.cur_frame >= source.num_frames - 1:
            self.cur_frame = source.num_frames - 2

        joint_name, local_pos, local_rot = self._aligned_local_pose(
            self.cur_source_id,
            self.cur_frame,
        )

        local_pos, local_rot = self._apply_inertialization(local_pos, local_rot)

        joint_translation, joint_orientation = self._fk_from_local(self.cur_source_id, local_pos, local_rot)

        self.last_local_pos = local_pos.copy()
        self.last_local_rot = local_rot.copy()
        self.last_joint_translation = joint_translation.copy()
        self.last_joint_orientation = joint_orientation.copy()
        self.cur_root_pos = joint_translation[0].copy()
        self.cur_root_rot = joint_orientation[0].copy()

        self.cur_frame += 1
        self.frames_since_switch += 1
        self.turn_cooldown = max(0, self.turn_cooldown - 1)
        return joint_name, joint_translation, joint_orientation

    def sync_controller_and_character(self, character_state):
        controller_pos = character_state[1][0].copy()
        controller_pos[1] = 0.0
        delta = np.linalg.norm(controller_pos - self.controller.position)
        if delta > 1.0:
            self.controller.set_pos(controller_pos)
        elif hasattr(self.controller, "position"):
            old_pos = self.controller.position.copy()
            self.controller.position = controller_pos
            shift = controller_pos - old_pos
            if hasattr(self.controller, "ctrl_position"):
                self.controller.ctrl_position += shift
            if hasattr(self.controller, "ctrl_center"):
                self.controller.ctrl_center += shift
            if hasattr(self.controller, "cameractrl"):
                camera = self.controller.cameractrl
                vec_type = type(camera.position)
                camera.position = vec_type(
                    -self.controller.ctrl_position[0],
                    -self.controller.ctrl_position[2],
                    self.controller.ctrl_position[1],
                )
                camera.center = vec_type(
                    -self.controller.ctrl_center[0],
                    -self.controller.ctrl_center[2],
                    self.controller.ctrl_center[1],
                )
                camera.look()
