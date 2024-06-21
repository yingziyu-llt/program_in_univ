import gym

env = gym.make("FrozenLake-v1")  # 创建环境
env.reset()
for t in range(100):
    env.render()  # 渲染画面
    a = env.action_space.sample()  # 随机采样动作
    observation, reward, done, truncated, _ = env.step(
        a
    )  # 环境执行动作，获得转移后的状态、奖励以及环境是否终止的指示
    if done:
        break
env.render()
