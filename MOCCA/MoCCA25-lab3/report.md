# MoCCA 2025 Lab 3 实验报告


## 1. 实验概述

本次实验从零实现一个基础的铰接刚体仿真器，并在该仿真器上完成机械臂的姿态控制和末端点轨迹跟踪。代码主体由 `problem_1.py` 到 `problem_6.py` 给出实验场景和可视化流程，实际需要补全的核心算法位于 `answer_1.py` 到 `answer_6.py` 中。

实验分为两个部分。第一部分实现刚体动力学和关节约束，包括自由刚体仿真、球窝关节约束和 hinge 关节约束。第二部分在三连杆机械臂模型上实现控制算法，包括重力补偿、关节 PD 控制和末端点轨迹跟踪控制。

实验使用 `numpy` 进行矩阵计算，使用 `scipy.spatial.transform.Rotation` 处理三维旋转，使用 `pyvista` 进行仿真结果可视化。

## 2. 自由刚体仿真

### 2.1 位置和朝向积分

在 `answer_1.py` 的 `integration` 函数中，我使用显式欧拉方法更新刚体质心位置：

```text
x_next = x + h v
```

对于旋转部分，角速度 `w` 在一个时间步 `h` 内对应旋转向量 `w h`。代码中通过 `Rotation.from_rotvec(w * h).as_matrix()` 将该增量转换为旋转矩阵，再左乘当前朝向矩阵：

```text
R_next = Delta_R R
```

旋转更新后再通过 `Rotation.from_matrix(...).as_matrix()` 重新生成旋转矩阵，用于减小数值积分导致的非正交误差，避免刚体在可视化中出现拉伸或形变。

### 2.2 牛顿-欧拉前向动力学

在 `forward_dynamics` 中，线速度按牛顿第二定律更新：

```text
v_next = v + h inv_m f
```

角速度使用 Newton-Euler 方程：

```text
I alpha + w x (I w) = tau
```

由于转动惯量 `I` 在刚体局部坐标系下给出，而角速度和力矩均使用世界坐标系表示，因此需要先将惯量矩阵变换到世界坐标系：

```text
I_world = R I R^T
inv_I_world = R inv_I R^T
```

随后计算 gyroscopic 项 `w x Iw`，得到角加速度：

```text
alpha = inv_I_world (tau - w x Iw)
w_next = w + h alpha
```

`problem_1.py` 中三个独立刚体在初速度、角速度和外力矩作用下运动。实现后，右侧刚体会在周期性力和力矩下产生平动与转动，中间未受力矩刚体的角动量在 debug 输出中基本保持守恒。

## 3. 铰接刚体约束仿真

### 3.1 关节局部锚点

在 `answer_2.py` 的 `create_joint` 中，需要将世界坐标系下的关节位置转换到两个刚体各自的局部坐标系。对于刚体 A 和 B：

```text
from_bodyA = R_A^T (joint_pos - x_A)
from_bodyB = R_B^T (joint_pos - x_B)
```

这样在仿真过程中，只要使用当前刚体朝向重新变换这些局部锚点，就能得到关节在世界坐标系下的位置。

### 3.2 球窝关节 Jacobian

球窝关节要求两个刚体在关节处的位置速度相同：

```text
v_A + w_A x r_A = v_B + w_B x r_B
```

整理后可写为：

```text
[I, -[r_A]x, -I, [r_B]x] [v_A, w_A, v_B, w_B]^T = rhs
```

因此 `ball_Jacobian` 为每个关节构造一个 `3 x 12` 的约束 Jacobian，其中前三维约束关节位置重合。为了缓解数值积分造成的关节错位，我加入 Baumgarte/ERP 修正项：

```text
rhs = -erp / h * (p_A - p_B)
```

其中 `p_A` 和 `p_B` 是从两个刚体分别计算出的关节世界坐标位置。该项会驱动约束误差在后续时间步中逐渐减小。

### 3.3 带约束动力学求解

`forward_dynamics_with_constraints` 是本实验的核心。设所有刚体的速度向量为：

```text
u = [v_1, w_1, v_2, w_2, ...]^T
```

先调用自由刚体动力学得到无约束速度 `u0`，再通过约束冲量修正速度：

```text
u_next = u0 + h M^-1 J^T lambda
```

同时要求约束满足：

```text
J u_next = rhs
```

代入后得到线性方程：

```text
(h J M^-1 J^T) lambda = rhs - J u0
```

代码中构造每个刚体的 `6 x 6` 逆质量块，对平动部分填入 `inv_m I_3`，对转动部分填入世界坐标系下的 `inv_I_world`，再用 `scipy.linalg.block_diag` 组成整体 `inv_M`。最后求解 `lambda` 并更新所有刚体速度。

