# 第二课作业
# 用pytorch实现卷积神经网络，对cifar10数据集进行分类
# 要求:1. 使用pytorch的nn.Module和Conv2d等相关的API实现卷积神经网络
#      2. 使用pytorch的DataLoader和Dataset等相关的API实现数据集的加载
#      3. 修改网络结构和参数，观察训练效果
#      4. 使用数据增强，提高模型的泛化能力

import os
import torch
from torch.nn.modules import Dropout
import torchvision

from torch import layout, nn, optim, promote_types
from torch.nn import MaxPool2d, functional as F
from torch.utils.data import DataLoader
from torchvision import transforms

import matplotlib.pyplot as plt

# Ref:https://arxiv.org/pdf/1409.1556.pdf
# 定义超参数
batch_size = 64
learning_rate = 0.005
num_epochs = 100

# 定义数据预处理方式
# 普通的数据预处理方式
transform = transforms.Compose(
    [
        transforms.ToTensor(),
    ]
)
# 数据增强的数据预处理方式\
"""
transform = v2.Compose(
    [
        v2.RandomHorizontalFlip(p=0.5),
        v2.RandomResizedCrop(size=(32, 32), antialias=True),
        v2.ToDtype(torch.float32, scale=True),
        v2.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]),
    ]
)
"""

# 定义数据集
train_dataset = torchvision.datasets.CIFAR10(
    root="./data", train=True, download=True, transform=transform
)
test_dataset = torchvision.datasets.CIFAR10(
    root="./data", train=False, download=True, transform=transform
)

# 定义数据加载器
batch_size = 64
train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True)
test_loader = DataLoader(test_dataset, batch_size=batch_size, shuffle=False)


class Net(nn.Module):
    def __init__(self) -> None:
        super(Net, self).__init__()
        self.layer1 = nn.Sequential(
            nn.Conv2d(3, 64, 3, padding=1),
            nn.Conv2d(64, 64, 3, padding=1),
            nn.LeakyReLU(),
            nn.MaxPool2d(2, 2),
        )
        self.layer2 = nn.Sequential(
            nn.Conv2d(64, 128, 3, padding=1),
            nn.Conv2d(128, 128, 3, padding=1),
            nn.LeakyReLU(),
            nn.MaxPool2d(2, 2),
        )
        self.flat = nn.Flatten(start_dim=1)
        self.fc1 = nn.Sequential(nn.Linear(8192, 1024), nn.ReLU(), Dropout())
        self.fc2 = nn.Sequential(nn.Linear(1024, 10))

    def forward(self, x):
        out = self.layer1(x)
        out = self.layer2(out)
        out = self.flat(out)
        out = self.fc1(out)
        out = self.fc2(out)
        return out


# 实例化模型
model = Net()

use_mlu = False
# 租不起mlu（哭）
# try:
#    use_mlu = torch.mlu.is_available()
# except:
#    use_mlu = False

if use_mlu:
    device = torch.device("mlu:0")
else:
    print("MLU is not available, use GPU/CPU instead.")
    if torch.cuda.is_available():
        device = torch.device("cuda:0")
    else:
        device = torch.device("cpu")

model = model.to(device)

# 定义损失函数和优化器
criterion = nn.CrossEntropyLoss()
# optimizer = optim.Adamax(params=model.parameters(), lr=0.001)
optimizer = optim.SGD(params=model.parameters(), lr=0.005)

num_epochs = 100
acc_train = []
acc_test = []
data_len = len(train_loader)
# 训练模型
for epoch in range(num_epochs):
    # 训练模式
    model.train()
    for i, (images, labels) in enumerate(train_loader):
        images = images.to(device)
        labels = labels.to(device)

        # 前向传播
        outputs = model(images)
        loss = criterion(outputs, labels)

        # 反向传播
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        accuracy = (outputs.argmax(1) == labels).float().mean()

        # 打印训练信息
        if (i + 1) % 100 == 0:
            print(
                "Epoch [{}/{}], Step [{}/{}], Loss: {:.4f}, Accuracy: {:.2f}%".format(
                    epoch + 1,
                    num_epochs,
                    i + 1,
                    len(train_loader),
                    loss.item(),
                    accuracy.item() * 100,
                )
            )
        if (i + 1) == data_len:
            acc_train.append(accuracy.item())

    # 测试模式
    model.eval()
    with torch.no_grad():
        correct = 0
        total = 0
        for images, labels in test_loader:
            images = images.to(device)
            labels = labels.to(device)

            outputs = model(images)
            _, predicted = torch.max(outputs.data, 1)
            total += labels.size(0)
            correct += (predicted == labels).sum().item()

        print(
            "Test Accuracy of the model on the 10000 test images: {} %".format(
                100 * correct / total
            )
        )
        acc_test.append(correct / total)

plt.figure(figsize=(10, 5))
plt.plot(acc_train, label="train_acc")
plt.plot(acc_test, label="test_acc")
plt.legend()
plt.savefig("MyCNN.jpg")
