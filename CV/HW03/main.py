from PIL import Image
import numpy as np
import matplotlib.pyplot as plt
import scipy
def visualize_matches(I1, I2, matches):
    # display two images side-by-side with matches
    # this code is to help you visualize the matches, you don't need
    # to use it to produce the results for the assignment

    I3 = np.zeros((I1.size[1],I1.size[0]*2,3) )
    I3[:,:I1.size[0],:] = I1
    I3[:,I1.size[0]:,:] = I2
    fig, ax = plt.subplots()
    ax.set_aspect('equal')
    ax.imshow(np.array(I3).astype(np.uint8))
    ax.plot(matches[:,0],matches[:,1],  '+r')
    ax.plot( matches[:,2] + I1.size[0], matches[:,3], '+r')
    ax.plot([matches[:,0], matches[:,2]+I1.size[0]],[matches[:,1], matches[:,3]], 'r')
    plt.show()

    

def normalize_points(pts):
    # Normalize points
    # 1. calculate mean and std
    # 2. build a transformation matrix
    # :return normalized_pts: normalized points
    # :return T: transformation matrix from original to normalized points

    mean = np.mean(pts, axis=0)
    std = np.std(pts, axis=0)
    T = np.array([[1/std[0], 0, -mean[0]/std[0]], [0, 1/std[1], -mean[1]/std[1]], [0, 0, 1]])
    normalized_pts = np.matmul(T, pts.T).T

    return normalized_pts, T

def fit_fundamental(matches: np.array):
    # Calculate fundamental matrix from ground truth matches
    N = matches.shape[0]
    pic1 = matches[:, :2]
    pic1 = np.hstack((pic1, np.ones((pic1.shape[0], 1))))  
    pic2 = matches[:, 2:]
    pic2 = np.hstack((pic2, np.ones((pic2.shape[0], 1))))  

    # Normalize points (if necessary)
    pic1, T1 = normalize_points(pic1)
    pic2, T2 = normalize_points(pic2)

    # Eight-Point Algorithm
    A = np.zeros((N, 9))
    for i in range(N):
        x1, y1, _ = pic1[i]
        x2, y2, _ = pic2[i]
        A[i] = [x1 * x2, y1 * x2, x2, x1 * y2, y1 * y2, y2, x1, y1, 1]

    U, E, VT = np.linalg.svd(A)

    F = VT[-1, :].reshape(3, 3)
    # denormalize(no rank2)
    # F = np.dot(T2.T, np.dot(F, T1))
    # return F
    # Enforce rank-2 constraint
    U, S, Vt = np.linalg.svd(F)
    S[2] = 0
    F_rank2 = U @ np.diag(S) @ Vt
    # Denormalize
    #F_rank2 = np.dot(T2.T, np.dot(F_rank2, T1))
    F_rank2 = np.matmul(T2.T, np.matmul(F_rank2, T1))
    return F_rank2


def visualize_fundamental(matches, F, I1, I2):
    # Visualize the fundamental matrix in image 2
    N = len(matches)
    M = np.c_[matches[:,0:2], np.ones((N,1))].transpose()
    L1 = np.matmul(F, M).transpose() # transform points from 
    # the first image to get epipolar lines in the second image

    # find points on epipolar lines L closest to matches(:,3:4)
    l = np.sqrt(L1[:,0]**2 + L1[:,1]**2)
    L = np.divide(L1, np.kron(np.ones((3,1)), l).transpose())   # rescale the line
    pt_line_dist = np.multiply(L, np.c_[matches[:, 2:4], np.ones((N, 1))]).sum(axis = 1)
    closest_pt = matches[:, 2:4] - np.multiply(L[:, 0:2],np.kron(np.ones((2, 1)), pt_line_dist).transpose())

    # find endpoints of segment on epipolar line (for display purposes)
    pt1 = closest_pt - np.c_[L[:,1], -L[:,0]] * 10    # offset from the closest point is 10 pixels
    pt2 = closest_pt + np.c_[L[:,1], -L[:,0]] * 10

    # display points and segments of corresponding epipolar lines
    fig, ax = plt.subplots()
    ax.set_aspect('equal')
    ax.imshow(np.array(I2).astype(np.uint8))
    ax.plot(matches[:, 2],matches[:, 3],  '+r')
    ax.plot([matches[:, 2], closest_pt[:, 0]],[matches[:, 3], closest_pt[:, 1]], 'r')
    ax.plot([pt1[:, 0], pt2[:, 0]],[pt1[:, 1], pt2[:, 1]], 'g')
    plt.show()

