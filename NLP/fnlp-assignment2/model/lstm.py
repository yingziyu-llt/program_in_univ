import torch
import torch.nn as nn
from torchcrf import CRF


class BiLSTM_CRF(nn.Module):
    def __init__(self, vocab_size, tag_to_idx, embedding_dim, hidden_dim, dropout=0.5, unk_idx=1):
        super(BiLSTM_CRF, self).__init__()
        self.embedding_dim = embedding_dim
        self.hidden_dim = hidden_dim
        self.vocab_size = vocab_size
        self.tag_to_idx = tag_to_idx
        self.target_size = len(tag_to_idx)
        self.unk_idx = unk_idx 

        self.embedding = nn.Embedding(vocab_size, embedding_dim, padding_idx=0)
        self.emb_drop = nn.Dropout(dropout)
        
        self.lstm = nn.LSTM(embedding_dim, hidden_dim // 2,
                            num_layers=1, bidirectional=True, batch_first=True)
        self.lstm_drop = nn.Dropout(dropout)
        
        self.hidden2tag = nn.Linear(hidden_dim, self.target_size)
        self.crf = CRF(self.target_size, batch_first=True)

    def _get_lstm_features(self, sentence):
        embeds = self.embedding(sentence)
        embeds = self.emb_drop(embeds) 
        
        lstm_out, _ = self.lstm(embeds)
        lstm_out = self.lstm_drop(lstm_out) 
        
        lstm_feats = self.hidden2tag(lstm_out)
        return lstm_feats

    def forward(self, sentence, tags, mask=None, word_dropout_rate=0.05):
        if self.training and word_dropout_rate > 0:
            prob = torch.rand(sentence.shape, device=sentence.device)
            drop_mask = (prob < word_dropout_rate) & (sentence != 0)
            sentence = sentence.clone()
            sentence[drop_mask] = self.unk_idx
            
        feats = self._get_lstm_features(sentence)
        return -self.crf(feats, tags, mask=mask, reduction='token_mean')

    def predict(self, sentence, mask=None):
        feats = self._get_lstm_features(sentence)
        return self.crf.decode(feats, mask=mask)