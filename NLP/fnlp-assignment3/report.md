# FNLP 2026 Assignment 3 实验报告

姓名：林乐天  
学号：2300012154

## 1. 实验任务

本次实验的目标是在 MuSiQue 多跳问答数据上完成多跳证据检索（multi-hop evidence retrieval）。对于每个问题，系统需要从全局段落语料库中返回 Top-5 段落，并使用 Recall@5 评价检索结果是否覆盖了标准支持性证据。


## 2. 数据集与评测方式

实验使用课程提供的 MuSiQue 数据处理结果：

1. 全局语料库：`data/corpus.jsonl`，共 `21,100` 个段落。
2. 测试集：
   - `data/test_2hop.jsonl`：200 个 2-hop 问题
   - `data/test_3hop.jsonl`：200 个 3-hop 问题
   - `data/test_4hop.jsonl`：200 个 4-hop 问题

从现有测试文件可以看到，每个问题的标准证据段落数与 hop 数一致，因此：

1. 2-hop 问题平均需要命中 2 个 gold 段落
2. 3-hop 问题平均需要命中 3 个 gold 段落
3. 4-hop 问题平均需要命中 4 个 gold 段落

评测指标为 Recall@5。对每个问题，记标准支持段落集合为 `Gold(q)`，检索出的前 5 个段落为 `Retrieved_5(q)`，则：


 $$Recall@5(q) = \frac{|Retrieved_5(q) \cap Gold(q)|}{|Gold(q)|}$$


最终结果为所有问题 Recall@5 的平均值。该指标很适合多跳证据检索，因为它直接衡量系统是否覆盖了推理链所需的所有关键段落。

## 3. BM25 稀疏检索

### 3.1 方法原理

BM25 是经典的词项匹配检索方法，本质上仍然是基于词袋模型的相关性打分。它综合考虑：

1. 查询词在文档中的出现频次
2. 查询词在整个语料中的逆文档频率（IDF）
3. 文档长度归一化

其核心思想是：如果问题中的关键词在某段落中多次出现，且这些词在整个语料中较少见，那么该段落更可能与问题相关。

### 3.2 实现细节

`scripts/run_bm25.py` 实现了 BM25 基线，主要细节如下：

1. 使用 `rank_bm25.BM25Okapi` 构建索引。
2. 将每个段落组织为 `title + ": " + text` 的形式参与索引。这样做的原因是标题往往包含实体名，对多跳检索尤其重要。
3. 对文本进行正则分词，只保留字母、数字和部分特殊形式（如连字符、`C++`、`C#`）。
4. 全部转为小写后，去除英文停用词。
5. 使用 `PorterStemmer` 做词干化，降低词形变化带来的影响。
6. 对每个问题直接检索 Top-5 段落，并保存为 JSONL 文件。

这套实现本质上是一个比较强的 lexical baseline。标题拼接、停用词过滤和词干化都能提高实体匹配和关系词匹配的效果。

### 3.3 BM25 实验结果

根据仓库中已有预测文件 `outputs/bm25/predictions_*hop.jsonl`，使用 `scripts/evaluate.py` 计算得到：

| 方法 | 2-hop | 3-hop | 4-hop | 三组平均 |
| --- | ---: | ---: | ---: | ---: |
| BM25 | 0.4975 | 0.3783 | 0.2150 | 0.3636 |
| 课程给定 Baseline | 0.3700 | 0.2700 | 0.1500 | 0.2633 |

可以看到，现有 BM25 实现明显超过了课程提供的稀疏检索 baseline，说明该实现中的文本预处理和标题增强是有效的。

## 4. Dense Retrieval 稠密检索

### 4.1 方法原理

稠密检索的核心思想是：使用预训练文本嵌入模型将问题和段落编码到同一个向量空间中，再通过向量相似度进行检索。与 BM25 依赖显式词项重合不同，稠密检索更擅长处理：

1. 近义表达
2. 改写表达
3. 词面不完全重合但语义接近的证据匹配

在多跳问答场景中，问题往往包含较长的关系链，词面形式比较复杂，因此稠密检索通常比纯词项匹配更有优势。

### 4.2 实现细节

仓库中的 `scripts/build_index.py` 和 `scripts/run_dense.py` 实现了稠密检索流程，主要细节如下：

1. 由于算力限制,难以运行更大的模型.实际使用的嵌入模型是 `BAAI/bge-large-en-v1.5`。
2. 语料编码阶段将每个段落表示为 `title + ": " + text`，与 BM25 保持一致。
3. 查询端使用指令前缀：
   `Represent this sentence for searching relevant passages: `
