import torch
import torch.nn as nn
import argparse
import torchvision
import torchvision.transforms as transforms
import matplotlib.pyplot as plt
import numpy as np
from torch.utils.tensorboard import SummaryWriter
writer = SummaryWriter()
np.random.seed(0)
torch.manual_seed(0)

class LinearClassifier(nn.Module):
    # define a linear classifier
    def __init__(self, in_channels: int, out_channels: int):
        super().__init__()
        # inchannels: dimenshion of input data. For example, a RGB image [3x32x32] is converted to vector [3 * 32 * 32], so dimenshion=3072
        # out_channels: number of categories. For CIFAR-10, it's 10
        self.flatten = nn.Flatten()
        self.linear = nn.Linear(in_channels, out_channels)

    def forward(self, x: torch.Tensor):
        return self.linear(self.flatten(x))


class FCNN(nn.Module):
    # def a full-connected neural network classifier
    def __init__(self, in_channels: int, hidden_channels: int, out_channels: int):
        super().__init__()
        # inchannels: dimenshion of input data. For example, a RGB image [3x32x32] is converted to vector [3 * 32 * 32], so dimenshion=3072
        # hidden_channels
        # out_channels: number of categories. For CIFAR-10, it's 10

        # full connected layer
        # activation function
        # full connected layer
        # ......
        
        self.flatten = nn.Flatten()
        self.fc1 = nn.Sequential(nn.Linear(in_channels, hidden_channels), nn.ReLU())
        self.fc2 = nn.Sequential(nn.Linear(hidden_channels, hidden_channels), nn.ReLU(),nn.Dropout())
        self.fc3 = nn.Sequential(nn.Linear(hidden_channels, hidden_channels), nn.ReLU(),nn.Dropout())
        self.fc4 = nn.Sequential(nn.Linear(hidden_channels, hidden_channels), nn.ReLU(),nn.Dropout())
        self.fc5 = nn.Sequential(nn.Linear(hidden_channels, out_channels))

    def forward(self, x: torch.Tensor): 
        return self.fc5(self.fc4(self.fc3(self.fc2(self.fc1(self.flatten(x))))))


class FCNN_Resnet(nn.Module):
    def __init__(self, in_channels: int, hidden_channels: int, out_channels: int):
        class ResidualBlock(nn.Module):
            def __init__(self, in_channels, out_channels):
                super().__init__()
                self.fc1 = nn.Linear(in_channels, out_channels)
                self.batchnorm = nn.BatchNorm1d(out_channels)
                self.relu = nn.ReLU()
            def forward(self, x):
                return self.relu(self.batchnorm(self.fc1(x)))+x
        super().__init__()
        self.flatten = nn.Flatten()
        self.fc1 = nn.Sequential(nn.Linear(in_channels, hidden_channels), nn.ReLU())
        self.fc2 = ResidualBlock(hidden_channels, hidden_channels)
        self.fc3 = ResidualBlock(hidden_channels, hidden_channels)
        self.fc4 = ResidualBlock(hidden_channels, hidden_channels)
        self.fc5 = nn.Sequential(nn.Linear(hidden_channels, out_channels))

    def forward(self, x: torch.Tensor): 
        return self.fc5(self.fc4(self.fc3(self.fc2(self.fc1(self.flatten(x))))))
    

