"""
数据预处理，对于每一局数据，建四个FeatureAgent（作为四个玩家视角），将对局中的事件传给相应的agent，整理出每个人每个决策点下的特征表示以及实际选择的动作，保存到data文件夹下。

不需要阅读，用就行
"""

from feature import FeatureAgent
import numpy as np
import json
import os


def filterData():
    global obs
    global actions
    newobs = [[] for i in range(4)]
    newactions = [[] for i in range(4)]
    for i in range(4):
        for j, o in enumerate(obs[i]):
            if (
                o["action_mask"].sum() > 1
            ):  # ignore states with single valid action (Pass)
                newobs[i].append(o)
                newactions[i].append(actions[i][j])
    obs = newobs
    actions = newactions


def saveData():
    assert [len(x) for x in obs] == [
        len(x) for x in actions
    ], "obs actions not matching!"
    l.append(sum([len(x) for x in obs]))
    np.savez(
        "data/%d.npz" % matchid,
        obs=np.stack([x["observation"] for i in range(4) for x in obs[i]]).astype(
            np.int8
        ),
        mask=np.stack([x["action_mask"] for i in range(4) for x in obs[i]]).astype(
            np.int8
        ),
        act=np.array([x for i in range(4) for x in actions[i]]),
    )
    for x in obs:
        x.clear()
    for x in actions:
        x.clear()


