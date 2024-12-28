import torch
import torch.nn as nn
import torch.nn.functional as F


class VGG(nn.Module):
    def __init__(self):
        super().__init__()
        def conv_block(in_channels, out_channels):
            layers = [nn.Conv2d(in_channels, out_channels, kernel_size=3, padding=1),nn.BatchNorm2d(out_channels), nn.ReLU(inplace=True)]
            return nn.Sequential(*layers)
        # 1. define multiple convolution and downsampling layers
        # 2. define full-connected layer to classify
        self.block1 = nn.Sequential(conv_block(3,64),nn.MaxPool2d(2,2))
        self.block2 = nn.Sequential(conv_block(64,128),nn.MaxPool2d(2,2))
        self.block3 = nn.Sequential(conv_block(128,256),conv_block(256,256),nn.MaxPool2d(2,2))
        self.block4 = nn.Sequential(conv_block(256,512),conv_block(512,512),nn.MaxPool2d(2,2))
        self.block5 = nn.Sequential(conv_block(512,512),conv_block(512,512),nn.MaxPool2d(2,2))
        self.fc1 = nn.Sequential(nn.Linear(512,4096),nn.Dropout(),nn.ReLU(inplace=True))
        self.fc2 = nn.Sequential(nn.Linear(4096,4096),nn.Dropout(),nn.ReLU(inplace=True))
        self.fc3 = nn.Linear(4096,10)

    def forward(self, x: torch.Tensor):
        # x: input image, shape: [B * C * H* W]
        # extract features
        # classification
        x = self.block1(x)
        x = self.block2(x)
        x = self.block3(x)
        x = self.block4(x)
        x = self.block5(x)
        x = x.view(x.size(0), -1)
        x = self.fc1(x)
        x = self.fc2(x)
        x = self.fc3(x)
        return x


class ResBlock(nn.Module):
    ''' residual block'''
    def __init__(self, in_channel, out_channel, stride):
        super().__init__()
        '''
        in_channel: number of channels in the input image.
        out_channel: number of channels produced by the convolution.
        stride: stride of the convolution.
        '''
        # 1. define double convolution
             # convolution
             # batch normalization
             # activate function
             # ......
        self.conv1 = nn.Sequential(nn.Conv2d(in_channel, out_channel, kernel_size=3, stride=stride, padding=1, bias=False),
                                    nn.BatchNorm2d(out_channel),
                                    nn.ReLU(inplace=True),
                                    nn.Dropout(0.2),
                                    nn.Conv2d(out_channel, out_channel, kernel_size=3, stride=1, padding=1, bias=False),
                                    nn.BatchNorm2d(out_channel),
                                    )
        
        if in_channel != out_channel or stride != 1:
            self.conv2 = nn.Sequential(nn.Conv2d(in_channel, out_channel, kernel_size=1, stride=stride, bias=False),nn.BatchNorm2d(out_channel))
        else:
            self.conv2 = nn.Identity()

        # 2. if in_channel != out_channel or stride != 1, deifine 1x1 convolution layer to change the channel or size.

        # Note: we are going to implement 'Basic residual block' by above steps, you can also implement 'Bottleneck Residual block'

    def forward(self, x: torch.Tensor):
        # x: input image, shape: [B * C * H* W]
        # 1. convolve the input
        # 2. if in_channel != out_channel or stride != 1, change the channel or size of 'x' using 1x1 convolution.
        # 3. Add the output of the convolution and the original data (or from 2.)
        # 4. relu
        out = nn.ReLU()(self.conv1(x) + self.conv2(x))
        return out


class ResNet(nn.Module):
    '''residual network'''
    def make_layer(self, input_channels, num_channels, num_residuals, first_block=False):
        layers = []
        for i in range(num_residuals):
            if i == 0 and not first_block:
                layers.append(ResBlock(input_channels, num_channels, 2))
            else:
                layers.append(ResBlock(num_channels, num_channels, 1))
        return nn.Sequential(*layers)
    
    def __init__(self):
        super().__init__()

        # 1. define convolution layer to process raw RGB image
        # 2. define multiple residual blocks
        # 3. define full-connected layer to classify
        conv1 = nn.Sequential(nn.Conv2d(3, 64, kernel_size=7, stride=2, padding=3, bias=False),
                                    nn.BatchNorm2d(64),
                                    nn.ReLU(inplace=True),
                                    nn.Dropout(0.2),
                                    nn.MaxPool2d(kernel_size=3, stride=2, padding=1))
        block1 = self.make_layer(64,64,3,first_block=True)
        block2 = self.make_layer(64,128,4)
        block3 = self.make_layer(128,256,6)
        block4 = self.make_layer(256,512,3)
        fc1 = nn.Linear(512,10)
        self.net = nn.Sequential(conv1,block1,block2,block3,block4,nn.AdaptiveAvgPool2d((1,1)),nn.Flatten(),nn.Dropout(),fc1)

    def forward(self, x: torch.Tensor):
        # x: input image, shape: [B * C * H* W]
        # extract features
        # classification

        return self.net(x)
    
    

