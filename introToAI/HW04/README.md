# imdb lstm 分类实验

## 执行命令：

```
python  example_imdb_lstm_torch.py
```

训练5 epoch, 耗时约1min，训练分类精度为 0.911

## 参考结果如下：

```log
vocab_size:  20001
ImdbNet(
  (embedding): Embedding(20001, 64)
  (lstm): LSTM(64, 64)
  (linear1): Linear(in_features=64, out_features=64, bias=True)
  (act1): ReLU()
  (linear2): Linear(in_features=64, out_features=2, bias=True)
)
Train Epoch: 1 Loss: 0.636723    Acc: 0.620707
Train Epoch: 2 Loss: 0.421770    Acc: 0.809355
Train Epoch: 3 Loss: 0.328943    Acc: 0.860972
Train Epoch: 4 Loss: 0.270409    Acc: 0.890625
Train Epoch: 5 Loss: 0.229245    Acc: 0.911292
```

