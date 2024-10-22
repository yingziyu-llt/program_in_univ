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
    grad_x = ndimage.sobel(img, axis=1)
    return grad_x

def gradient_y(img):
    grad_y = ndimage.sobel(img, axis=0)
    return grad_y

def harris_response(img, alpha, win_size):
    # In this function you are going to calculate harris response R.
    # Please refer to 04_Feature_Detection.pdf page 29 for details. 
    # You have to discover how to calculate det(M) and trace(M), and
    # remember to smooth the gradients. 
    # Avoid using too much "for" loops to speed up.
    
    par_x = gradient_x(ndimage.gaussian_filter(img, sigma=1))
    par_y = gradient_y(ndimage.gaussian_filter(img, sigma=1))
    par_x_2 = par_x * par_x 
    par_y_2 = par_y * par_y
    par_xy = par_y * par_x
    R = np.zeros_like(img)

    stride_shape = (win_size, win_size)

    S_par_x2 = as_strided(par_x_2,stride_shape)
    S_par_y2 = as_strided(par_y_2,stride_shape)
    S_par_xy = as_strided(par_xy,stride_shape)

    tr = S_par_x2 + S_par_y2
    det = S_par_x2 * S_par_y2 - S_par_xy * S_par_xy
    R = det - alpha * (tr ** 2)
    
    return -R

def corner_selection(R, thresh, min_dist):
    # non-maximal suppression for R to get R_selection and transform selected corners to list of tuples
    # hint: 
    #   use ndimage.maximum_filter()  to achieve non-maximum suppression
    #   set those which aren’t **local maximum** to zero.

    R_selection = np.zeros(R.shape)
    R_selection = ndimage.maximum_filter(R, size=min_dist, mode='constant')
    R_selection = R_selection * (R > thresh * R_selection)
    pix = np.argwhere(R_selection > 0)

    return pix

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
    n = 8
    m = 5
    grad_x = gradient_x(img)
    grad_y = gradient_y(img)
    grad_dir = np.arctan2(grad_y, grad_x)
    grad_len = grad_x * grad_x + grad_y * grad_y
    features = []
    for i in pix:
        x, y = i
        x1 = max(0, x - m // 2)
        x2 = min(img.shape[0], x + m // 2)
        y1 = max(0, y - m // 2)
        y2 = min(img.shape[1], y + m // 2)
        block = img[x1:x2, y1:y2]
        block_grad_x = grad_x[x1:x2, y1:y2]
        block_grad_y = grad_y[x1:x2, y1:y2]
        grad_dir_block = grad_dir[x1:x2, y1:y2]
        block = block.flatten()
        grad_dir_block = grad_dir_block.flatten()
        grad_len_block = grad_len[x1:x2, y1:y2]
        grad_len_block = grad_len_block.flatten()
        hist, _ = np.histogram(grad_dir_block, bins=n, weights=grad_len_block, range=(-np.pi, np.pi))
        hist = hist / hist.sum()
        main_dir_idx = np.argmax(hist)
        main_dir = main_dir_idx * (2 * np.pi / n)
        adjusted_grad_dir = (grad_dir_block - main_dir)
        adjusted_grad_dir = np.mod(adjusted_grad_dir + np.pi, 2 * np.pi) - np.pi
        hist, _ = np.histogram(adjusted_grad_dir, bins=n, weights=grad_len_block)
        hist = hist / hist.sum()
        features.append(hist)
        
    return features

def feature_matching(img_1, img_2):
    R1 = harris_response(img_1, 0.04, 9)
    R2 = harris_response(img_2, 0.04, 9)
    print(R1)
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
        x1, y1 = pixels_1[i]
        x2, y2 = pixels_2[i]
        plt.plot([x1, x2+W_1], [y1, y2])

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
