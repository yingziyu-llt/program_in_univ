import os
import cv2
import numpy as np
import matplotlib.pyplot as plt
import argparse
from pyglet.canvas.base import Display
import trimesh
import multiprocessing as mp
from tqdm import tqdm
from typing import Tuple
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D


def normalize_disparity_map(disparity_map):
    '''Normalize disparity map for visualization
    disparity should be larger than zero
    '''
    return np.maximum(disparity_map, 0.0) / (disparity_map.max() + 1e-10)


def visualize_disparity_map(disparity_map, gt_map, save_path=None):
    '''Visualize or save disparity map and compare with ground truth
    '''
    # Normalize disparity maps
    disparity_map = normalize_disparity_map(disparity_map)
    gt_map = normalize_disparity_map(gt_map)
    # Visualize or save to file
    if save_path is None:
        concat_map = np.concatenate([disparity_map, gt_map], axis=1)
        plt.imshow(concat_map, 'gray')
        plt.show()
    else:
        os.makedirs(os.path.dirname(save_path), exist_ok=True)
        concat_map = np.concatenate([disparity_map, gt_map], axis=1)
        plt.imsave(save_path, concat_map, cmap='gray')

'''
非插值版本
'''
'''
def compute_disparity_for_row(args):
    """计算某一行的视差"""
    ref_img, sec_img, y, window_half, dmin, dmax, matching_function = args
    H, W = ref_img.shape
    ref_img = ref_img.astype(np.float32)
    sec_img = sec_img.astype(np.float32)
    ref_img /= 255.0
    sec_img /= 255.0
    disparity_row = np.zeros(W, dtype=np.float32)

    for x in range(window_half, W - window_half):
        ref_window = ref_img[y - window_half:y + window_half + 1, x - window_half:x + window_half + 1].flatten()
        best_disparity = 0
        best_cost = np.inf

        # 计算整数视差代价
        for d in range(dmin, dmax + 1):
            if x - d - window_half < 0:
                continue
            sec_window = sec_img[y - window_half:y + window_half + 1, x - d - window_half:x - d + window_half + 1].flatten()

            if matching_function == 'SSD':
                cost = np.sum((ref_window - sec_window) ** 2)
            elif matching_function == 'SAD':
                cost = np.sum(np.abs(ref_window - sec_window))
            elif matching_function == 'normalized_correlation':
                cost = -np.sum(sec_window * ref_window) / np.sqrt(np.sum(sec_window ** 2) * np.sum(ref_window ** 2))


            if cost < best_cost:
                best_cost = cost
                best_disparity = d

        disparity_row[x] = best_disparity
    return (y, disparity_row)
'''


def compute_disparity_for_row(args):
    """计算某一行的视差，支持子像素插值"""
    ref_img, sec_img, y, window_half, dmin, dmax, matching_function = args
    ref_img = ref_img.astype(np.float32)
    sec_img = sec_img.astype(np.float32)
    ref_img /= 255.0
    sec_img /= 255.0
    H, W = ref_img.shape
    disparity_row = np.zeros(W, dtype=np.float32)

    for x in range(window_half, W - window_half):
        ref_window = ref_img[y - window_half:y + window_half + 1, x - window_half:x + window_half + 1].flatten()
        best_disparity = 0
        best_cost = np.inf
        costs = []

        # 计算整数视差代价
        for d in range(dmin, dmax + 1):
            if x - d - window_half < 0:
                costs.append(np.inf)
                continue
            sec_window = sec_img[y - window_half:y + window_half + 1, x - d - window_half:x - d + window_half + 1].flatten()

            if matching_function == 'SSD':
                cost = np.sum((ref_window - sec_window) ** 2)
            elif matching_function == 'SAD':
                cost = np.sum(np.abs(ref_window - sec_window))
            elif matching_function == 'normalized_correlation':
                cost = -np.sum(sec_window * ref_window) / np.sqrt(np.sum(sec_window ** 2) * np.sum(ref_window ** 2))

            costs.append(cost)

            if cost < best_cost:
                best_cost = cost
                best_disparity = d

        # 子像素插值
        if dmin < best_disparity < dmax:
            d0 = best_disparity
            c0 = costs[d0 - dmin]
            c1 = costs[d0 - 1 - dmin] if d0 - 1 >= dmin else np.inf
            c2 = costs[d0 + 1 - dmin] if d0 + 1 <= dmax else np.inf

            # 二次插值公式
            if c1 != np.inf and c2 != np.inf:
                disparity_row[x] = d0 + (c1 - c2) / (2 * (c1 - 2 * c0 + c2))
            else:
                disparity_row[x] = d0  # 无法插值时使用整数视差
        else:
            disparity_row[x] = best_disparity
    disparity_row *= dmax

    return (y, disparity_row)


