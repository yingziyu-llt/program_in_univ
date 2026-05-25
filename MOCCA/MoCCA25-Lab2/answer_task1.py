##############
# 姓名： 林乐天
# 学号： 2300012154
##############
"""
注释里统一N表示帧数，M表示关节数
position, rotation表示局部平移和旋转
translation, orientation表示全局平移和旋转
"""
import numpy as np
import copy
from scipy.spatial.transform import Rotation as R
from scipy.spatial.transform import Slerp
from bvh_motion import BVHMotion
from smooth_utils import *

def Slerp_rotation(rot1, rot2, alpha):
    num_joints = rot1.shape[0]
    res_rot = np.zeros_like(rot1)
    
    for j in range(num_joints):
        slerp = Slerp([0, 1], R.from_quat(np.vstack([rot1[j], rot2[j]])))
        # Assign the directly returned (4,) array
        res_rot[j] = slerp(alpha).as_quat()
        
    return res_rot

# part1
def blend_two_motions(bvh_motion1:BVHMotion, bvh_motion2:BVHMotion, v:float=None, input_alpha:np.ndarray=None) -> BVHMotion:
    '''
    输入: 两个将要blend的动作，类型为BVHMotion
          将要生成的BVH的速度v
          如果给出插值的系数alpha就不需要再计算了
          target_fps,将要生成BVH的fps
    输出: blend两个BVH动作后的动作，类型为BVHMotion
    假设两个动作的帧数分别为n1, n2
    首先需要制作blend 的权重适量 alpha
    插值系数alpha: 0~1之间的浮点数组，形状为(n3,)
    返回的动作有n3帧，第i帧由(1-alpha[i]) * bvh_motion1[j] + alpha[i] * bvh_motion2[k]得到
    i均匀地遍历0~n3-1的同时，j和k应该均匀地遍历0~n1-1和0~n2-1
    Tips:
        1. 计算给出两端动作的速度，两个BVH已经将Root Joint挪到(0.0, 0.0)的XOZ位置上了，为了便于你计算，我们假定提供的bvh都是沿着z轴正方向向前运动的
        2. 利用v计算插值系数alpha
        3. 线性插值以及Slerp
    '''

    ### Your code here
    num_joints = bvh_motion1.joint_position.shape[1]
    n1 = bvh_motion1.joint_position.shape[0]
    n2 = bvh_motion2.joint_position.shape[0]
    v1 = (bvh_motion1.joint_position[-1, 0, 2] - bvh_motion1.joint_position[0, 0, 2]) / (n1 / 60)
    v2 = (bvh_motion2.joint_position[-1, 0, 2] - bvh_motion2.joint_position[0, 0, 2]) / (n2 / 60)
    print(f"v1: {v1}, v2: {v2}")
    
    if input_alpha is not None:
        alpha = input_alpha
        n3 = len(alpha)
    else:
        if v is not None:
            alpha_val = np.clip((v - v1) / (v2 - v1 + 1e-8), 0.0, 1.0)
        else:
            alpha_val = 0.5
            
        n3 = int((1.0 - alpha_val) * n1 + alpha_val * n2)
        alpha = np.full(n3, alpha_val)

    res = bvh_motion1.raw_copy()
    res.joint_position = np.zeros((n3, num_joints, 3))
    res.joint_rotation = np.zeros((n3, num_joints, 4))
    res.joint_rotation[..., 3] = 1.0

    for i in range(n3):
        j = min(int((i / max(n3 - 1, 1)) * (n1 - 1)), n1 - 1)
        k = min(int((i / max(n3 - 1, 1)) * (n2 - 1)), n2 - 1)
        
        res.joint_position[i] = (1 - alpha[i]) * bvh_motion1.joint_position[j] + alpha[i] * bvh_motion2.joint_position[k]
        
        res.joint_rotation[i] = Slerp_rotation(bvh_motion1.joint_rotation[j], bvh_motion2.joint_rotation[k], alpha[i])        
    ### Your code here
    return res

