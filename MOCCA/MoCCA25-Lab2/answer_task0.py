##############
# 姓名： 林乐天
# 学号： 2300012154
##############
import numpy as np

# part 0
def load_meta_data(bvh_file_path):
    """
    请把lab1-FK-part1的代码复制过来
    请填写以下内容
    输入： bvh 文件路径
    输出:
        joint_name: List[str]，字符串列表，包含着所有关节的名字
        joint_parent: List[int]，整数列表，包含着所有关节的父关节的索引,根节点的父关节索引为-1
        channels: List[int]，整数列表，joint的自由度，根节点为6(三个平动三个转动)，其余节点为3(三个转动)
        joint_offset: np.ndarray，形状为(M, 3)的numpy数组，包含着所有关节的偏移量
    Tips:
        joint_name顺序应该和bvh一致
    """
    

    joint_name = []
    joint_parent = []
    joint_offset = []
    channels = []

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
        elif tokens[0] == 'CHANNELS':
            channels.append(int(tokens[1]))
        elif tokens[0] == '}':
            stack.pop()
        elif tokens[0] == 'OFFSET':
            offset = [float(x) for x in tokens[1:4]]
            joint_offset.append(offset)
        elif tokens[0] == 'End':
            joint_name.append(joint_name[-1] + '_end')
            channels.append(0)
            joint_parent.append(stack[-1])
        elif tokens[0] == 'MOTION':
            break
    joint_offset = np.array(joint_offset)
        
    return joint_name, joint_parent, channels, joint_offset

