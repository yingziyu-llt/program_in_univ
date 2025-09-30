# Lab 1: SMPL-X Human Body Modeling

## Background

This assignment serves as a comprehensive recap of an important topic covered in our course: human body representation using SMPL-X. You will work with 3D human pose visualization and joint-to-smplx optimization that are widely used in computer vision and graphics research.

**SMPL-X (SMPL eXpressive)** is a expressive body model that represents the human body shape and pose through a parametric model. It extends the original SMPL model to include facial expressions and hand articulation, making it suitable for full-body human modeling applications.

In this assignment, you will implement key components for this topic, demonstrating your understanding of 3D human modeling.

## Learning Objectives

By completing this assignment, you will:

- Understand the SMPL-X parametric human body model
- Gain hands-on experience with PyTorch and 3D graphics libraries

## Environment Setup

### Prerequisites

- Python 3.8 or higher
- Linux environment (recommended)
- CUDA-capable GPU (recommended)

### Installation

1. Unzip and navigate to the lab directory

2. Create and activate a conda virtual environment:

    ```bash
    conda create -n core_lab1 python=3.8
    conda activate core_lab1
    ```

3. Install Required Packages
    ```bash
    pip3 install -r requirements.txt
    ```

4. Verify Installation:

    ```bash
    python -c "import torch; import smplx; import trimesh; print('Environment setup successful')"
    ```

## Assignment Tasks

### Task 1: SMPL-X Pose Visualization (50 points)

**Objective:** Implement a function to visualize 3D human poses using the SMPL-X model.

**Description:** You will complete the `visualize_smplx()` function in `framework.py`. Given SMPL-X parameters (translation, body pose, and global orientation), your function should generate and display a 3D mesh visualization of the human pose and save it in .png format.

#### Key Requirements

- Load the SMPL-X model correctly


#### Expected Output

- 3D mesh visualization saved as image .png file


### Task 2: Joints to SMPL-X Pose Regression (50 points)

**Objective:** Implement an optimization function to estimate SMPL-X parameters from 3D joint positions.

**Description:** Sometimes you can only get the 3D joint positions of a human pose, therefore you need to convert the joint positions to SMPL-X parameters. Complete the `optimize_smplx()` function that takes 3D joint positions as input and optimizes SMPL-X parameters to best fit these target joints. This involves setting up an optimization loop with appropriate loss functions.

#### Key Requirements

- Define appropriate loss function (joint position error)
- Implement gradient-based optimization using PyTorch
- Return optimized SMPL-X parameters


#### Expected Output

- Optimized SMPL-X parameters (translation, body_pose, global_orient)
- Final loss value (about 8e-5 after 100 iterations)
- **You should achieve a similar mesh rendering result as in Task 1**

## Implementation Guidelines

### Code Structure

- **Only modify code within `## TODO` sections in `framework.py`**
- Do not change function signatures or import statements
- Maintain consistent tensor shapes and data types


### Debugging Tips

- Use `breakpoint()` to check intermediate results
- Visualize intermediate results when possible


## Hand-in Requirements

### Submission Format

- **Submit only one file:** `framework.py`
- **Platform:** Gradescope autograder


### File Requirements

- Your `framework.py` must contain all implemented functions
- Do not modify function signatures or imports
- Include necessary comments explaining your implementation approach

### Autograder Testing

- Your submission will be tested against multiple test cases
- Partial credit will be awarded for partially correct implementations
- Timeout limit: 10 minutes in total


## Academic Integrity

This is an individual assignment. While you may discuss general concepts with classmates, all submitted code must be your own work. Plagiarism will result in course failure.

### Allowed

- Discussing assignment requirements and clarifications
- Sharing debugging strategies (without code)
- Using official documentation and course materials

### Not Allowed

- Sharing or copying code solutions
- Using external implementations without permission
- Collaborating on actual implementation

## Additional Resources

### Documentation

- [SMPL-X Official Documentation](https://smpl-x.is.tue.mpg.de/)
- [PyTorch Documentation](https://pytorch.org/docs/)


**Good luck with your assignment!**