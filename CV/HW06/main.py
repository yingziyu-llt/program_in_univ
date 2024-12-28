import torch
import torch.nn.functional as F
from models import VGG, ResNet, ResNext
from torchvision import datasets, transforms
from torch.utils.data import DataLoader
from torch.utils.tensorboard import SummaryWriter
import matplotlib.pyplot as plt
import numpy as np
torch.manual_seed(0)
torch.backends.cudnn.deterministic = True
torch.backends.cudnn.benchmark = False
np.random.seed(0)

device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

def train(model, args):
    '''
    Model training function
    input: 
        model: linear classifier or full-connected neural network classifier
        args: configuration
    '''
    # create dataset, data augmentation
    train_transform = transforms.Compose([
        transforms.RandomHorizontalFlip(),
        transforms.ColorJitter(brightness=0.1, contrast=0.1, saturation=0.1, hue=0.1),
        transforms.RandomRotation(20),
        transforms.RandomResizedCrop(32, scale=(0.8, 1.0)),
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])
    ])
    validation_transform = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])
    ])
    dataset = datasets.CIFAR10(root='./data', train=True, download=True)
    train_dataset, validation_dataset = torch.utils.data.random_split(dataset, [int(len(dataset) * 0.9), int(len(dataset) * 0.1)])
    class Datasets(torch.utils.data.Dataset):
        def __init__(self, dataset, transform=None):
            self.dataset = dataset
            self.transform = transform
        def __len__(self):
            return len(self.dataset)
        def __getitem__(self, idx):
            image, label = self.dataset[idx]
            if self.transform:
                image = self.transform(image)
            return image, label
    train_dataset = Datasets(train_dataset, train_transform)
    validation_dataset = Datasets(validation_dataset, validation_transform)
    # create dataloader
    train_loader = DataLoader(train_dataset, batch_size=256, shuffle=True,num_workers=16)
    test_loader = DataLoader(validation_dataset, batch_size=256, shuffle=False,num_workers=16)
    epochs = 50

    # create optimizer
    optimizer = torch.optim.AdamW(model.parameters(), lr=0.001,weight_decay=1e-4)
    # create scheduler 
    scheduler = torch.optim.lr_scheduler.CosineAnnealingLR(optimizer, T_max=epochs)
    # create summary writer
    writer = SummaryWriter()
    training_loss = []
    training_accuracy = []
    testing_loss = []
    testing_accuracy = []
        # train
    for epoch in range(epochs):
        model.train()
        curr_loss = 0
        correct = 0
        total = 0
        for i, (inputs, labels) in enumerate(train_loader):
            inputs, labels = inputs.to(device), labels.to(device)
            optimizer.zero_grad()
            outputs = model(inputs)
            loss = F.cross_entropy(outputs, labels,label_smoothing=0.1)
            loss.backward()
            optimizer.step()
            curr_loss += loss.item()
            _, predicted = outputs.max(1)
            total += labels.size(0)
            correct += predicted.eq(labels).sum().item()
            if i % 100 == 0:
                print(f'Epoch {epoch + 1}/{epochs}, Iteration {i}/{len(train_loader)}, Loss: {loss.item()}, Accuracy: {correct / total}')
        training_loss.append(curr_loss / len(train_loader))
        training_accuracy.append(correct / total)
        with torch.no_grad():
            curr_loss = 0
            correct = 0
            total = 0
            model.eval()
            for i, (inputs, labels) in enumerate(test_loader):
                inputs, labels = inputs.to(device), labels.to(device)
                outputs = model(inputs)
                loss = F.cross_entropy(outputs, labels)
                curr_loss += loss.item()
                _, predicted = outputs.max(1)
                total += labels.size(0)
                correct += predicted.eq(labels).sum().item()
            testing_loss.append(curr_loss / len(test_loader))
            testing_accuracy.append(correct / total)
        scheduler.step()
        writer.add_scalars(f'{args.model}_Loss', {'train': training_loss[-1], 'test': testing_loss[-1]}, epoch + 1)
        writer.add_scalars(f'{args.model}_Accuracy', {'train': training_accuracy[-1], 'test': testing_accuracy[-1]}, epoch + 1)
        print(f'Epoch {epoch + 1}/{epochs}, Training Loss: {training_loss[-1]}, Training Accuracy: {training_accuracy[-1]}, Validation Loss: {testing_loss[-1]}, Validation Accuracy: {testing_accuracy[-1]}')
    torch.save(model.state_dict(), f'models/{args.model}_checkpoint.pth')
    writer.close()
    plt.plot(training_loss, label='train')
    plt.plot(testing_loss, label='test')
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.title(f'{args.model} Loss')
    plt.legend()
    plt.savefig(f'{args.model}_loss.png')
    plt.close()
    plt.cla()
    plt.plot(training_accuracy, label='train')
    plt.plot(testing_accuracy, label='test')
    plt.xlabel('Epoch')
    plt.ylabel('Accuracy')
    plt.title(f'{args.model} Accuracy')
    plt.legend()
    plt.savefig(f'{args.model}_acc.png')
    plt.close()
    # save checkpoint (Tutorial: https://pytorch.org/tutorials/recipes/recipes/saving_and_loading_a_general_checkpoint.html)


def test(model, args):
    '''
    input: 
        model: linear classifier or full-connected neural network classifier
        args: configuration
    '''
    model.load_state_dict(torch.load(f'models/{args.model}_checkpoint.pth'))
    test_transform = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])
    ])
    test_dataset = datasets.CIFAR10(root='./data', train=False, download=True, transform=test_transform)
    test_loader = DataLoader(test_dataset, batch_size=32, shuffle=False)
    correct = 0
    total = 0
    with torch.no_grad():
        model.eval()
        for i, (inputs, labels) in enumerate(test_loader):
            inputs, labels = inputs.to(device), labels.to(device)
            outputs = model(inputs)
            _, predicted = outputs.max(1)
            total += labels.size(0)
            correct += predicted.eq(labels).sum().item()
    print(f'Accuracy of the network on the 10000 test images: {100 * correct / total}%')

if __name__ == '__main__':
    import argparse
    
    parser = argparse.ArgumentParser(description='The configs')
    parser.add_argument('--model', type=str, default='VGG')
    parser.add_argument('--run', type=str, default='train')
    args = parser.parse_args()

    if args.model == 'VGG':
        model = VGG()
    elif args.model == 'ResNet':
        model = ResNet()
    elif args.model == 'ResNext':
        model = ResNext()
    model = model.to(device)
    # train / test
    if args.run == 'train':
        train(model, args)
    else:
        test(model, args)
