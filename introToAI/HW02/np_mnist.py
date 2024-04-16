# -*- coding: utf-8 -*-
"""
@ author: Yiliang Liu
"""


# 作业内容：更改loss函数、网络结构、激活函数，完成训练MLP网络识别手写数字MNIST数据集

import numpy as np

from tqdm import tqdm

import matplotlib.pyplot as plt


# 加载数据集,numpy格式
X_train = np.load('./mnist/X_train.npy')  # (60000, 784), 数值在0.0~1.0之间
y_train = np.load('./mnist/y_train.npy')  # (60000, )
y_train = np.eye(10)[y_train]  # (60000, 10), one-hot编码

X_val = np.load('./mnist/X_val.npy')  # (10000, 784), 数值在0.0~1.0之间
y_val = np.load('./mnist/y_val.npy')  # (10000,)
y_val = np.eye(10)[y_val]  # (10000, 10), one-hot编码

X_test = np.load('./mnist/X_test.npy')  # (10000, 784), 数值在0.0~1.0之间
y_test = np.load('./mnist/y_test.npy')  # (10000,)
y_test = np.eye(10)[y_test]  # (10000, 10), one-hot编码


# 定义激活函数
def relu(x):
    '''
    relu函数
    '''
    y = np.zeros_like(x)
    y[x > 0] = x[x > 0]
    return y


def relu_prime(x):
    y = np.zeros_like(x)
    y[x > 0] = 1
    return y


# 输出层激活函数
def f(x):
    exps = np.exp(x)
    return exps / np.sum(exps, axis=1, keepdims=True)


def f_prime(x):
    '''
    softmax函数的导数
    '''
    s = f(x)
    return s * (1 - s)


# 定义损失函数
def loss_fn(y_true, y_pred):
    '''
    y_true: (batch_size, num_classes), one-hot编码
    y_pred: (batch_size, num_classes), softmax输出
    '''
    epsilon = 1e-13
    return -np.mean(y_true * np.log(y_pred + epsilon))


def loss_fn_prime(y_true, y_pred):
    '''
    y_true: (batch_size, num_classes), one-hot编码
    y_pred: (batch_size, num_classes), softmax输出
    '''
    return y_pred - y_true


# 定义权重初始化函数
def init_weights(shape=()):
    '''
    初始化权重
    '''
    return np.random.normal(loc=0.0, scale=np.sqrt(2.0/shape[0]), size=shape)

# 定义网络结构


class Network(object):
    '''
    MNIST数据集分类网络
    '''

    def __init__(self, input_size, hidden_size, output_size, lr=0.01):
        '''
        初始化网络结构
        '''
        self.W1 = init_weights((input_size, hidden_size))
        self.b1 = init_weights((hidden_size,))
        self.W2 = init_weights((hidden_size, output_size))
        self.b2 = init_weights((output_size,))
        self.lr = lr

    def forward(self, x):
        '''
        前向传播
        '''
        self.hidden = np.matmul(x, self.W1) + self.b1
        self.hidden = relu(self.hidden)
        self.output = np.matmul(self.hidden, self.W2) + self.b2
        self.output = f(self.output)
        return self.output

    def step(self, x_batch, y_batch):
        '''
        一步训练
        '''
        batch_size = len(x_batch)

        grads_W2 = np.zeros_like(self.W2)
        grads_b2 = np.zeros_like(self.b2)
        grads_W1 = np.zeros_like(self.W1)
        grads_b1 = np.zeros_like(self.b1)
        # 前向传播
        y_hat = self.forward(x_batch)

        # 计算损失和准确率
        loss = loss_fn(y_batch, y_hat)
        acc = np.mean(np.argmax(y_batch, axis=1) == np.argmax(y_hat, axis=1))

        # 反向传播
        output_error = loss_fn_prime(y_batch, y_hat)
        hidden_error = np.dot(output_error, self.W2.T) * \
            relu_prime(self.hidden)
        grads_W2 = np.dot(self.hidden.T, output_error)
        grads_b2 = np.sum(output_error, axis=0)
        grads_W1 = np.dot(x_batch.T, hidden_error)
        grads_b1 = np.sum(hidden_error, axis=0)
        # 更新权重
        self.W2 -= self.lr * grads_W2 / batch_size
        self.b2 -= self.lr * grads_b2 / batch_size
        self.W1 -= self.lr * grads_W1 / batch_size
        self.b1 -= self.lr * grads_b1 / batch_size
        return loss, acc


if __name__ == '__main__':
    # 训练网络
    net = Network(input_size=784, hidden_size=256, output_size=10, lr=0.01)
    loss_in_val = []
    acc_in_val = []
    for epoch in range(10):
        losses = []
        accuracies = []
        p_bar = tqdm(range(0, len(X_train), 64))
        for i in p_bar:
            x_batch = X_train[i:i+64]
            y_batch = y_train[i:i+64]
            loss, acc = net.step(x_batch, y_batch)
            losses.append(loss)
            accuracies.append(acc)
            p_bar.set_description(
                f'Epoch {epoch}, Loss: {np.mean(losses):.4f}, Acc: {np.mean(accuracies):.4f}')

        val_loss = []
        val_acc = []
        for i in range(0, len(X_val), 64):
            x_batch = X_val[i:i + 64]
            y_batch = y_val[i:i+64]
            output = net.forward(x_batch)
            val_loss.append(loss_fn(y_batch, output))
            val_acc.append(np.mean(np.argmax(y_batch, axis=1)
                                   == np.argmax(output, axis=1)))
        loss_in_val.append(np.mean(val_loss))
        acc_in_val.append(np.mean(val_acc))

    plt.figure(figsize=(10, 5))
    plt.plot(acc_in_val, label='val_acc', alpha=0.5, color='b')
    plt.legend()
    plt.xlabel("epoch")
    plt.ylabel("acc %")
    plt.title("numpy implement mnist")
    plt.savefig("./numpy_acc.jpg")
