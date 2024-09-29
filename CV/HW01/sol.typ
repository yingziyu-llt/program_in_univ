#import "@local/PKU-Typst-Template:0.1.0": 通用作业

#show: 通用作业.config.with(
  course_name: "计算机视觉",
  student_id: 2300012154,
  student_name: "林乐天",
  hw_no: 1,
)
#set text(size: 20pt)
= Short questions
#set text(size: 14pt)
#set par(justify: true,first-line-indent: 2em)
#show heading: it =>  {
    it
    par()[#text()[#h(0.0em)]]
}
#通用作业.hw_prob("Homogeneous coordination")[
  In the Cartesian coordinate system, a point in $N$ -dimensional space is represented by $N$ coordinates as $P = (x_1, x_2, . . . , x_N )$. In homogeneous coordinates, the same point is represented with $N + 1$ coordinates as $P' = (x_1, x_2, . . . , x_N , w)$, where $w$ is a non-zero real number representing the weight or scale of the point. For simplicity, $P ′$ can be rewritten as $(x_1', x_2', . . . , x_N' , 1)$

Please:
+ Derive the mathematical relationship between $x_i$ and $x'_i$. (3 points)
+ Explain at least two advantages of using homogeneous coordinates, and why they are adopted in computational photography. (7 points)
][
  #set enum(numbering: "(1)")
  + For $x_i$,it equals to $x'_i/w$. So the relationship is $x_i = x'_i/w$.
  + Advantages:
    - It makes it easier to perform geometric transformations like translation, for we can simply multiply it with a matrix $ mat(I_"3x3",T;0,1) $
    - It can show points in infinity, for it can represent points at infinity by setting the $w$ coordinate to 0.
    - It enables the processing of lines and points in the same way, as it is more convenient to deal with.
]
#通用作业.hw_prob("Dolly zoom")[



A dolly zoom (also known as a Hitchcock shot or Vertigo shot) is an in-camera effect that appears to undermine normal visual perception. This technique can produce striking effects, where the background appears to expand rapidly in size and detail, overwhelming the foreground, or where the foreground swells to dominate its surroundings, depending on how the dolly zoom is performed. Figure 1 illustrates aclassic example of this effect. *Please explain the mechanics behind how this effect is achieved*.][  
  The dolly zoom effect is achieved by moving the camera while adjusting the focal length of the lens.

  During the process of moving the camera forward, adjust the focal length to zoom out, ensuring that the subject remains in focus while the field of view continually changes,and the main character's size remains constant. This creates the illusion of the background expanding and the foreground shrinking, creating an artistic effect of disorientation.
]

#通用作业.hw_prob("Camera parameters from the image")[Multiple sets of parallel structures on the spatial horizontal plane can provide
geometric information about the camera or scene, such as camera pose and object
heights.

#figure(
  image("imgs/campus.jpg"),
  caption: "A figure of our campus is provided. Your task is to calculate the camera’s height and focal length using the information from this image")
#par()[#text()[#h(0.0em)]]
Figure 2 is a picture with 4096 × 3072 pixels of our campus. We can establish a
right-handed world coordinate system where the ground is the $z = 0$ plane, the edge of
the roadside lawn is the x-axis, and the edge below the flowerbed serves as the y-axis.
All measurements are in meters (m). The height of the first flowerbed (red rectangle)
is 0.76 meters, and the camera’s coordinates are (13.4, 4.5) in the x and y directions.
Additionally, the edge point A of the hexagonal flowerbed has coordinates (0, 3.74, 0.76)
in the world coordinate system.

Please:
- *Find the vanishing line and calculate the camera height $H$. (10 points)*
- *Write down the intrinsic parameter matrix and derive the camera focal
length $f$ .(15 points)*][
  === Problem 1
  ==== Sub-problem 1
  Firstly we need to find the vanishing line. A easy idea is to find two vanishing points on the vanishing line and then find the line between them. However, there is no two vanishing points that easily to be find.Therefore, we firstly find a vanishing point. We find several parallel lines(drawn in Fig 2) and find their section point. After that, we can know that the line in the middle of road and the line are parallel and they are parallel in the image. So the vanishing line is parallel with it. The figure is shown as below.
#figure(
  image("imgs/campus_edited.jpg"),
  caption: "The vanishing line is shown as the red line in the image")
#par()[#text()[#h(0.0em)]]
  ==== Sub-problem 2
  For problem 1 sub-problem 2,we need to estimate the height of the camera. We can use the vanishing line and the height of the flowerbed to estimate the height of the camera.
