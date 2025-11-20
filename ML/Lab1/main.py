import torch
import torch.nn as nn
import torch.optim as optim
from torchvision import transforms
from tqdm import tqdm
import numpy as np
import matplotlib.pyplot as plt
import os

from data import create_kfold_splits, get_dataloader
# 确保你的 model.py 中有 LightCNN
from model import CNN, LightCNN 


def train_one_epoch(model, dataloader, criterion, optimizer, device):
    model.train()
    total_loss, total_correct, total_samples = 0, 0, 0

    for images, labels in tqdm(dataloader, desc='Training', leave=False):
        images, labels = images.to(device), labels.to(device)
        optimizer.zero_grad()
        outputs = model(images)
        loss = criterion(outputs, labels)
        loss.backward()
        optimizer.step()

        total_loss += loss.item() * images.size(0)
        _, preds = outputs.max(1)
        total_correct += preds.eq(labels).sum().item()
        total_samples += labels.size(0)

    return total_loss / total_samples, total_correct / total_samples


def validate(model, dataloader, criterion, device):
    model.eval()
    total_loss, total_correct, total_samples = 0, 0, 0

    with torch.no_grad():
        for images, labels in tqdm(dataloader, desc='Validating', leave=False):
            images, labels = images.to(device), labels.to(device)
            outputs = model(images)
            loss = criterion(outputs, labels)

            total_loss += loss.item() * images.size(0)
            _, preds = outputs.max(1)
            total_correct += preds.eq(labels).sum().item()
            total_samples += labels.size(0)

    return total_loss / total_samples, total_correct / total_samples


def plot_cv_results(all_logs, epochs, save_path='cv_results.png'):
    """
    绘制10折交叉验证的均值和标准差包络图
    """
    print(f"\nGenerating CV plots and saving to {save_path}...")
    
    # 准备一个 1 到 80 的 x 轴
    epoch_range = np.arange(1, epochs + 1)
    
    # 计算均值和标准差
    metrics_mean_std = {}
    for metric in all_logs.keys():
        # all_logs['train_loss'] 是一个 (10, 80) 的列表
        # 转换为 numpy 数组
        metric_data = np.array(all_logs[metric])
        mean = np.mean(metric_data, axis=0)
        std = np.std(metric_data, axis=0)
        metrics_mean_std[metric] = (mean, std)

    plt.figure(figsize=(16, 7))

    # 1. 绘制 Loss (Train vs Test/Validation)
    plt.subplot(1, 2, 1)
    
    # 绘制训练 Loss
    mean, std = metrics_mean_std['train_loss']
    plt.plot(epoch_range, mean, label='Train Loss (Mean)', color='blue')
    plt.fill_between(epoch_range, mean - std, mean + std, color='blue', alpha=0.2, label='Train Loss (Std Dev)')

    # 绘制验证 Loss
    mean, std = metrics_mean_std['val_loss']
    plt.plot(epoch_range, mean, label='Validation Loss (Mean)', color='orange')
    plt.fill_between(epoch_range, mean - std, mean + std, color='orange', alpha=0.2, label='Validation Loss (Std Dev)')
    
    plt.title('Cross-Validation Loss')
    plt.xlabel('Epochs')
    plt.ylabel('Loss')
    plt.legend()
    plt.grid(True)

    # 2. 绘制 Accuracy (Train vs Test/Validation)
    plt.subplot(1, 2, 2)

    # 绘制训练 Acc
    mean, std = metrics_mean_std['train_acc']
    plt.plot(epoch_range, mean, label='Train Accuracy (Mean)', color='blue')
    plt.fill_between(epoch_range, mean - std, mean + std, color='blue', alpha=0.2, label='Train Accuracy (Std Dev)')

    # 绘制验证 Acc
    mean, std = metrics_mean_std['val_acc']
    plt.plot(epoch_range, mean, label='Validation Accuracy (Mean)', color='orange')
    plt.fill_between(epoch_range, mean - std, mean + std, color='orange', alpha=0.2, label='Validation Accuracy (Std Dev)')

    plt.title('Cross-Validation Accuracy')
    plt.xlabel('Epochs')
    plt.ylabel('Accuracy')
    plt.legend()
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig(save_path)
    print("Plots saved.")


