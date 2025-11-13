# framework.py

import numpy as np
import itertools
import matplotlib.pyplot as plt
import networkx as nx

# =================================================================
# [提供的底层工具函数 - 你无需修改]
# =================================================================

# 导入 causallearn 的相关模块
from causallearn.utils.cit import FisherZ
from causallearn.graph.GeneralGraph import GeneralGraph
from causallearn.graph.GraphNode import GraphNode
from causallearn.graph.SHD import SHD
from causallearn.utils.DAG2CPDAG import dag2cpdag


def fisherz(data, i, j, S):
    """
    使用Fisher Z检验进行条件独立性检验。
    """
    return FisherZ(data)(i, j, S)


def get_neighbors(graph, node):
    """获取一个节点的所有邻居（不区分边的方向）"""
    neighbors = []
    # 检查从 node 出发的边
    for i, edge in enumerate(graph[node, :]):
        if edge != 0:
            neighbors.append(i)
    # 检查指向 node 的边
    for i, edge in enumerate(graph[:, node]):
        if edge != 0:
            neighbors.append(i)
    # 使用 set 去除重复的邻居
    return list(set(neighbors))


def plot_graph(graph, node_names, title):
    """
    可视化因果图。
    - 无向边: - (灰色)
    - 有向边: -> (黑色)
    """
    G = nx.DiGraph()
    if len(node_names) != graph.shape[0]:
        raise ValueError("The length of node_names must match the dimensions of the graph.")

    for i in range(len(node_names)):
        G.add_node(node_names[i])
    
    for i in range(graph.shape[0]):
        for j in range(graph.shape[1]):
            if graph[i, j] == -1 and graph[j, i] == -1:
                if i < j: G.add_edge(node_names[i], node_names[j], style='undirected')
            elif graph[i, j] == 1 and graph[j, i] == 0:
                G.add_edge(node_names[i], node_names[j], style='solid')

    pos = nx.circular_layout(G)
    plt.figure(figsize=(10, 10))
    
    nx.draw_networkx_nodes(G, pos, node_size=3000, node_color='lightblue')
    nx.draw_networkx_labels(G, pos, font_size=12)
    
    directed_edges = [e for e, style in nx.get_edge_attributes(G, 'style').items() if style == 'solid']
    undirected_edges = [e for e, style in nx.get_edge_attributes(G, 'style').items() if style == 'undirected']
    
    nx.draw_networkx_edges(G, pos, edgelist=directed_edges, edge_color='black', 
                           arrowstyle='->', arrowsize=20, width=1.5, node_size=3000)
    nx.draw_networkx_edges(G, pos, edgelist=undirected_edges, edge_color='gray', 
                           arrows=False, width=1.5, node_size=3000)
    
    plt.title(title, fontsize=16)
    plt.show()


def create_ground_truth_graph(node_names):
    """真实的因果图 (DAG)"""
    num_nodes = len(node_names)
    nodes = [GraphNode(name) for name in node_names]
    truth_graph = GeneralGraph(nodes)
    
    edges = [
        ("X1", "X13"), ("X1", "X3"), ("X1", "X4"), ("X1", "X8"),
        ("X10", "X12"), ("X10", "X16"), ("X11", "X14"), ("X11", "X15"),
        ("X12", "X15"), ("X15", "X19"), ("X16", "X20"), ("X2", "X13"),
        ("X2", "X20"), ("X2", "X3"), ("X3", "X10"), ("X3", "X20"),
        ("X3", "X6"), ("X3", "X9"), ("X4", "X14"), ("X4", "X15"),
        ("X4", "X5"), ("X4", "X6"), ("X5", "X20"), ("X5", "X7"),
        ("X6", "X7"), ("X7", "X14"), ("X8", "X14"), ("X8", "X18"),
        ("X8", "X19"), ("X8", "X9")
    ]
    
    for start, end in edges:
        start_node = next(n for n in nodes if n.get_name() == start)
        end_node = next(n for n in nodes if n.get_name() == end)
        truth_graph.add_directed_edge(start_node, end_node)
        
    return truth_graph


