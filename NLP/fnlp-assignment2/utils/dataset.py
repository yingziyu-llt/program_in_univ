import torch
from torch.utils.data import Dataset, DataLoader
from torch.nn.utils.rnn import pad_sequence
from collections import Counter
from collections import Counter

def build_word_vocab(token_lists, min_freq=1):
    word_counts = Counter()
    for seq in token_lists:
        word_counts.update(seq)
    word_to_idx = {"<PAD>": 0, "<UNK>": 1}
    for word, count in word_counts.items():
        if count >= min_freq:
            word_to_idx[word] = len(word_to_idx)
            
    return word_to_idx

class POSDataset(Dataset):
    def __init__(self, token_lists, tag_lists, word_to_idx,tag_to_idx):
        self.sentences = []
        self.tags = []
        
        for tokens, tags in zip(token_lists, tag_lists):
            word_indices = [word_to_idx.get(w, word_to_idx["<UNK>"]) for w in tokens]
            self.sentences.append(torch.tensor(word_indices, dtype=torch.long))
            tag_indices = [tag_to_idx.get(t, -100) for t in tags]
            self.tags.append(torch.tensor(tag_indices, dtype=torch.long))    
            
    def __len__(self):
        return len(self.sentences)

    def __getitem__(self, idx):
        return self.sentences[idx], self.tags[idx]
    
def pos_collate_fn(batch):
    sentences, tags = zip(*batch)
    sent_padded = pad_sequence(sentences, batch_first=True, padding_value=0)
    tags_padded = pad_sequence(tags, batch_first=True, padding_value=-100)    
    lengths = torch.tensor([len(s) for s in sentences])
    return sent_padded, tags_padded, lengths