import numpy as np
from scipy import ndimage, spatial
import cv2
from os import listdir
import matplotlib.pyplot as plt
def visualize_R(R):
    plt.figure(figsize=(20, 10), dpi=300)
    plt.subplot(1, 2, 1)
    plt.imshow(R, cmap='gray')
    # R取值直方图
    plt.subplot(1, 2, 2)
    plt.hist(R.flatten(), bins=100)
    plt.show()

def visualize_corner(corners,img):
    plt.cla()
    plt.imshow(img)
    plt.scatter(corners[:, 1], corners[:, 0], c='r',s=5)
    plt.show()

def mirror(ori,total):
    if ori < 0:
       return -ori - 1
    elif ori >= total:
        return 2 * total - ori - 1
    else:
        return ori

def split_cell(img,x,y,blockSize,cellSize):  
    cells = np.empty((blockSize * blockSize,cellSize,cellSize))
    for i in range(blockSize):
        for j in range(blockSize):
            for k in range(cellSize):
                for l in range(cellSize):
                    cells[i * blockSize + j,k,l] = img[mirror(x + (i - blockSize // 2) * cellSize + k, img.shape[0]), mirror(y + (j - blockSize // 2) * cellSize + l, img.shape[1])]
    return cells

def apply_homography(homo_matrix,points):
    transformed_points = []
    for (x, y) in points:
        vector = np.array([y, x, 1])
        transformed = homo_matrix @ vector
        transformed /= transformed[2]  # Normalize to make it homogeneous
        transformed_points.append((transformed[1], transformed[0]))
    return transformed_points

def compute_inliers(H, pixels_1, pixels_2, threshold=1.0):
    """
    Compute the number of inliers for a given homography matrix H.
    An inlier is a point pair that, when transformed, has a distance less than the threshold.
    """
    inliers = []
    transformed_points = apply_homography(H, pixels_1)
    for i, (x2, y2) in enumerate(pixels_2):
        x2_est, y2_est = transformed_points[i]
        distance = np.sqrt((x2 - x2_est) ** 2 + (y2 - y2_est) ** 2)
        if distance < threshold:
            inliers.append(i)
    return inliers