def calculate_shd(estimated_graph_matrix, truth_cpdag, node_names):
    """计算估计图与真实图之间的结构汉明距离 (SHD)"""
    
    # 1. 将PC算法得到的邻接矩阵转换为 causallearn 内部表示的邻接矩阵
    num_nodes = len(node_names)
    # 初始化一个全零矩阵
    causal_learn_matrix = np.zeros((num_nodes, num_nodes), dtype=int)

    for i in range(num_nodes):
        for j in range(i, num_nodes):
            # 情况1: i --- j (无向边)
            if estimated_graph_matrix[i, j] == -1 and estimated_graph_matrix[j, i] == -1:
                causal_learn_matrix[i, j] = -1
                causal_learn_matrix[j, i] = -1
            # 情况2: i --> j (有向边)
            elif estimated_graph_matrix[i, j] == 1 and estimated_graph_matrix[j, i] == 0:
                causal_learn_matrix[j, i] = 1
                causal_learn_matrix[i, j] = -1
            # 情况3: j --> i (有向边)
            elif estimated_graph_matrix[j, i] == 1 and estimated_graph_matrix[i, j] == 0:
                causal_learn_matrix[i, j] = 1
                causal_learn_matrix[j, i] = -1

    # 2. 创建 GeneralGraph 对象并直接为其底层的 .graph 属性赋值
    nodes = [GraphNode(name) for name in node_names]
    estimated_graph = GeneralGraph(nodes)
    estimated_graph.graph = causal_learn_matrix

    # 3. 计算 SHD
    shd_calculator = SHD(truth_cpdag, estimated_graph)
    return shd_calculator.get_shd()


# =================================================================
# [你需要实现的部分]
# =================================================================

def skeleton_discovery(data, alpha):
    """
    Subtask 1: 实现PC算法的骨架发现阶段。
    
    参数:
    - data (np.ndarray): 数据集, shape (n_samples, n_features)
    - alpha (float): 显著性水平
    
    返回:
    - graph (np.ndarray): 一个邻接矩阵表示的无向图骨架。如果i和j之间有边，则graph[i, j] = graph[j, i] = -1。
    - sepsets (dict): 一个字典，记录了分离两个节点的分离集。键是排序后的(i, j)元组，值是分离集S。
    """
    n_features = data.shape[1]
    # 初始化一个全连接的无向图。-1 代表无向边, 0 代表没有边
    graph = np.full((n_features, n_features), -1)
    np.fill_diagonal(graph, 0)
    sepsets = {}
    
    d = 0 # 条件集的大小，从0开始
    while True:
        print(f"正在测试大小为 {d} 的条件集...")
        edges_to_remove = []
        max_d_reached = True # 标记是否所有节点的邻居数都小于d
        has_edges_to_test = False # 增加一个标志位来检查是否还有边可以测试
        
        ## TODO: 在下方补全骨架发现的核心逻辑 ##
        # 提示:
        # 1. 遍历所有节点对 (i, j)
        # 2. 检查它们之间是否还有边。
        # 3. 获取节点i的所有邻居，并排除j本身。
        # 4. 如果邻居数量足够大，则生成所有大小为d的条件集S。
        # 5. 对每个条件集S，进行条件独立性检验：`p_val = fisherz(data, i, j, list(S))`。
        # 6. 如果 p_val > alpha，说明 i 和 j 在给定 S 时是独立的。
        for i in range(n_features):
            graph[i, i] = 0 

        for i in range(n_features):
            for j in range(n_features):
                if graph[i,j] != 0:
                    neighbor_i = get_neighbors(graph, i)
                    neighbor_i = [n for n in neighbor_i if n != j]
                    if len(neighbor_i) >= d:
                        max_d_reached = False
                        has_edges_to_test = True
                        for S in itertools.combinations(neighbor_i, d):
                            p_val = fisherz(data, i, j, list(S))
                            if p_val > alpha:
                                edges_to_remove.append((i, j))
                                
                                key = (min(i, j), max(i, j))
                                if key not in sepsets:
                                    sepsets[key] = S
                                break
        
        # 统一移除本轮中找到的所有应被移除的边
        for i, j in edges_to_remove:
            graph[i, j] = graph[j, i] = 0

        # 终止条件：如果所有节点的邻居数都小于当前的条件集大小d，或者图中没有边了，则无法再进行测试，算法结束
        if max_d_reached or not has_edges_to_test:
            break

        d += 1
            
    return graph, sepsets

