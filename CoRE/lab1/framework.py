import os
import torch
import smplx
import numpy as np
import trimesh


ROOT_DIR = os.path.dirname(os.path.abspath(__file__))
SMPL_DIR = os.path.join(ROOT_DIR, 'smpl_models')

JOINTS_IND = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 23, 24, 25, 34, 40, 49]  # only use the body joints

def get_smplx_model(device):
    smplx_model = smplx.create(SMPL_DIR, model_type='smplx',
                              gender='male', ext='npz',
                              num_betas=10,
                              use_pca=False,
                              create_global_orient=True,
                              create_body_pose=True,
                              create_betas=True,
                              create_left_hand_pose=True,
                              create_right_hand_pose=True,
                              create_expression=True,
                              create_jaw_pose=True,
                              create_leye_pose=True,
                              create_reye_pose=True,
                              create_transl=True,
                              batch_size=1,
                              ).to(device)
    smplx_model.eval()
    
    return smplx_model


## Task 1: SMPL-X Pose Visualization

def visualize_smplx(input_path, save_path, render_img=True, device='cuda'):

    with open(input_path, 'rb') as f:
        pose_data = np.load(f, allow_pickle=True).item()

    ## TODO: Read the smplx parameters from the pose_data dictionary
    ## left_hand and right_hand should be set to zero vectors of shape (1, 45)
    ## Use get_smplx_model() to load the SMPL-X model
    body_pose = torch.from_numpy(pose_data['body_pose']).float().unsqueeze(0).to(device)
    global_orient = torch.from_numpy(pose_data['global_orient']).float().unsqueeze(0).to(device)
    transl = torch.from_numpy(pose_data['transl']).float().unsqueeze(0).to(device)
    left_hand = torch.zeros((1, 45)).to(device)
    right_hand = torch.zeros((1, 45)).to(device)
    smplx_model = get_smplx_model(device)

    ## End of TODO #########################################################


    with torch.no_grad():
        smpl_output = smplx_model(transl=transl, 
                                 body_pose=body_pose, 
                                 global_orient=global_orient, 
                                 return_verts=True,
                                 left_hand_pose=left_hand,
                                 right_hand_pose=right_hand,
                                 )

    joints = smpl_output.joints[:, JOINTS_IND, :].cpu().numpy()[0]
    with open('joints.npy', 'wb') as f:
        np.save(f, joints)


    if render_img:
        vertices = smpl_output.vertices[0].cpu().numpy()
        faces = smplx_model.faces

        mesh = trimesh.Trimesh(vertices, faces, process=False)

        # save the mesh as a png file
        # scene = mesh.scene()
        # png = scene.save_image(resolution=(256, 256), visible=True)
        # with open(save_path, 'wb') as f:
        #     f.write(png)




## Task 2: Joints to SMPL-X Pose Regression

def optimize_smplx(joints, device='cuda'):
    smplx_model = get_smplx_model(device)

    joints = joints.reshape(1, -1, 3)
    body_pose = torch.nn.Parameter(torch.zeros(1, 63).to(device), requires_grad=True)
    global_orient = torch.nn.Parameter(torch.zeros(1, 3).to(device), requires_grad=True)
    transl = torch.nn.Parameter(torch.zeros(1, 3).to(device), requires_grad=True)
    left_hand = torch.zeros((1, 45)).to(device)
    right_hand = torch.zeros((1, 45)).to(device)

    optimizer = torch.optim.Adam(params=[body_pose, global_orient, transl], lr=0.05)
    loss_fn = torch.nn.MSELoss()


    for _ in range(100):
        
        ## TODO: Optimize the body_pose, global_orient, and transl parameters 
        ## to minimize the loss between the input joints and the joints output 
        ## from the SMPL-X model
        loss = loss_fn(smplx_model(body_pose=body_pose,
                                   global_orient=global_orient,
                                   transl=transl,
                                   left_hand_pose=left_hand,
                                   right_hand_pose=right_hand).joints[:, JOINTS_IND, :], joints)
        ## End of TODO #########################################################

        print('loss: ', loss.item())

    return body_pose.squeeze().detach().cpu().numpy(), \
            global_orient.squeeze().detach().cpu().numpy(), \
            transl.squeeze().detach().cpu().numpy()


if __name__ == '__main__':
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

    # Task 1: Visualize SMPL-X Pose
    input_path = 'smplx_pose.pkl'
    save_path_1 = 'smplx_output.png'
    visualize_smplx(input_path, save_path_1, device=device)


    # Task 2: Joints to SMPL-X Pose Regression
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    with open('joints.npy', 'rb') as f:
        joints = np.load(f)
    joints = torch.from_numpy(joints).to(device)
    body_pose, global_orient, transl = optimize_smplx(joints, device=device)

    save_path_2 = 'optimized_smplx_output.pkl'
    optimized_smplx_dict = {
        'body_pose': body_pose,
        'global_orient': global_orient,
        'transl': transl
    }
    with open(save_path_2, 'wb') as f:
        np.save(f, optimized_smplx_dict, allow_pickle=True)

    # Visualize the optimized SMPL-X Pose
    visualize_smplx(save_path_2, 'optimized_smplx_output.png', device=device)

