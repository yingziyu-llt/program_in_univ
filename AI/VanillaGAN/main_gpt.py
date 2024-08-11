import torch
import torch.nn as nn
import torch.optim as optim
import torchvision.datasets as datasets
import torchvision.transforms as transforms
from torch.utils.data import DataLoader
import torchvision.utils as vutils

# 设置设备
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

# 超参数设置
image_size = 28 * 28  # MNIST的图片大小是28x28
latent_size = 64      # 噪声向量的维度
hidden_size = 256     # 隐藏层大小
batch_size = 100      # 批次大小
num_epochs = 100      # 训练轮数
learning_rate = 0.0002

# 数据预处理和加载
transform = transforms.Compose([
    transforms.ToTensor(),
    transforms.Normalize(mean=[0.5], std=[0.5])  # 标准化到[-1, 1]范围
])

mnist = datasets.MNIST(root='./data', train=True, transform=transform, download=True)
dataloader = DataLoader(dataset=mnist, batch_size=batch_size, shuffle=True)

# 生成器网络
class Generator(nn.Module):
    def __init__(self, input_size, hidden_size, output_size):
        super(Generator, self).__init__()
        self.main = nn.Sequential(
            nn.Linear(input_size, hidden_size),
            nn.ReLU(True),
            nn.Linear(hidden_size, hidden_size),
            nn.ReLU(True),
            nn.Linear(hidden_size, output_size),
            nn.Tanh()  # 输出范围为[-1, 1]
        )

    def forward(self, x):
        return self.main(x)

# 判别器网络
class Discriminator(nn.Module):
    def __init__(self, input_size, hidden_size, output_size):
        super(Discriminator, self).__init__()
        self.main = nn.Sequential(
            nn.Linear(input_size, hidden_size),
            nn.LeakyReLU(0.2, inplace=True),
            nn.Linear(hidden_size, hidden_size),
            nn.LeakyReLU(0.2, inplace=True),
            nn.Linear(hidden_size, output_size),
            nn.Sigmoid()  # 输出范围为[0, 1]
        )

    def forward(self, x):
        return self.main(x)

# 初始化网络
G = Generator(latent_size, hidden_size, image_size).to(device)
D = Discriminator(image_size, hidden_size, 1).to(device)

# 损失函数和优化器
criterion = nn.BCELoss()
optimizerD = optim.Adam(D.parameters(), lr=learning_rate)
optimizerG = optim.Adam(G.parameters(), lr=learning_rate)

# 训练循环
for epoch in range(num_epochs):
    for i, (images, _) in enumerate(dataloader):
        # 准备真实数据和标签
        images = images.view(batch_size, -1).to(device)
        real_labels = torch.ones(batch_size, 1).to(device)
        fake_labels = torch.zeros(batch_size, 1).to(device)

        # 训练判别器
        D.zero_grad()
        outputs = D(images)
        d_loss_real = criterion(outputs, real_labels)
        d_loss_real.backward()

        # 生成假数据并计算判别器的损失
        z = torch.randn(batch_size, latent_size).to(device)
        fake_images = G(z)
        outputs = D(fake_images.detach())
        d_loss_fake = criterion(outputs, fake_labels)
        d_loss_fake.backward()

        # 更新判别器
        optimizerD.step()

        # 训练生成器
        G.zero_grad()
        outputs = D(fake_images)
        g_loss = criterion(outputs, real_labels)  # 生成器希望判别器认为生成的图像是真实的
        g_loss.backward()

        # 更新生成器
        optimizerG.step()

        if (i + 1) % 200 == 0:
            print(f'Epoch [{epoch+1}/{num_epochs}], Step [{i+1}/{len(dataloader)}], '
                  f'D Loss: {d_loss_real.item() + d_loss_fake.item():.4f}, G Loss: {g_loss.item():.4f}')

    # 每个epoch结束后保存部分生成的图像（可选）
    if epoch % 10 == 9:
        with torch.no_grad():
            fake_images = G(z).reshape(-1, 1, 28, 28)
            vutils.save_image(fake_images, f'fake_images_epoch_{epoch + 1}.png', nrow=10, normalize=True)

# 保存最终生成的模型（可选）
torch.save(G.state_dict(), 'generator.pth')
torch.save(D.state_dict(), 'discriminator.pth')

