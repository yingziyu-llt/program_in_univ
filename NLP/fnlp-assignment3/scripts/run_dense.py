import argparse
import json
from pathlib import Path

import faiss
import numpy as np
import torch
from evaluate import evaluate
from sentence_transformers import SentenceTransformer

PROJECT_ROOT = Path(__file__).resolve().parent.parent
DATA_DIR = PROJECT_ROOT / "data"
OUTPUT_DIR = PROJECT_ROOT / "outputs" / "dense"
MODEL_NAME = "BAAI/bge-large-en-v1.5"
QUERY_PREFIX = "Represent this sentence for searching relevant passages: "


def load_jsonl(path):
    records = []
    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if line:
                records.append(json.loads(line))
    return records


def save_jsonl(records, path):
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w", encoding="utf-8") as f:
        for record in records:
            f.write(json.dumps(record, ensure_ascii=False) + "\n")


def load_dense_resources(device):
    model = SentenceTransformer(MODEL_NAME, device=device)
    model.max_seq_length = 512

    index = faiss.read_index(str(DATA_DIR / "corpus.faiss"))
    corpus_records = load_jsonl(DATA_DIR / "corpus.jsonl")
    corpus_ids = [record["corpus_id"] for record in corpus_records]

    return model, index, corpus_ids


def encode_queries(model, questions, batch_size=64):
    queries = [QUERY_PREFIX + question.strip() for question in questions]
    return model.encode(
        queries,
        batch_size=batch_size,
        show_progress_bar=True,
        normalize_embeddings=True,
    ).astype(np.float32)


def retrieve_batch(questions, model, index, corpus_ids, k=5, batch_size=64):
    question_embeddings = encode_queries(model, questions, batch_size=batch_size)
    scores, indices = index.search(question_embeddings, k)
    return [[corpus_ids[i] for i in row] for row in indices]


def predict_dataset(
    test_path, output_path, model, index, corpus_ids, k=5, batch_size=64
):
    test_records = load_jsonl(test_path)
    questions = [item["question"] for item in test_records]

    retrieved_id_lists = retrieve_batch(
        questions=questions,
        model=model,
        index=index,
        corpus_ids=corpus_ids,
        k=k,
        batch_size=batch_size,
    )

    predictions = [
        {"id": item["id"], "retrieved_corpus_ids": r_ids}
        for item, r_ids in zip(test_records, retrieved_id_lists)
    ]

    save_jsonl(predictions, output_path)


def run_for_hop(hop, model, index, corpus_ids, k=5, batch_size=64, run_evaluate=False):
    test_path = DATA_DIR / f"test_{hop}hop.jsonl"
    output_path = OUTPUT_DIR / f"predictions_{hop}hop.jsonl"
    predict_dataset(
        test_path, output_path, model, index, corpus_ids, k=k, batch_size=batch_size
    )

    if run_evaluate:
        recall, n = evaluate(output_path, test_path, k=k)
        print(f"{hop}-hop Recall@{k}: {recall:.4f}  ({n} questions)")


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--hop", type=int, choices=[2, 3, 4])
    parser.add_argument("--all", action="store_true")
    parser.add_argument("--k", type=int, default=5)
    parser.add_argument("--batch-size", type=int, default=64)
    parser.add_argument("--device", type=str, default="cuda:0")
    parser.add_argument("--evaluate", action="store_true")
    return parser.parse_args()


def main():
    args = parse_args()
    if not args.all and args.hop is None:
        raise SystemExit("Please pass either --hop {2,3,4} or --all")

    model, index, corpus_ids = load_dense_resources(args.device)

    hops = [2, 3, 4] if args.all else [args.hop]
    for hop in hops:
        run_for_hop(
            hop,
            model,
            index,
            corpus_ids,
            k=args.k,
            batch_size=args.batch_size,
            run_evaluate=args.evaluate,
        )


if __name__ == "__main__":
    main()
