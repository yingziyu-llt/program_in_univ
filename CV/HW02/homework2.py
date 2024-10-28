# This is a raw framework for image stitching using Harris corner detection.
# For libraries you can use modules in numpy, scipy, cv2, os, etc.
import numpy as np
from scipy import ndimage, spatial
import cv2
from os import listdir
import matplotlib.pyplot as plt

from numpy.lib.stride_tricks import as_strided
IMGDIR = 'Problem2Images'


def gradient_x(img):
    # convert img to grayscale
    # should we use int type to calculate gradient?
    # should we conduct some pre-processing to remove noise? which kernel should we apply?
    # which kernel should we choose to calculate gradient_x?
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    img = img.astype(np.float32)
    #img = ndimage.gaussian_filter(img, sigma=0.1).astype(np.float32)
    grad_x = ndimage.sobel(img, axis=1)
    grad_x = ndimage.gaussian_filter(grad_x, sigma=0.1).astype(np.float32)
    return grad_x

def gradient_y(img):
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    img = img.astype(np.float32)
    #img = ndimage.gaussian_filter(img, sigma=0.1).astype(np.float32)
    grad_y = ndimage.sobel(img, axis=0)
    grad_y = ndimage.gaussian_filter(grad_y, sigma=0.1).astype(np.float32)
    return grad_y

def harris_response(img, alpha, win_size):
    # In this function you are going to calculate harris response R.
    # Please refer to 04_Feature_Detection.pdf page 29 for details. 
    # You have to discover how to calculate det(M) and trace(M), and
    # remember to smooth the gradients. 
    # Avoid using too much "for" loops to speed up.
    grad_x = gradient_x(img)
    grad_y = gradient_y(img)
    Ixx = grad_x**2
    Ixy = grad_x*grad_y
    Iyy = grad_y**2

    # Use uniform_filter to accumulate sums over the window
    Sxx = ndimage.uniform_filter(Ixx, size=win_size)
    Sxy = ndimage.uniform_filter(Ixy, size=win_size)
    Syy = ndimage.uniform_filter(Iyy, size=win_size)

    Sxx = ndimage.gaussian_filter(Sxx, sigma=0.1)
    Sxy = ndimage.gaussian_filter(Sxy, sigma=0.1)
    Syy = ndimage.gaussian_filter(Syy, sigma=0.1)

    # Calculate determinant and trace
    det = Sxx * Syy - Sxy**2
    trace = Sxx + Syy
    
    # Calculate the response
    output = det - alpha * (trace**2)
    
    return output

def corner_selection(R, thresh, min_dist):
    # non-maximal suppression for R to get R_selection and transform selected corners to list of tuples
    # hint: 
    #   use ndimage.maximum_filter()  to achieve non-maximum suppression
    #   set those which aren’t **local maximum** to zero.
    

    # 非极大值抑制
    local_max = ndimage.maximum_filter(R, size=3)
    # 阈值筛选，保留大于阈值的响应
    nms = (R == local_max)
    corners = np.zeros_like(R)
    corners[nms & (R > thresh)] = R[nms & (R > thresh)]
    corners = np.argwhere(corners > 0)

    values = R[corners[:, 0], corners[:, 1]]
    # 按照阈值从大到小排序
    corners = corners[np.argsort(values)[::-1]]
    tree = spatial.KDTree(corners)
    keep = np.ones(len(corners), dtype=bool)
    for i,point in enumerate(corners):
        if keep[i] == 1:
            indices = tree.query_ball_point(point, min_dist)
            keep[np.setdiff1d(indices, [i])] = 0

    corners = corners[keep]
    return corners


def split_cell(img,x,y,blockSize,cellSize):
    def mirror(ori,total):
        if ori < 0:
            return -ori - 1
        elif ori >= total:
            return 2 * total - ori - 1
        else:
            return ori
    
    cells = np.empty((blockSize * blockSize,cellSize,cellSize))
    for i in range(blockSize):
        for j in range(blockSize):
            for k in range(cellSize):
                for l in range(cellSize):
                    cells[i * blockSize + j,k,l] = img[mirror(x + i * cellSize + k, img.shape[0]), mirror(y + j * cellSize + l, img.shape[1])]
    return cells
    