def task1_compute_disparity_map_simple(
    ref_img: np.ndarray,        # shape (H, W)
    sec_img: np.ndarray,        # shape (H, W)
    window_size: int,
    disparity_range: Tuple[int, int],   # (min_disparity, max_disparity)
    matching_function: str      # can be 'SSD', 'SAD', 'normalized_correlation'
):
    '''Assume image planes are parallel to each other
    Compute disparity map using simple stereo system following the steps:
    1. For each row, scan all pixels in that row
    2. Generate a window for each pixel in ref_img
    3. Search for a disparity (d) within (min_disparity, max_disparity) in sec_img
    4. Select the best disparity that minimize window difference between ref_img[row, col] and sec_img[row, col - d]
    '''
    H, W = ref_img.shape
    window_half = window_size // 2
    dmin, dmax = disparity_range
    disparity_map = np.zeros((H, W), dtype=np.float32)

    tasks = [
            (ref_img, sec_img, y, window_half, dmin, dmax, matching_function)
            for y in range(window_half, H - window_half)
        ]

    with mp.Pool() as pool:
        results = pool.map(compute_disparity_for_row, tasks)
        for y, disparity_row in results:
            disparity_map[y, :] = disparity_row

    return disparity_map

import time
def task1_simple_disparity(ref_img, sec_img, gt_map, img_name='tsukuba'):
    '''Compute disparity maps for different settings
    '''
    #window_sizes = range(1,31)  # Try different window sizes
    window_sizes = [15]
    disparity_range = (0, 64)  # Determine appropriate disparity range
    #matching_functions = ['SSD', 'SAD', 'normalized_correlation']  # Try different matching functions
    matching_functions = ['SSD']

    disparity_maps = []
    time_list = []

    # Generate disparity maps for different settings
    for window_size in window_sizes:
        time_list.append([])
        for matching_function in matching_functions:
            print(f"Computing disparity map for window_size={window_size}, disparity_range={disparity_range}, matching_function={matching_function}")
            time1 = time.time()
            disparity_map = task1_compute_disparity_map_simple(
                ref_img, sec_img,
                window_size, disparity_range, matching_function)
            time2 = time.time()
            print(f"Time: {time2 - time1}")
            time_list[-1].append(time2 - time1)
            disparity_maps.append((disparity_map, window_size, matching_function, disparity_range))
            dmin, dmax = disparity_range
            visualize_disparity_map(
                disparity_map, gt_map,
                save_path=f"output/task1_{img_name}_{window_size}_{dmin}_{dmax}_{matching_function}.png")
    '''
    plt.plot([row[0] for row in time_list],label='SSD')
    plt.plot([row[1] for row in time_list],label='SAD')
    plt.plot([row[2] for row in time_list],label='normalized_correlation')
    plt.legend()
    plt.xlabel('window_size')
    plt.ylabel('time')
    plt.show()
    window_size = 10
    matching_function = 'SSD'
    time_list = []
    for dmax in range(1,65):
        time_list.append([])
        print(f"Computing disparity map for window_size={window_size}, disparity_range={disparity_range}, matching_function={matching_function}")
        time1 = time.time()
        disparity_map = task1_compute_disparity_map_simple(
            ref_img, sec_img,
            window_size, (0,dmax), matching_function)
        time2 = time.time()
        print(f"Time: {time2 - time1}")
        time_list[-1].append(time2 - time1)
        disparity_maps.append((disparity_map, window_size, matching_function, disparity_range))
        visualize_disparity_map(
            disparity_map, gt_map,
            save_path=f"output/task1_{img_name}_{window_size}_{0}_{dmax}_{matching_function}.png")
    plt.plot(time_list)
    plt.xlabel('dmax')
    plt.ylabel('time')
    plt.show()
    '''
    return disparity_maps
    


