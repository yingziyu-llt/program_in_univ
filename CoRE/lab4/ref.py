from causallearn.search.ConstraintBased.PC import pc
from causallearn.utils.cit import fisherz
from causallearn.graph.SHD import SHD
from causallearn.utils.DAG2CPDAG import dag2cpdag
from causallearn.utils.TXT2GeneralGraph import txt2generalgraph

import numpy as np


data_path = 'data_linear_10.txt'
truth_graph_path = 'graph.10.txt'
data = np.loadtxt(data_path, skiprows=1)
truth_dag = txt2generalgraph(truth_graph_path)
truth_cpdag = dag2cpdag(truth_dag)


alpha_list = [0.01, 0.05, 0.2]

for alpha in alpha_list:
    cg = pc(data, alpha, fisherz)
    shd = SHD(truth_cpdag, cg.G)
    print(f'alpha: {alpha}, SHD: {shd.get_shd()}')