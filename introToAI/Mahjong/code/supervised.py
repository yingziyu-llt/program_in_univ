"""
监督学习流程。划分数据集，创建网络和优化器，训练过程中每一个epoch结束会在验证集上测试。

提示：可以考虑按照下列方式修改代码：
1. 改变数据集划分、batch大小、学习率等超参数；
2. 从上一次训练的checkpoint加载模型接续训练；
3. 对训练进展进行监控和分析（如打印日志到tensorboard等）。
"""

from numpy import load
from dataset import MahjongGBDataset
from torch.utils.data import DataLoader
from model import ResNet34, CNNModel
import torch.nn.functional as F
import torch
import os
from torch.utils.tensorboard import SummaryWriter
import re
import random


# 定义序数牌和花色牌的交换函数

if __name__ == "__main__":
    logdir = "model/"
    savemodel = False
    if savemodel:
        try:
            os.mkdir(logdir + "checkpoint")
        except:
            pass

    # Load dataset
    splitRatio = 0.9
    batchSize = 1024
    trainDataset = MahjongGBDataset(0, splitRatio, True)
    validateDataset = MahjongGBDataset(splitRatio, 1, False)
    loader = DataLoader(
        dataset=trainDataset, batch_size=batchSize, shuffle=True, num_workers=12
    )
    vloader = DataLoader(
        dataset=validateDataset, batch_size=batchSize, shuffle=False, num_workers=12
    )

    # Load model
    model = CNNModel().to("cuda")
    optimizer = torch.optim.Adam(model.parameters(), lr=5e-4)
    model = model.cuda()
    writer = SummaryWriter("log/tensorboard")

    # Train and validate
    for e in range(20):
        print("Epoch", e)
        if savemodel:
            torch.save(model.state_dict(), logdir + "checkpoint/%d.pkl" % e)
        training_loss = 0
        for i, d in enumerate(loader):
            input_dict = {
                "is_training": True,
                "obs": {"observation": d[0].cuda(), "action_mask": d[1].cuda()},
            }
            logits = model(input_dict)
            loss = F.cross_entropy(logits, d[2].long().cuda())
            if i % 128 == 0:
                print(
                    "Iteration %d/%d" % (i, len(trainDataset) // batchSize + 1),
                    "policy_loss",
                    loss.item(),
                )
            training_loss += loss.item()
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
        training_loss /= len(trainDataset) // batchSize
        print("Training loss:", training_loss)
        print("Run validation:")
        correct = 0
        for i, d in enumerate(vloader):
            input_dict = {
                "is_training": False,
                "obs": {"observation": d[0].cuda(), "action_mask": d[1].cuda()},
            }
            with torch.no_grad():
                logits = model(input_dict)
                pred = logits.argmax(dim=1)
                correct += torch.eq(pred, d[2].cuda()).sum().item()
        acc = correct / len(validateDataset)
        print("Epoch", e + 1, "Validate acc:", acc)
        writer.add_scalars(
            "CNN+feature+dropout",
            {"train loss": training_loss, "validate accuacry": acc},
            e + 1,
        )