def task2_compute_depth_map(disparity_map, baseline, focal_length):
    '''Compute depth map by z = fB / (x - x')
    Note that a disparity less or equal to zero should be ignored (set to zero)
    '''
    depth_map = focal_length * baseline / np.maximum(disparity_map, 0)
    return depth_map


def task2_visualize_pointcloud(
    ref_img: np.ndarray,        # shape (H, W, 3)
    disparity_map: np.ndarray,  # shape (H, W)
    save_path: str = 'output/task2_tsukuba.ply'
):
    '''Visualize 3D pointcloud from disparity map following the steps:
    1. Calculate depth map from disparity
    2. Set pointcloud's XY as image's XY and and pointcloud's Z as depth
    3. Set pointcloud's color as ref_img's color
    4. Save pointcloud to ply files for visualizationh. We recommend to open ply file with MeshLab
    5. Adjust the baseline and focal_length for better performance
    6. You may need to cut some outliers for better performance
    '''
    baseline = 100 * ((np.mean(disparity_map) / 86) ** 1.25)
    focal_length = 100
    #depth_map = task2_compute_depth_map(disparity_map, baseline, focal_length)
    H,W = disparity_map.shape
    threshold = disparity_map.mean() -  0.5 * disparity_map.std()

    # Points
    points = []
    colors = []
    for i in range(H):
        for j in range(W):
            if disparity_map[i, j] < threshold:
                continue
            depth = focal_length * baseline / disparity_map[i, j]
            points.append((j, i, depth))
            colors.append(ref_img[i, j])

    # Save pointcloud to ply file
    pointcloud = trimesh.PointCloud(points, colors)
    os.makedirs(os.path.dirname(save_path), exist_ok=True)
    pointcloud.export(save_path, file_type='ply')
    pointcloud.show()



def task3_compute_disparity_map_dp(ref_img, sec_img):
    ''' Conduct stereo matching with dynamic programming
    '''
    # 设置参数
    window_size = 5  # 窗口大小
    max_disparity = 64  # 最大视差
    Occlusion = 0.8
    ref_img = ref_img.astype(np.float32)
    sec_img = sec_img.astype(np.float32)
    ref_img /= 255.0
    sec_img /= 255.0

    disparity_map = np.zeros_like(ref_img, dtype=np.float32)
    path = np.zeros((ref_img.shape[1],ref_img.shape[1]), dtype=np.int32)

    for y in tqdm(range(window_size, ref_img.shape[0] - window_size)):
        e = np.full((max_disparity, ref_img.shape[1]), np.inf)
        c = np.zeros((ref_img.shape[1], ref_img.shape[1]))
        for x in range(window_size, ref_img.shape[1] - window_size):
            # 初始化代价矩阵
            for d in range(max_disparity):
                if x - d < window_size:
                    continue
                # 计算左右窗口的代价
                ref_window = ref_img[y - window_size:y + window_size, x - window_size:x + window_size].flatten()
                sec_window = sec_img[y - window_size:y + window_size, x - d - window_size:x - d + window_size].flatten()
                e[d, x] = np.sum((ref_window - sec_window) ** 2)
        for i in range(ref_img.shape[1]):
            c[i,0] = i * Occlusion
        for i in range(ref_img.shape[1]):
            c[0,i] = i * Occlusion
        for i in range(1, ref_img.shape[1]):
            for j in range(1, ref_img.shape[1]):
                min1 = 0
                if j - i < 0 or j - i >= max_disparity:
                    min1 = np.Inf
                else:
                    min1 = c[i-1,j-1] + e[j - i,j]
                min2 = c[i-1,j] + Occlusion
                min3 = c[i,j-1] + Occlusion
                c[i,j] = min(min1,min2,min3)
                if c[i,j] == min1:
                    path[i,j] = 1
                elif c[i,j] == min2:
                    path[i,j] = 2
                else:
                    path[i,j] = 3
        p = ref_img.shape[1] - 1
        q = ref_img.shape[1] - 1
        while p > 0 and q > 0:
            if path[p,q] == 1:
                disparity_map[y,p] = q - p
                p -= 1
                q -= 1
            elif path[p,q] == 2:
                disparity_map[y,p] = -1
                p -= 1
            else:
                q -= 1
        q = ref_img.shape[1] - 1
        while q > 0:
            if disparity_map[y,q] == -1:
                if q + 1 > ref_img.shape[1] - 1:
                    disparity_map[y,q] = 0
                else:
                    disparity_map[y,q] = disparity_map[y,q+1]
            q -= 1
    plt.imshow(disparity_map)
    plt.show()
    return disparity_map


