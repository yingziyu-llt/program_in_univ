= 计算机视觉 lab6 实验报告

由于显存、计算资源有限，我训练的参数量相对较小。其中FNCCNet为3隐藏层，每层神经元个数为初始输入神经元的4倍。CNN为4卷积、4池化、2全连接层，卷积核大小为3x3，池化核大小为2x2，kernel数目为2的幂次方，使用batchnorm。

== Linear Regression

由于线性回归的拟合能力较差，故其无论是在训练集还是测试集上的表现都不好。训练40 epoch后，其准确率只能达到0.4左右。
#figure(image("linear.png"),caption: "Linear Regression")

== Vanilla Multi-layer Perceptron
=== Optmizers

作业要求做SGD和AdamW的对比，下面是两种优化器的结果对比。

#figure(table(columns: 2,image("SGD.png"), image("adam.png")) , caption:"SGD vs AdamW")

很显然，AdamW的收敛速度更快，且最终准确率更高，约0.465。SGD的收敛速度较慢，且最终准确率较低，约0.355。所以一般情况下，如果不对参数进行微调，使用AdamW是一个不错的选择。

=== scheduler

相对而言，cosine annealing的效果更好，最终能达到0.48的准确率，这是可能是由于其lr的变化更加平滑，在训练前期的lr较大，在训练后期的lr较小。下图是cosine annealing和stepLR的对比。

#figure(table(columns: 2,image("cosine.png"), image("step.png")), caption:"cosine vs step")

== Multi-layer Perceptron with ResBlock

时间、计算资源限制，我们选择了一个比较小的网络结构，即3层隐藏层。每个隐藏层均为一层线性层+一个normalize1d+一个relu+残差。训练40 epoch后，其准确率达到了0.53左右。

不可否认的是，出现了很严重的过拟合。通过使用weight_decay、dropout等方法，可以缓解过拟合。然而，使用这些方法后，测试集准确率确有上升，但整体来看效果差异有限。这呼吁着数据增强、更强大的的网络结构等方法。

这里也展示其不同优化器、不同scheduler的效果。

=== Optimizers

#figure(table(columns: 2,image("res_sgd.png"), image("res_adam.png")), caption:"ResBlock with SGD vs AdamW")

虽然在测试集上差异不大，但在训练集上，AdamW的效果比sgd好得多，且收敛速度更快。

=== scheduler

#figure(table(columns: 2,image("res_cos.png"), image("res_step.png")), caption:"ResBlock with cosine vs step")

cosine由于其lr的变化更加平滑，导致其过拟合现象比step更加严重。这说明在不同的数据情况下需要不同的lr调整策略。
== CNN

由于FCNN始终无法达到60%的准确率，故我尝试了CNN。CNN的结构为4卷积、4池化、2全连接层，卷积核大小为3x3，池化核大小为2x2，kernel数目为2的幂次方，使用batchnorm。训练40 epoch后，其准确率达到了0.75左右。

#figure(image("cnn.png"),caption: "CNN")