#figure(image("imgs/campus_edited_2.png"),
  caption: "find the height of camera")
#par()[#text()[#h(0.0em)]]
  By graphic editing tool GIMP, we can find that the height of flowerbed measures 606 pixels on the black line, and the height of the camera (aka. the section of orange line and black line) measures 1334 pixels in black line.So, we can know that the height of the camera is 1334/606*0.76=1.63m.

  === Problem 2
  First,we need to represent the intrinsic matrix $ K = mat(f, 0, c_x; 0, f, c_y; 0, 0, 1) $

  $ c_x = 4096/2 = 2048 $,$ c_y = 3072/2 = 1536 $,$ f_x = (Z_c ​⋅( u-c_x)) / X_w $
  I don't really understand the next step...
]

== Problem 3 Image filtering and subsampling 解题报告

这道题让我们实现卷积、高斯模糊、低通滤波、图像下采样、高斯金字塔
#figure(image("imgs/Lena.png"), caption: "Original image")
#par()[#text()[#h(0.0em)]]
=== 整体思路

+ 实现卷积函数，用于计算图像的卷积结果
+ 实现高斯模糊函数，计算出对应大小的高斯核
+ 实现低通滤波函数，利用高斯模糊函数产生的高斯核，对图像进行卷积操作
+ 实现图像下采样函数
+ 利用上述函数，对图像进行高斯模糊$->$下采样，重复以上操作，获得我们要求的高斯金字塔

具体实现如下。

=== 卷积函数

先实现`cross_correlation_2d`函数。该函数用于计算图像的卷积结果。一个显然的实现是这样的：

```python
def cross_correlation_2d(I1, I2, position):
    height, width,_ = I1.shape
    kernel_height, kernel_width = I2.shape
    x, y = position
    res = [0,0,0]

    for dx in range(kernel_width):
        if x + dx >= width:
            break # actually,it means padding outsider with 0 hhh
        for dy in range(kernel_height):
            if y + dy >= height:
                break
            res += I1[y + dy, x + dx] * I2[dy, dx]
```

但是，这个函数实在是太慢了。对于`Lena.png`做金字塔时，它需要运行`10s`。我们使用numpy来优化他。具体实现如下：  

```python
def cross_correlation_2d(I1, I2, position):
    height, width,_ = I1.shape
    kernel_height, kernel_width = I2.shape
    x, y = position
    res = [0,0,0]
    for color in range(3):
        region = I1[y:y+kernel_height, x:x+kernel_width,color]
        res[color] = np.sum(region * I2)
    
    return res
```

这里的region指的是需要进行一次卷积操作的小区域，直接将其与卷积核逐元素相乘，然后求和，即可得到卷积结果。

下面进行卷积操作，最显然的方法就是按照定义进行。

```python
def convolve_2d(I1, I2, stride = 1):
    height, width,_ = I1.shape
    kernel_height, kernel_width = I2.shape
    I3 = np.zeros(((height - kernel_height) // stride + 1, (width - kernel_width)// stride + 1,3))
    for y in range(0,height - kernel_height,stride):
        for x in range(0,width - kernel_width,stride):
            I3[y // stride, x // stride] = cross_correlation_2d(I1, I2, (x, y))
    return I3
```

没什么技巧。

但是还是有些慢，其核心问题在于循环耗时太久，可不可以用`numpy`加速？查阅资料，得知可以使用滑动窗口方法，将原本矩阵变成一系列小矩阵来计算，可以提高一定的运行效率。具体代码如下

```python
from numpy.lib.stride_tricks import as_strided

def convolve_2d(I1, I2, stride=1):
    height, width, _ = I1.shape
    kernel_height, kernel_width = I2.shape
    strided_shape = (output_height, output_width, kernel_height, kernel_width, 3)
    strided_strides = (I1.strides[0] * stride, I1.strides[1] * stride, I1.strides[0], I1.strides[1], I1.strides[2])
    I3 = np.zeros(((height - kernel_height) // stride + 1, (width - kernel_width)// stride + 1,3))
    I1_strided = as_strided(I1, shape=strided_shape, strides=strided_strides)
    for y in range(output_height):
        for x in range(output_width):
            I3[y, x] = cross_correlation_2d(I1_strided[y, x], I2,(0,0)) 
    return I3
```

