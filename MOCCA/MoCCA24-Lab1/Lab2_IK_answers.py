##############
# 姓名：林乐天
# 学号：2300012154
##############
import numpy as np
from scipy.spatial.transform import Rotation as R

def dist(a, b):
    return np.linalg.norm(a - b)

def part1_inverse_kinematics(meta_data, input_joint_positions, input_joint_orientations, target_pose):
    """
    完成函数，计算逆运动学
    输入: 
        meta_data: 为了方便，将一些固定信息进行了打包，见上面的meta_data类
        joint_positions: 当前的关节位置，是一个numpy数组，shape为(M, 3)，M为关节数
        joint_orientations: 当前的关节朝向，是一个numpy数组，shape为(M, 4)，M为关节数
        target_pose: 目标位置，是一个numpy数组，shape为(3,)
    输出:
        经过IK后的姿态
        joint_positions: 计算得到的关节位置，是一个numpy数组，shape为(M, 3)，M为关节数
        joint_orientations: 计算得到的关节朝向，是一个numpy数组，shape为(M, 4)，M为关节数
    """    
    joint_positions = input_joint_positions.copy()
    joint_orientations = input_joint_orientations.copy()
    num_joints = len(joint_positions)
    
    local_rotations = [R.identity() for _ in range(num_joints)]
    for i in range(num_joints):
        parent_idx = meta_data.joint_parent[i]
        if parent_idx == -1:
            local_rotations[i] = R.from_quat(joint_orientations[i])
        else:
            p_ori_inv = R.from_quat(joint_orientations[parent_idx]).inv()
            c_ori = R.from_quat(joint_orientations[i])
            local_rotations[i] = p_ori_inv * c_ori

    path, _, _, _ = meta_data.get_path_from_root_to_end()
    end_joint_idx = path[-1]
    
    for _ in range(50):
        if np.linalg.norm(joint_positions[end_joint_idx] - target_pose) < 1e-3:
            break
            
        for i in range(len(path) - 2, -1, -1):
            cur_idx = path[i]
            v_end = joint_positions[end_joint_idx] - joint_positions[cur_idx]
            v_target = target_pose - joint_positions[cur_idx]
            
            norm_e, norm_t = np.linalg.norm(v_end), np.linalg.norm(v_target)
            if norm_e < 1e-7 or norm_t < 1e-7: continue
            
            axis = np.cross(v_end / norm_e, v_target / norm_t)
            if np.linalg.norm(axis) < 1e-7: continue
            
            angle = np.arccos(np.clip(np.dot(v_end / norm_e, v_target / norm_t), -1.0, 1.0))
            delta_rot = R.from_rotvec(axis / np.linalg.norm(axis) * angle)
            
            for j in range(i, len(path)):
                idx = path[j]
                joint_orientations[idx] = (delta_rot * R.from_quat(joint_orientations[idx])).as_quat()
                if idx != cur_idx:
                    offset = joint_positions[idx] - joint_positions[cur_idx]
                    joint_positions[idx] = joint_positions[cur_idx] + delta_rot.apply(offset)

    for i in path:
        parent_idx = meta_data.joint_parent[i]
        if parent_idx != -1:
            p_ori_inv = R.from_quat(joint_orientations[parent_idx]).inv()
            c_ori = R.from_quat(joint_orientations[i])
            local_rotations[i] = p_ori_inv * c_ori
        else:
            local_rotations[i] = R.from_quat(joint_orientations[i])

    for i in range(num_joints):
        p_idx = meta_data.joint_parent[i]
        if p_idx == -1:
            continue 
        else:
            parent_ori = R.from_quat(joint_orientations[p_idx])
            joint_orientations[i] = (parent_ori * local_rotations[i]).as_quat()
            
            bone_offset = meta_data.joint_initial_position[i] - meta_data.joint_initial_position[p_idx]
            joint_positions[i] = joint_positions[p_idx] + parent_ori.apply(bone_offset)

    return joint_positions, joint_orientations

def part2_inverse_kinematics(meta_data, joint_positions, joint_orientations, relative_x, relative_z, target_height):
    # 1. 基础信息准备
    root_pos = joint_positions[0]
    final_target = np.array([root_pos[0] + relative_x, target_height, root_pos[2] + relative_z])
    
    end_joint_idx = meta_data.joint_name.index(meta_data.end_joint)
    palm_idx = meta_data.joint_parent[end_joint_idx]
    
    # 2. 预定义手掌目标旋转 (竖直向上)
    target_up_ori = R.from_euler('XYZ', [0, 0, 90], degrees=True) 
    
    # 3. 【核心改进】计算手腕应该去的位置
    # 获取手腕到手指末端的 T-pose 偏移向量
    bone_offset = meta_data.joint_initial_position[end_joint_idx] - meta_data.joint_initial_position[palm_idx]
    # 手腕的目标位置 = 最终目标 - 预设旋转作用后的偏移
    wrist_target_pos = final_target - target_up_ori.apply(bone_offset)

    # 4. 对“手腕”运行 IK (修改 meta_data 的 end_joint 逻辑)
    # 注意：这里需要你的 part1 能够支持指定不同的 end_joint，或者临时修改 meta_data
    original_end_joint = meta_data.end_joint
    meta_data.end_joint = meta_data.joint_name[palm_idx] # 临时把末端设为手腕
    
    joint_positions, joint_orientations = part1_inverse_kinematics(
        meta_data, joint_positions, joint_orientations, wrist_target_pos
    )
    
    # 还原 meta_data
    meta_data.end_joint = original_end_joint

    # 5. 强制设定手腕旋转
    joint_orientations[palm_idx] = target_up_ori.as_quat()

    # 6. 全局 FK 刷新 (Healing Pass)
    num_joints = len(joint_positions)
    for i in range(num_joints):
        p_idx = meta_data.joint_parent[i]
        if p_idx == -1: continue
        
        # 重新计算所有关节位置，确保手腕转动后，手指末端正好落在 final_target 上
        b_offset = meta_data.joint_initial_position[i] - meta_data.joint_initial_position[p_idx]
        parent_global_ori = R.from_quat(joint_orientations[p_idx])
        joint_positions[i] = joint_positions[p_idx] + parent_global_ori.apply(b_offset)

    return joint_positions, joint_orientations