def process_data(file_path, start_line, end_line, offset, cpu_id):
    global obs, actions, matchid, l
    obs = [[] for i in range(4)]
    actions = [[] for i in range(4)]
    matchid = offset - 1
    l = []

    with open("data/data.txt", encoding="UTF-8") as f:
        for _ in range(start_line):
            f.readline()  # Skip lines until start_line
        line = f.readline()
        line_number = start_line
        # print(start_line)
        if end_line is None:
            end_line = 1e9
        # print(line)
        while line and line_number <= end_line:
            t = line.split()
            if len(t) == 0:
                line = f.readline()
                line_number += 1
                continue
            if t[0] == "Match":
                matchid += 1
                # 检查是否已经存在 data/%d.npz 文件，如果存在就跳过
                # is_skip = False
                # skip_begin_line_number = line_number
                # while os.path.exists(f"data/{matchid}.npz"):
                #     is_skip = True
                #     try:
                #         data = np.load(f"data/{matchid}.npz")
                #         if data["obs"].shape[0] == 0:
                #             raise
                #         l.append(data["obs"].shape[0])
                #     except Exception:
                #         print(f"Detect corrupted file in {matchid}, retry")
                #         break
                #     # 更新 l
                #     # sum([len(x) for x in obs]))
                #     # 跳过当前局的所有行
                #     line = f.readline()
                #     while not line.startswith("Match") and line:
                #         line = f.readline()
                #         # 如果 EOF 了，就退出
                #         line_number += 1
                #     matchid += 1
                #     if cpu_id == 23 and matchid % 100 == 0:
                #         print(
                #             f"Skip {matchid}.npz, {skip_begin_line_number} - {line_number}"
                #         )
                # # if is_skip and cpu_id == 23:
                # #     print(
                # #         f"Skip {matchid}.npz, {skip_begin_line_number} - {line_number}"
                # #     )
                # if cpu_id == 23 and matchid % 1000 == 0:
                #     print(f"Processing {matchid}.npz")
                agents = [FeatureAgent(i) for i in range(4)]
            elif t[0] == "Wind":
                for agent in agents:
                    agent.request2obs(line)
            elif t[0] == "Player":
                p = int(t[1])
                if t[2] == "Deal":
                    agents[p].request2obs(" ".join(t[2:]))
                elif t[2] == "Draw":
                    for i in range(4):
                        if i == p:
                            obs[p].append(agents[p].request2obs(" ".join(t[2:])))
                            actions[p].append(0)
                        else:
                            agents[i].request2obs(" ".join(t[:3]))
                elif t[2] == "Play":
                    actions[p].pop()
                    actions[p].append(agents[p].response2action(" ".join(t[2:])))
                    for i in range(4):
                        if i == p:
                            agents[p].request2obs(line)
                        else:
                            obs[i].append(agents[i].request2obs(line))
                            actions[i].append(0)
                    curTile = t[3]
                elif t[2] == "Chi":
                    actions[p].pop()
                    actions[p].append(
                        agents[p].response2action("Chi %s %s" % (curTile, t[3]))
                    )
                    for i in range(4):
                        if i == p:
                            obs[p].append(
                                agents[p].request2obs("Player %d Chi %s" % (p, t[3]))
                            )
                            actions[p].append(0)
                        else:
                            agents[i].request2obs("Player %d Chi %s" % (p, t[3]))
                elif t[2] == "Peng":
                    actions[p].pop()
                    actions[p].append(agents[p].response2action("Peng %s" % t[3]))
                    for i in range(4):
                        if i == p:
                            obs[p].append(
                                agents[p].request2obs("Player %d Peng %s" % (p, t[3]))
                            )
                            actions[p].append(0)
                        else:
                            agents[i].request2obs("Player %d Peng %s" % (p, t[3]))
                elif t[2] == "Gang":
                    actions[p].pop()
                    actions[p].append(agents[p].response2action("Gang %s" % t[3]))
                    for i in range(4):
                        agents[i].request2obs("Player %d Gang %s" % (p, t[3]))
                elif t[2] == "AnGang":
                    actions[p].pop()
                    actions[p].append(agents[p].response2action("AnGang %s" % t[3]))
                    for i in range(4):
                        if i == p:
                            agents[p].request2obs("Player %d AnGang %s" % (p, t[3]))
                        else:
                            agents[i].request2obs("Player %d AnGang" % p)
                elif t[2] == "BuGang":
                    actions[p].pop()
                    actions[p].append(agents[p].response2action("BuGang %s" % t[3]))
                    for i in range(4):
                        if i == p:
                            agents[p].request2obs("Player %d BuGang %s" % (p, t[3]))
                        else:
                            obs[i].append(
                                agents[i].request2obs("Player %d BuGang %s" % (p, t[3]))
                            )
                            actions[i].append(0)
                elif t[2] == "Hu":
                    actions[p].pop()
                    actions[p].append(agents[p].response2action("Hu"))
                # Deal with Ignore clause
                if t[2] in ["Peng", "Gang", "Hu"]:
                    for k in range(5, 15, 5):
                        if len(t) > k:
                            p = int(t[k + 1])
                            if t[k + 2] == "Chi":
                                actions[p].pop()
                                actions[p].append(
                                    agents[p].response2action(
                                        "Chi %s %s" % (curTile, t[k + 3])
                                    )
                                )
                            elif t[k + 2] == "Peng":
                                actions[p].pop()
                                actions[p].append(
                                    agents[p].response2action("Peng %s" % t[k + 3])
                                )
                            elif t[k + 2] == "Gang":
                                actions[p].pop()
                                actions[p].append(
                                    agents[p].response2action("Gang %s" % t[k + 3])
                                )
                            elif t[k + 2] == "Hu":
                                actions[p].pop()
                                actions[p].append(agents[p].response2action("Hu"))
                        else:
                            break
            elif t[0] == "Score":
                filterData()
                saveData()
            line = f.readline()
            line_number += 1
    with open(f"data/count-{cpu_id}.json", "w") as f:
        json.dump(l, f)


# python preprocess.py {file_path} {start_line} {end_line} {offset} {cpu_id}
if __name__ == "__main__":
    import sys

    file_path = sys.argv[1]
    start_line = int(sys.argv[2])
    if sys.argv[3] == "None":
        end_line = None
    else:
        end_line = int(sys.argv[3])
    offset = int(sys.argv[4])
    cpu_id = int(sys.argv[5])
    process_data(file_path, start_line, end_line, offset, cpu_id)
