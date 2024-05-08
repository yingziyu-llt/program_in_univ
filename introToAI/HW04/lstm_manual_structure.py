# 用Pytorch手写一个LSTM网络，在IMDB数据集上进行训练

import os
import numpy as np
import torch.nn.functional as F
import torch
import torch.nn as nn

from torch.utils.data import Dataset, DataLoader
from utils import load_imdb_dataset, Accuracy
import sys

from utils import load_imdb_dataset, Accuracy

use_mlu = False
# try:
#    import torch_mlu
#    import torch_mlu.core.mlu_model as ct
#    global ct
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


X_train, y_train, X_test, y_test = load_imdb_dataset(
    "data", nb_words=20000, test_split=0.2
)

seq_Len = 200
vocab_size = len(X_train) + 1


class ImdbDataset(Dataset):
    def __init__(self, X, y):
        self.X = X
        self.y = y

    def __getitem__(self, index):
        data = self.X[index]
        data = torch.tensor(
            data[:seq_Len] + [0] * (seq_Len - len(data)), dtype=torch.long
        )  # 补0对齐长度
        label = torch.tensor(self.y[index], dtype=torch.long)  # 将 label 转换为张量
        return data, label

    #    data = self.X[index]
    #    data = np.concatenate([data[:seq_Len], [0] * (seq_Len - len(data))]).astype(
    #        "int32"
    #    )  # set
    #    label = self.y[index].astype("int32")
    #    return data, label

    def __len__(self):
        return len(self.y)


class LSTMCell(nn.Module):
    """
    Implementation of LSTM Cell with matrix multiplication
    """

    def __init__(self, input_size, hidden_size):
        super(LSTMCell, self).__init__()

        self.hidden_size = hidden_size

        self.W = nn.Linear(input_size + hidden_size, 4 * hidden_size)

    def forward(self, x, h, c):
        combined = torch.cat((x, h), dim=1)
        gates = self.W(combined)

        # Split the combined gates into input, forget, cell, and output gates
        ingate, forgetgate, cellgate, outgate = gates.chunk(4, 1)

        ingate = torch.sigmoid(ingate)
        forgetgate = torch.sigmoid(forgetgate)
        cellgate = torch.tanh(cellgate)
        outgate = torch.sigmoid(outgate)

        c = forgetgate * c + ingate * cellgate
        h = outgate * torch.tanh(c)

        return h, c


# class LSTMCell(nn.Module):
#    """
#    手写lstm，可以用全连接层nn.Linear，不能直接用nn.LSTM
#    """
#
#    def __init__(self, input_size, hidden_size):
#        super(LSTMCell, self).__init__()
#
#        self.hidden_size = hidden_size
#
#        # LSTM层
#        self.f = nn.Sequential(
#            nn.Linear(input_size + hidden_size, hidden_size), nn.Sigmoid()
#        )
#        self.i = nn.Sequential(
#            nn.Linear(input_size + hidden_size, hidden_size), nn.Sigmoid()
#        )
#        self.c_hat = nn.Sequential(
#            nn.Linear(input_size + hidden_size, hidden_size), nn.Tanh()
#        )
#        self.o = nn.Sequential(
#            nn.Linear(input_size + hidden_size, hidden_size), nn.Sigmoid()
#        )
#
#    def forward(self, x, h, c):
#        concat = torch.cat((x, h), dim=1)
#        f_t = self.f(concat)
#        i_t = self.i(concat)
#        c_hat_t = self.c_hat(concat)
#        o_t = self.o(concat)
#
#        c = f_t * c + i_t * c_hat_t
#        h_t = o_t * torch.tanh(c)
#        return h_t, c
#


