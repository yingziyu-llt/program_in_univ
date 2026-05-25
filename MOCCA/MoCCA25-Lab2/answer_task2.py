from graph import *
from answer_task1 import *
from typing import Optional
from bvh_motion import BVHMotion
from scipy.spatial.transform import Rotation as R
from scipy.spatial.transform import Slerp
import numpy as np

# ─────────────────────────────────────────────────────────────
# Helpers
# ─────────────────────────────────────────────────────────────

def _yaw_from_quat(q: np.ndarray) -> float:
    euler = R.from_quat(q).as_euler('YXZ')
    return float(euler[0, 0] if euler.ndim == 2 else euler[0])


def _yaw_error_rad(cur_q: np.ndarray, desired_q: np.ndarray) -> float:
    """Signed yaw error (rad), positive = need to turn left/CCW."""
    diff = _yaw_from_quat(desired_q) - _yaw_from_quat(cur_q)
    return float((diff + np.pi) % (2 * np.pi) - np.pi)


def _node_yaw_delta(node) -> float:
    """Total signed yaw change of the node's raw clip (rad)."""
    yaw0 = _yaw_from_quat(node.motion.joint_rotation[0,  0])
    yaw1 = _yaw_from_quat(node.motion.joint_rotation[-1, 0])
    return float(((yaw1 - yaw0) + np.pi) % (2 * np.pi) - np.pi)


def _align_motion(motion: BVHMotion,
                  root_pos: np.ndarray,
                  root_rot_q: np.ndarray) -> BVHMotion:
    """Return copy of motion with frame-0 aligned to root_pos / root_rot_q."""
    facing = (R.from_quat(root_rot_q)
               .apply(np.array([0.0, 0.0, 1.0]))
               .flatten()[[0, 2]])
    return motion.translation_and_rotation(0, root_pos[[0, 2]], facing)


# ─────────────────────────────────────────────────────────────
# CharacterController
# ─────────────────────────────────────────────────────────────

