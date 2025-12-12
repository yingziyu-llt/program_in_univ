import numpy as np
import torch_geometric as pyg
from torch_geometric.utils import to_networkx

def create_alias_table(probs: np.array) -> tuple[np.array, np.array]:
    """
    Create alias table for efficient sampling.

    Args:
        probs (list or np.array): List of probabilities.
    Returns:
        J (np.array): Alias table.
        q (np.array): Probability table.
    """

    n = len(probs)
    J = np.zeros(n, dtype=int)
    q = np.zeros(n)

    queue_small = []
    queue_large = []

    K = n
    probs = probs / np.sum(probs)
    scaled_probs = probs * K

    for idx, prob in enumerate(scaled_probs):
        if prob < 1.0:
            queue_small.append(idx)
        else:
            queue_large.append(idx)
    
    while queue_small and queue_large:
        small = queue_small.pop()
        large = queue_large.pop()

        q[small] = scaled_probs[small]
        J[small] = large

        scaled_probs[large] = scaled_probs[large] + scaled_probs[small] - 1.0

        if scaled_probs[large] < 1.0:
            queue_small.append(large)
        else:
            queue_large.append(large)
    while queue_large:
        large = queue_large.pop()
        q[large] = 1.0
    while queue_small:
        small = queue_small.pop()
        q[small] = 1.0
    return J, q

def alias_sample(J, q) -> int:
    """
    Draw a sample from the alias table in O(1).
    """
    N = len(J)
    
    # 1. 随机选一个桶 (0 ~ N-1)
    k = int(np.random.rand() * N) 
    
    # 2. 掷骰子看是选“原主”还是“别名”
    if np.random.rand() < q[k]:
        return k
    else:
        return J[k]
import networkx as nx
class Node2Vec:
    def __init__(self, data, p, q) -> None:
        """
        args:
            data: PyG Data object (dataset[0])
            p: Return parameter
            q: In-out parameter
        """
        # self.G = to_networkx(data, to_undirected=True)
        if isinstance(data, pyg.data.Data):
            self.G = to_networkx(data, to_undirected=True)
        elif isinstance(data, nx.Graph):
            self.G = data
        self.p = p
        self.q = q
        
        self.alias_nodes = {}
        self.alias_edges = {}

    def get_alias_edge(self, src, dst) -> tuple[np.array, np.array]:
        G = self.G
        p = self.p
        q = self.q

        unnormalized_probs = []
        
        dst_neighbors = list(G.neighbors(dst))

        for neighbor in dst_neighbors:
            
            weight = 0 
            
            if neighbor == src:
                weight = 1 / p
            elif G.has_edge(neighbor, src):
                weight = 1
            else:
                weight = 1 / q
            unnormalized_probs.append(weight)

        return create_alias_table(unnormalized_probs)

    def preprocess_transition_probs(self) -> None:
        G = self.G
        
        print("Preprocessing transition probabilities...")

        # 1. 处理 alias_nodes (用于游走的第一步)
        # 第一步没有"上一个节点"，所以就按简单的边权重采样
        for node in G.nodes():
            unnormalized_probs = [1.0 for _ in G.neighbors(node)] # Cora无权图，全是1
            self.alias_nodes[node] = create_alias_table(unnormalized_probs)

        # 2. 处理 alias_edges (用于游走的后续步)
        edges = list(G.edges())
        # 把 (u, v) 和 (v, u) 都加进去
        directed_edges = edges + [(v, u) for (u, v) in edges]

        for src, dst in directed_edges:
            self.alias_edges[(src, dst)] = self.get_alias_edge(src, dst)

        print("Preprocessing done.")
    
    def simulate_walks(self, num_walks, walk_length):
        """
        生成随机游走序列
        """
        G = self.G
        walks = []
        nodes = list(G.nodes())
        
        print("Simulating walks...")
        
        # 对每个节点生成 num_walks 条游走
        for _ in range(num_walks):
            # 为了随机性，每次迭代最好打乱节点顺序
            np.random.shuffle(nodes)
            
            for node in nodes:
                walk = [node]
                
                while len(walk) < walk_length:
                    curr = walk[-1]
                    
                    if len(walk) == 1:
                        # 第一步：使用节点的 Alias Table
                        # 注意：alias_nodes[curr] 返回的是 (J, q)
                        J, q = self.alias_nodes[curr]
                        next_node_idx = alias_sample(J, q)
                        # 需要把 index 映射回真实节点 ID
                        next_node = list(G.neighbors(curr))[next_node_idx]
                        
                    else:
                        # 后续步：使用边的 Alias Table
                        prev = walk[-2]
                        # 查找从 prev -> curr 之后，下一步该去哪
                        J, q = self.alias_edges[(prev, curr)]
                        next_node_idx = alias_sample(J, q)
                        next_node = list(G.neighbors(curr))[next_node_idx]
                    
                    walk.append(next_node)
                
                # 把节点 ID 转成 string，因为 Word2Vec 训练需要 string 列表
                walks.append([str(n) for n in walk])
                
        print("Walks simulation done.")
        return walks