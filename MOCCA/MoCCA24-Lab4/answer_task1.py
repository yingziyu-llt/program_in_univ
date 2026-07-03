import numpy as np
from scipy.spatial.transform import Rotation as R
from bvh_loader import BVHMotion
from physics_warpper import PhysicsInfo


def _quat_mul(q1, q2):
    x1, y1, z1, w1 = np.moveaxis(q1, -1, 0)
    x2, y2, z2, w2 = np.moveaxis(q2, -1, 0)
    return np.stack([
        w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2,
        w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2,
        w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2,
        w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2,
    ], axis=-1)


def _quat_inv(q):
    res = q.copy()
    res[..., :3] *= -1
    return res


def _quat_normalize(q):
    return q / np.linalg.norm(q, axis=-1, keepdims=True)


def _quat_apply(q, v):
    qv = q[..., :3]
    t = 2.0 * np.cross(qv, v)
    return v + q[..., 3:4] * t + np.cross(qv, t)


def _quat_to_rotvec(q):
    q = _quat_normalize(q)
    q = np.where(q[..., 3:4] < 0.0, -q, q)
    vec = q[..., :3]
    norm = np.linalg.norm(vec, axis=-1, keepdims=True)
    angle = 2.0 * np.arctan2(norm, q[..., 3:4])
    scale = np.divide(angle, norm, out=np.full_like(norm, 2.0), where=norm > 1e-8)
    return vec * scale


def _gain_for_joints(gain, joint_id):
    gain = np.asarray(gain, dtype=np.float64)
    if gain.ndim == 0:
        return gain
    gain = gain[joint_id]
    if gain.ndim == 1:
        gain = gain[:, None]
    return gain


def part1_cal_torque(pose, physics_info: PhysicsInfo, **kargs):
    '''
    输入： pose: (20, 4)的numpy数组，表示每个关节的目标旋转(相对于父关节的)
           physics_info: PhysicsInfo类，包含了当前的物理信息，参见physics_warpper.py
           **kargs: 指定参数，可能包含kp,kd
    输出： global_torque: (20, 3)的numpy数组，表示每个关节的全局坐标下的目标力矩，因为不需要控制方向，根节点力矩会被后续代码无视
    '''
    # ------一些提示代码，你可以随意修改------------#
    kp = kargs.get('kp', 400) # 需要自行调整kp和kd！ 而且也可以是一个数组，指定每个关节的kp和kd
    kd = kargs.get('kd', 40)
    torque_limit = kargs.get('torque_limit', 400)
    parent_index = physics_info.parent_index
    joint_name = physics_info.joint_name
    # 注意关节没有自己的朝向和角速度，这里用子body的朝向和角速度表示此时关节的信息
    joint_orientation = physics_info.get_body_orientation()
    # print(physics_info.get_root_pos_and_vel())
    parent_index = physics_info.parent_index
    joint_avel = physics_info.get_body_angular_velocity()
    target_rotation = pose

    global_torque = np.zeros((len(joint_name),3))
    # 你的代码
    joint_id = np.arange(1, len(joint_name))
    parent_id = np.asarray(parent_index, dtype=np.int64)[joint_id]
    wrist_id = np.array([
        i for i, name in enumerate(joint_name)
        if name in ('rWrist', 'lWrist')
    ], dtype=np.int64)

    parent_rot = _quat_normalize(joint_orientation[parent_id])
    child_rot = _quat_normalize(joint_orientation[joint_id])
    current_local_rot = _quat_mul(_quat_inv(parent_rot), child_rot)
    target_local_rot = _quat_normalize(target_rotation[joint_id])

    rot_diff = _quat_mul(target_local_rot, _quat_inv(current_local_rot))
    angle_diff = _quat_to_rotvec(rot_diff)

    rel_avel = joint_avel[joint_id] - joint_avel[parent_id]
    local_rel_avel = _quat_apply(_quat_inv(parent_rot), rel_avel)

    local_torque = _gain_for_joints(kp, joint_id) * angle_diff \
        - _gain_for_joints(kd, joint_id) * local_rel_avel
    limit = _gain_for_joints(torque_limit, joint_id)
    local_torque = np.clip(local_torque, -limit, limit)
    if len(wrist_id) > 0:
        wrist_mask = np.isin(joint_id, wrist_id)
        local_torque[wrist_mask] *= kargs.get('wrist_torque_scale', 0.1)

    global_torque[joint_id] = _quat_apply(parent_rot, local_torque)
    
    
    # 抹去根节点力矩
    global_torque[0] = np.zeros_like(global_torque[0])
    return global_torque