# part2
def build_loop_motion(bvh_motion:BVHMotion, ratio:float, half_life:float) -> BVHMotion:
    '''
    输入: 将要loop化的动作，类型为BVHMotion
          damping在前在后的比例ratio, ratio介于[0,1]
          弹簧振子damping效果的半衰期 half_life
          如果你使用的方法不含上面两个参数，就忽视就可以了，因接口统一保留
    输出: loop化后的动作，类型为BVHMotion
    
    Tips:
        1. 计算第一帧和最后一帧的旋转差、Root Joint位置差 (不用考虑X和Z的位置差)
        2. 如果使用"inertialization"，可以利用`smooth_utils.py`的
        `quat_to_avel`函数计算对应角速度的差距，对应速度的差距请自己填写
        3. 逐帧计算Rotations和Postions的变化
        4. 注意 BVH的fps需要考虑，因为需要算对应时间
        5. 可以参考`smooth_utils.py`的注释或者 https://theorangeduck.com/page/creating-looping-animations-motion-capture
    
    '''
    res = bvh_motion.raw_copy()
    ### Your code here
    fps=60
    n = bvh_motion.joint_position.shape[0]
    dt = 1 / fps
    num_joints = bvh_motion.joint_position.shape[1]
    pos = bvh_motion.joint_position
    pos_vel = (pos[1:] - pos[:-1]) / dt
    pos_vel = np.concatenate([pos_vel, pos_vel[-1:]], axis=0)
    
    rot = bvh_motion.joint_rotation
    rot_avel = quat_to_avel(rot, dt)
    rot_avel = np.concatenate([rot_avel, rot_avel[-1:]], axis=0)    

    pos_diff = pos[-1] - pos[0]
    pos_diff[0, [0, 2]] = 0.0 
    
    vel_diff = pos_vel[-1] - pos_vel[0]
    vel_diff[0, [0, 2]] = 0.0

    rot_start = R.from_quat(rot[0])
    rot_end = R.from_quat(rot[-1])
    rot_diff = (rot_end * rot_start.inv()).as_rotvec()
    
    avel_diff = rot_avel[-1] - rot_avel[0]

    pos_offset_total = np.zeros_like(pos)
    rot_offset_total = np.zeros((n, num_joints, 3))

    for i in range(n):
        t_f = i * dt
        t_b = (n - 1 - i) * dt
        
        p_off_f, _ = decay_spring_implicit_damping_pos(pos_diff * ratio, vel_diff * ratio, half_life, t_f)
        r_off_f, _ = decay_spring_implicit_damping_rot(rot_diff * ratio, avel_diff * ratio, half_life, t_f)
        
        p_off_b, _ = decay_spring_implicit_damping_pos(-pos_diff * (1 - ratio), -vel_diff * (1 - ratio), half_life, t_b)
        r_off_b, _ = decay_spring_implicit_damping_rot(-rot_diff * (1 - ratio), -avel_diff * (1 - ratio), half_life, t_b)
        
        pos_offset_total[i] = p_off_f + p_off_b
        rot_offset_total[i] = r_off_f + r_off_b
        
    res.joint_position += pos_offset_total
    
    offset_rot_flat = R.from_rotvec(rot_offset_total.reshape(-1, 3))
    orig_rot_flat = R.from_quat(res.joint_rotation.reshape(-1, 4))
    
    res.joint_rotation = (offset_rot_flat * orig_rot_flat).as_quat().reshape(n, num_joints, 4)
    ### Your code here
    return res

# part3
def concatenate_two_motions(bvh_motion1:BVHMotion, bvh_motion2:BVHMotion, mix_frame1:int, mix_time:int):
    '''
    将两个bvh动作平滑地连接起来
    输入: 将要连接的两个动作，类型为BVHMotion
          混合开始时间是第一个动作的第mix_frame1帧
          mix_time表示用于混合的帧数
    输出: 平滑地连接后的动作，类型为BVHMotion
    
    Tips:
        你可能需要用到BVHMotion.sub_sequence 和 BVHMotion.append
    '''
    m2_aligned = bvh_motion2.raw_copy()
    num_joints = bvh_motion1.joint_rotation.shape[1]
    n2 = m2_aligned.joint_position.shape[0]
    
    # 1. Align Root heading (Y-axis rotation only)
    rot1 = R.from_quat(bvh_motion1.joint_rotation[mix_frame1, 0])
    rot2 = R.from_quat(bvh_motion2.joint_rotation[0, 0])
    
    vec1 = rot1.apply(np.array([0, 0, 1]))
    vec2 = rot2.apply(np.array([0, 0, 1]))
    angle1 = np.arctan2(vec1[0], vec1[2])
    angle2 = np.arctan2(vec2[0], vec2[2])
    align_rot = R.from_rotvec(np.array([0, angle1 - angle2, 0]))
    
    # 2. Align Root position (XZ plane translation)
    m2_pos = m2_aligned.joint_position[:, 0, :].copy()
    m2_pos_xz_rel = m2_pos - m2_pos[0]
    m2_pos_xz_rel[:, 1] = 0.0 # Keep original Y height
    
    rotated_xz_rel = align_rot.apply(m2_pos_xz_rel)
    
    m2_aligned.joint_position[:, 0, 0] = rotated_xz_rel[:, 0] + bvh_motion1.joint_position[mix_frame1, 0, 0]
    m2_aligned.joint_position[:, 0, 2] = rotated_xz_rel[:, 2] + bvh_motion1.joint_position[mix_frame1, 0, 2]
    
    m2_root_rot = R.from_quat(m2_aligned.joint_rotation[:, 0, :])
    m2_aligned.joint_rotation[:, 0, :] = (align_rot * m2_root_rot).as_quat()
    
    # 3. Create blended transition sequence
    blend_motion = bvh_motion1.sub_sequence(mix_frame1, mix_frame1 + mix_time)
    
    for i in range(mix_time):
        alpha = i / (mix_time - 1) if mix_time > 1 else 1.0
        idx1 = min(mix_frame1 + i, bvh_motion1.joint_position.shape[0] - 1)
        
        # Position interpolation
        pos1 = bvh_motion1.joint_position[idx1]
        pos2 = m2_aligned.joint_position[i]
        blend_motion.joint_position[i] = (1 - alpha) * pos1 + alpha * pos2
        
        # Rotation interpolation (Slerp)
        for j in range(num_joints):
            r1 = bvh_motion1.joint_rotation[idx1, j]
            r2 = m2_aligned.joint_rotation[i, j]
            slerp = Slerp([0, 1], R.from_quat(np.vstack([r1, r2])))
            blend_motion.joint_rotation[i, j] = slerp(alpha).as_quat()
            
    # 4. Concatenate sequences
    part1 = bvh_motion1.sub_sequence(0, mix_frame1)
    part3 = m2_aligned.sub_sequence(mix_time, n2)
    
    part1.append(blend_motion)
    part1.append(part3)
    res = part1
    ### Your code here

    return res