class CNN(nn.Module): 
    def __init__(self, in_channels, out_channels):
        super().__init__()
        self.conv1 = nn.Sequential(nn.Conv2d(3,16,3,1,1),
                                   nn.BatchNorm2d(16),
                                   nn.ReLU())
        self.maxpool1 = nn.MaxPool2d(2,2)
        self.conv2 = nn.Sequential(nn.Conv2d(16,32,3,1,1),
                                   nn.BatchNorm2d(32),
                                   nn.ReLU())
        self.maxpool2 = nn.MaxPool2d(2,2)
        self.conv3 = nn.Sequential(nn.Conv2d(32,64,3,1,1),
                                   nn.BatchNorm2d(64),
                                   nn.ReLU())
        self.maxpool3 = nn.MaxPool2d(2,2)
        self.conv4 = nn.Sequential(nn.Conv2d(64,128,3,1,1),
                                   nn.BatchNorm2d(128),
                                   nn.ReLU())
        self.maxpool4 = nn.MaxPool2d(2,2)
        self.flatten = nn.Flatten()
        self.fc1 = nn.Sequential(nn.Linear(128 * 2 * 2, 512),
                                 nn.Dropout(),
                                 nn.ReLU())
        self.fc2 = nn.Sequential(nn.Linear(512, out_channels))

    def forward(self, x):
        x = self.conv1(x)
        x = self.maxpool1(x)
        x = self.conv2(x)
        x = self.maxpool2(x)
        x = self.conv3(x)
        x = self.maxpool3(x)
        x = self.conv4(x)
        x = self.maxpool4(x)
        x = self.flatten(x)
        x = self.fc1(x)
        x = self.fc2(x)

        return x
        



def train(model, optimizer, scheduler, args,epoches):
    '''
    Model training function
    input: 
        model: linear classifier or full-connected neural network classifier
        loss_function: Cross-entropy loss
        optimizer: Adamw or SGD
        scheduler: step or cosine
        args: configuration
    '''
    # create dataset
    train_data = torchvision.datasets.CIFAR10(root='./data', train=True, download=True, transform=transforms.ToTensor())
    test_data = torchvision.datasets.CIFAR10(root='./data', train=False, download=True, transform=transforms.ToTensor())
    # create dataloader
    train_loader = torch.utils.data.DataLoader(train_data, batch_size=64, shuffle=True)
    test_loader = torch.utils.data.DataLoader(test_data, batch_size=64, shuffle=False)

    training_loss = []
    test_loss = []
    training_accuracy = []
    test_accuracy = []
    for epoch in range(epoches):
        model.train()
        l = 0
        accuracy = 0
        total_length = 0
        for i, data in enumerate(train_loader, 0):
            x,y = data
            x = x.to(device)
            y = y.to(device)
            optimizer.zero_grad()
            output = model(x)
            loss = nn.CrossEntropyLoss()(output, y)
            loss.backward()
            l += loss.item()
            optimizer.step()
            accuracy += (output.argmax(1) == y).sum().item()
            total_length += y.size(0)
            if i % 2000 == 1999:
                print(f'[{epoch + 1}, {i + 1}] loss: {loss.item()}')
        training_loss.append(l / len(train_loader))
        training_accuracy.append(accuracy / total_length)
        print(f'Epoch{epoch + 1},Training Loss: {l / len(train_loader)}, Training Accuracy: {accuracy / total_length}')
        scheduler.step()
        with torch.no_grad():
            model.eval()
            correct = 0
            total = 0
            loss = 0
            for data in test_loader:
                x,y = data
                x = x.to(device)
                y = y.to(device)
                output = model(x)
                _, predicted = torch.max(output.data, 1)
                total += y.size(0)
                correct += (predicted == y).sum().item()
                loss += nn.CrossEntropyLoss()(output, y).item()
        test_loss.append(loss / len(test_loader))
        test_accuracy.append(correct / total)
        writer.add_scalars(f'{args.model}_{args.optimizer}_{args.scheduler}/loss', {'train': l / len(train_loader), 'test': loss / len(test_loader)}, epoch)
        writer.add_scalars(f'{args.model}_{args.optimizer}_{args.scheduler}/accuracy', {'train': accuracy / total_length, 'test': correct / total}, epoch)
        print(f'Epoch{epoch + 1},Test Loss: {loss / len(test_loader)}, Test Accuracy: {correct / total}')
    plt.plot(training_loss, label='Training Loss')
    plt.plot(test_loss, label='Test Loss')
    plt.legend()    
    plt.savefig(f"pic/{args.model}_{args.optimizer}_{args.scheduler}_loss.png")
    plt.cla()
    plt.plot(training_accuracy, label='Training Accuracy')
    plt.plot(test_accuracy, label='Test Accuracy')
    plt.legend()
    plt.savefig(f"pic/{args.model}_{args.optimizer}_{args.scheduler}_accuracy.png")

    torch.save(model.state_dict(), f"models/{args.model}_{args.optimizer}_{args.scheduler}.pth")
    # save checkpoint (Tutorial: https://pytorch.org/tutorials/recipes/recipes/saving_and_loading_a_general_checkpoint.html)