class CharacterController:
    """
    Responsive motion-graph controller.

    Responsiveness
    --------------
    Every frame we score all candidate clips against the desired trajectory.
    If any candidate beats the current clip by SWITCH_MARGIN, we switch
    immediately – no waiting for clip_done.

    Continuity (Task 3)
    -------------------
    On every switch we call concatenate_two_motions:
      part1 = sub_sequence of the CURRENT aligned clip  [cur_frame … cur_frame+MIX_FRAMES)
      part2 = next clip aligned to the current world root
      mix_frame1=0, mix_time=MIX_FRAMES
    This produces a cross-faded motion whose姿势 starts exactly where the
    character is now and smoothly arrives at the new clip's motion.
    We then play this blended motion from frame 0.

    Walk loop (Task 3)
    ------------------
    The walk clip is loop-ified once at startup with build_loop_motion so
    it can tile indefinitely without a seam.

    Turn gating
    -----------
    Turn clips are penalised unless |yaw_err| is large.
    Wrong-direction turns get an additional signed penalty.
    A turn clip playing is not interrupted until MIN_PLAY_FRAMES elapsed.
    """

    MIX_FRAMES          = 12
    TURN_THRESHOLD_RAD  = np.deg2rad(20)
    MIN_PLAY_FRAMES     = 15
    WALK_NODE_ID        = 0
    SWITCH_MARGIN       = 0.15   # candidate must beat current by this to switch

    W_ROT    = 3.0
    W_VEL    = 1.5
    W_POS    = 0.3
    WALK_BONUS = 0.70

    LOOP_RATIO     = 0.5
    LOOP_HALF_LIFE = 0.2

    # ── init ─────────────────────────────────────────────────────────────

    def __init__(self, controller) -> None:
        self.controller = controller
        self.graph = Graph('./nodes.npy')
        self.graph.load_from_file()

        self.cur_root_pos: np.ndarray = None
        self.cur_root_rot: np.ndarray = None
        self.cur_node = None
        self.cur_frame = 0

        # active_motion  – BVHMotion in *local* space (for sub_sequence / concat)
        # active_jt / jo – global FK cache derived from active_motion
        self._active_motion: BVHMotion = None
        self._active_jt: np.ndarray = None   # (N, M, 3)
        self._active_jo: np.ndarray = None   # (N, M, 4)
        self._active_end: int = 0

        # Per-node signed yaw delta (precomputed once)
        self._yaw_delta = {
            node.identity: _node_yaw_delta(node)
            for node in self.graph.nodes
        }

        # Loop-ified walk clip
        walk_raw = self.graph.nodes[self.WALK_NODE_ID].motion
        self._walk_loop: BVHMotion = build_loop_motion(
            walk_raw, self.LOOP_RATIO, self.LOOP_HALF_LIFE)

        self.initialize()

    # ── initialise ───────────────────────────────────────────────────────

    def initialize(self):
        self.cur_node  = self.graph.nodes[self.WALK_NODE_ID]
        self.cur_frame = 0

        p = self.cur_node.motion.joint_position[0, 0, :].copy()
        p[1] = 0.0
        self.cur_root_pos = p
        self.cur_root_rot, _ = BVHMotion.decompose_rotation_with_yaxis(
            self.cur_node.motion.joint_rotation[0, 0])

        self._set_active(_align_motion(
            self._walk_loop, self.cur_root_pos, self.cur_root_rot))

    # ── activate a motion (compute FK, reset frame ptr) ──────────────────

    def _set_active(self, motion: BVHMotion):
        """Install motion as the active clip and run FK."""
        self._active_motion = motion
        self._active_jt, self._active_jo = motion.batch_forward_kinematics()
        self._active_end = motion.motion_length
        self.cur_frame   = 0

    # ── raw clip for a node (walk → looped version) ───────────────────────

    def _raw(self, node) -> BVHMotion:
        return self._walk_loop if node.identity == self.WALK_NODE_ID \
               else node.motion

    # ── matching cost ─────────────────────────────────────────────────────

    def _cost(self, node, desired_rot_list, desired_vel_list,
              yaw_err: float = 0.0) -> float:
        """Score node (aligned from frame-0 to current root). Lower = better."""
        facing = (R.from_quat(self.cur_root_rot)
                   .apply(np.array([0.0, 0.0, 1.0]))
                   .flatten()[[0, 2]])
        aligned  = self._raw(node).translation_and_rotation(
            0, self.cur_root_pos[[0, 2]], facing)
        n        = aligned.motion_length
        T        = desired_rot_list.shape[0]

        score = 0.0
        count = 0
        dt    = 1.0 / 60.0
        exp_xz = self.cur_root_pos[[0, 2]].copy()

        for offset, step, t_idx in zip([10, 20, 40], [10, 10, 20], range(3)):
            if t_idx >= T:
                break
            exp_xz = exp_xz + desired_vel_list[t_idx, [0, 2]] * step * dt
            f  = min(offset, n - 1)
            f1 = min(f + 1,  n - 1)

            rot_err = 1.0 - abs(float(np.dot(
                aligned.joint_rotation[f, 0], desired_rot_list[t_idx])))

            pv = (aligned.joint_position[f1, 0, [0, 2]] -
                  aligned.joint_position[f,  0, [0, 2]]) * 60.0
            vel_err = np.linalg.norm(pv - desired_vel_list[t_idx, [0, 2]])

            pos_err = np.linalg.norm(
                aligned.joint_position[f, 0, [0, 2]] - exp_xz)

            score += self.W_ROT * rot_err + self.W_VEL * vel_err + self.W_POS * pos_err
            count += 1

        if count == 0:
            return float('inf')
        score /= count

        # Wrong-direction penalty for turn clips
        if node.identity != self.WALK_NODE_ID:
            nd = self._yaw_delta[node.identity]
            if abs(yaw_err) > 0.05 and abs(nd) > 0.05 and yaw_err * nd < 0:
                score += 4.0 * abs(nd)

        if node.identity == self.WALK_NODE_ID:
            score *= self.WALK_BONUS

        return score

    # ── transition via concatenate_two_motions ────────────────────────────

    def _switch_to(self, next_node):
        """
        Switch to next_node immediately using concatenate_two_motions for
        a seamless姿势 transition.

        part1 : MIX_FRAMES-long stub from current active clip
        part2 : next node's clip, aligned to current world root
        mix   : blend from frame 0 of part1 over MIX_FRAMES
        """
        # Freeze world root at this exact frame (Task 2)
        f = min(self.cur_frame, self._active_end - 1)
        self.cur_root_pos = self._active_jt[f, 0].copy()
        self.cur_root_rot, _ = BVHMotion.decompose_rotation_with_yaxis(
            self._active_jo[f, 0])

        # part1: sub-sequence of the current local-space motion
        stub_start = f
        stub_end   = min(f + self.MIX_FRAMES, self._active_end)

        # If remaining frames are fewer than MIX_FRAMES, pad by re-using the
        # loop-ified walk (or the raw clip looped) appended to current stub.
        part1 = self._active_motion.sub_sequence(stub_start, stub_end)
        if part1.motion_length < self.MIX_FRAMES:
            pad = _align_motion(self._raw(self.cur_node),
                                self.cur_root_pos, self.cur_root_rot)
            part1.append(pad)
            # trim to exactly MIX_FRAMES
            part1 = part1.sub_sequence(0, self.MIX_FRAMES)

        # part2: next clip aligned to current root
        part2 = _align_motion(self._raw(next_node),
                               self.cur_root_pos, self.cur_root_rot)

        # Concatenate with full blend from the very start of part1
        try:
            blended = concatenate_two_motions(
                part1, part2,
                mix_frame1=0,
                mix_time=self.MIX_FRAMES,
            )
        except Exception:
            blended = part2   # fallback

        self.cur_node = next_node
        self._set_active(blended)

    # ── main update ───────────────────────────────────────────────────────

    def update_state(self, desired_pos_list, desired_rot_list,
                     desired_vel_list, desired_avel_list):
        """Called once per frame. Returns (joint_name, jt, jo)."""

        # ── Situation ────────────────────────────────────────────────── #
        yaw_err   = _yaw_error_rad(self.cur_root_rot, desired_rot_list[0])
        need_turn = abs(yaw_err) > self.TURN_THRESHOLD_RAD
        clip_done = (self.cur_frame >= self._active_end - 1)

        currently_turning = (self.cur_node.identity != self.WALK_NODE_ID)
        played_enough     = (self.cur_frame >= self.MIN_PLAY_FRAMES)

        # ── Score current clip ────────────────────────────────────────── #
        cur_score = self._cost(self.cur_node, desired_rot_list,
                               desired_vel_list, yaw_err)

        # ── Score candidates ──────────────────────────────────────────── #
        walk_node  = self.graph.nodes[self.WALK_NODE_ID]
        candidates = [e.destination for e in self.cur_node.edges]
        if walk_node not in candidates:
            candidates.append(walk_node)

        best_node  = None
        best_score = float('inf')
        desired_speed = float(np.linalg.norm(desired_vel_list[0, [0, 2]]))

        for cand in candidates:
            s = self._cost(cand, desired_rot_list, desired_vel_list, yaw_err)
            if 'spin' in cand.name.lower() and desired_speed > 0.15:
                s += 5.0
            if cand.identity != self.WALK_NODE_ID and not need_turn:
                s += 3.0
            if s < best_score:
                best_score = s
                best_node  = cand

        # ── Switch decision ───────────────────────────────────────────── #
        # Interrupting a turn clip is only allowed after MIN_PLAY_FRAMES.
        can_interrupt = (not currently_turning) or played_enough

        do_switch = clip_done or (
            best_node is not None and
            best_node is not self.cur_node and
            best_score < cur_score - self.SWITCH_MARGIN and
            can_interrupt
        )

        if do_switch and best_node is not None:
            self._switch_to(best_node)

        # ── Extract pose ──────────────────────────────────────────────── #
        frame = min(self.cur_frame, self._active_end - 1)
        jt = self._active_jt[frame].copy()
        jo = self._active_jo[frame].copy()

        # ── Update world root ─────────────────────────────────────────── #
        self.cur_root_pos = self._active_jt[frame, 0].copy()
        self.cur_root_rot, _ = BVHMotion.decompose_rotation_with_yaxis(
            self._active_jo[frame, 0])

        self.cur_frame += 1
        return self.cur_node.motion.joint_name, jt, jo