def evaluate_fundamental(matches, F):
    N = len(matches)
    points1, points2 = matches[:, :2], matches[:, 2:]
    points1_homogeneous = np.concatenate([points1, np.ones((N, 1))], axis=1)
    points2_homogeneous = np.concatenate([points2, np.ones((N, 1))], axis=1)
    product = np.dot(np.dot(points2_homogeneous, F), points1_homogeneous.T)
    diag = np.diag(product)
    residual = np.mean(diag ** 2)
    return residual

## Task 0: Load data and visualize
## load images and match files for the first example
## matches[:, :2] is a point in the first image
## matches[:, 2:] is a corresponding point in the second image

library_image1 = Image.open('data/library1.jpg')
library_image2 = Image.open('data/library2.jpg')
library_matches = np.loadtxt('data/library_matches.txt')

lab_image1 = Image.open('data/lab1.jpg')
lab_image2 = Image.open('data/lab2.jpg')
lab_matches = np.loadtxt('data/lab_matches.txt')

## Visualize matches
# visualize_matches(library_image1, library_image2, library_matches)
# visualize_matches(lab_image1, lab_image2, lab_matches)

## Task 1: Fundamental matrix
## display second image with epipolar lines reprojected from the first image

# first, fit fundamental matrix to the matches
# Report your fundamental matrices, visualization and evaluation results
library_F = fit_fundamental(library_matches) # this is a function that you should write
visualize_fundamental(library_matches, library_F, library_image1, library_image2)
print(f"library_F = {library_F}")
print("error =",evaluate_fundamental(library_matches, library_F))
assert evaluate_fundamental(library_matches, library_F) < 0.5

lab_F = fit_fundamental(lab_matches) # this is a function that you should write
visualize_fundamental(lab_matches, lab_F, lab_image1, lab_image2) 
print(f"lab_F = {lab_F}")
print("error =",evaluate_fundamental(lab_matches, lab_F))
assert evaluate_fundamental(lab_matches, lab_F) < 0.5


## Task 2: Camera Calibration

def normalize_points_3d(pts):
    pts = np.hstack((pts, np.ones((len(pts), 1))))
    mean = np.mean(pts, axis=0)
    std = np.std(pts, axis=0)
    T = np.array([[1/std[0], 0,0, -mean[0]/std[0]], [0, 1/std[1],0, -mean[1]/std[1]], [0, 0, 1/std[2], -mean[2]/std[2]], [0, 0, 0, 1]])
    normalized_pts = np.matmul(T, pts.T).T

    return normalized_pts, T

def calc_projection(points_2d, points_3d):
    # Calculate camera projection matrices
    # 1. Points_2d = P * Points_3d -> AX = 0
    # X = (p_11, p_12, ..., p_34) is flatten of P
    # build matrix A(2*N, 12) from points_2d
    # 2. SVD decomposite A
    # 3. take the eigen vector(12, ) of smallest eigen value
    # 4. return projection matrix(3, 4)
    # :param points_2d: 2D points N x 2
    # :param points_3d: 3D points N x 3
    # :return P: projection matrix
    A = np.zeros((2 * len(points_2d), 12))
    points_2d = np.hstack((points_2d, np.ones((len(points_2d), 1))))
    # points_2d, T2 = normalize_points(points_2d)
    # points_3d, T3 = normalize_points_3d(points_3d)


    for i in range(len(points_2d)):
        A[i * 2] = [points_3d[i][0], points_3d[i][1], points_3d[i][2], 1,
                    0, 0, 0, 0,
                    -points_2d[i][0] * points_3d[i][0], -points_2d[i][0] * points_3d[i][1], -points_2d[i][0] * points_3d[i][2], -points_2d[i][0]]
        A[i * 2 + 1] = [0, 0, 0, 0,
                        points_3d[i][0], points_3d[i][1], points_3d[i][2], 1,
                        -points_2d[i][1] * points_3d[i][0], -points_2d[i][1] * points_3d[i][1], -points_2d[i][1] * points_3d[i][2], -points_2d[i][1]]

    U, S, Vt = np.linalg.svd(A)
    P = Vt[-1, :].reshape(3, 4)

    # P = np.dot(np.dot(np.linalg.inv(T2), P), T3)
    return P

def rq_decomposition(P):
    # Use RQ decomposition to calculte K, R, T
    # 1. perform QR decomposition on left-most 3x3 matrix of P(3 x 4) to get K, R
    # 2. calculate T by P = K[R|T]
    # 3. normalize to set K[2, 2] = 1
    # :param P: projection matrix
    # :return K, R, T: camera matrices

    K, R = scipy.linalg.rq(P)
    K = K[:,1:]
    R = R * K[2,2]
    K = K / K[2, 2]
    T = R[:, 3]
    R = R[:,:3]

    return K, R, T

