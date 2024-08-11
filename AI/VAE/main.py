import torch
import torch.nn as nn
from torchvision import datasets,transforms
import torch.nn.functional as F
from torch.utils.data import DataLoader
import matplotlib.pyplot as plt
import torchvision

class VAE(nn.Module):
    def __init__(self,input_dim,hidden_dim,latent_dim):
        super(VAE, self).__init__()
        self.fc1 = nn.Linear(input_dim, hidden_dim)
        self.fc21 = nn.Linear(hidden_dim, latent_dim)  # mu
        self.fc22 = nn.Linear(hidden_dim, latent_dim)  # logvar
        self.fc3 = nn.Linear(latent_dim, hidden_dim)
        self.fc4 = nn.Linear(hidden_dim, input_dim)

    def encoder(self, x):
        h1 = F.relu(self.fc1(x))
        return self.fc21(h1), self.fc22(h1)

    def reparament(self, mu, logvar):
        std = torch.exp(0.5 * logvar)
        eps = torch.randn_like(std)
        return mu + eps * std

    def decoder(self, z):
        h3 = F.relu(self.fc3(z))
        return torch.sigmoid(self.fc4(h3))  # 使用sigmoid，适合BCELoss

    def forward(self, x):
        mu, logvar = self.encoder(x.view(-1, 784))
        z = self.reparament(mu, logvar)
        return self.decoder(z), mu, logvar

transform = transforms.Compose([
    transforms.ToTensor(),  # 将图像转换为张量
])


train_dataset = datasets.MNIST(root='./data', train=True, download=True, transform=transform)
test_dataset = datasets.MNIST(root='./data', train=False, download=True, transform=transform)
train_loader = DataLoader(train_dataset, batch_size=64, shuffle=True)
test_loader = DataLoader(test_dataset, batch_size=64, shuffle=False)

def loss_function(recon_x, x, mu, logvar):
    BCE = nn.functional.binary_cross_entropy(recon_x, x.view(-1, 784), reduction='sum')
    # 计算KL散度
    KLD = -0.5 * torch.sum(1 + logvar - mu.pow(2) - logvar.exp())
    return BCE + KLD

def main():
    input_dim = 784
    hidden_dim = 400
    latent_dim = 20
    device = "cuda"
    model = VAE(input_dim,hidden_dim,latent_dim).to(device)
    optimizer = torch.optim.Adam(model.parameters(), lr=1e-3)
    for epoch in range(100):
        model.train()
        train_loss = 0
        total_length = 0
        for batch_idx, (data, _) in enumerate(train_loader):
            total_length += data.size(0)
            data = data.to(device)
            optimizer.zero_grad()
            recon_batch, mu, logvar = model(data)
            loss = loss_function(recon_batch, data, mu, logvar)
            loss.backward()
            train_loss += loss.item()
            total_length += data.size(0)
            optimizer.step()
        print(f'Epoch {epoch}, Training Loss: {train_loss/total_length}')
        model.eval()
        test_loss = 0
        total_length = 0
        with torch.no_grad():
            for data, _ in test_loader:
                total_length += data.size(0)
                data = data.to(device)
                recon_batch, mu, logvar = model(data.view(-1,784))
                loss = loss_function(recon_batch, data, mu, logvar)
                test_loss += loss.item()
                total_length += data.size(0)
        print(f'Epoch {epoch}, Test Loss: {test_loss/total_length}')

    model.eval()
    with torch.no_grad():
        z = torch.randn(64, latent_dim).to(device)  # 生成64个随机隐变量
        sample = model.decoder(z).cpu()
        # 这里的sample是生成的图像，可以进一步处理或显示

    # 将生成的样本转化为可视化格式
    sample = sample.view(64, 1, 28, 28)  # MNIST 是 28x28 的图像
    
    # 创建一个 8x8 的图像网格来展示生成的样本
    grid_img = torchvision.utils.make_grid(sample, nrow=8)
    
    plt.imshow(grid_img.permute(1, 2, 0), cmap='gray')
    plt.show()


if __name__ == '__main__':
    main()