class ResNextBlock(nn.Module):
    '''ResNext block'''
    def __init__(self, in_channel, out_channel, bottle_neck, group, stride):
        super().__init__()
        # in_channel: number of channels in the input image
        # out_channel: number of channels produced by the convolution
        # bottle_neck: int, bottleneck= out_channel / hidden_channel 
        # group: number of blocked connections from input channels to output channels
        # stride: stride of the convolution.

        # 1. define convolution
             # 1x1 convolution
             # batch normalization
             # activate function
             # 3x3 convolution
             # ......
             # 1x1 convolution
             # ......
        self.conv1 = nn.Sequential(
            nn.Conv2d(in_channel, bottle_neck, kernel_size=1, stride=1, bias=False),
            nn.BatchNorm2d(bottle_neck),
            nn.ReLU(inplace=True),
            nn.Conv2d(bottle_neck, bottle_neck, kernel_size=3, stride=stride, padding=1, groups=group, bias=False),
            nn.BatchNorm2d(bottle_neck),
            nn.ReLU(inplace=True),
            nn.Conv2d(bottle_neck, out_channel, kernel_size=1, stride=1, bias=False),
            nn.BatchNorm2d(out_channel)
        )

        # 2. if in_channel != out_channel or stride != 1, deifine 1x1 convolution layer to change the channel or size.
        self.shortcut = nn.Sequential()
        if in_channel != out_channel or stride != 1:
            self.shortcut = nn.Sequential(
                nn.Conv2d(in_channel, out_channel, kernel_size=1, stride=stride, bias=False),  # 1x1 conv for shortcut
                nn.BatchNorm2d(out_channel)
            )
        else:
            self.shortcut = nn.Identity()
        self.dropout = nn.Dropout2d(0.1)


    def forward(self, x: torch.Tensor):
        # x: input image, shape: [B * C * H* W]
        # 1. convolve the input
        # 2. if in_channel != out_channel or stride != 1, change the channel or size of 'x' using 1x1 convolution.
        # 3. Add the output of the convolution and the original data (or from 2.)
        # 4. relu
        out = self.conv1(x)
        out = self.dropout(out)
        out += self.shortcut(x)
        out = F.relu(out)
        return out


class ResNext(nn.Module):
    def _make_layer(self,in_channel, out_channel, blocks, stride, groups):
        layers = []
        layers.append(ResNextBlock(in_channel, out_channel, bottle_neck=out_channel // 2, group=groups, stride=stride))
        for _ in range(1, blocks):
            layers.append(ResNextBlock(out_channel, out_channel, bottle_neck=out_channel // 2, group=groups, stride=1))
        return nn.Sequential(*layers)

    
    def __init__(self):
        super().__init__()
        # 1. define convolution layer to process raw RGB image
        # 2. define multiple residual blocks
        # 3. define full-connected layer to classify
        self.conv1 = nn.Sequential(nn.Conv2d(3, 64, kernel_size=7, stride=2, padding=3, bias=False),
                                    nn.BatchNorm2d(64),
                                    nn.ReLU(inplace=True),
                                    nn.MaxPool2d(kernel_size=3, stride=2, padding=1))
        self.block1 = self._make_layer(64,128,2,1,16)
        self.block2 = self._make_layer(128,256,2,2,16)
        self.block3 = self._make_layer(256,512,2,2,16)
        self.block4 = self._make_layer(512,1024,2,2,16)
        self.fc1 = nn.Sequential(nn.Dropout(0.5),nn.Linear(1024, 10))
    def forward(self, x: torch.Tensor):
        # x: input image, shape: [B * C * H* W]
        # extract features
        # classification
        x = self.conv1(x)
        x = self.block1(x)
        x = self.block2(x)
        x = self.block3(x)
        x = self.block4(x)
        x = F.avg_pool2d(x, x.size()[2:])
        x = x.view(x.size(0), -1)
        x = self.fc1(x)
        return x