4. 使用 `SentenceTransformer` 编码，`max_seq_length=512`。
5. 对向量做 `normalize_embeddings=True` 归一化。
6. 使用 `FAISS` 的 `IndexFlatIP` 建立向量索引；在向量归一化后，内积检索等价于余弦相似度检索。
7. `build_index.py` 支持多进程/多 GPU 分块编码全文语料，先保存各分块的 `.npy` 文件，再拼接为完整的 `corpus_embeddings.npy` 并写入 `corpus.faiss`。

这一实现没有进行监督训练或额外微调，属于预训练嵌入模型的零样本检索（zero-shot retrieval）。

### 4.3 Dense 实验结果

根据已有预测文件 `outputs/dense/predictions_*hop.jsonl`，评测结果如下：

| 方法 | 2-hop | 3-hop | 4-hop | 三组平均 |
| --- | ---: | ---: | ---: | ---: |
| Dense Retrieval | 0.6300 | 0.5050 | 0.2963 | 0.4771 |
| 课程给定 Baseline | 0.6000 | 0.5300 | 0.3000 | 0.4767 |

从平均值上看，当前稠密检索结果 `0.4771` 略高于课程 baseline 的 `0.4767`。不过分 hop 观察可以发现：

1. 2-hop 结果优于 baseline
2. 3-hop 和 4-hop 略低于 baseline
3. 由于评分规则按三组平均 Recall@5 计算，因此整体上仍然达到 baseline 水平

## 5. 结果对比与分析

### 5.1 总体对比

将两种方法放在一起比较：

| 方法 | 2-hop | 3-hop | 4-hop | 三组平均 |
| --- | ---: | ---: | ---: | ---: |
| BM25 | 0.4975 | 0.3783 | 0.2150 | 0.3636 |
| Dense Retrieval | 0.6300 | 0.5050 | 0.2963 | 0.4771 |
| Dense - BM25 | +0.1325 | +0.1267 | +0.0813 | +0.1135 |

可以得到几个明显结论：

1. 稠密检索在三个 hop 设置下都优于 BM25。
2. 两种方法的性能都会随着 hop 数增加而明显下降。
3. Dense 相对 BM25 的优势在 2-hop 和 3-hop 时更明显，到 4-hop 时优势变小。

### 5.2 为什么 hop 数越高越难

随着 hop 数增加，问题需要覆盖的 gold 段落数量也增加，但系统始终只能返回 Top-5 段落，因此检索难度显著提升：

1. 对 2-hop 问题，只要命中 2 个关键段落即可获得满分。
2. 对 4-hop 问题，Top-5 中必须尽量容纳 4 个 gold 段落，容错空间只剩 1 个干扰项。
3. 一旦某一步桥接实体没有被正确召回，整条多跳链条的证据覆盖率就会明显下降。

因此，4-hop 场景不仅需要更强的相关性判断，还要求模型在多个证据点之间保持较稳定的覆盖能力。

### 5.3 稠密检索优于 BM25 的原因

Dense Retrieval 的主要优势体现在以下几方面：

1. 能处理问题中的改写表达和抽象关系，不完全依赖关键词重合。
2. 能利用上下文语义，把“地点、人物、组织、事件”之间的关联映射到同一向量空间。
3. 能显著减少完全检索失败的情况。

从现有结果统计来看，Dense 的 zero-recall 数量明显少于 BM25：

| hop | BM25 零命中数 | Dense 零命中数 |
| --- | ---: | ---: |
| 2-hop | 44 | 14 |
| 3-hop | 43 | 14 |
| 4-hop | 71 | 34 |

这说明稠密检索更不容易出现“前五个段落一个标准证据都没有”的灾难性错误。

### 5.4 BM25 仍然有效的场景

尽管整体性能较低，BM25 在某些场景仍然很有竞争力：

1. 问题中包含罕见实体名、作品名、地名时，词面精确匹配非常有效。
2. 如果 gold 段落标题中直接包含问题中的核心实体，BM25 往往能快速命中。
3. 在某些长链条问题中，BM25 对少数关键桥接词的抓取反而比稠密检索更稳定。

换言之，BM25 的优势不在“语义泛化”，而在“实体对齐”和“字面锚定”。

## 6. Case Study

### 6.1 BM25 检索成功但 Dense 表现较差

#### 案例一：县城与县治的精确匹配

- 问题：`What is the capital of the county where Fort Deposit is located?`
- Gold 段落：`Fort Deposit, Alabama`、`Hayneville, Alabama`
- 结果：BM25=`1.0`，Dense=`0.5`

分析：

1. `Fort Deposit, Alabama` 段落中明确出现了 `Lowndes County`。
2. `Hayneville, Alabama` 段落中明确出现了 `county seat of Lowndes County`。
3. BM25 通过 `Fort Deposit`、`county`、`capital/county seat` 这类词面线索直接命中两个 gold。
4. Dense 只命中了第一个 gold，后续却被多个含有 `Fort` 或 `county seat` 的相似地理段落干扰，例如 `Fort Scott National Cemetery` 等。

