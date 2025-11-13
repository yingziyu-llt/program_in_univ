import numpy as np
import torch
import torch.nn as nn


def compute_bc_loss(predicted_actions: torch.Tensor, target_actions: torch.Tensor) -> torch.Tensor:
    """
    [Homework Blank 1] Compute the behavior cloning loss function

    Args:
        predicted_actions: Predicted actions by the model, shape (batch_size, action_dim)
        target_actions: Expert actions, shape (batch_size, action_dim)

    Returns:
        loss: Scalar tensor, representing the mean squared error loss between predicted and target actions
    """
    # TODO
    loss = nn.MSELoss()(predicted_actions, target_actions)
    return loss


def normalize_observations(observations: np.ndarray) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """
    [Homework Blank 2] Normalize observation data

    Args:
        observations: Raw observation data, shape (num_samples, obs_dim)

    Returns:
        normalized_obs: Normalized observations, same shape as input
        mean: Mean of the observations, shape (obs_dim,)
        std: Standard deviation of observations, shape (obs_dim,)
    """
    normalized_obs, mean, std = None, None, None
    # TODO
    mean = np.mean(observations, axis=0)
    std = np.std(observations, axis=0) + 1e-8
    normalized_obs = (observations - mean) / std
    return normalized_obs, mean, std


def create_actor_network(state_dim: int, action_dim: int) -> nn.Module:
    """
    [Homework Blank 3] Design the Actor network structure

    Args:
        state_dim: State space dimension
        action_dim: Action space dimension

    Returns:
        network: Designed neural network model (nn.Module)

    Requirements:
        1. The network must have at least 2 hidden layers
        2. Use appropriate activation functions
        3. Final layer output dimension must equal action_dim
        4. Dropout, batch normalization, etc. can be used
        5. Total parameter count should be between 10K and 1M
    """
    network = None
    # TODO
    network = nn.Sequential(
        nn.Linear(state_dim, 256),
        nn.BatchNorm1d(256),
        nn.Dropout(0.1),
        nn.GELU(),
        nn.Linear(256, 256),
        nn.BatchNorm1d(256),
        nn.Dropout(0.1),
        nn.GELU(),
        nn.Linear(256, 256),
        nn.BatchNorm1d(256),
        nn.Dropout(0.1),
        nn.GELU(),
        nn.Linear(256, action_dim)
    )



    return network
