import numpy as np
from collections import defaultdict, Counter

class MajorityVoting:
    def __init__(self, ignore_index=-100):
        self.word_to_majority_tag = {}
        self.global_majority_tag = None
        self.ignore_index = ignore_index

    def train(self, train_loader,args = None):
        word_tag_counts = defaultdict(Counter)
        global_counts = Counter()

        for sents, tags, lengths in train_loader:
            for i in range(len(lengths)):
                valid_words = sents[i, :lengths[i]].tolist()
                valid_tags = tags[i, :lengths[i]].tolist()
                for word_idx, tag_idx in zip(valid_words, valid_tags):
                    if tag_idx != self.ignore_index:
                        word_tag_counts[word_idx][tag_idx] += 1
                        global_counts[tag_idx] += 1

        for word_idx, counts in word_tag_counts.items():
            self.word_to_majority_tag[word_idx] = max(counts, key=counts.get)
        
        if global_counts:
            self.global_majority_tag = max(global_counts, key=global_counts.get)

    def predict(self, test_loader):
        all_predictions = []
        for sents, _, lengths in test_loader:
            batch_predictions = []
            for i in range(len(lengths)):
                sent_indices = sents[i, :lengths[i]].tolist()
                pred_tags = [self.word_to_majority_tag.get(w, self.global_majority_tag) for w in sent_indices]
                batch_predictions.append(pred_tags)
            
            all_predictions.extend(batch_predictions)
            
        return all_predictions