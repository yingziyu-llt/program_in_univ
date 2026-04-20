##############
# 姓名：林乐天
# 学号：2300012154
##############
import numpy as np
from scipy.spatial.transform import Rotation as R

def load_motion_data(bvh_file_path):
    """part2 辅助函数，读取bvh文件"""
    with open(bvh_file_path, 'r') as f:
        lines = f.readlines()
        for i in range(len(lines)):
            if lines[i].startswith('Frame Time'):
                break
        motion_data = []
        for line in lines[i+1:]:
            data = [float(x) for x in line.split()]
            if len(data) == 0:
                break
            motion_data.append(np.array(data).reshape(1,-1))
        motion_data = np.concatenate(motion_data, axis=0)
    return motion_data



def part1_calculate_T_pose(bvh_file_path):
    """请填写以下内容
    输入： bvh 文件路径
    输出:
        joint_name: List[str]，字符串列表，包含着所有关节的名字
        joint_parent: List[int]，整数列表，包含着所有关节的父关节的索引,根节点的父关节索引为-1
        joint_offset: np.ndarray，形状为(M, 3)的numpy数组，包含着所有关节的偏移量

    Tips:
        joint_name顺序应该和bvh一致
    """
    joint_name = []
    joint_parent = []
    joint_offset = []

    stack = []

    with open(bvh_file_path, 'r') as f:
        lines = f.readlines()
    
    for line in lines:
        line = line.strip()
        tokens = line.split()

        if tokens[0] == 'ROOT' or tokens[0] == 'JOINT':
            joint_name.append(tokens[1])
            if len(stack) == 0:
                joint_parent.append(-1)
            else:
                joint_parent.append(stack[-1])
        elif tokens[0] == '{':
            stack.append(len(joint_name) - 1)
        elif tokens[0] == '}':
            stack.pop()
        elif tokens[0] == 'OFFSET':
            offset = [float(x) for x in tokens[1:4]]
            joint_offset.append(offset)
        elif tokens[0] == 'End':
            joint_name.append(joint_name[-1] + '_end')
            joint_parent.append(stack[-1])
        elif tokens[0] == 'MOTION':
            break
        
    return joint_name, joint_parent, joint_offset


def part2_forward_kinematics(joint_name, joint_parent, joint_offset, motion_data, frame_id):
    num_joints = len(joint_name)
    joint_positions = np.zeros((num_joints, 3))
    joint_orientations = np.zeros((num_joints, 4))

    current_motion = motion_data[frame_id]
    channel_index = 0

    for i in range(num_joints):
        if joint_parent[i] == -1:
            # 根节点
            joint_positions[i] = current_motion[channel_index:channel_index+3] + joint_offset[i]
            joint_orientations[i] = R.from_euler('XYZ', current_motion[channel_index+3:channel_index+6], degrees=True).as_quat()
            channel_index += 6
        else:
            parent_position = joint_positions[joint_parent[i]]
            parent_orientation = joint_orientations[joint_parent[i]]

            if 'end' in joint_name[i]:
                joint_positions[i] = parent_position + R.from_quat(parent_orientation).apply(joint_offset[i])
                joint_orientations[i] = parent_orientation
            else:
                local_rotation = R.from_euler('XYZ', current_motion[channel_index:channel_index+3], degrees=True)
                channel_index += 3
                global_rotation = R.from_quat(parent_orientation) * local_rotation
                joint_orientations[i] = global_rotation.as_quat()
                joint_positions[i] = parent_position + R.from_quat(parent_orientation).apply(joint_offset[i])

    return joint_positions, joint_orientations

def part3_retarget_func(T_pose_bvh_path, A_pose_bvh_path):
    """
    将 A-pose的bvh重定向到T-pose上
    输入: 两个bvh文件的路径
    输出: 
        motion_data: np.ndarray，形状为(N,X)的numpy数组，其中N为帧数，X为Channel数。retarget后的运动数据
    
    Tips:
        两个bvh的joint name顺序可能不一致哦(
        as_euler时也需要大写的XYZ
    """
    T_name, T_parent, T_offset = part1_calculate_T_pose(T_pose_bvh_path)
    A_name, A_parent, A_offset = part1_calculate_T_pose(A_pose_bvh_path)
    A_motion_data = load_motion_data(A_pose_bvh_path)

    A_name_to_idx = {name: i for i, name in enumerate(A_name)}
    A_channel_map = {}
    curr_ch = 0
    for i, name in enumerate(A_name):
        if 'end' in name:
            A_channel_map[name] = -1
        else:
            A_channel_map[name] = curr_ch
            curr_ch += (6 if A_parent[i] == -1 else 3)
    T_total_channels = 0
    for i in range(len(T_name)):
        if 'end' not in T_name[i]:
            T_total_channels += (6 if T_parent[i] == -1 else 3)
    
    num_frames = A_motion_data.shape[0]
    motion_data = np.zeros((num_frames, T_total_channels))

    joint_corrections = []
    for i in range(len(T_name)):
        if T_parent[i] == -1 or 'end' in T_name[i]:
            joint_corrections.append(R.identity())
            continue
        
        a_idx = A_name_to_idx[T_name[i]]
        v_t = T_offset[i]
        v_a = A_offset[a_idx]
        
        norm_t = np.linalg.norm(v_t)
        norm_a = np.linalg.norm(v_a)
        
        if norm_t < 1e-6 or norm_a < 1e-6:
            joint_corrections.append(R.identity())
        else:
            u_t = v_t / norm_t
            u_a = v_a / norm_a
            dot = np.dot(u_t, u_a)
            
            if dot > 0.999999:
                joint_corrections.append(R.identity())
            elif dot < -0.999999: 
                axis = [0, 1, 0] if abs(u_t[0]) > 0.9 else [1, 0, 0]
                ortho = np.cross(u_t, axis)
                joint_corrections.append(R.from_rotvec(ortho / np.linalg.norm(ortho) * np.pi))
            else:
                axis = np.cross(u_t, u_a)
                angle = np.arccos(dot)
                joint_corrections.append(R.from_rotvec(axis / np.linalg.norm(axis) * angle))

    curr_ch_T = 0
    for i, name in enumerate(T_name):
        if 'end' in name:
            continue
        
        idx_A = A_channel_map[name]
        
        if T_parent[i] == -1:
            motion_data[:, curr_ch_T : curr_ch_T+6] = A_motion_data[:, idx_A : idx_A+6]
            curr_ch_T += 6
        else:
            r_corr = joint_corrections[i]
            for f in range(num_frames):
                euler_A = A_motion_data[f, idx_A : idx_A+3]
                r_a = R.from_euler('XYZ', euler_A, degrees=True)
                r_t = r_corr * r_a
                motion_data[f, curr_ch_T : curr_ch_T+3] = r_t.as_euler('XYZ', degrees=True)
            curr_ch_T += 3
            
    return motion_data