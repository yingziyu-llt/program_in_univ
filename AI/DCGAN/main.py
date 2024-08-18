import torch
import torch.nn as nn
from torch.utils.data import DataLoader
from torchvision import datasets,transforms
import torchvision

class Generator(nn.Module):
    def __init__(self,input_size):
        super().__init__()
        self.main = nn.Sequential(
            nn.ConvTranspose2d(in_channels=100, out_channels=256, kernel_size=7, stride=1, padding=0),
            nn.BatchNorm2d(256),
            nn.LeakyReLU(0.2,True),

            nn.ConvTranspose2d(in_channels=256, out_channels=128, kernel_size=4, stride=2, padding=1),
            nn.BatchNorm2d(128),
            nn.LeakyReLU(0.2,True),

            nn.ConvTranspose2d(in_channels=128, out_channels=1, kernel_size=4, stride=2, padding=1),
            nn.Sigmoid()  
        )

    def forward(self,x):
        return self.main(x)

class Discriminator(nn.Module):
    def __init__(self,input_size):
        super().__init__()
        self.main = nn.Sequential(
            nn.Conv2d(in_channels=1, out_channels=16, kernel_size=3, stride=2, padding=1),  
            nn.LeakyReLU(0.2,True),
            nn.Conv2d(in_channels=16, out_channels=32, kernel_size=3, stride=2, padding=1), 
            nn.BatchNorm2d(32),
            nn.LeakyReLU(0.2,True),
            nn.Conv2d(in_channels=32, out_channels=64, kernel_size=3, stride=2, padding=1), 
            nn.BatchNorm2d(64),
            nn.LeakyReLU(0.2,True),
            nn.Conv2d(in_channels=64, out_channels=128, kernel_size=3, stride=2, padding=0), 
            nn.Flatten(),
            nn.Linear(128,100),  # 修正为 512
            nn.ReLU(),
            nn.Linear(100,1),
            nn.Sigmoid()
        )

    def forward(self,x):
        return self.main(x)

train_data = datasets.MNIST(root='data',train=True,download=True,transform=transforms.ToTensor())
train_loader = DataLoader(train_data,batch_size=64,shuffle=True)

test_data = datasets.MNIST(root='data',train=False,download=True,transform=transforms.ToTensor())
test_loader = DataLoader(test_data,batch_size=64,shuffle=True)

loss_fn = nn.BCELoss().cuda()
gen = Generator(100).cuda()
gen_opt = torch.optim.Adam(gen.parameters(),lr=0.0002)

disc = Discriminator(784).cuda()
disc_opt = torch.optim.Adam(disc.parameters(),lr=0.002)

fixed_noise = torch.randn(64,100,1,1).cuda()
for epoch in range(100):
    for i,(images,labels) in enumerate(train_loader):
        # discriminator training
        images = images.cuda() + 0.05 * torch.randn_like(images).cuda()
        disc.zero_grad()
        batch_size = images.size(0)
        labels = torch.full((batch_size,),0.9,dtype=torch.float).cuda()
        output = disc(images).view(-1)
        err_real = loss_fn(output,labels)
        err_real.backward()
        D_x = output.mean().item()

        noise = torch.randn(batch_size,100,1,1).cuda()
        fake_images = gen(noise).detach()
        output = disc(fake_images).view(-1)
        labels.fill_(0)
        err_fake = loss_fn(output, labels)
        err_fake.backward()
        D_G_z1 = output.mean().item()
        err = err_real + err_fake
        disc_opt.step()

        #generator training
        gen.zero_grad()
        noise = torch.randn(batch_size,100,1,1).cuda()  # 重新生成噪声
        fake_images = gen(noise)
        output = disc(fake_images).view(-1)
        labels.fill_(1)
        err_g = loss_fn(output, labels)
        err_g.backward()
        D_G_z2 = output.mean().item()
        gen_opt.step()

        if i % 50 == 0:
            print(f'[{epoch}/100][{i}/{len(train_loader)}] '
                  f'Loss_D: {err.item():.4f} Loss_G: {err_g.item():.4f} '
                  f'D(x): {D_x:.4f} D(G(z)): {D_G_z1:.4f} / {D_G_z2:.4f}')
    if epoch % 10 == 9:
        with torch.no_grad():
            fake_images = gen(fixed_noise).reshape(-1, 1, 28, 28)
            torchvision.utils.save_image(fake_images, f'fake_images_epoch_{epoch + 1}.png', nrow=10, normalize=True)

