"""
Evaluate retrieval predictions using Recall@K.

Usage:
    python scripts/evaluate.py --prediction predictions.jsonl --gold data/test_2hop.jsonl [--k 5]

Prediction format (JSONL): {"id": str, "retrieved_corpus_ids": [int, ...]}
Gold format (JSONL):       {"id": str, "gold_corpus_ids": [int, ...], ...}
"""

import argparse
import json
import sys
from pathlib import Path


def load_jsonl(path):
    with open(path, "r", encoding="utf-8") as f:
        return [json.loads(line) for line in f if line.strip()]


def compute_recall_at_k(gold_ids, retrieved_ids, k):
    """Recall@K for a single query."""
    if not gold_ids:
        return 1.0
    retrieved_set = set(retrieved_ids[:k])
    gold_set = set(gold_ids)
    return len(retrieved_set & gold_set) / len(gold_set)


def evaluate(pred_path, gold_path, k=5):
    preds = load_jsonl(pred_path)
    golds = load_jsonl(gold_path)

    gold_map = {g["id"]: g["gold_corpus_ids"] for g in golds}

    pred_map = {}
    for p in preds:
        pred_map[p["id"]] = p["retrieved_corpus_ids"]

    missing = set(gold_map.keys()) - set(pred_map.keys())
    if missing:
        print(f"Warning: {len(missing)} gold questions have no predictions")

    recalls = []
    for qid, gold_ids in gold_map.items():
        retrieved = pred_map.get(qid, [])
        r = compute_recall_at_k(gold_ids, retrieved, k)
        recalls.append(r)

    avg_recall = sum(recalls) / len(recalls) if recalls else 0.0
    return avg_recall, len(recalls)


def main():
    parser = argparse.ArgumentParser(description="Evaluate retrieval with Recall@K")
    parser.add_argument("--prediction", required=True, help="Path to prediction JSONL")
    parser.add_argument("--gold", required=True, help="Path to gold test JSONL")
    parser.add_argument("--k", type=int, default=5, help="K for Recall@K (default: 5)")
    args = parser.parse_args()

    avg_recall, n = evaluate(args.prediction, args.gold, args.k)
    print(f"Recall@{args.k}: {avg_recall:.4f}  ({n} questions)")


if __name__ == "__main__":
    main()
