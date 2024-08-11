import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import DataLoader
from torchvision import datasets,transforms
import torchvision

class Generator(nn.Module):
    def __init__(self,input_size,output_size):
        super().__init__()
        self.fc1 = nn.Linear(input_size,32)
        self.fc2 = nn.Linear(32,64)
        self.fc3 = nn.Linear(64,output_size)

    def forward(self,x):
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = F.tanh(self.fc3(x))
        return x

class Discriminator(nn.Module):
    def __init__(self,input_size):
        super().__init__()
        self.fc1 = nn.Linear(input_size,32)
        self.fc2 = nn.Linear(32,64)
        self.fc3 = nn.Linear(64,1)

    def forward(self,x):
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = F.sigmoid(self.fc3(x))
        return x

train_data = datasets.MNIST(root='data',train=True,download=True,transform=transforms.ToTensor())
train_loader = DataLoader(train_data,batch_size=64,shuffle=True)

test_data = datasets.MNIST(root='data',train=False,download=True,transform=transforms.ToTensor())
test_loader = DataLoader(test_data,batch_size=64,shuffle=True)

loss_fn = nn.BCEWithLogitsLoss().cuda()
gen = Generator(100,784).cuda()
gen_opt = torch.optim.Adam(gen.parameters(),lr=0.001)

disc = Discriminator(784).cuda()
disc_opt = torch.optim.Adam(disc.parameters(),lr=0.001)

fixed_noise = torch.randn(1,100).cuda()
for epoch in range(100):
    for i,(images,labels) in enumerate(train_loader):
        # discriminator training
        images = images.view(images.size(0),-1).to(torch.float32).cuda()
        disc.zero_grad()
        batch_size = images.size(0)
        labels = torch.full((batch_size,),1,dtype=torch.float).cuda()
        output = disc(images).view(-1)
        err_real = loss_fn(output,labels)
        err_real.backward()
        D_x = output.mean().item()

        noise = torch.randn(batch_size,100).cuda()
        fake_images = gen(noise)
        labels.fill_(0)
        output = disc(fake_images.detach()).view(-1)
        err_fake = loss_fn(output, labels)
        err_fake.backward()
        D_G_z1 = output.mean().item()
        err = err_real + err_fake
        disc_opt.step()

        #generator training
        gen.zero_grad()
        labels.fill_(1)
        output = disc(fake_images).view(-1)
        err_g = loss_fn(output, labels)
        err_g.backward()
        D_G_z2 = output.mean().item()
        gen_opt.step()

        if i % 50 == 0:
            print(f'[{epoch}/10][{i}/{len(train_loader)}] '
                  f'Loss_D: {err.item():.4f} Loss_G: {err_g.item():.4f} '
                  f'D(x): {D_x:.4f} D(G(z)): {D_G_z1:.4f} / {D_G_z2:.4f}')
    if epoch % 10 == 9:
        with torch.no_grad():
            fake_images = gen(fixed_noise).detach().cpu().view(28,28)
        torchvision.utils.save_image(fake_images, f'output/fake_samples_epoch_{epoch}.png', normalize=True)


