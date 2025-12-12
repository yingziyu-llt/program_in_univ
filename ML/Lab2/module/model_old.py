import random

import torch
import torch.nn as nn
from torch.nn.utils.rnn import pack_padded_sequence


class Seq2Seq(nn.Module):
    def __init__(self, vocab_size, embed_dim, hidden_dim, n_layer, dropout=0.2):
        super().__init__()
        self.vocab_size = vocab_size
        self.embedding = nn.Embedding(vocab_size, embed_dim)

        # Encoder
        self.encoder = nn.GRU(
            embed_dim,
            hidden_dim,
            n_layer,
            batch_first=True,
            dropout=dropout if n_layer > 1 else 0,
        )

        # Decoder
        self.decoder = nn.GRU(
            embed_dim,
            hidden_dim,
            n_layer,
            batch_first=True,
            dropout=dropout if n_layer > 1 else 0,
        )

        self.fc = nn.Linear(hidden_dim, vocab_size)
        self.dropout = nn.Dropout(dropout)

    # 修改 forward，增加 src_len 参数
    def forward(self, src, src_len, trg=None, teacher_forcing_ratio=0.5):
        # src: [batch, src_len]
        # src_len: [batch] (记录每个句子的真实长度)

        batch_size = src.shape[0]
        max_len = trg.shape[1] if trg is not None else 32
        outputs = torch.zeros(batch_size, max_len, self.vocab_size).to(src.device)

        # --- Encoder 部分修改 ---
        embedded_src = self.dropout(self.embedding(src))

        # 1. 打包序列 (Pack)
        # enforce_sorted=False 允许乱序，src_len 必须在 CPU 上
        packed_src = pack_padded_sequence(
            embedded_src, src_len.cpu(), batch_first=True, enforce_sorted=False
        )

        # 2. 传入 Encoder
        # packed_output 是打包后的输出（我们这里不需要），hidden 是最后时刻的隐藏状态
        _, hidden = self.encoder(packed_src)

        # PyTorch 的 GRU 在处理 packed sequence 时，
        # 返回的 hidden 会自动提取每个样本“最后一个真实 token”对应的状态，
        # 而不是 padding 结束后的状态。这正是我们想要的！
        # ----------------------

        # Decoding (保持不变)
        input_token = (
            trg[:, 0] if trg is not None else src.new_full((batch_size,), 2)
        )  # SOS

        for t in range(1, max_len):
            embedded_input = self.dropout(self.embedding(input_token.unsqueeze(1)))
            output, hidden = self.decoder(embedded_input, hidden)
            prediction = self.fc(output.squeeze(1))
            outputs[:, t, :] = prediction

            if trg is not None:
                teacher_force = random.random() < teacher_forcing_ratio
                top1 = prediction.argmax(1)
                input_token = trg[:, t] if teacher_force else top1
            else:
                input_token = prediction.argmax(1)

        return outputs
