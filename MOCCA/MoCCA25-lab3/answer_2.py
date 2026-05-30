from typing import List, Tuple

import numpy as np
import scipy
import scipy.linalg
import utils
from answer_1 import forward_dynamics
from answer_3 import hinge_Jacobian
from joints import Joint
from scipy.linalg import block_diag


def create_joint(
    x: np.ndarray, R: np.ndarray, joint_pos: np.ndarray, bodyA: int, bodyB: int
) -> Joint:
    """根据输入信息创建关节定义

    给出一组刚体的位置、朝向，以及一组关节的信息，创建关节实例（参见 joints.py 中 Joint 类的定义）

    输入：
        x: (num_bodies, 3)      刚体（质心）的位置
        R: (num_bodies, 3, 3)   刚体的朝向，表示为一组矩阵
        joint_pos: (3)          关节的位置
        bodyA: int              关节连接的刚体A的index
        bodyB: int              关节连接的刚体B的index

    x,R,joint_pos 均为世界坐标系表示

    输出：
        一个 Joint 实例，包含关节的定义信息

    提示：
        * 你需要计算每个关节的 from_bodyA, from_bodyB，即关节在对应刚体局部坐标系下的位置
        * from_bodyA, from_bodyB 表示同一个关节在两个刚体局部坐标系下的位置
        * 通过 joint_def = Joint(bodyA, bodyB, from_bodyA, from_bodyB) 生成关节定义
    """

    ####### 你的回答 #######
    # 提示：需要计算每个关节的 from_bodyA, from_bodyB，即关节在对应刚体局部坐标系下的位置
    # 通过 joint_def = Joint(bodyA, bodyB, from_bodyA, from_bodyB) 生成关节定义

    # 下面的例子可以删除
    from_bodyA = np.einsum("ij,j->i", R[bodyA, :, :].T, joint_pos - x[bodyA, :])
    from_bodyB = np.einsum("ij,j->i", R[bodyB, :, :].T, joint_pos - x[bodyB, :])
    joint = Joint(bodyA, bodyB, from_bodyA, from_bodyB)

    #######################

    return joint


def ball_Jacobian(
    x: np.ndarray,
    R: np.ndarray,
    v: np.ndarray,
    w: np.ndarray,
    joints: List[Joint],
    h: float = 0.001,
) -> Tuple[np.ndarray, np.ndarray]:
    """计算关节约束的 Jacobian 矩阵

    给出一组刚体的位置、朝向，以及一组关节的定义（参见 joints.py 中 Joint 类的定义）

    计算每个关节的约束 Jacobian 矩阵

    输入：
        x: (num_bodies, 3)    刚体（质心）的位置
        R: (num_bodies, 3, 3) 刚体的朝向，表示为一组矩阵
        v: (num_bodies, 3)    刚体的（质心）线速度
        w: (num_bodies, 3)    刚体的角速度
        joints:               关节定义列表，包含 num_joints 个关节

    x,R,v,w 均为世界坐标系表示

    输出：
        J: (num_joints, 3, 12)  所有关节约束的Jacobian矩阵，对应速度向量 [bodyA_v, bodyA_w, bodyB_v, bodyB_w]
        rhs: (num_joints, 3)     关节约束的右端项，表示为一个修正项，用于修正仿真误差导致的关节错位

    提示：
        * 你需要计算每个关节的 Jacobian 矩阵，参考 ppt 中相关内容
        * 关节约束的速度表示为 J v = rhs，其中 rhs=0 或者是一个修正项，用于修正仿真误差导致的关节错位，参考 ppt 中关于 ERP 的内容
        * 你可能不需要用到所有的输入信息
    """

    num_bodies = x.shape[0]
    num_joints = len(joints)

    bodyA = [jnt.bodyA for jnt in joints]
    bodyB = [jnt.bodyB for jnt in joints]

    from_bodyA = np.array([jnt.from_bodyA for jnt in joints])
    from_bodyB = np.array([jnt.from_bodyB for jnt in joints])

    x_bodyA, x_bodyB = x[bodyA, :], x[bodyB, :]
    R_bodyA, R_bodyB = R[bodyA, :, :], R[bodyB, :, :]

    J = np.zeros((num_joints, 3, 12))
    rhs = np.zeros((num_joints, 3))

    if num_joints == 0:
        return J, rhs

    # Transform anchor points to world frame
    r_A_world = np.einsum("nij,nj->ni", R_bodyA, from_bodyA)
    r_B_world = np.einsum("nij,nj->ni", R_bodyB, from_bodyB)

    # Build Jacobian matrix J [v_A, w_A, v_B, w_B]
    J[:, :, 0:3] = np.eye(3)

    J[:, 0, 4] = r_A_world[:, 2]
    J[:, 0, 5] = -r_A_world[:, 1]
    J[:, 1, 3] = -r_A_world[:, 2]
    J[:, 1, 5] = r_A_world[:, 0]
    J[:, 2, 3] = r_A_world[:, 1]
    J[:, 2, 4] = -r_A_world[:, 0]

    J[:, :, 6:9] = -np.eye(3)

    J[:, 0, 10] = -r_B_world[:, 2]
    J[:, 0, 11] = r_B_world[:, 1]
    J[:, 1, 9] = r_B_world[:, 2]
    J[:, 1, 11] = -r_B_world[:, 0]
    J[:, 2, 9] = -r_B_world[:, 1]
    J[:, 2, 10] = r_B_world[:, 0]

    # Calculate position error for Baumgarte stabilization (ERP)
    p_A = x_bodyA + r_A_world
    p_B = x_bodyB + r_B_world

    erp_factor = 0.2
    rhs = -erp_factor / h * (p_A - p_B)

    return J, rhs


