import os
import pandas as pd
from sklearn.model_selection import KFold
from torch.utils.data import Dataset, DataLoader
from PIL import Image

class CUBDataset(Dataset):
    def __init__(self, data_root, samples, transform=None):
        self.data_root = data_root
        self.samples = samples
        self.transform = transform

    def __len__(self):
        return len(self.samples)

    def __getitem__(self, idx):
        img_path, label = self.samples[idx]
        img_full_path = os.path.join(self.data_root, 'images', img_path)
        image = Image.open(img_full_path).convert('RGB')

        if self.transform:
            image = self.transform(image)
        return image, label


def load_cub_metadata(data_root):
    images_file = os.path.join(data_root, 'images.txt')
    labels_file = os.path.join(data_root, 'image_class_labels.txt')

    df_images = pd.read_csv(images_file, sep=' ', names=['id', 'img_path'])
    df_labels = pd.read_csv(labels_file, sep=' ', names=['id', 'label'])
    df = pd.merge(df_images, df_labels, on='id')
    df['label'] = df['label'] - 1  # 标签从0开始
    return df


def create_kfold_splits(data_root, n_splits=10, seed=42, save_dir='splits'):
    os.makedirs(save_dir, exist_ok=True)
    df = load_cub_metadata(data_root)

    kf = KFold(n_splits=n_splits, shuffle=True, random_state=seed)

    for fold, (train_idx, val_idx) in enumerate(kf.split(df)):
        train_df = df.iloc[train_idx]
        val_df = df.iloc[val_idx]
        train_df.to_csv(os.path.join(save_dir, f'train_fold{fold}.csv'), index=False)
        val_df.to_csv(os.path.join(save_dir, f'val_fold{fold}.csv'), index=False)
        print(f"Fold {fold} saved: {len(train_df)} train / {len(val_df)} val")


def get_split(data_root, fold_idx, train_transform, val_transform, split_dir='splits'):
    train_csv = os.path.join(split_dir, f'train_fold{fold_idx}.csv')
    val_csv = os.path.join(split_dir, f'val_fold{fold_idx}.csv')

    train_df = pd.read_csv(train_csv)
    val_df = pd.read_csv(val_csv)

    train_samples = list(zip(train_df['img_path'], train_df['label']))
    val_samples = list(zip(val_df['img_path'], val_df['label']))

    train_dataset = CUBDataset(data_root, train_samples, train_transform)
    val_dataset = CUBDataset(data_root, val_samples, val_transform)

    return train_dataset, val_dataset


def get_dataloader(data_root, fold_idx, train_transform, val_transform, batch_size=32, num_workers=4, split_dir='splits'):
    train_dataset, val_dataset = get_split(data_root, fold_idx, train_transform, val_transform, split_dir)
    train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True, num_workers=num_workers)
    val_loader = DataLoader(val_dataset, batch_size=batch_size, shuffle=False, num_workers=num_workers)
    return train_loader, val_loader