def test(model, args):
    '''
    input: 
        model: linear classifier or full-connected neural network classifier
        loss_function: Cross-entropy loss
    '''
    model = torch.load(f'models/{args.model}_{args.optimizer}_{args.scheduler}.pth', weights_only=False)
    test_data = torchvision.datasets.CIFAR10(root='./data', train=False, download=True, transform=transforms.ToTensor())
    test_loader = torch.utils.data.DataLoader(test_data, batch_size=64, shuffle=False)
    with torch.no_grad():
        model.eval()
        correct = 0
        total = 0
        loss = 0
        for data in test_loader:
            x,y = data
            x = x.to(device)
            y = y.to(device)
            output = model(x)
            _, predicted = torch.max(output.data, 1)
            total += y.size(0)
            correct += (predicted == y).sum().item()
            loss += nn.CrossEntropyLoss()(output, y).item() 
        print(f"Accuracy: {correct / total}")
    # load checkpoint (Tutorial: https://pytorch.org/tutorials/recipes/recipes/saving_and_loading_a_general_checkpoint.html)
    # create testing dataset
    # create dataloader
    # test
        # forward
        # calculate accuracy

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='The configs')
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

    parser.add_argument('--run', type=str, default='train')
    parser.add_argument('--model', type=str, default='linear')
    parser.add_argument('--optimizer', type=str, default='adamw')
    parser.add_argument('--scheduler', type=str, default='step')
    epoches=40
    args = parser.parse_args()
    print(f"model: {args.model}, optimizer: {args.optimizer}, scheduler: {args.scheduler}")

    # create model
    if args.model == 'linear':
        model = LinearClassifier(3072, 10)
    elif args.model == 'fcnn':
        args.model = 'fcnn_large'
        model = FCNN(3072, 3072 * 4, 10)
    elif args.model == 'cnn':
        model = CNN(3, 10)
    elif args.model == 'fcnn_resnet':
        model = FCNN_Resnet(3072, 3072, 10)
    else: 
        raise AssertionError
    
    model.to(device)
    # create optimizer
    if args.optimizer == 'adamw':
        # create Adamw optimizer
        optimizer = torch.optim.AdamW(model.parameters(), lr=1e-3,weight_decay=1e-4)
    elif args.optimizer == 'sgd':
        # create SGD optimizer
        optimizer = torch.optim.SGD(model.parameters(), lr=1e-3)
    else:
        raise AssertionError
    
    # create scheduler
    if args.scheduler == 'step':
        # create torch.optim.lr_scheduler.StepLR scheduler
        scheduler = torch.optim.lr_scheduler.StepLR(optimizer, step_size=epoches / 10, gamma=0.5)
    elif args.scheduler == 'cosine':
        # create torch.optim.lr_scheduler.CosineAnnealingLR scheduler
        scheduler = torch.optim.lr_scheduler.CosineAnnealingLR(optimizer, T_max=epoches)
    else:
        raise AssertionError

    if args.run == 'train':
        train(model, optimizer, scheduler, args,epoches)
    elif args.run == 'test':
        test(model, args)
    else: 
        raise AssertionError
    
# You need to implement training and testing function that can choose model, optimizer, scheduler and so on by command, such as:
# python main.py --run=train --model=fcnn --optimizer=adamw --scheduler=step