def identify_v_structures(graph, sepsets):
    """
    Subtask 2: 实现V-结构判定。
    
    参数:
    - graph (np.ndarray): 从 skeleton_discovery 得到的图骨架。
    - sepsets (dict): 从 skeleton_discovery 得到的分离集。
    
    返回:
    - new_graph (np.ndarray): 定向了V-结构后的图。
      - `graph[i, j] = 1, graph[j, i] = 0` 表示 `i -> j`。
      - `graph[i, j] = -1, graph[j, i] = -1` 仍表示无向边 `i -- j`。
    """
    n_features = graph.shape[0]
    new_graph = graph.copy()
    
    ## TODO: 在下方补全V-结构判定的核心逻辑 ##
    # 提示:
    # 1. 遍历图中所有非邻接的节点对(i, j)及其公共邻居k，形成三元组 i-k-j。
    # 2. 检查i和j之间是否没有边。
    # 3. 检查中间节点k是否 **不** 在 (i, j)的分离集中。
    # 4. 如果满足条件，说明 i-k-j 是一个V-结构，将 i-k 和 j-k 定向为 i->k 和 j->k。
    for i in range(n_features):
        for j in range(n_features):
            if i != j and new_graph[i, j] == 0:
                neighbors_i = get_neighbors(new_graph, i)
                neighbors_j = get_neighbors(new_graph, j)
                common_neighbors = set(neighbors_i).intersection(set(neighbors_j))
                for k in common_neighbors:
                    if k != i and k != j:
                        sepset_ij = sepsets.get((min(i,j), max(i,j)), [])
                        if k not in sepset_ij:
                            new_graph[i, k] = 1
                            new_graph[k, i] = 0
                            new_graph[j, k] = 1
                            new_graph[k, j] = 0

    # ## 代码补全结束 ##

    return new_graph

def orient_edges(graph):
    """
    Subtask 3: 实现边的定向（Meek规则）。
    
    参数:
    - graph (np.ndarray): 定向了V-结构后的图。
    
    返回:
    - new_graph (np.ndarray): 应用Meek规则后的最终图（一个CPDAG）。
    """
    new_graph = graph.copy()
    n_features = new_graph.shape[0]

    while True:
        oriented_new_edge = False
        
        ## TODO: 在下方补全Meek规则的核心逻辑 ##
        
        # **规则1: 如果存在 X -> Y - Z, 且 X, Z 不相邻, 则必须定向 Y -> Z**
        #    (因为如果定向成 Y <- Z, 会产生新的V-结构 X -> Y <- Z, 但X和Z不相邻, 这与V-结构判定的前提矛盾)
        # 提示:
        # 1. 遍历所有无向边 (y, z)，使用 `if new_graph[y, z] == -1:`。
        # 2. 对每个无向边，检查是否存在一个节点x，满足 x->y 且 x与z不相邻。
        # 3. 别忘了检查对称情况：是否存在一个节点x，满足 x->z 且 x与y不相邻。
        for y in range(n_features):
            for z in range(y + 1, n_features):
                if new_graph[y, z] == -1: # y - z
                    for x in range(n_features):
                        if new_graph[x, y] == 1 and new_graph[y, x] == 0 and new_graph[x, z] == 0 and new_graph[z, x] == 0: # x -> y and x not adjacent to z
                            new_graph[y, z] = 1
                            new_graph[z, y] = 0
                            oriented_new_edge = True
                            break
                        if new_graph[x, z] == 1 and new_graph[z, x] == 0 and new_graph[x, y] == 0 and new_graph[y, x] == 0: # x -> z and x not adjacent to y
                            new_graph[z, y] = 1
                            new_graph[y, z] = 0
                            oriented_new_edge = True
                            break
                if oriented_new_edge:
                    break
            if oriented_new_edge:
                break
        if oriented_new_edge:
            continue
        
        
        # **规则2: 如果存在链 X -> Y -> Z, 且 X - Z, 则必须定向 X -> Z**
        #    (因为如果定向成 X <- Z, 会产生环路 X -> Y -> Z <- X)
        # 提示:
        # 1. 遍历所有无向边 (x, z)。
        # 2. 寻找是否存在一个中间节点y，使得 x -> y -> z。
        for x in range(n_features):
            for z in range(x + 1, n_features):
                if new_graph[x, z] == -1: # x - z
                    for y in range(n_features):
                        if new_graph[x, y] == 1 and new_graph[y, x] == 0 and new_graph[y, z] == 1 and new_graph[z, y] == 0: # x -> y -> z
                            new_graph[x, z] = 1
                            new_graph[z, x] = 0
                            oriented_new_edge = True
                            break
                        if new_graph[z, y] == 1 and new_graph[y, z] == 0 and new_graph[y, x] == 1 and new_graph[x, y] == 0: # z -> y -> x
                            new_graph[z, x] = 1
                            new_graph[x, z] = 0
                            oriented_new_edge = True
                            break
                if oriented_new_edge:
                    break
            if oriented_new_edge:
                break
        if oriented_new_edge:
            continue
        
        
        # **规则3: 如果存在两个路径 X-K->Y 和 X-L->Y, 且K和L不相邻, 则必须定向 X -> Y**
        for y in range(n_features):
            for x in range(n_features):
                if new_graph[x, y] == -1: # 找到 X - Y
                    
                    # 1. 找到所有 K->Y 的父节点
                    parents_of_y = []
                    for k in range(n_features):
                        if new_graph[k, y] == 1 and new_graph[y, k] == 0:
                            parents_of_y.append(k)
                    
                    if len(parents_of_y) < 2:
                        continue
                        
                    # 2. 获取 X 的所有邻居 (get_neighbors 会处理所有方向)
                    adj_x = get_neighbors(new_graph, x) 
                    adj_x_set = set(adj_x)

                    # 3. 遍历 Y 的所有父节点对 (K, L)
                    for i in range(len(parents_of_y)):
                        for j in range(i + 1, len(parents_of_y)):
                            k = parents_of_y[i]
                            l = parents_of_y[j]
                            
                            # 4. 检查 K 和 L 是否不相邻
                            if new_graph[k, l] == 0 and new_graph[l, k] == 0:
                                
                                # 5. [正确逻辑] 检查 K 和 L 是否都与 X 相邻
                                if k in adj_x_set and l in adj_x_set:
                                    
                                    # 6. 定向 X -> Y
                                    new_graph[x, y] = 1
                                    new_graph[y, x] = 0
                                    oriented_new_edge = True
                                    break
                        if oriented_new_edge:
                            break
                if oriented_new_edge:
                    break
            if oriented_new_edge:
                break        

        # ## 提示代码 (可参考) ##
        # for y in range(n_features):
        #     for z in range(y + 1, n_features):
        #         # 规则1
        #         if new_graph[y, z] == -1: # y - z
        #             for x in range(n_features):
        #                 if new_graph[x, y] == 1 and new_graph[y, x] == 0 and new_graph[x, z] == 0 and new_graph[z, x] == 0: # x -> y and x not adjacent to z
        #                     new_graph[y, z] = 1
        #                     new_graph[z, y] = 0
        #                     oriented_new_edge = True
        #                     break
        #                 if new_graph[x, z] == 1 and new_graph[z, x] == 0 and new_graph[x, y] == 0 and new_graph[y, x] == 0: # x -> z and x not adjacent to y
        #                     new_graph[z, y] = 1
        #                     new_graph[y, z] = 0
        #                     oriented_new_edge = True
        #                     break
        # if oriented_new_edge: continue
        
        # for x in range(n_features):
        #     for z in range(x + 1, n_features):
        #         # 规则2
        #         xxxxxxx
        # if oriented_new_edge: continue

        # ## 代码补全结束 ##

        # 如果在一轮循环中没有新的边被定向，则算法收敛，可以结束
        if not oriented_new_edge:
            break
            
    return new_graph


