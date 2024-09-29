import numpy as np
from PIL import Image

def cross_correlation_2d(I1, I2, position):
    height, width,_ = I1.shape
    kernel_height, kernel_width = I2.shape
    x, y = position
    res = [0,0,0]

    # use almost no numpy, soooo slow
    # for dx in range(kernel_width):
    #     if x + dx >= width:
    #         break # actually,it means padding outsider with 0 hhh
    #     for dy in range(kernel_height):
    #         if y + dy >= height:
    #             break
    #         res += I1[y + dy, x + dx] * I2[dy, dx]

    for color in range(3):
        region = I1[y:y+kernel_height, x:x+kernel_width,color]
        res[color] = np.sum(region * I2)
    
    return res

# def convolve_2d(I1, I2, stride = 1):
#     height, width,_ = I1.shape
#     kernel_height, kernel_width = I2.shape
#     I3 = np.zeros(((height - kernel_height) // stride + 1, (width - kernel_width)// stride + 1,3))
#     for y in range(0,height - kernel_height,stride):
#         for x in range(0,width - kernel_width,stride):
#             I3[y // stride, x // stride] = cross_correlation_2d(I1, I2, (x, y))
#     return I3

from numpy.lib.stride_tricks import as_strided
# 
# def convolve_2d(I1, I2, stride=1):
#     height, width, _ = I1.shape
#     kernel_height, kernel_width = I2.shape
# 
#     # 计算输出图像的尺寸
#     output_height = (height - kernel_height) // stride + 1
#     output_width = (width - kernel_width) // stride + 1
#     
#     # 创建一个视图，表示滑动窗口
#     strided_shape = (output_height, output_width, kernel_height, kernel_width, 3)
#     strided_strides = (I1.strides[0] * stride, I1.strides[1] * stride, I1.strides[0], I1.strides[1], I1.strides[2])
#     
#     # 使用 as_strided 创建滑动窗口
#     I1_strided = as_strided(I1, shape=strided_shape, strides=strided_strides)
# 
#     I3 = np.zeros((output_height, output_width, 3), dtype=I1.dtype)
# 
#     # 对每个位置计算卷积
#     for y in range(output_height):
#         for x in range(output_width):
#             I3[y, x] = cross_correlation_2d(I1_strided[y, x], I2,(0,0)) 
# 
#     return I3

def convolve_2d(I1, I2, stride=1):
    height, width, channels = I1.shape
    I2 = np.stack([I2]*3, axis=2)
    kernel_height, kernel_width, _ = I2.shape  # 确保 I2 是三通道的

    # 计算输出图像的尺寸
    output_height = (height - kernel_height) // stride + 1
    output_width = (width - kernel_width) // stride + 1
    
    # 创建一个视图，表示滑动窗口
    strided_shape = (output_height, output_width, kernel_height, kernel_width, channels)
    strided_strides = (I1.strides[0] * stride, I1.strides[1] * stride, I1.strides[0], I1.strides[1], I1.strides[2])
    
    # 使用 as_strided 创建滑动窗口
    I1_strided = as_strided(I1, shape=strided_shape, strides=strided_strides)

    # 初始化输出图像，保持三维
    I3 = np.zeros((output_height, output_width, channels), dtype=I1.dtype)

    # 对每个通道进行卷积
    for c in range(channels):
        I3[:, :, c] = np.tensordot(I1_strided[..., c], I2[..., c], axes=([2, 3], [0, 1]))

    return I3  # 输出为 (output_height, output_width, channels)


def gaussian_blur_kernel_2d(size, sigma=1.0):
    if size % 2 == 0:
        raise ValueError('size should be odd')
    kernel = np.zeros((size, size))
    center = size // 2
    for i in range(size):
        for j in range(size):
            kernel[i, j] = np.exp(-0.5 * (np.square(i - center) + np.square(j - center)) / np.square(sigma))
    return kernel / np.sum(kernel)

def low_pass(img, ker_size, sigma):
    ker = gaussian_blur_kernel_2d(ker_size, sigma)
    return convolve_2d(img, ker,1)

def image_subsampling(img):
    return img[::2, ::2]

def gaussian_pyramid(img, level, sigma, ker_size):
    res = [img]
    if level != len(sigma) or level != len(ker_size):
        raise ValueError('level should be equal to Vangoghsigma) and Vangoghker_size)')
    for i in range(level):
        res.append(image_subsampling(low_pass(res[-1], ker_size[i], sigma[i])))
    return res

img = Image.open('imgs/Vangogh.png')
img = img.convert('RGB')
img = np.array(img)
#ker = np.array([[1,2,1],[2,4,2],[1,2,1]]) / 16
ker = gaussian_blur_kernel_2d(5)
#img = convolve_2d(img, ker)
#Image.fromarray(img.astype(np.uint8)).save(f'imgs/Vangogh_conv.png')
#img = low_pass(img, 5, 1)
#img = image_subsampling(img)
    

sigma = [1, 1, 1]
ker_size = [5, 5, 5]

img = gaussian_pyramid(img, 3, sigma, ker_size)

for i in range(4):
    Image.fromarray(img[i].astype(np.uint8)).save(f'imgs/Vangogh_conv/Vangogh_conv{i}.png')

# 验证一下效果
# 下面用最临近插值看看效果

for i in range(4):
    sz0 = img[i].shape[0]
    sz1 = img[i].shape[1]
    image = Image.fromarray(img[i].astype(np.uint8))
    image = image.resize((sz1, sz0), Image.NEAREST)
    image.save(f'imgs/Vangogh_resize/Vangogh_resize{i}.png')

# 试试直接downsample再插值看看效果
iimg = img[0]
sz = list(img[0].shape)
for i in range(1,4):
    iimg = image_subsampling(iimg)
    Image.fromarray(iimg.astype(np.uint8))\
        .resize((sz[1], sz[0]), Image.NEAREST)\
        .save(f'imgs/Vangogh_downsample/Vangogh_downsample{i}.png')
    sz[0] //= 2
    sz[1] //= 2
