Prove the submodularity of the **influence spread** \( f(S) \) in influence maximization problem using Linear Threshold (LT) model. \( f(S) \) is defined as the number of activated nodes starting from the initial seed set \( S \).

证明：  
我们要证明的命题是：
$$f(S \cup \{t\}) - f(S) \geq f(R \cup \{t\}) - f(R)$$
其中$S \subseteq R$,且$t \notin S$

设节点$v$有多个激活了的入边邻居$u$，权重为$b_{u,v}$。$v$的阈值为随机值$\theta_b \in [0,1]$.

我们容易构造出一个等价的过程。将区间$[0,1]$分割为$b_{u,v}$的几段，最后一段为$1 - \sum b_{u,v}$，每次随机抽签，落到哪个区间就称这个区间为决定者，将二者边保留，若落入空区间，则这次更新中心节点未被激活。

那么可以发现，这个过程最后生成的图节点的分布和LTM是一致的。

所以我们就将问题转化为，在一个确定的图 $G$ 中，令 $R(S)$ 为从 $S$ 出发能到达的节点数，证明 $R(S)$ 是子模的。

从$S$中取一个子集$T$，那么容易发现，对于一个新节点 $t$ 所产生的可到达节点：  
1. $S$可以到达，而$T$不能到达
2. $S$和$T$都不能到达

因此，$R(S \cup \{t\}) - R(S) \leq R(T \cup \{t\}) - R(T)$
，$R(S)$是子模的。故证。