class LSTM(nn.Module):
    def __init__(self, input_size, hidden_size):
        super(LSTM, self).__init__()
        self.hidden_size = hidden_size
        self.cell = LSTMCell(input_size, hidden_size)
        # self.cell2 = LSTMCell(hidden_size + input_size, hidden_size)

    def forward(self, x):
        batch_size = x.size(0)
        h = torch.zeros(batch_size, self.hidden_size).to(x.device)
        c = torch.zeros(batch_size, self.hidden_size).to(x.device)
        # h2 = torch.zeros(batch_size, self.hidden_size).to(x.device)
        # c2 = torch.zeros(batch_size, self.hidden_size).to(x.device)

        outputs = []

        for i in range(seq_Len):
            h, c = self.cell(x[:, i, :], h, c)
            # h2, c2 = self.cell2(torch.concat((x[:, i, :], h), dim=1), h2, c2)
            outputs.append(h.unsqueeze(1))

        outputs = torch.cat(outputs, dim=1)

        return outputs


class Net(nn.Module):
    """
    一层LSTM的文本分类模型
    """

    def __init__(self, embedding_size=64, hidden_size=128, num_classes=2):
        super(Net, self).__init__()

        # 词嵌入层
        self.embedding = nn.Embedding(vocab_size, embedding_size)
        # LSTM层
        self.lstm = LSTM(input_size=embedding_size, hidden_size=hidden_size)
        # 全连接层
        self.fc1 = nn.Linear(hidden_size, hidden_size)
        self.fc2 = nn.Linear(hidden_size, num_classes)

    def forward(self, x):
        """
        x: 输入, shape: (seq_len, batch_size)
        """

        # 词嵌入
        x = self.embedding(x)
        # LSTM层
        x = self.lstm(x)
        x = torch.mean(x, dim=1)
        x = F.relu(self.fc1(x))
        x = self.fc2(x)
        return x


n_epoch = 5
batch_size = 64
print_freq = 2
train_dataset = ImdbDataset(X=X_train, y=y_train)
train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True)
test_dataset = ImdbDataset(X=X_test, y=y_test)
test_loader = DataLoader(test_dataset, batch_size=batch_size, shuffle=False)


net = Net()
metric = Accuracy()
print(net)


def train(model, device, train_loader, optimizer, epoch):
    model = model.to(device)
    model.train()
    loss_func = torch.nn.CrossEntropyLoss(reduction="mean")
    train_acc = 0
    train_loss = 0
    n_iter = 0
    for batch_idx, (data, target) in enumerate(train_loader):
        target = target.long()
        data, target = data.to(device), target.to(device)
        optimizer.zero_grad()
        output = model(data)
        # loss = F.nll_loss(output, target)
        loss = loss_func(output, target)
        loss.backward()
        optimizer.step()
        metric.update(output, target)
        train_acc += metric.result()
        train_loss += loss.item()
        metric.reset()
        n_iter += 1
    print(
        "Train Epoch: {} Loss: {:.6f} \t Acc: {:.6f}".format(
            epoch, train_loss / n_iter, train_acc / n_iter
        )
    )


def test(model, device, test_loader):
    model = model.to(device)
    model.eval()
    loss_func = torch.nn.CrossEntropyLoss(reduction="mean")
    test_loss = 0
    test_acc = 0
    n_iter = 0
    with torch.no_grad():
        for data, target in test_loader:
            target = target.long()
            data, target = data.to(device), target.to(device)
            output = model(data)
            # test_loss += F.nll_loss(output, target, reduction='sum').item()  # sum up batch loss
            test_loss += loss_func(output, target).item()
            metric.update(output, target)
            test_acc += metric.result()
            metric.reset()
            n_iter += 1
    test_loss /= n_iter
    test_acc /= n_iter
    print(
        "Test set: Average loss: {:.4f}, Accuracy: {:.4f}".format(test_loss, test_acc)
    )
    return test_loss, test_acc


optimizer = torch.optim.Adam(net.parameters(), lr=1e-3, weight_decay=0.0)
gamma = 0.7
for epoch in range(1, n_epoch + 1):
    train(net, device, train_loader, optimizer, epoch)
    test(net, device, test_loader)
