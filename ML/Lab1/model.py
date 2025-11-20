import torch
import torch.nn as nn
import torch.nn.functional as F

class CNN(nn.Module):
    def __init__(self, num_classes=200):
        super(CNN, self).__init__()
        self.conv = nn.Conv2d(3, 16, kernel_size=3, padding=1, stride=2)
        self.pool = nn.MaxPool2d(4, 4)
        self.fc = nn.Linear(16 * 28 * 28, num_classes)

    def forward(self, x):
        x = self.pool(F.relu(self.conv(x)))
        x = x.view(x.size(0), -1)
        x = self.fc(x)
        return x

class LightCNN(nn.Module):
    def __init__(self, num_classes=200):
        super(LightCNN, self).__init__()
        
        self.layer1 = self._make_layer(3, 32)   # 224 -> 112
        self.layer2 = self._make_layer(32, 64)  # 112 -> 56
        self.layer3 = self._make_layer(64, 128) # 56 -> 28
        self.layer4 = self._make_layer(128, 256) # 28 -> 14
        self.layer5 = self._make_layer(256, 512) # 14 -> 7

        self.avg_pool = nn.AdaptiveAvgPool2d((1, 1))
        
        # 关键修改：用一个更强大的分类头替换掉 nn.Linear(512, 200)
        self.classifier_head = nn.Sequential(
            nn.Linear(512, 1024), # 扩展特征
            nn.ReLU(inplace=True),
            nn.Dropout(0.5),
            nn.Linear(1024, num_classes) # 最终分类
        )

    def _make_layer(self, in_channels, out_channels):
        """创建一个 (Conv -> BN -> ReLU -> MaxPool) 的块"""
        return nn.Sequential(
            nn.Conv2d(in_channels, out_channels, kernel_size=3, padding=1, bias=False),
            nn.BatchNorm2d(out_channels),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2) # 尺寸减半
        )

    def forward(self, x):
        x = self.layer1(x)
        x = self.layer2(x)
        x = self.layer3(x)
        x = self.layer4(x)
        x = self.layer5(x)
        
        x = self.avg_pool(x)     # (B, 512, 1, 1)
        x = x.view(x.size(0), -1) # (B, 512)
        x = self.classifier_head(x) # (B, 200)
        return x