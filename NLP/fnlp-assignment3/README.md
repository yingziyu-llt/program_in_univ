# FNLP Lab 3: Multi-hop Evidence Retrieval

This repository contains baseline retrieval code for FNLP Lab 3 on the MuSiQue multi-hop evidence retrieval task.

The task is to retrieve the top-5 supporting paragraphs for each question and evaluate with Recall@5.

## Environment

Use Python 3.10+ if possible. Install the required packages:

```bash
pip install huggingface_hub rank_bm25 numpy torch sentence-transformers tqdm
```

Install FAISS according to your machine:

```bash
# CPU version
pip install faiss-cpu

# Or, if you use conda and need GPU support
conda install -c pytorch -c nvidia faiss-gpu
```

The dense baseline uses `Qwen/Qwen3-Embedding-0.6B`, so the first run will download the model from HuggingFace.

## 1. Prepare Data

Download MuSiQue validation data from HuggingFace, build the global paragraph corpus, and sample 200 questions for each hop count:

```bash
python scripts/prepare_data.py
```

This generates:

```text
data/corpus.jsonl
data/test_2hop.jsonl
data/test_3hop.jsonl
data/test_4hop.jsonl
```
We have already completed the data preparation, so you can skip this section.

## 2. Build Dense Retrieval Index

Before running dense retrieval, encode the full corpus and build the FAISS index:

```bash
python scripts/build_index.py --batch-size 32 --devices cuda:0
```

For multiple GPUs, pass a comma-separated device list:

```bash
python scripts/build_index.py --batch-size 32 --devices cuda:0,cuda:1,cuda:2,cuda:3
```

This generates:

```text
data/corpus_embeddings.npy
data/corpus.faiss
```

If these files already exist, the script reuses them.

## 3. Evaluate Prediction Files

The baseline scripts evaluate automatically, but any prediction file can also be evaluated manually:

```bash
python scripts/evaluate.py \
  --prediction outputs/bm25/predictions_2hop.jsonl \
  --gold data/test_2hop.jsonl \
  --k 5
```

Prediction files must be JSONL, one object per line:

```json
{"id": "question_id", "retrieved_corpus_ids": [1, 2, 3, 4, 5]}
```

## Notes

- The assignment zip should include source code, the report PDF, and the final prediction JSONL files required by the course.
- Dataset files and model weights do not need to be submitted.
