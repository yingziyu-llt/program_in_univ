from typing import List, Tuple

import numpy as np
import utils
from joints import Joint


def hinge_Jacobian(
    x: np.ndarray, R: np.ndarray, v: np.ndarray, w: np.ndarray, joints: List
) -> Tuple[np.ndarray, np.ndarray]:
    num_bodies = x.shape[0]
    num_joints = len(joints)

    bodyA = [jnt.bodyA for jnt in joints]
    bodyB = [jnt.bodyB for jnt in joints]

    # 铰链轴通常定义在 bodyA 的局部坐标系下
    hinge_axes_local = np.array([jnt.hinge_axis for jnt in joints])

    R_bodyA = R[bodyA, :, :]
    R_bodyB = R[bodyB, :, :]

    J = np.zeros((num_joints, 2, 12))
    rhs = np.zeros((num_joints, 2))

    if num_joints == 0:
        return J, rhs

    # 1. 计算世界坐标系下的铰链轴 s
    s_world = np.einsum("nij,nj->ni", R_bodyA, hinge_axes_local)

    # 2. 为每个关节构建正交基 (s, t, b)
    for i in range(num_joints):
        s = s_world[i]

        # 寻找不平行的参考向量以构造起始正交向量 t
        if abs(s[0]) < 0.7:
            ref = np.array([1, 0, 0])
        else:
            ref = np.array([0, 1, 0])

        t = np.cross(s, ref)
        t /= np.linalg.norm(t)
        b = np.cross(s, t)  # 得到第三个正交轴

        # 3. 填充 Jacobian 矩阵
        # 约束 1: (w_A - w_B) · t = 0
        J[i, 0, 3:6] = t  # bodyA_w
        J[i, 0, 9:12] = -t  # bodyB_w

        # 约束 2: (w_A - w_B) · b = 0
        J[i, 1, 3:6] = b  # bodyA_w
        J[i, 1, 9:12] = -b  # bodyB_w

        # 4. 计算 rhs (Baumgarte Stabilization)
        # 这里的目标是让 bodyB 坐标系下的铰链轴与 bodyA 保持一致
        # 我们假设 jnt.hinge_axis 在两个 body 的局部空间定义是相同的
        s_B_world = R_bodyB[i] @ hinge_axes_local[i]

        # 旋转偏差可以用叉乘表示：error = s_A_world x s_B_world
        # 我们需要修正的是在 t 和 b 方向上的偏移
        error = np.cross(s, s_B_world)

        erp = 0.2
        rhs[i, 0] = erp * np.dot(error, t)
        rhs[i, 1] = erp * np.dot(error, b)

    return J, rhs