def evaluate_points(P, points_2d, points_3d):
    # Visualize the actual 2D points and the projected 2D points calculated from
    # the projection matrix
    # You do not need to modify anything in this function, although you can if you
    # want to
    # :param P: projection matrix 3 x 4
    # :param points_2d: 2D points N x 2
    # :param points_3d: 3D points N x 3
    # :return points_3d_proj: project 3D points to 2D by P
    # :return residual: residual of points_3d_proj and points_2d

    N = len(points_3d)
    points_3d = np.hstack((points_3d, np.ones((N, 1))))
    points_3d_proj = np.dot(P, points_3d.T).T
    u = points_3d_proj[:, 0] / points_3d_proj[:, 2]
    v = points_3d_proj[:, 1] / points_3d_proj[:, 2]
    residual = np.sum(np.hypot(u-points_2d[:, 0], v-points_2d[:, 1]))
    points_3d_proj = np.hstack((u[:, np.newaxis], v[:, np.newaxis]))
    return points_3d_proj, residual

def triangulate_points(P1, P2, point1, point2):
    # Use linear least squares to triangulation 3d points
    # 1. Solve: point1 = P1 * point_3d
    #           point2 = P2 * point_3d
    # 2. use SVD decomposition to solve linear equations
    # :param P1, P2 (3 x 4): projection matrix of two cameras
    # :param point1, point2: points in two images
    # :return point_3d: 3D points calculated by triangulation

    A = np.zeros((4,4))
    A[0] = [P1[0, 0] - point1[0] * P1[2,0],P1[0, 1] - point1[0] * P1[2,1],P1[0, 2] - point1[0] * P1[2,2],P1[0, 3] - point1[0] * P1[2,3]]
    A[1] = [P1[1, 0] - point1[1] * P1[2,0],P1[1, 1] - point1[1] * P1[2,1],P1[1, 2] - point1[1] * P1[2,2],P1[1, 3] - point1[1] * P1[2,3]]
    A[2] = [P2[0, 0] - point2[0] * P2[2,0],P2[0, 1] - point2[0] * P2[2,1],P2[0, 2] - point2[0] * P2[2,2],P2[0, 3] - point2[0] * P2[2,3]]
    A[3] = [P2[1, 0] - point2[1] * P2[2,0],P2[1, 1] - point2[1] * P2[2,1],P2[1, 2] - point2[1] * P2[2,2],P2[1, 3] - point2[1] * P2[2,3]]

    U, S, Vt = np.linalg.svd(A)

    point_3d = Vt[-1, :3]/Vt[-1, 3]

    return point_3d


lab_points_3d = np.loadtxt('data/lab_3d.txt')

projection_matrix = dict()
for key, points_2d in zip(["lab_a", "lab_b"], [lab_matches[:, :2], lab_matches[:, 2:]]):
    P = calc_projection(points_2d, lab_points_3d)
    points_3d_proj, residual = evaluate_points(P, points_2d, lab_points_3d)
    #print((points_2d - points_3d_proj))
    distance = np.mean(np.linalg.norm(points_2d - points_3d_proj))
    # Check: residual should be < 20 and distance should be < 4 
    assert residual < 20.0 and distance < 4.0
    print("{} residual: {}, distance: {}".format(key, residual, distance))
    projection_matrix[key] = P
print(projection_matrix)

## Task 3
## Camera Centers
projection_library_a = np.loadtxt('data/library1_camera.txt')
projection_library_b = np.loadtxt('data/library2_camera.txt')
projection_matrix["library_a"] = projection_library_a
projection_matrix["library_b"] = projection_library_b

print(projection_matrix)

for (key,P) in projection_matrix.items():
    # Paste your K, R, T results in your report
    K, R, T = rq_decomposition(P)
    print(key)
    print("P =",P)
    print("k=",K)
    print("r=",R)
    print("t=",T)


## Task 4: Triangulation
lab_points_3d_estimated = []
print("Lab Triangulation:")
for point_2d_a, point_2d_b, point_3d_gt in zip(lab_matches[:, :2], lab_matches[:, 2:], lab_points_3d):
    point_3d_estimated = triangulate_points(projection_matrix['lab_a'], projection_matrix['lab_b'], point_2d_a, point_2d_b)

    # Residual between ground truth and estimated 3D points
    residual_3d = np.sum(np.linalg.norm(point_3d_gt - point_3d_estimated))
    print(f"residual_3d:{residual_3d :.3f}")
    assert residual_3d < 0.1
    lab_points_3d_estimated.append(point_3d_estimated)
for i in lab_points_3d_estimated:
    print(f"{i[0]:.3f},{i[1]:.3f},{i[2]:.3f}")
