# Lab 2: Linear Inverted Pendulum Model

## Background

This assignment focuses on a fundamental concept in bipedal robotics: the Linear Inverted Pendulum Model (LIPM). You will implement and visualize a simplified 2D LIPM system that captures the essential dynamics of walking robots and is widely used in humanoid robotics research.

**The Linear Inverted Pendulum Model (LIPM)** simplifies bipedal walking dynamics by modeling the robot's center of mass (CoM) as a point mass moving under gravity while constrained to a constant height. This abstraction enables real-time motion planning and control for walking robots by reducing the complex full-body dynamics to tractable equations.

In this assignment, you will implement key components of LIPM dynamics and foot placement strategies.

## Learning Objectives

By completing this assignment, you will:

- Understand the dynamics of the Linear Inverted Pendulum Model
- Implement CoM trajectory computation for single and multiple steps
- Learn capture point theory for stable foot placement

## Environment Setup

### Prerequisites

- Python 3.8 or higher
- Linux environment (recommended)

### Installation

1. Unzip and navigate to the lab directory

2. Create and activate a conda virtual environment:

    ```bash
    conda create -n core_lab2 python=3.8
    conda activate core_lab2
    ```

3. Install Required Packages
    ```bash
    pip3 install -r requirements.txt
    ```

4. Verify Installation:

    ```bash
    python -c "import numpy; import matplotlib; print('Environment setup successful')"
    ```

## Assignment Tasks

### Task 1: Single Step LIPM Dynamics (50 points)

**Objective:** Implement the core dynamics of LIPM by computing the center of mass trajectory during a single step.

**Description:** You will complete the `updateXtVt()` function in `LIPM.py`. This function computes the current position and velocity of the center of mass based on the LIPM dynamics equations.

#### Testing Your Implementation

Run the following command to visualize a single step:

```bash
python test.py --task single_step
```


#### Expected Output

- Smooth trajectory of the CoM during one step
- **Reference output video: `./expected_output/single_step.mp4`**



### Task 2: Multi-Step Walking with Capture Point (50 points)

**Objective:** Implement orbital energy computation and capture point calculation for stable multi-step walking.

**Description:** Complete two functions that enable stable walking over multiple steps:
- `getOrbitalEnergy()`: Computes the orbital energy of the LIPM system, a conserved quantity useful for stability analysis
- `updateCapturePoint()`: Calculates the capture point, which determines the optimal foot placement location for maintaining balance

These functions work together to enable leg switching and continuous walking by determining where to place the next step iteratively.

#### Key Requirements

- Correctly compute orbital energy based on current state
- Implement capture point dynamics

#### Testing Your Implementation
Run the following command to visualize multi-step walking:

```bash
python test.py --task multi_step
```


#### Expected Output

- Continuous walking motion over multiple steps
- **Reference output video: `./expected_output/multi_step.mp4`**


## Implementation Guidelines

### Code Structure

- **Only modify code within `## TODO` sections in `LIPM.py`**
- Do not change function signatures or class structures


### Debugging Tips

- Verify energy conservation in your orbital energy calculation
- Use `breakpoint()` to check intermediate results
- Compare your output with the provided reference videos

## Hand-in Requirements

### Submission Format

- **Submit only one file:** `LIPM.py`
- **Platform:** Gradescope autograder

### File Requirements

- Your `LIPM.py` must contain all implemented functions
- Do not modify function signatures or imports


## Academic Integrity

This is an individual assignment. While you may discuss general concepts with classmates, all submitted code must be your own work. Plagiarism will result in course failure.

### Allowed

- Discussing theory and concepts
- Sharing debugging strategies (without code)
- Using course materials and recommended references

### Not Allowed

- Sharing or copying code solutions
- Using external implementations without permission
- Collaborating on actual implementation

**Good luck with your assignment!**