def my_pc(data, alpha):
    """
    将PC算法的三个阶段组合起来。你无需修改此函数。
    """
    print("="*20)
    print(f"开始使用 alpha = {alpha} 进行因果发现...")
    
    # 阶段 1: 骨架发现
    graph, sepsets = skeleton_discovery(data, alpha)
    
    # 阶段 2: V-结构判定
    graph = identify_v_structures(graph, sepsets)
    
    # 阶段 3: 边定向
    graph = orient_edges(graph)
    
    print(f"因果发现完成!")
    print("="*20 + "\n")
    return graph


def analyze_causal_graph(data_path):
    """
    Subtask 4: 使用你实现的PC算法进行因果发现和分析。
    """
    data = np.loadtxt(data_path, skiprows=1)
    n_features = data.shape[1]
    node_names = [f'X{i+1}' for i in range(n_features)]
    
    # 1. 创建真实图的CPDAG表示，用于后续比较
    truth_dag = create_ground_truth_graph(node_names)
    truth_cpdag = dag2cpdag(truth_dag)
    print(f"真实图中有 {truth_dag.get_num_edges()} 条有向边。")
    
    alphas_to_test = [0.01, 0.05, 0.2]
    
    for alpha in alphas_to_test:
        # 2. 运行学生实现的PC算法
        estimated_graph_matrix = my_pc(data, alpha)
        
        # 3. 计算并打印SHD
        shd = calculate_shd(estimated_graph_matrix, truth_cpdag, node_names)
        print(f"当 alpha = {alpha}, 计算出的 SHD (结构汉明距离) 为: {shd}")
        print("="*20 + "\n")
        
        # 4. 可视化结果
        title = f'Causal Graph with alpha = {alpha}\nSHD = {shd}'
        plot_graph(estimated_graph_matrix, node_names, title)

    
    # ## 代码补全结束 ##

# =================================================================
# [主程序入口]
# =================================================================

if __name__ == '__main__':
    data_file = 'data_linear_10.txt'
    
    try:
        analyze_causal_graph(data_file)
    except FileNotFoundError:
        print(f"\n[错误] 找不到数据文件 '{data_file}'。")
        print("请确保该文件与您的Python脚本位于同一目录下。")
    except Exception as e:
        print(f"\n程序运行中出现了一个意料之外的错误: {e}")