但是经过测试，发现其运行速度并没有提升，反而更慢了。分析：`as_strided`函数将原本的矩阵变成一系列小矩阵，但是实际计算的时候还是遍历所有小矩阵，实际上和前一种方法完全一致，还是要遍历所有元素，所以速度并没有提升。

询问ChatGPT后得知，有函数`np.tensordot`可以有效提升效率，下面的这段代码由ChatGPT提供后我将其调整至和我的整体代码契合

```python
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
```

其整体过程如下：

+ 将卷积核扩展为三通道
+ 计算输出图像的尺寸
+ 创建一个视图，表示滑动窗口
+ 使用`np.tensordot`进行卷积操作,其中，axes=([2, 3], [0, 1])表示将I1_strided的后两个维度和I2的前两个维度进行卷积操作，实际上就是将每个做完滑动窗口的小矩阵和卷积核做哈密尔顿乘后再相加，即卷积操作。

经过测试，发现其运行速度显著提升，高斯金字塔时间从5s压缩到1s以内，可以满足要求。

=== 高斯模糊核

利用高斯分布的公式$ G(x,y) = 1/(2pi sigma^2) * e^(-(x^2+y^2)/(2sigma^2)) $我们可以构造出需要的高斯核。注意到$1/(2pi sigma^2)$实际上是归一化系数，所以在实际计算高斯核的时候，可以先不去管他，只需要计算$e^(-(x^2+y^2)/(2sigma^2))$，最后再除以所有元素之和即可。我们将要构造的高斯核的中心点设置为原点，即$G(0,0)$，构造出整个高斯核。具体代码如下：

```python
def gaussian_blur_kernel_2d(size, sigma=1.0):
    if size % 2 == 0:
        raise ValueError('size should be odd')
    kernel = np.zeros((size, size))
    center = size // 2
    for i in range(size):
        for j in range(size):
            kernel[i, j] = np.exp(-0.5 * (np.square(i - center) + np.square(j - center)) / np.square(sigma))
    return kernel / np.sum(kernel)
```

=== 低通滤波

低通滤波就是将图像中的高频成分去除，保留低频成分。我们可以使用高斯核进行低通滤波，具体代码如下：

```python
def low_pass(img, ker_size, sigma):
    ker = gaussian_blur_kernel_2d(ker_size, sigma)
    return convolve_2d(img, ker,1)
```

=== 图像下采样

图像下采样就是将图像的尺寸缩小，具体代码如下：

```python
def image_subsampling(img):
    return img[::2, ::2]
```

=== 高斯金字塔

只需要重复low_pass和image_subsampling操作即可，具体代码如下：

```python
def gaussian_pyramid(img, level, sigma, ker_size):
    res = [img]
    if level != len(sigma) or level != len(ker_size):
        raise ValueError('level should be equal to len(sigma) and len(ker_size)')
    for i in range(level):
        res.append(image_subsampling(low_pass(res[-1], ker_size[i], sigma[i])))
    return res
```

== 效果分析与成果展示

效率：对于Lena和Vangogh两张图片，做4层高斯金字塔耗时0.580s和0.648s(测试方法：time python gaussian_pyramid.py)，效率可观。

就图像质量而言，我认为保留信息效果很好。为了证明这一观点，我将对比高斯金字塔卷积后再通过最近邻插值恢复的图像和原图像，以及直接进行下采样并通过最近邻插值恢复的图像来证明其效果的优异性。参数：sigma = [1,1,1], ker_size = [5, 5, 5]

对于Lena图像，其效果如下：

#figure(
  table(columns: 3)[#image("imgs/Lena_resize/Lena_resize1.png")][#image("imgs/Lena.png")][#image("imgs/Lena_downsample/Lena_downsample2.png")],
  caption: "Lena图像高斯金字塔卷积后再通过最近邻插值恢复的图像，中间为原图，左边为高斯金字塔卷积后再通过最近邻插值恢复的图像，右边为直接下采样后再通过最近邻插值恢复的图像"
)

对于Vangogh图像，其效果更加明显


#figure(
  table(columns: 3)[#image("imgs/Vangogh_resize/Vangogh_resize1.png")][#image("imgs/Vangogh.png")][#image("imgs/Vangogh_downsample/Vangogh_downsample2.png")], caption: "Vangogh图像高斯金字塔卷积后再通过最近邻插值恢复的图像，中间为原图，左边为高斯金字塔卷积后再通过最近邻插值恢复的图像，右边为直接下采样后再通过最近邻插值恢复的图像"
)