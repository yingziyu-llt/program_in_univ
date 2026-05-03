import torch
import torch.optim as optim
from torch.utils.data import DataLoader
import pandas as pd
import numpy as np
from model import MajorityVoting, BiLSTM_CRF
from utils.dataset import POSDataset, build_word_vocab, pos_collate_fn

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
BATCH_SIZE = 64
EMBEDDING_DIM = 128
HIDDEN_DIM = 256
LEARNING_RATE = 0.001
EPOCHS = 50
DATASETS = ['english','english_perturbed','alien']

torch.manual_seed(42)
if torch.cuda.is_available():
    torch.cuda.manual_seed_all(42)


from collections import Counter

def analyze_mismatches(test_loader, mv_model, lstm_model, word_to_idx, device):
    lstm_model.eval()
    idx_to_word = {i: w for w, i in word_to_idx.items()}
    
    mv_right_lstm_wrong = [] 
    oov_mismatches = 0
    total_mismatches = 0

    # 获取所有测试集的 Token 和真实标签
    all_tokens = []
    all_labels = []
    for sents, tags, lengths in test_loader:
        for i in range(len(lengths)):
            all_tokens.append(sents[i, :lengths[i]].tolist())
            all_labels.append(tags[i, :lengths[i]].tolist())

    # 获取模型预测
    mv_preds = mv_model.predict(test_loader)
    
    all_lstm_preds = []
    with torch.no_grad():
        for sents, _, _ in test_loader:
            sents = sents.to(device)
            mask = (sents != 0)
            all_lstm_preds.extend(lstm_model.predict(sents, mask=mask))

    # 逐 Token 对比
    for tokens, labels, mv_p, lstm_p in zip(all_tokens, all_labels, mv_preds, all_lstm_preds):
        for t_idx, lab, m_p, l_p in zip(tokens, labels, mv_p, lstm_p):
            if m_p == lab and l_p != lab:
                total_mismatches += 1
                word = idx_to_word.get(t_idx, "<UNK>")
                mv_right_lstm_wrong.append(word)
                if t_idx == word_to_idx.get("<UNK>", 1):
                    oov_mismatches += 1

    # 输出统计报告
    print(f"\n--- 预测一致性分析报告 ---")
    print(f"基线对但 LSTM 错的总 Token 数: {total_mismatches}")
    if total_mismatches > 0:
        print(f"其中为 <UNK> 的比例: {oov_mismatches / total_mismatches:.2%}")
    
    # 找出错误最集中的前 10 个单词
    common_errors = Counter(mv_right_lstm_wrong).most_common(10)
    print(f"LSTM 预测失败的高频单词: {common_errors}")
    
    return common_errors

def calc_acc(preds, labels):
    total_correct = 0
    total_tokens = 0
    for p_seq, l_seq in zip(preds, labels):
        total_correct += sum(p == l for p, l in zip(p_seq, l_seq))
        total_tokens += len(l_seq)
    return total_correct / total_tokens if total_tokens > 0 else 0

def check_oov_rate(test_df, word_to_idx):
    test_tokens = [w for seq in test_df['tokens'] for w in seq]
    oov_count = sum(1 for w in test_tokens if w not in word_to_idx)
    print(f"OOV Rate: {oov_count / len(test_tokens):.2%}")

def run_experiment(dataset_name, train_df, test_df):
    print(f"\n{'='*20} Dataset: {dataset_name} {'='*20}")
    word_to_idx = build_word_vocab(train_df['tokens'].tolist())

    unique_tags = set(tag for seq in train_df['pos_tags'].tolist() for tag in seq)
    tag_to_idx = {tag: i for i, tag in enumerate(sorted(unique_tags))}
    
    train_ds = POSDataset(train_df['tokens'].tolist(), train_df['pos_tags'].tolist(), word_to_idx, tag_to_idx)
    test_ds = POSDataset(test_df['tokens'].tolist(), test_df['pos_tags'].tolist(), word_to_idx, tag_to_idx)    
    train_loader = DataLoader(train_ds, batch_size=BATCH_SIZE, shuffle=True, collate_fn=pos_collate_fn, num_workers=4,pin_memory=True, persistent_workers=True)
    test_loader = DataLoader(test_ds, batch_size=BATCH_SIZE, shuffle=False, collate_fn=pos_collate_fn, num_workers=4,pin_memory=True, persistent_workers=True)
    check_oov_rate(test_df, word_to_idx)

    print(f"Training Majority Voting Baseline...")
    mv_model = MajorityVoting(ignore_index=-100)
    mv_model.train(train_loader)
    mv_preds = mv_model.predict(test_loader)
    
    true_labels = []
    for _, tags, lengths in test_loader:
        for i in range(len(lengths)):
            true_labels.append(tags[i, :lengths[i]].tolist())
            
    mv_acc = calc_acc(mv_preds, true_labels)
    print(f"[{dataset_name}] Majority Voting Accuracy: {mv_acc:.4f}")

    print(f"Training Bi-LSTM-CRF Model...")
    model = BiLSTM_CRF(len(word_to_idx), tag_to_idx, EMBEDDING_DIM, HIDDEN_DIM).to(device)
    optimizer = optim.Adam(model.parameters(), lr=LEARNING_RATE)
    
    best_acc = 0
    scheduler = optim.lr_scheduler.CosineAnnealingLR(optimizer, T_max=EPOCHS, eta_min=1e-5)
    for epoch in range(EPOCHS):
        model.train()
        total_loss = 0
        for sents, tags, lengths in train_loader:
            sents, tags = sents.to(device), tags.to(device)
            mask = (sents != 0)
            tags = torch.where(tags < 0, torch.tensor(0, device=device), tags)
            tags = torch.where(tags >= len(tag_to_idx), torch.tensor(0, device=device), tags)
            loss = model(sents, tags, mask=mask)
            optimizer.zero_grad()
            loss.backward()
            torch.nn.utils.clip_grad_norm_(model.parameters(), max_norm=5.0)
            optimizer.step()
            total_loss += loss.item()
        avg_loss = total_loss / len(train_loader)
        scheduler.step()
            
        # Evaluation Phase
        model.eval()
        all_preds = []
        with torch.no_grad():
            for sents, _, lengths in test_loader:
                sents = sents.to(device)
                mask = (sents != 0)
                batch_preds = model.predict(sents, mask=mask)
                all_preds.extend(batch_preds)
        
        current_acc = calc_acc(all_preds, true_labels)
        if current_acc > best_acc:
            best_acc = current_acc
            torch.save(model.state_dict(), f"best_model_{dataset_name}.pt")

        print(f"Epoch {epoch+1}/{EPOCHS} | Avg Loss: {avg_loss:.4f} | Acc: {current_acc:.4f}")
    
    analyze_mismatches(test_loader, mv_model, model, word_to_idx, device)

    return mv_acc, best_acc

results = []
for ds in DATASETS:
    train_df = pd.read_parquet(f"data/{ds}_train.parquet")
    test_df = pd.read_parquet(f"data/{ds}_test.parquet")
    mv_res, lstm_res = run_experiment(ds, train_df, test_df)
    results.append({"Dataset": ds, "MajorityVoting": mv_res, "Bi-LSTM-CRF": lstm_res})

summary_df = pd.DataFrame(results)
print("\n" + "="*20 + " Final Results Comparison " + "="*20)
print(summary_df.to_string(index=False))