def histogram_of_gradients(img, pix):
    # no template for coding, please implement by yourself.
    # You can refer to implementations on Github or other websites
    # Hint: 
    #   1. grad_x & grad_y
    #   2. grad_dir by arctan function
    #   3. for each interest point, choose m*m blocks with each consists of m*m pixels
    #   4. I divide the region into n directions (maybe 8).
    #   5. For each blocks, calculate the number of derivatives in those directions and normalize the Histogram. 
    #   6. After that, select the prominent gradient and take it as principle orientation.
    #   7. Then rotate it’s neighbor to fit principle orientation and calculate the histogram again. 
    
    block_size = 2
    cell_size = 8
    divide = 9
    grad_x = gradient_x(img)
    grad_y = gradient_y(img)

    grad_dir = np.arctan2(grad_y , grad_x)
    grad_mag = np.sqrt(grad_x**2 + grad_y**2)
    grad_dir += np.pi
    features = np.empty((0, block_size * block_size * divide))
    for p in pix:
        x, y = p
        cell_dir = split_cell(grad_dir,x,y,block_size,cell_size)
        cell_mag = split_cell(grad_mag,x,y,block_size,cell_size)
        cell_feature = np.zeros((block_size * block_size, divide))

        for i in range(block_size * block_size):
            cell_feature[i] = np.histogram(cell_dir[i],divide,[0,2*np.pi],weights=cell_mag[i])[0]
        
        cell_feature = cell_feature.flatten()
        
        cell_feature_norm = np.linalg.norm(cell_feature)

        if cell_feature_norm != 0:
            cell_feature = cell_feature / cell_feature_norm

        main_dir = np.mod(np.argmax(cell_feature),divide)
        
        feature = cell_feature
        # print(feature.shape)

        roll_feat = np.array([])

        for k in range(block_size**2):
            tmp = cell_feature[k * divide:(k + 1) * divide]
            tmp = np.roll(tmp, -main_dir)
            roll_feat = np.append(roll_feat, tmp)
        features = np.concatenate((features, [roll_feat]), axis=0)
    
    return features


def visualize_R(R):
    plt.figure(figsize=(20, 10), dpi=300)
    plt.subplot(1, 2, 1)
    plt.imshow(R, cmap='gray')
    # R取值直方图
    plt.subplot(1, 2, 2)
    plt.hist(R.flatten(), bins=100)
    plt.show()

def visualize_corner(corners,img):
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    plt.imshow(img, cmap='gray')
    plt.scatter(corners[:, 1], corners[:, 0], c='r',s=5)
    plt.show()


def feature_matching(img_1, img_2):
    R1 = harris_response(img_1, 0.04, 9)
    R2 = harris_response(img_2, 0.04, 9)
    cor1 = corner_selection(R1, 0.01*np.max(R1), 5)
    cor2 = corner_selection(R2, 0.01*np.max(R1), 5)
    fea1 = histogram_of_gradients(img_1, cor1)
    fea2 = histogram_of_gradients(img_2, cor2)
    dis = spatial.distance.cdist(fea1, fea2, metric='euclidean')
    threshold = 0.6
    pixels_1 = []
    pixels_2 = []
    p1, p2 = np.shape(dis)
    if p1 < p2:
        for p in range(p1):
            dis_min = np.min(dis[p])
            pos = np.argmin(dis[p])
            dis[p][pos] = np.max(dis)
            if dis_min/np.min(dis[p]) <= threshold:
                pixels_1.append(cor1[p])
                pixels_2.append(cor2[pos])
                dis[:, pos] = np.max(dis)

    else:
        for p in range(p2):
            dis_min = np.min(dis[:, p])
            pos = np.argmin(dis[:, p])
            dis[pos][p] = np.max(dis)
            if dis_min/np.min(dis[:, p]) <= threshold:
                pixels_2.append(cor2[p])
                pixels_1.append(cor1[pos])
                dis[pos] = np.max(dis)
    min_len = min(np.shape(cor1)[0], np.shape(cor2)[0])
    rate = np.shape(pixels_1)[0]/min_len
    assert rate >= 0.03, "Fail to Match!"
    return pixels_1, pixels_2

def test_matching():    
    img_1 = cv2.imread(f'{IMGDIR}/1_1.jpg')
    img_2 = cv2.imread(f'{IMGDIR}/1_2.jpg')

    img_gray_1 = cv2.cvtColor(img_1, cv2.COLOR_BGR2GRAY)
    img_gray_2 = cv2.cvtColor(img_2, cv2.COLOR_BGR2GRAY)

    pixels_1, pixels_2 = feature_matching(img_1, img_2)

    H_1, W_1 = img_gray_1.shape
    H_2, W_2 = img_gray_2.shape

    img = np.zeros((max(H_1, H_2), W_1 + W_2, 3))
    img[:H_1, :W_1, (2, 1, 0)] = img_1 / 255
    img[:H_2, W_1:, (2, 1, 0)] = img_2 / 255
    
    plt.figure(figsize=(20, 10), dpi=300)
    plt.imshow(img)

    N = len(pixels_1)
    for i in range(N):
        y1, x1 = pixels_1[i]
        y2, x2 = pixels_2[i]
        plt.plot([x1, x2+W_1], [y1, y2],linewidth=0.4)

    # plt.show()
    plt.savefig('test.jpg')

def compute_homography(pixels_1, pixels_2):
    # compute the best-fit homography using the Singular Value Decomposition (SVD)
    # homography matrix is a (3,3) matrix consisting rotation, translation and projection information.
    # consider how to form matrix A for U, S, V = np.linalg.svd((np.transpose(A)).dot(A))
    # homo_matrix = np.reshape(V[np.argmin(S)], (3, 3))
    # TODO
    homo_matrix = []
    return homo_matrix

