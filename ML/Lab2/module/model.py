import random

import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.nn.utils.rnn import pack_padded_sequence, pad_packed_sequence


class Encoder(nn.Module):
    def __init__(self, vocab_size, embed_dim, enc_hid_dim, n_layers, dropout):
        super().__init__()
        self.embedding = nn.Embedding(vocab_size, embed_dim)
        # 使用双向 GRU，效果更好
        self.rnn = nn.GRU(
            embed_dim,
            enc_hid_dim,
            n_layers,
            bidirectional=True,
            batch_first=True,
            dropout=dropout if n_layers > 1 else 0,
        )
        self.dropout = nn.Dropout(dropout)

    def forward(self, src, src_len):
        # src: [batch, src_len]
        embedded = self.dropout(self.embedding(src))

        # Pack sequence (处理变长输入，忽略 PAD)
        # enforce_sorted=False 允许数据不用预先按长度排序
        packed_embedded = pack_padded_sequence(
            embedded, src_len.cpu(), batch_first=True, enforce_sorted=False
        )

        # packed_outputs: 包含所有时刻的输出
        # hidden: [n_layers * 2, batch, hid_dim]
        packed_outputs, hidden = self.rnn(packed_embedded)

        # Unpack (Pad 回去)
        # outputs: [batch, src_len, enc_hid_dim * 2]
        outputs, _ = pad_packed_sequence(
            packed_outputs, batch_first=True, total_length=src.shape[1]
        )

        return outputs, hidden


class Attention(nn.Module):
    def __init__(self, enc_hid_dim, dec_hid_dim):
        super().__init__()
        # Bahdanau Attention:
        # score = v * tanh(W1 * encoder_outputs + W2 * decoder_hidden)
        # 输入维度是 enc_hid_dim * 2 (因为是双向) + dec_hid_dim
        self.attn = nn.Linear((enc_hid_dim * 2) + dec_hid_dim, dec_hid_dim)
        self.v = nn.Linear(dec_hid_dim, 1, bias=False)

    def forward(self, hidden, encoder_outputs, mask):
        # hidden: [batch, dec_hid_dim] (Decoder 上一步的 hidden)
        # encoder_outputs: [batch, src_len, enc_hid_dim * 2]
        # mask: [batch, src_len]

        batch_size = encoder_outputs.shape[0]
        src_len = encoder_outputs.shape[1]

        # 将 hidden 重复 src_len 次，以便与 encoder_outputs 进行矩阵运算
        # hidden: [batch, src_len, dec_hid_dim]
        hidden = hidden.unsqueeze(1).repeat(1, src_len, 1)

        # 计算 Energy (注意力得分)
        # energy: [batch, src_len, dec_hid_dim]
        energy = torch.tanh(self.attn(torch.cat((hidden, encoder_outputs), dim=2)))

        # attention: [batch, src_len]
        attention = self.v(energy).squeeze(2)

        # Masking: 将 Pad 位置的 attention 分数设为极小值，使其 Softmax 后为 0
        attention = attention.masked_fill(mask == 0, -1e10)

        return F.softmax(attention, dim=1)


class Decoder(nn.Module):
    def __init__(
        self, output_dim, embed_dim, enc_hid_dim, dec_hid_dim, n_layers, dropout
    ):
        super().__init__()
        self.output_dim = output_dim
        self.attention = Attention(enc_hid_dim, dec_hid_dim)

        self.embedding = nn.Embedding(output_dim, embed_dim)
        # 输入 = Embedding + Context Vector (Enc_Hid * 2)
        self.rnn = nn.GRU(
            (enc_hid_dim * 2) + embed_dim,
            dec_hid_dim,
            n_layers,
            batch_first=True,
            dropout=dropout if n_layers > 1 else 0,
        )

        self.fc_out = nn.Linear((enc_hid_dim * 2) + dec_hid_dim + embed_dim, output_dim)
        self.dropout = nn.Dropout(dropout)

    def forward(self, input, hidden, encoder_outputs, mask):
        # input: [batch] (当前时间步的一个词)
        # hidden: [n_layers, batch, dec_hid_dim]
        # encoder_outputs: [batch, src_len, enc_hid_dim * 2]
        # mask: [batch, src_len]

        input = input.unsqueeze(1)  # [batch, 1]
        embedded = self.dropout(self.embedding(input))  # [batch, 1, embed_dim]

        # 1. 计算 Attention Weights
        # 取 Decoder 最后一层的 hidden 用于计算 attention
        # hidden[-1]: [batch, dec_hid_dim]
        a = self.attention(hidden[-1], encoder_outputs, mask)  # [batch, src_len]

        # 2. 计算 Context Vector (加权求和)
        # a.unsqueeze(1): [batch, 1, src_len]
        # encoder_outputs: [batch, src_len, enc_hid_dim * 2]
        # context: [batch, 1, enc_hid_dim * 2]
        context = torch.bmm(a.unsqueeze(1), encoder_outputs)

        # 3. 拼接 Embedding 和 Context 送入 RNN
        # rnn_input: [batch, 1, (enc_hid_dim*2) + embed_dim]
        rnn_input = torch.cat((embedded, context), dim=2)

        # output: [batch, 1, dec_hid_dim]
        output, hidden = self.rnn(rnn_input, hidden)

        # 4. 预测下一个词
        # 将 Embedding, Output, Context 拼接在一起进行预测 (信息最大化)
        prediction = self.fc_out(
            torch.cat(
                (output.squeeze(1), context.squeeze(1), embedded.squeeze(1)), dim=1
            )
        )

        return prediction, hidden