# Residual between re-projected and observed 2D points
lab_points_3d_estimated = np.stack(lab_points_3d_estimated)
_, residual_a = evaluate_points(projection_matrix['lab_a'], lab_matches[:, :2], lab_points_3d_estimated)
_, residual_b = evaluate_points(projection_matrix['lab_b'], lab_matches[:, 2:], lab_points_3d_estimated)
print(f"residual_a:{residual_a : .3f}")
print(f"residual_b:{residual_b : .3f}")
assert residual_a < 20 and residual_b < 20

print("Library Triangulation:")
library_points_3d_estimated = []
for point_2d_a, point_2d_b in zip(library_matches[:, :2], library_matches[:, 2:]):
    point_3d_estimated = triangulate_points(projection_matrix['library_a'], projection_matrix['library_b'], point_2d_a, point_2d_b)
    library_points_3d_estimated.append(point_3d_estimated)
for i in lab_points_3d_estimated:
    print(f"{i[0]:.3f},{i[1]:.3f},{i[2]:.3f}")
# Residual between re-projected and observed 2D points
_, residual_a = evaluate_points(projection_matrix['library_a'], library_matches[:, :2], library_points_3d_estimated)
_, residual_b = evaluate_points(projection_matrix['library_b'], library_matches[:, 2:], library_points_3d_estimated)
print(f"residual_a:{residual_a : .3f}")
print(f"residual_b:{residual_b : .3f}")
assert residual_a < 30 and residual_b < 30

## Task 5: Fundamental matrix estimation without ground-truth matches
import cv2

def fit_fundamental_without_gt(image1, image2):
    # Calculate fundamental matrix without groundtruth matches
    # 1. convert the images to gray
    # 2. compute SIFT keypoints and descriptors
    # 3. match descriptors with Brute Force Matcher
    # 4. select good matches
    # 5. extract matched keypoints
    # 6. compute fundamental matrix with RANSAC
    # :param image1, image2: two-view images
    # :return fundamental_matrix
    # :return matches: selected matched keypoints 
    image1 = cv2.cvtColor(image1, cv2.COLOR_BGR2GRAY)
    image2 = cv2.cvtColor(image2, cv2.COLOR_BGR2GRAY)
    np.random.seed(0)

    sift = cv2.SIFT_create()
    kp1, des1 = sift.detectAndCompute(image1, None)
    kp2, des2 = sift.detectAndCompute(image2, None)

    bf = cv2.BFMatcher()
    matches = bf.knnMatch(des1, des2, k=2)

    good = []
    for m, n in matches:
        if m.distance < 0.9 * n.distance:
            good.append(m)

    src_pts = np.float32([kp1[m.queryIdx].pt for m in good]).reshape(-1, 2)
    dst_pts = np.float32([kp2[m.trainIdx].pt for m in good]).reshape(-1, 2)

    matches = np.concatenate((src_pts, dst_pts), axis = 1)
    max_inlier = 0
    inlier_max = None
    for _ in range(10000):
        sample = np.random.choice(matches.shape[0], 8, replace = False)
        sample = matches[sample]
        assert sample.shape == (8, 4)
        F = fit_fundamental(sample)
        N = len(matches)
        points1, points2 = matches[:, :2], matches[:, 2:]
        points1_homogeneous = np.concatenate([points1, np.ones((N, 1))], axis=1)
        points2_homogeneous = np.concatenate([points2, np.ones((N, 1))], axis=1)
        product = np.dot(np.dot(points2_homogeneous, F), points1_homogeneous.T)
        diag = np.diag(product)
        inlier = np.where(diag ** 2 < 0.001)
        inlier = np.array(inlier).flatten()
        if inlier.shape[0] > max_inlier:
            max_inlier = inlier.shape[0]
            inlier_max = inlier
        if inlier.shape[0] > 0.9 * matches.shape[0]:
            break
    fundamental_matrix = fit_fundamental(matches[inlier_max])
    print("error = ",evaluate_fundamental(matches, fundamental_matrix))
    print("inlier =",inlier_max.shape[0])
    return fundamental_matrix, matches[inlier_max]

house_image1 = Image.open('data/house1.jpg')
house_image2 = Image.open('data/house2.jpg')

house_F, house_matches = fit_fundamental_without_gt(np.array(house_image1), np.array(house_image2))
visualize_fundamental(house_matches, house_F, house_image1, house_image2)
print(f"house_F = {house_F}")

gaudi_image1 = Image.open('data/gaudi1.jpg')
gaudi_image2 = Image.open('data/gaudi2.jpg')

gaudi_F, gaudi_matches = fit_fundamental_without_gt(np.array(gaudi_image1), np.array(gaudi_image2))
visualize_fundamental(gaudi_matches, gaudi_F, gaudi_image1, gaudi_image2)
print(f"gaudi_F = {gaudi_F}")