def align_pair(pixels_1, pixels_2):
    # utilize \verb|homo_coordinates| for homogeneous pixels
    # and \verb|compute_homography| to calculate homo_matrix
    # implement RANSAC to compute the optimal alignment.
    # you can refer to implementations online.
    est_homo = []
    return est_homo

def stitch_blend(img_1, img_2, est_homo):
    # hint: 
    # First, project four corner pixels with estimated homo-matrix
    # and converting them back to Cartesian coordinates after normalization.
    # Together with four corner pixels of the other image, we can get the size of new image plane.
    # Then, remap both image to new image plane and blend two images using Alpha Blending.
    h1, w1, d1 = np.shape(img_1)  # d=3 RGB
    h2, w2, d2 = np.shape(img_2)
    p1 = est_homo.dot(np.array([0, 0, 1]))
    p2 = est_homo.dot(np.array([0, h1, 1]))
    p3 = est_homo.dot(np.array([w1, 0, 1]))
    p4 = est_homo.dot(np.array([w1, h1, 1]))
    p1 = np.int16(p1/p1[2])
    p2 = np.int16(p2/p2[2])
    p3 = np.int16(p3/p3[2])
    p4 = np.int16(p4/p4[2])
    x_min = min(0, p1[0], p2[0], p3[0], p4[0])
    x_max = max(w2, p1[0], p2[0], p3[0], p4[0])
    y_min = min(0, p1[1], p2[1], p3[1], p4[1])
    y_max = max(h2, p1[1], p2[1], p3[1], p4[1])
    x_range = np.arange(x_min, x_max+1, 1)
    y_range = np.arange(y_min, y_max+1, 1)
    x, y = np.meshgrid(x_range, y_range)
    x = np.float32(x)
    y = np.float32(y)
    homo_inv = np.linalg.pinv(est_homo)
    trans_x = homo_inv[0, 0]*x+homo_inv[0, 1]*y+homo_inv[0, 2]
    trans_y = homo_inv[1, 0]*x+homo_inv[1, 1]*y+homo_inv[1, 2]
    trans_z = homo_inv[2, 0]*x+homo_inv[2, 1]*y+homo_inv[2, 2]
    trans_x = trans_x/trans_z
    trans_y = trans_y/trans_z
    est_img_1 = cv2.remap(img_1, trans_x, trans_y, cv2.INTER_LINEAR)
    est_img_2 = cv2.remap(img_2, x, y, cv2.INTER_LINEAR)
    alpha1 = cv2.remap(np.ones(np.shape(img_1)), trans_x,
                       trans_y, cv2.INTER_LINEAR)
    alpha2 = cv2.remap(np.ones(np.shape(img_2)), x, y, cv2.INTER_LINEAR)
    alpha = alpha1+alpha2
    alpha[alpha == 0] = 2
    alpha1 = alpha1/alpha
    alpha2 = alpha2/alpha
    est_img = est_img_1*alpha1 + est_img_2*alpha2
    return est_img


def generate_panorama(ordered_img_seq):
    len = np.shape(ordered_img_seq)[0]
    mid = int(len/2) # middle anchor
    i = mid-1
    j = mid+1
    principle_img = ordered_img_seq[mid]
    while(j < len):
        pixels1, pixels2 = feature_matching(ordered_img_seq[j], principle_img)
        homo_matrix = align_pair(pixels1, pixels2)
        principle_img = stitch_blend(
            ordered_img_seq[j], principle_img, homo_matrix)
        principle_img=np.uint8(principle_img)
        j = j+1  
    while(i >= 0):
        pixels1, pixels2 = feature_matching(ordered_img_seq[i], principle_img)
        homo_matrix = align_pair(pixels1, pixels2)
        principle_img = stitch_blend(
            ordered_img_seq[i], principle_img, homo_matrix)
        principle_img=np.uint8(principle_img)
        i = i-1  
    est_pano = principle_img
    return est_pano

if __name__ == '__main__':
    # make image list
    # call generate panorama and it should work well
    # save the generated image following the requirements
    test_matching()
    assert 1 == 0
    # an example
    img_1 = cv2.imread(f'{IMGDIR}/panoramas/parrington/prtn00.jpg')
    img_2 = cv2.imread(f'{IMGDIR}/panoramas/parrington/prtn01.jpg')
    img_3 = cv2.imread(f'{IMGDIR}/panoramas/parrington/prtn02.jpg')
    img_4 = cv2.imread(f'{IMGDIR}/panoramas/parrington/prtn03.jpg')
    img_5 = cv2.imread(f'{IMGDIR}/panoramas/parrington/prtn04.jpg')
    img_list=[]
    img_list.append(img_1)
    img_list.append(img_2)
    img_list.append(img_3)
    img_list.append(img_4)
    img_list.append(img_5)
    pano = generate_panorama(img_list)
    cv2.imwrite("outputs/panorama_3.jpg", pano)