def train(data_root='data/CUB_200_2011', epochs=80, batch_size=128, lr=1e-3, n_folds=10):
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    print(f"Using device: {device}")

    # 1. 创建 K-Fold 划分 (只会创建一次)
    create_kfold_splits(data_root, n_splits=n_folds)

    # 2. 定义 transform (训练和验证使用相同的 transform)
    # transform = transforms.Compose([
    #     transforms.Resize(256),                     
    #     transforms.CenterCrop(224),                 
    #     transforms.ToTensor(),
    #     transforms.Normalize(                       
    #         mean=[0.485, 0.456, 0.406],
    #         std=[0.229, 0.224, 0.225]
    #     ),
    # ])
    train_transform = transforms.Compose([
        transforms.Resize(256), 
        transforms.CenterCrop(224),
        transforms.RandomHorizontalFlip(),
        transforms.ColorJitter(brightness=0.2, contrast=0.2, saturation=0.2, hue=0.1),
        transforms.RandomRotation(15),
        transforms.ToTensor(),
        transforms.Normalize(
            mean=[0.485, 0.456, 0.406],
            std=[0.229, 0.224, 0.225]
        ),
    ])
    val_transform = transforms.Compose([
        transforms.Resize(256),                     
        transforms.CenterCrop(224),
        transforms.ToTensor(),
        transforms.Normalize(                       
            mean=[0.485, 0.456, 0.406],
            std=[0.229, 0.224, 0.225]
        ),
    ])

    # 3. 准备一个字典来存储所有折的日志
    # 结构: {'train_loss': [[fold1_e1, e2...], [fold2_e1, e2...], ...], ...}
    all_fold_logs = {
        'train_loss': [],
        'train_acc': [],
        'val_loss': [],
        'val_acc': []
    }

    # 4. 开始 10 折交叉验证循环
    for fold_idx in range(n_folds):
        print(f"\n" + "="*50)
        print(f"--- Starting Fold {fold_idx + 1}/{n_folds} ---")
        print("="*50)

        # 4.1. 为当前折获取 Dataloader
        train_loader, val_loader = get_dataloader(
            data_root=data_root,
            fold_idx=fold_idx,
            train_transform=train_transform,
            val_transform=val_transform,
            batch_size=batch_size,
            num_workers=8
        )

        # 4.2. 重新初始化模型和优化器
        model = LightCNN(num_classes=200).to(device)
        criterion = nn.CrossEntropyLoss(label_smoothing=0.1)
        optimizer = optim.AdamW(model.parameters(), lr=lr,weight_decay=1e-5)

        # 4.3. 准备一个列表来存储当前折的日志
        fold_logs = {'train_loss': [], 'train_acc': [], 'val_loss': [], 'val_acc': []}

        # 4.4. 开始 80 轮的 epoch 循环
        for epoch in range(epochs):
            print(f"\n[Fold {fold_idx + 1}/{n_folds}] Epoch {epoch + 1}/{epochs}")

            train_loss, train_acc = train_one_epoch(model, train_loader, criterion, optimizer, device)
            val_loss, val_acc = validate(model, val_loader, criterion, device)

            print(f"Train Loss: {train_loss:.4f} | Train Acc: {train_acc:.4f}")
            print(f"Val   Loss: {val_loss:.4f} | Val   Acc: {val_acc:.4f}")

            # 记录当前 epoch 的指标
            fold_logs['train_loss'].append(train_loss)
            fold_logs['train_acc'].append(train_acc)
            fold_logs['val_loss'].append(val_loss)
            fold_logs['val_acc'].append(val_acc)
        
        # 4.5. 当前折训练完毕，将其日志添加到总日志中
        all_fold_logs['train_loss'].append(fold_logs['train_loss'])
        all_fold_logs['train_acc'].append(fold_logs['train_acc'])
        all_fold_logs['val_loss'].append(fold_logs['val_loss'])
        all_fold_logs['val_acc'].append(fold_logs['val_acc'])

    # 5. 所有折训练完毕
    print("\n" + "="*50)
    print("--- Cross-Validation Finished ---")
    print("="*50)
    import json
    all_fold_logs_path = 'cv_logs.json'
    with open(all_fold_logs_path, 'w') as f:
        json.dump(all_fold_logs, f)
    print(f"All fold logs saved to {all_fold_logs_path}")
    
    # 6. 绘制并保存结果图
    plot_cv_results(all_fold_logs, epochs=epochs, save_path='cv_results.png')


if __name__ == "__main__":
    # 使用你指定的 lr 和 80 epochs 运行
    train(lr=1e-3, epochs=80)