为了提高数值稳定性，求解前在矩阵 `A` 的对角线上加入了很小的 CFM 项 `1e-8`。


## 4. Hinge 关节约束

在 `answer_3.py` 的 `hinge_Jacobian` 中，hinge 关节在球窝位置约束基础上额外限制相对角速度，使两个刚体只能绕指定 hinge 轴相对旋转。

具体做法是先将定义在 bodyA 局部坐标系下的 hinge 轴变换到世界坐标系：

```text
s = R_A hinge_axis
```

然后构造与 `s` 正交的两个单位向量 `t` 和 `b`。hinge 约束要求相对角速度在这两个方向上的分量为零：

```text
(w_A - w_B) dot t = 0
(w_A - w_B) dot b = 0
```

因此每个 hinge 关节额外产生一个 `2 x 12` 的 Jacobian，只作用在两个刚体的角速度列上。右端项中使用 bodyA 与 bodyB hinge 轴方向的叉乘误差进行简单稳定化，使两个刚体的 hinge 轴保持对齐。


## 5. 机械臂重力补偿

在 `answer_4.py` 的 `gravity_compensation` 中，我将三摆系统视作一条父子结构明确的机械臂。每个关节需要承担其子树中所有刚体的重力力矩。

代码首先根据 `bodyA -> bodyB` 建立 children 列表。对于每个关节，计算关节世界坐标位置 `joint_pos`，随后遍历该关节子刚体及所有后代刚体。每个后代刚体的重力为：

```text
F_i = m_i g
```

其对当前关节的力矩为：

```text
tau_i = (x_i - joint_pos) x F_i
```

为了抵消重力造成的旋转趋势，关节补偿力矩取该力矩的相反方向并累加到当前关节。越靠近根部的关节需要支撑越多后代刚体，因此补偿力矩也更大。

在 `problem_4.py` 中，每隔一段时间会随机生成机械臂姿态。加入重力补偿后，机械臂能够在新姿态下基本保持静止。


## 6. 关节 PD 姿态控制

在 `answer_5.py` 的 `PD_control` 中，我根据当前关节相对旋转和目标关节旋转计算控制力矩。

当前关节相对旋转为：

```text
R_current = R_A^T R_B
```

目标旋转由 `problem_5.py` 传入。旋转误差使用：

```text
R_error = R_target R_current^T
```

再将其转换为旋转向量，作为三维旋转误差。由于该误差位于 bodyA 局部坐标系下，代码中使用 `R_A` 将其转换到世界坐标系。关节角速度误差使用两个刚体的相对角速度：

```text
qdot = w_B - w_A
```

最终控制律为：

```text
tau = kp error - kd qdot
```

该控制力矩经 `joint_torque_converter` 转换为施加在父子刚体上的等大反向力矩。仿真结果中，机械臂会向目标姿态回正，并在目标附近产生一定摆动。

## 7. 末端点轨迹跟踪

在 `answer_6.py` 的 `end_effector_track_control` 中，我使用重力补偿加 Jacobian transpose 思路实现末端点控制。

首先计算当前末端点位置和速度：

```text
p_e = x_body + R_body r_e
v_e = v_body + w_body x (R_body r_e)
```

随后根据目标位置和目标速度构造末端虚拟力：

```text
F_e = kp (p_target - p_e) + kd (v_target - v_e)
```

代码中使用 `kp = 500`、`kd = 50`。该虚拟力通过关节到末端点的力臂转换为关节力矩：

```text
tau_j = (p_e - p_j) x F_e
```

只有位于末端执行器祖先链上的关节才会受到该末端虚拟力的影响。最后再叠加 `gravity_compensation` 返回的重力补偿力矩，使机械臂在跟踪轨迹时同时抵消重力。

`problem_6.py` 中目标轨迹是位于倾斜平面内的圆形轨迹。实现后，机械臂末端能够围绕红色目标轨迹运动，并持续跟踪目标点。

## 8. 实验结果与验证

本实验的六个问题分别对应以下运行命令：

```bash
python problem_1.py
python problem_2.py
python problem_3.py
python problem_4.py
python problem_5.py
python problem_6.py
```

其中问题 1、2、3 可以使用 `--debug-mode` 检查无外力矩或无外力系统中的角动量守恒情况：

```bash
python problem_1.py --debug-mode
python problem_2.py --debug-mode
python problem_3.py --debug-mode
```
具体运行试验结果见压缩包中的xxx.mp4文件,均为按照要求录屏的结果.由于 wm 是平铺的,故录屏都是细长条,但不影响结果的观察.
