# MoCCA Lab4 实验报告

姓名：林乐天
学号：2300012154

## 1. 四元数处理

我在文件开头补了几个四元数函数：乘法、求逆、归一化、旋转向量、以及把四元数误差转成旋转向量。后面三部分都用到了这些函数。

需要特别注意的是四元数的顺序。代码里使用的是 `[x, y, z, w]`，所以乘法和求逆都按这个顺序写。误差四元数转旋转向量时，我把 `w < 0` 的四元数整体取反，避免同一个旋转走到较长的那条弧上。角度很小时用近似比例，避免除以接近 0 的数。

## 2. Part 1：PD 控制

`part1_cal_torque` 负责根据目标 pose 给每个关节算力矩。对第 0 个根节点以外的关节，我先取父 body 和子 body 的世界朝向：

```text
q_parent, q_child
```

当前局部旋转为：

```text
q_current_local = q_parent^{-1} * q_child
```

目标局部旋转直接来自 BVH pose。旋转误差写成：

```text
q_error = q_target_local * q_current_local^{-1}
```

把 `q_error` 转成旋转向量后，就可以放进 PD 公式。角速度部分也要用相对角速度，不能直接拿子 body 的角速度：

```text
omega_local = q_parent^{-1} * (omega_child - omega_parent)
tau_local = kp * angle_error - kd * omega_local
```

我最后使用的默认参数是：

```text
kp = 400
kd = 40
torque_limit = 400
```

算完局部力矩后，用父关节朝向把它转回世界坐标。根节点力矩按要求置零。我一开始没有单独处理手腕，运行后发现左右手腕会过度扭曲，所以把 `rWrist` 和 `lWrist` 的力矩乘以 `0.1`，用来压住末端关节的异常旋转。

这一部分的现象是：角色能跟着 BVH 做出关节动作，但跑步时因为没有根节点控制，身体会倒下。四肢动作还在继续，说明姿态跟踪本身是起作用的。

## 3. Part 2：根节点辅助力

`part2_cal_float_base_torque` 在 Part 1 的关节力矩外，又算了根节点的辅助力和辅助力矩。辅助力只负责把 root 拉回目标位置附近：

```text
F_root = root_kp * (x_target - x_current) - root_kd * v_root
```

参数如下：

```text
root_kp = 4000
root_kd = 20
root_height_offset = 0.05
```

我把目标高度稍微抬高了 `0.05`，否则竖直支撑力不太够。力的大小裁剪到 `[-800, 800]`。在 `task1_standing.py` 的调用里，最后只保留 y 方向的力，所以角色向前的运动不是靠直接推 root，而是靠脚和地面的接触。

root 的朝向控制单独算一个旋转 PD：

```text
tau_root = root_rot_kp * angle_error - root_rot_kd * omega_root
```

参数为：

```text
root_rot_kp = 400
root_rot_kd = 40
```

这部分加上后，root 有了竖直方向的支撑和朝向控制，跑步时可以维持不倒下。

## 4. Part 3：静态平衡

第三部分不能再给 root 直接加辅助外力。这里的外力只指环境给角色的随机水平扰动，是用来测试平衡能力的；控制器本身只返回关节力矩。我保留第 0 帧站立姿态作为目标，然后在腿部关节上补一项由质心反馈得到的力矩。

基础姿态跟踪仍然调用 Part 1，只是参数调小：

```text
kp = 350
kd = 35
torque_limit = 350
wrist_torque_scale = 0.0
```

这样站立时不会因为上肢末端动作造成多余抖动。

接着用脚踝和脚尖的位置估计支撑中心。目标 root 的水平位置不直接用 BVH 原始位置，而是放到支撑中心附近。质心和质心速度按质量加权计算：

```text
com = sum(m_i * x_i) / sum(m_i)
v_com = sum(m_i * v_i) / sum(m_i)
```

水平虚拟力由三项组成：

```text
F_virtual =
    K_com * (x_target - com)
  - D_com * v_com
  - K_tilt * tilt_error
```

其中 `tilt_error` 取 root 朝上方向在水平面的偏移。`F_virtual` 只保留 xz 平面分量，范围限制在 `[-160, 160]`，不让腿部力矩突然变得太大。

最后把这个虚拟力通过 Jacobian Transpose 的近似分配到左右腿：

```text
tau = (com - joint_pos) x F
```

我把左右腿各分一半，作用在 `Hip`、`Knee`、`Ankle` 上。脚尖关节力矩置零，下肢力矩裁剪到 `[-700, 700]`。这样处理后，角色受到环境的水平随机外力时，会用腿部关节做一些补偿，而不是把控制力直接加到 root 上。

运行视频见附件.