def forward_dynamics_with_constraints(
    m: np.ndarray,
    I: np.ndarray,
    inv_m: np.ndarray,
    inv_I: np.ndarray,
    x: np.ndarray,
    R: np.ndarray,
    v: np.ndarray,
    w: np.ndarray,
    f: np.ndarray,
    tau: np.ndarray,
    h: float,
    joints: List[Joint],
) -> Tuple[np.ndarray, np.ndarray]:
    """带约束的刚体前向运动学函数

    给出一组刚体的位置、朝向，以及当前速度、角速度，
    给出一组关节的定义，利用欧拉积分计算下一时刻刚体的位置和朝向

    输入：
        m: (num_bodies, )         刚体的质量
        I: (num_bodies, 3, 3)     刚体的转动惯量
        inv_m: (num_bodies, )     刚体的质量倒数
        inv_I: (num_bodies, 3, 3) 刚体的转动惯量倒数

        x: (num_bodies, 3)        刚体（质心）的位置
        R: (num_bodies, 3, 3)     刚体的朝向，表示为一组矩阵
        v: (num_bodies, 3)        刚体的（质心）线速度
        w: (num_bodies, 3)        刚体的角速度

        f: (num_bodies, 3)        施加在每个刚体质心上的力
        tau: (num_bodies, 3)      施加在每个刚体上的力矩
        h: ()                     时间步长

        joints:             关节定义列表，包含 num_joints 个关节

    所有量均为世界坐标系表示

    输出：
        v_next: (num_bodies, 3)        刚体下一时刻的速度
        w_next: (num_bodies, 3)        刚体下一时刻的角速度

    提示：
        * 使用带约束的 Newton-Euler 方程来求解下一时刻刚体的状态，参考ppt中相关内容
        * 输入包括了场景中所有的刚体，你可以使用 for 循环来处理每个刚体（不推荐），或者
          使用 numpy 的向量化操作来处理所有刚体
        * 你可能不需要用到所有的输入信息
        * 你需要使用 ball_Jacobian 函数来计算关节约束的 Jacobian 矩阵
    """

    num_bodies = x.shape[0]
    num_joints = len(joints)

    hinge_joints = [jnt for jnt in joints if jnt.hinge_axis is not None]
    num_hinge_joints = len(hinge_joints)

    v_next, w_next = v.copy(), w.copy()

    ####### 你的回答 #######
    # ------------------------------------------------------------------
    # 以下是一个示例框架，你可以在其基础上修改，也可以删除改为你自己的实现
    # 注意：这个框架的效率比较低，更高效的计算需要考虑质量矩阵、约束 Jacobian 矩阵的稀疏性

    # 运动方程的矩阵形式： 由牛顿欧拉方程推导而来，考虑加速度的近似： a = (v_next - v) / h
    #   M * [v_next, w_next] = M * [v, w] - h * [0, w x Iw] + h * [f, tau] + h * J^T * lambda
    # 可以得到
    #   [v_next, w_next] = ([v, w] + h * M^-1 * ([f, tau] - [0, w x Iw])) + h * M^-1 * J^T * lambda
    #                      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    #                    = >[v0, w0]< + h * M^-1 * J^T * lambda
    # 如果你完成了问题1中的 forward_dynamics函数，你会发现第一项 [v0, w0] 就是 forward_dynamics 函数的输出
    # 约束方程为：
    #   J * [v_next, w_next] = rhs
    # 带入上面的结果，并整理可以得到
    #   A lambda = b
    # 其中 A = h * J * M^-1 * J^T
    #      b = rhs - J * [v0, w0]
    # 为了数值稳定，我们可以对 A 进行修正： diag(A) += cfms
    # 进而求解得到
    #   lambda = A^-1 b
    # 然后带入上面的结果，得到 [v_next, w_next] = [v0, w0] + h * M^-1 * J^T * lambda
    # ------------------------------------------------------------------

    # ------------------------------------------------------------------
    # [1]. 构造所有刚体的质量矩阵，M = diag([m_1, I_1,... m_n, I_n])，及其逆矩阵
    # ------------------------------------------------------------------
    M = np.zeros((num_bodies * 6, num_bodies * 6))
    inv_M = np.zeros((num_bodies * 6, num_bodies * 6))
    # 在对角线上填充质量和转动惯量及其倒数
    # 注意转动惯量 I 及其逆 inv_I 需要随着旋转进行相应的变换
    # 你可以考虑使用 scipy.linalg.block_diag 来构造对角矩阵

    ##### 你的代码 #####
    inv_M_blocks = []
    for i in range(num_bodies):
        Rt = R[i].T
        inv_I_world = R[i] @ inv_I[i] @ Rt

        block = np.zeros((6, 6))
        block[0:3, 0:3] = np.eye(3) * inv_m[i]
        block[3:6, 3:6] = inv_I_world
        inv_M_blocks.append(block)

    inv_M = block_diag(*inv_M_blocks)
    ###################

    # ------------------------------------------------------------------
    # [2]. 计算无约束下刚体的速度和角速度 [v0, w0]
    # ------------------------------------------------------------------
    # 你可以使用 forward_dynamics 来帮你计算，也可以复制代码到这里，因为后面的计算可能需要用到一些中间变量

    ##### 你的代码 #####
    v0, w0 = v.copy(), w.copy()

    ####### 你的回答 #######
    v0, w0 = forward_dynamics(m, I, inv_m, inv_I, x, R, v, w, f, tau, h)

    # 转换成列向量 u0: (6N, 1)
    u0 = np.zeros((num_bodies * 6, 1))
    for i in range(num_bodies):
        u0[i * 6 : i * 6 + 3, 0] = v0[i]
        u0[i * 6 + 3 : i * 6 + 6, 0] = w0[i]

    ###################

    # ------------------------------------------------------------------
    # [3]. 构造所有约束的 Jacobian 矩阵 J 和右端项 rhs
    # ------------------------------------------------------------------
    constraint_dim = num_joints * 3 + num_hinge_joints * 2
    J = np.zeros((constraint_dim, num_bodies * 6))
    rhs_vec = np.zeros((constraint_dim, 1))

    # 计算 Ball 约束
    J_ball, rhs_ball = ball_Jacobian(x, R, v, w, joints, h)

    for i in range(num_joints):
        row_idx = i * 3
        idx_A = joints[i].bodyA
        idx_B = joints[i].bodyB

        # 只有当索引 >= 0 时才填充，-1 代表固定在世界上的点，不参与速度更新
        if idx_A >= 0:
            J[row_idx : row_idx + 3, idx_A * 6 : idx_A * 6 + 6] = J_ball[i, :, 0:6]

        if idx_B >= 0:
            J[row_idx : row_idx + 3, idx_B * 6 : idx_B * 6 + 6] = J_ball[i, :, 6:12]

        rhs_vec[row_idx : row_idx + 3, 0] = rhs_ball[i]

    # 填充 Hinge 约束 (同理)
    if num_hinge_joints > 0:
        J_hinge, rhs_hinge = hinge_Jacobian(x, R, v, w, hinge_joints)
        for i in range(num_hinge_joints):
            row_idx = num_joints * 3 + i * 2
            idx_A = hinge_joints[i].bodyA
            idx_B = hinge_joints[i].bodyB

            if idx_A >= 0:
                J[row_idx : row_idx + 2, idx_A * 6 : idx_A * 6 + 6] = J_hinge[i, :, 0:6]

            if idx_B >= 0:
                J[row_idx : row_idx + 2, idx_B * 6 : idx_B * 6 + 6] = J_hinge[
                    i, :, 6:12
                ]

            rhs_vec[row_idx : row_idx + 2, 0] = rhs_hinge[
                i
            ]  # ------------------------------------------------------------------
    # [4&5]. 构造约束方程的矩阵 A 和右端项 b
    # ------------------------------------------------------------------
    A = np.zeros((constraint_dim, constraint_dim))
    b = np.zeros((constraint_dim, 1))

    A = h * J @ inv_M @ J.T

    # b = rhs - J * u0
    b = rhs_vec - J @ u0

    A += np.eye(constraint_dim) * 1e-8
    lambd = np.linalg.solve(A, b)
    # ------------------------------------------------------------------
    # [6]. 计算约束对速度和角速度的影响，得到 [v_next, w_next] = [v0, w0] + h * M^-1 * J^T * lambda
    # ------------------------------------------------------------------

    ##### 你的代码 #####

    delta_u = h * inv_M @ J.T @ lambd
    u_next = u0 + delta_u

    v_next = np.zeros((num_bodies, 3))
    w_next = np.zeros((num_bodies, 3))
    for i in range(num_bodies):
        v_next[i] = u_next[i * 6 : i * 6 + 3, 0]
        w_next[i] = u_next[i * 6 + 3 : i * 6 + 6, 0]

    #######################

    return v_next, w_next