def part2_cal_float_base_torque(target_position, pose, physics_info, **kargs):
    '''
    输入： target_position: (3,)的numpy数组，表示根节点的目标位置，其余同上
    输出： global_root_force: (3,)的numpy数组，表示根节点的全局坐标下的辅助力，在后续仿真中只会保留y方向的力
           global_root_torque: (3,)的numpy数组，表示根节点的全局坐标下的辅助力矩，用来控制角色的朝向，实际上就是global_torque的第0项
           global_torque: 同上
    注意：
        1. 你需要自己计算kp和kd，并且可以通过kargs调整part1中的kp和kd
        2. global_torque[0]在track静止姿态时会被无视，但是track走路时会被加到根节点上，不然无法保持根节点朝向
        3. 可以适当将根节点目标位置上提以产生更大的辅助力，使角色走得更自然
    '''
    # ------一些提示代码，你可以随意修改------------#
    global_torque = part1_cal_torque(pose, physics_info)
    kp = kargs.get('root_kp', 4000) # 需要自行调整root的kp和kd！
    kd = kargs.get('root_kd', 20)
    root_position, root_velocity = physics_info.get_root_pos_and_vel()
    global_root_force = np.zeros((3,))
    global_root_torque = global_torque[0]
    # 你的代码
    target_position = target_position.copy()
    target_position[1] += kargs.get('root_height_offset', 0.05)

    pos_diff = target_position - root_position
    global_root_force = kp * pos_diff - kd * root_velocity
    global_root_force = np.clip(global_root_force, -800, 800)

    body_orientation = physics_info.get_body_orientation()
    body_avel = physics_info.get_body_angular_velocity()

    target_root_rot = _quat_normalize(pose[0])
    current_root_rot = _quat_normalize(body_orientation[0])

    rot_diff = _quat_mul(target_root_rot, _quat_inv(current_root_rot))
    angle_diff = _quat_to_rotvec(rot_diff)

    root_rot_kp = kargs.get('root_rot_kp', 400)
    root_rot_kd = kargs.get('root_rot_kd', 40)
    global_root_torque = root_rot_kp * angle_diff - root_rot_kd * body_avel[0]
    global_root_torque = np.clip(global_root_torque, -400, 400)
    
    ########
    return global_root_force, global_root_torque, global_torque


def part3_cal_static_standing_torque(bvh: BVHMotion, physics_info):
    '''
    输入： bvh: BVHMotion类，包含了当前的动作信息，参见bvh_loader.py
    输出： 带反馈的global_torque: (20, 3)的numpy数组，因为不需要控制方向，根节点力矩会被无视
    Tips: 
        只track第0帧就能保持站立了
        为了保持平衡可以把目标的根节点位置适当前移，比如把根节点位置和左右脚的中点加权平均，但要注意角色还会收到一个从背后推他的外力
        可以定义一个全局的frame_count变量来标记当前的帧数，在站稳后根据帧数使角色进行周期性左右摇晃，如果效果好可以加分（0-20），可以考虑让角色站稳后再摇晃
    '''
    # ------一些提示代码，你可以随意修改------------#
    tar_pos = bvh.joint_position[0][0].copy()
    pose = bvh.joint_rotation[0]
    joint_name = physics_info.joint_name

    torque = part1_cal_torque(
        pose, physics_info,
        kp=350, kd=35, torque_limit=350,
        wrist_torque_scale=0.0,
    )
    # 你的代码
    joint_positions = physics_info.get_joint_translation()
    name_to_id = {name: i for i, name in enumerate(joint_name)}

    support_names = ('rAnkle', 'lAnkle', 'rToeJoint', 'lToeJoint')
    support_ids = [name_to_id[name] for name in support_names if name in name_to_id]
    if len(support_ids) > 0:
        support_center = np.mean(joint_positions[support_ids], axis=0)
        tar_pos[0] = support_center[0]
        tar_pos[2] = support_center[2]

    body_positions = physics_info.get_body_position()
    body_velocities = physics_info.get_body_velocity()
    body_mass = physics_info.get_body_mass().reshape(-1, 1)
    com = np.sum(body_positions * body_mass, axis=0) / np.sum(body_mass)
    com_velocity = np.sum(body_velocities * body_mass, axis=0) / np.sum(body_mass)

    root_up = _quat_apply(_quat_normalize(physics_info.get_body_orientation()[0]), np.array([0.0, 1.0, 0.0]))
    tilt_error = np.array([root_up[0], 0.0, root_up[2]])

    # Jacobian Transpose: tau_i = J_i^T F.
    # For a ball joint, J_i maps angular velocity to linear velocity by
    # v = omega x (com - joint_pos), so tau = (com - joint_pos) x F.
    com_error = tar_pos - com
    virtual_force = np.array([800.0, 0.0, 800.0]) * com_error \
        - np.array([160.0, 0.0, 160.0]) * com_velocity \
        - np.array([400.0, 0.0, 400.0]) * tilt_error
    virtual_force[1] = 0.0
    virtual_force = np.clip(
        virtual_force,
        np.array([-160.0, 0.0, -160.0]),
        np.array([160.0, 0.0, 160.0]),
    )

    leg_chains = [
        ('rHip', 'rKnee', 'rAnkle'),
        ('lHip', 'lKnee', 'lAnkle'),
    ]
    for chain in leg_chains:
        if not all(name in name_to_id for name in chain):
            continue
        for name in chain:
            joint_idx = name_to_id[name]
            jacobian_torque = np.cross(
                com - joint_positions[joint_idx],
                virtual_force * 0.5,
            )
            torque[joint_idx] -= jacobian_torque

    lower_body_names = (
        'rHip', 'lHip', 'rKnee', 'lKnee',
        'rAnkle', 'lAnkle', 'rToeJoint', 'lToeJoint',
    )
    lower_body_ids = [name_to_id[name] for name in lower_body_names if name in name_to_id]
    toe_ids = [name_to_id[name] for name in ('rToeJoint', 'lToeJoint') if name in name_to_id]
    torque[toe_ids] = 0.0
    torque[lower_body_ids] = np.clip(torque[lower_body_ids], -700.0, 700.0)
    
    
    
    # 抹去根节点力矩
    torque[0] = np.zeros_like(torque[0])
    return torque