class Seq2Seq(nn.Module):
    def __init__(self, vocab_size, embed_dim, hidden_dim, n_layer, dropout=0.2):
        super().__init__()
        # 这里的 hidden_dim 既用于 encoder 也就用于 decoder
        # 实际使用中 encoder (bidirectional) 的 hidden 输出是 2 * hidden_dim
        # 我们需要一个全连接层来统一维度

        self.encoder = Encoder(vocab_size, embed_dim, hidden_dim, n_layer, dropout)
        self.decoder = Decoder(
            vocab_size, embed_dim, hidden_dim, hidden_dim, n_layer, dropout
        )

        # 将 Encoder 双向的 hidden state 变换为 Decoder 单向的 hidden state
        self.fc_hidden = nn.Linear(hidden_dim * 2, hidden_dim)

        self.vocab_size = vocab_size

    def create_mask(self, src):
        # src: [batch, src_len]
        # pad_id 默认为 0，如果 tokenizer 改了需同步。通常 Tokenizer.pad_id=0
        mask = src != 0
        return mask

    def forward(self, src, src_len, trg=None, teacher_forcing_ratio=0.5):
        # src: [batch, src_len]
        # src_len: [batch]
        # trg: [batch, trg_len]

        batch_size = src.shape[0]
        max_len = trg.shape[1] if trg is not None else 32

        # 1. 编码 (Encoder)
        encoder_outputs, hidden = self.encoder(src, src_len)

        # 2. 处理 Hidden State
        # Encoder 是双向的 [n_layers*2, batch, hid]，Decoder 是单向的 [n_layers, batch, hid]
        # 这里为了简化，我们只取最后一层的正向和反向 hidden 拼接，然后线性映射
        # 注意：如果 n_layers > 1，这里只是初始化了 Decoder 的所有层用相同的值，或者只处理最后一层
        # 为保证鲁棒性，这里取 Encoder 最后一层 (Forward + Backward) 做映射作为 Decoder 的初始 Hidden

        # hidden[-2,:,:] 是最后一层 Forward, hidden[-1,:,:] 是最后一层 Backward
        hidden = torch.tanh(
            self.fc_hidden(torch.cat((hidden[-2, :, :], hidden[-1, :, :]), dim=1))
        )

        # 将映射后的 hidden 扩展回 [n_layers, batch, hid] 以适应 Decoder
        # (假设 Decoder n_layers 跟 Encoder 一样，且都需要初始化)
        # 这里简单的 repeat 一下。更严谨的做法是每层都映射。
        hidden = hidden.unsqueeze(0).repeat(self.decoder.rnn.num_layers, 1, 1)

        # 3. 准备 Mask
        mask = self.create_mask(src)

        # 4. 解码 (Decoder)
        outputs = torch.zeros(batch_size, max_len, self.vocab_size).to(src.device)

        # 第一个输入是 <SOS> (id=2)
        input_token = trg[:, 0] if trg is not None else src.new_full((batch_size,), 2)

        for t in range(1, max_len):
            # 将 encoder_outputs 和 mask 传给 decoder
            output, hidden = self.decoder(input_token, hidden, encoder_outputs, mask)

            outputs[:, t, :] = output

            if trg is not None:
                teacher_force = random.random() < teacher_forcing_ratio
                top1 = output.argmax(1)
                input_token = trg[:, t] if teacher_force else top1
            else:
                input_token = output.argmax(1)

        return outputs