def main(tasks):

    # Read images and ground truth disparity maps
    moebius_img1 = cv2.imread("data/moebius1.png")
    moebius_img1_gray = cv2.cvtColor(moebius_img1, cv2.COLOR_BGR2GRAY)
    moebius_img2 = cv2.imread("data/moebius2.png")
    moebius_img2_gray = cv2.cvtColor(moebius_img2, cv2.COLOR_BGR2GRAY)
    moebius_gt = cv2.imread("data/moebius_gt.png", cv2.IMREAD_GRAYSCALE)

    tsukuba_img1 = cv2.imread("data/tsukuba1.jpg")
    tsukuba_img1_gray = cv2.cvtColor(tsukuba_img1, cv2.COLOR_BGR2GRAY)
    tsukuba_img2 = cv2.imread("data/tsukuba2.jpg")
    tsukuba_img2_gray = cv2.cvtColor(tsukuba_img2, cv2.COLOR_BGR2GRAY)
    tsukuba_gt = cv2.imread("data/tsukuba_gt.jpg", cv2.IMREAD_GRAYSCALE)

    # Task 0: Visualize cv2 Results
    if '0' in tasks:
        # Compute disparity maps using cv2
        stereo = cv2.StereoBM.create(numDisparities=64, blockSize=15)
        moebius_disparity_cv2 = stereo.compute(moebius_img1_gray, moebius_img2_gray)
        visualize_disparity_map(moebius_disparity_cv2, moebius_gt)
        tsukuba_disparity_cv2 = stereo.compute(tsukuba_img1_gray, tsukuba_img2_gray)
        visualize_disparity_map(tsukuba_disparity_cv2, tsukuba_gt)

        if '2' in tasks:
            print('Running task2 with cv2 results ...')
            task2_visualize_pointcloud(tsukuba_img1, tsukuba_disparity_cv2, save_path='output/task2_tsukuba_cv2.ply')
            task2_visualize_pointcloud(moebius_img1, moebius_disparity_cv2, save_path='output/task2_moebius_cv2.ply')


    ######################################################################
    # Note. Running on moebius may take a long time with your own code   #
    # In this homework, you are allowed only to deal with tsukuba images #
    ######################################################################

    # Task 1: Simple Disparity Algorithm
    if '1' in tasks:
        print('Running task1 ...')
        disparity_maps = task1_simple_disparity(tsukuba_img1_gray, tsukuba_img2_gray, tsukuba_gt, img_name='tsukuba ')

        #####################################################
        # If you want to run on moebius images,             #
        # parallelizing with multiprocessing is recommended #
        #####################################################
        task1_simple_disparity(moebius_img1_gray, moebius_img2_gray, moebius_gt, img_name='moebius')

        if '2' in tasks:
            print('Running task2 with disparity maps from task1 ...')
            for (disparity_map, window_size, matching_function, disparity_range) in disparity_maps:
                dmin, dmax = disparity_range
                task2_visualize_pointcloud(
                    tsukuba_img1, disparity_map,
                    save_path=f'output/task2_tsukuba_{window_size}_{dmin}_{dmax}_{matching_function}_interpolation.ply')

    # Task 3: Non-local constraints
    if '3' in tasks:
        print(np.max(tsukuba_gt))
        print('----------------- Task 3 -----------------')
        tsukuba_disparity_dp = task3_compute_disparity_map_dp(tsukuba_img1_gray, tsukuba_img2_gray)
        visualize_disparity_map(tsukuba_disparity_dp, tsukuba_gt, save_path='output/task3_tsukuba.png')

        if '2' in tasks:
            print('Running task2 with disparity maps from task3 ...')
            task2_visualize_pointcloud(tsukuba_img1, tsukuba_disparity_dp, save_path='output/task2_tsukuba_dp.ply')

if __name__ == '__main__':
    # Set tasks to run
    parser = argparse.ArgumentParser(description='Homework 4')
    parser.add_argument('--tasks', type=str, default='0123')
    args = parser.parse_args()

    main(args.tasks)
