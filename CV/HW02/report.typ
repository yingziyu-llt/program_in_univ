#set text(font: "Times New Roman", size: 14pt)

= 计算机视觉 第二次作业汇报

== Implementation

容易观察到，输入给我们的图片是一个彩色图片（坑1），所以我们要先把这个图片变成黑白的再处理。又由于变成黑白的之后数据类型还是不方便后续处理，我们将其转化为np.array，类型为float32/float64.

=== 梯度

直接对转化后的图像先做高斯模糊，再做sobel滤波即可。根据实验结果，第一步高斯模糊的意义有限，反而会使得很多本来就不清晰的角点响应函数彻底变成0,于是在最后的实现时直接删去了这一步。

=== Harris Response Function

求出梯度后，计算得到harris响应函数需求的$I_x^2$,$I_y^2$和$I_x I_y$.为了得到$M$矩阵，我们还要对每个窗口分别对窗口内的三个函数求和。这里，我们可以发现一个优化，即这个求和等价于对图像做一个uniform filter。最后再做一个高斯滤波即可（题目要求）。

=== 角点选择

首先先做非极大值抑制，只需要对所有点做一个max_filter,size取min_dist，其原因是在min_dist距离下之可能有一个点，这个点一定是局部最大值。这个改动相较于将其设定为一个较小的固定值相比，运行时间从30s变成了23s.

再做阈值筛选，只保留大于阈值的响应。

再将其按照R的大小排序，做KDTree查询，就可以得到要求的角点.

=== HOG

先对整个图像求梯度，用欧几里得量度计量梯度的长度，用 `np.arctan2(grad_y , grad_x)`计量梯度方向。

对每个关键点所在的`32*32`区域做计算，划分为`4*4`的cell,每个cell大小为`8*8`，方向投影到9个方向上。使用`np.histogram`统计每个cell的方向分布作为这个cell的特征。

对全部cell的特征构成的特征做归一化。将对应角的特征加起来，取这里的最大值作为主方向。使用`np.roll`将这个主方向旋转到水平方向即可。

=== 计算homography

按照ppt上的流程计算即可。需要指出的是，我们这里的x和y坐标是反的，所以要交换一下。

=== RANSAC

随机选取4个点，计算H矩阵。用H矩阵对第一张图片的点做变换，求得的坐标和第二张图上的对应点计算距离，小于阈值则认为是正确的点。迭代5000次，选取最多的点再计算H矩阵作为真实H矩阵。

其余无改动

== 结果展示

#figure(image("match1.jpg"), caption: "图像匹配结果1")
#figure(image("match2.jpg"), caption: "图像匹配结果2")
#figure(image("match3.jpg"), caption: "图像匹配结果3")

#figure(image("outputs/panorama_1.jpg"), caption: "全景图1")
#figure(image("outputs/panorama_2.jpg"), caption: "全景图2(3张图)")
#figure(image("outputs/panorama_3.jpg"), caption: "全景图3")
#figure(image("outputs/panorama_4.jpg"), caption: "全景图4(3张图)")

对于library和雪山入口，程序表现较差，可能是照相机移动时不平稳、角度变化很大，使得图像发生了很大的扭曲造成的，这导致特征匹配的失败。

== 分析

对于超参选择而言，我发现对结果影响最严重的超参就是在做HOG时选取的cell数量、大小。通过分别独立调整cell size和block size，多次做图像拼接实验，可以发现，cell size,block size二者较小的时候，容易出现fail to match错误，即无法找到足够的点做匹配，而二者很大的时候，拼图耗时过久。权衡后，我选择使用前文提及的参数进行计算。

对于相机移动而言，我通过观察图片拼接后变形的情况来判断。容易看出，library和雪山两图在拍摄时移动不稳定，有上下移动和视角变化，这使得拼图失败。