这说明在实体名非常明确、推理链几乎可以由字面信息串联时，BM25 的精确匹配有明显优势。

#### 案例二：稀有实体链导致语义检索漂移

- 问题：`What forced the departure from the country responsible for Phoe Pyonn Cho of the people once defied by new coinage?`
- Gold 段落：`Phoe Pyonn Cho`、`Ottoman Empire`、`Myanmar`
- 结果：BM25=`0.6667`，Dense=`0.0`

分析：

1. 问题中包含很罕见的实体 `Phoe Pyonn Cho`，BM25 能借助这个强词面信号命中对应电影条目。
2. `Ottoman Empire` 段落中直接提到 Somali Muslim Ajuran Empire defied the Portuguese。
3. `Myanmar` 的 gold 段落中又直接出现 defeated the Portuguese 的描述。
4. Dense 在这个问题上几乎完全漂移到了 `North Korea`、`Korean War` 等主题相近但实体链错误的段落。

这个案例表明：当问题包含多个冷门实体且依赖严格的实体跳转时，稠密向量很容易被抽象语义带偏，而 BM25 更依赖“关键词是否真的出现过”。

### 6.2 Dense 检索成功但 BM25 失败

#### 案例三：表面词误导 BM25

- 问题：`When did the crowning of the person under whom the Chapter House was built happen?`
- Gold 段落：两个 `Westminster Abbey` 相关段落
- 结果：BM25=`0.0`，Dense=`1.0`

分析：

1. 问题里出现 `crowning`，BM25 被词面上相近的 `Crown` 误导，返回了 `A Crown of Swords` 等完全不相关的条目。
2. Dense 则能够把 `Chapter House`、`Henry III`、`coronation` 这几个语义片段关联起来，成功找回两个 Westminster Abbey 段落。

这说明 BM25 的局限在于：它只能利用表面词项，无法判断“看起来像相关词”的结果在语义上是否真的相关。

#### 案例四：多跳关系需要语义桥接

- 问题：`When was the Palau de la Generalitat constructed in the city where Martin from where Jaulin is located died?`
- Gold 段落：`Jaulín`、`Martin of Aragon`、`Gothic architecture`
- 结果：BM25=`0.0`，Dense=`1.0`

分析：

1. 这道题需要完成 `Jaulín -> Aragon -> Martin of Aragon -> Palau de la Generalitat` 的多步桥接。
2. BM25 返回了大量 `Barcelona Metro` 之类的段落，说明它被 `Palau`、`city` 等表面词牵引到了错误语境。
3. Dense 能把 `Jaulín`、`Aragon`、建筑历史等语义线索放到同一表示空间中，因此成功覆盖 3 个 gold 段落。

这个案例反映出稠密检索对复杂关系链更敏感，尤其适合处理词面不完全重合的桥接式问题。

## 7. 进一步讨论

从现有结果看，Dense 虽然整体优于 BM25，但在 4-hop 上仍然只有 `0.2963`，说明仅靠单轮 Top-5 检索仍然不足以稳定覆盖长推理链。后续可以考虑以下改进方向：

1. **稀疏 + 稠密融合检索**：例如将 BM25 与 Dense 的结果做加权融合或 Reciprocal Rank Fusion，兼顾实体精确匹配和语义召回。
2. **重排序（reranking）**：先用 BM25/Dense 召回更多候选，再使用 cross-encoder 对候选段落做精排。
3. **问题分解或迭代检索**：多跳问题可以先显式拆分成多个子问题，再逐跳扩展检索，而不是只做一次性 Top-5 检索。
4. **更强或更适配的嵌入模型**：当前方法没有微调，如果使用针对检索任务训练过的模型，或在 MuSiQue 上进行对比学习微调，结果还有提升空间。

## 8. 结论

本次作业完成了 BM25 稀疏检索与 Dense Retrieval 稠密检索的实验分析。

1. BM25 在三个测试集上的 Recall@5 分别为 `0.4975 / 0.3783 / 0.2150`，平均 `0.3636`，显著超过课程给定的稀疏 baseline。
2. Dense Retrieval 在三个测试集上的 Recall@5 分别为 `0.6300 / 0.5050 / 0.2963`，平均 `0.4771`，整体上略高于课程给定的稠密 baseline。
3. Dense 在所有 hop 上都优于 BM25，说明语义表示对多跳证据检索确实更有效。
4. 但随着 hop 数增加，两类方法都会快速退化，说明多跳证据覆盖仍是当前系统的主要瓶颈。
