"""
BM25 sparse retrieval baseline for FNLP Assignment 3.

Run one test set:

    conda activate ir
    python scripts/run_bm25.py --hop 2

Run all three test sets and evaluate:

    conda activate ir
    python scripts/run_bm25.py --all --evaluate

Expected prediction format:

    {"id": "question_id", "retrieved_corpus_ids": [1, 2, 3, 4, 5]}
"""

import argparse
import json
import re
from pathlib import Path

import nltk
from evaluate import evaluate
from nltk.corpus import stopwords
from nltk.stem import PorterStemmer
from rank_bm25 import BM25Okapi

PROJECT_ROOT = Path(__file__).resolve().parent.parent
DATA_DIR = PROJECT_ROOT / "data"
OUTPUT_DIR = PROJECT_ROOT / "outputs" / "bm25"

nltk.download("stopwords")
STOPWORDS = set(stopwords.words("english"))
STEMMER = PorterStemmer()


def load_jsonl(path):
    """Load a JSONL file into a list of dictionaries."""
    records = []
    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if line:
                records.append(json.loads(line))
    return records


def save_jsonl(records, path):
    """Save a list of dictionaries as JSONL."""
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w", encoding="utf-8") as f:
        for record in records:
            f.write(json.dumps(record, ensure_ascii=False) + "\n")


def tokenize(text):
    """
    Convert text into tokens for BM25.

    filter out stopwords and punctuation, and apply stemming.
    Keep hyphenated wordsq and programming languages like "C++" and "C#".

    """
    pattern = r"\b[a-z0-9]+(?:-[a-z0-9]+)*\b|c\+\+|c#"
    tokens = re.findall(pattern, text.lower())
    return [STEMMER.stem(t) for t in tokens if t not in STOPWORDS]


def corpus_record_to_text(record):
    """
    Convert one corpus record into the searchable text used by BM25.

    Titles are important for entity matching, so the baseline indexes title + text.
    """
    return f"{record['title']}: {record['text']}"


def build_bm25(corpus):
    """
    Build a BM25 index over the global corpus.

    Args:
        corpus: list of records from data/corpus.jsonl

    Returns:
        bm25: BM25Okapi object
        corpus_ids: list where corpus_ids[i] is the original corpus_id of document i
    """
    corpus_ids = []
    tokenized_corpus = []

    for record in corpus:
        corpus_ids.append(record["corpus_id"])
        text = corpus_record_to_text(record)
        tokenized_corpus.append(tokenize(text))

    bm25 = BM25Okapi(tokenized_corpus)
    return bm25, corpus_ids


def retrieve_top_k(question, bm25, corpus_ids, k=5):
    """
    Retrieve top-k corpus ids for one question.
    """
    query_tokens = tokenize(question)
    scores = bm25.get_scores(query_tokens)
    top_indices = sorted(range(len(scores)), key=lambda i: scores[i], reverse=True)[:k]
    return [corpus_ids[i] for i in top_indices]


def predict_dataset(test_path, output_path, bm25, corpus_ids, k=5):
    """Run BM25 retrieval for every question in one test file."""
    test_records = load_jsonl(test_path)
    predictions = []

    for item in test_records:
        retrieved_ids = retrieve_top_k(
            question=item["question"],
            bm25=bm25,
            corpus_ids=corpus_ids,
            k=k,
        )
        predictions.append(
            {
                "id": item["id"],
                "retrieved_corpus_ids": retrieved_ids,
            }
        )

    save_jsonl(predictions, output_path)
    print(f"Saved {len(predictions)} predictions to {output_path}")


def run_for_hop(hop, bm25, corpus_ids, k=5, run_evaluate=False):
    """Run retrieval for one hop count: 2, 3, or 4."""
    test_path = DATA_DIR / f"test_{hop}hop.jsonl"
    output_path = OUTPUT_DIR / f"predictions_{hop}hop.jsonl"
    predict_dataset(test_path, output_path, bm25, corpus_ids, k=k)

    if run_evaluate:
        recall, n = evaluate(output_path, test_path, k=k)
        print(f"{hop}-hop Recall@{k}: {recall:.4f}  ({n} questions)")


def parse_args():
    parser = argparse.ArgumentParser(description="BM25 retrieval for Assignment 3")
    parser.add_argument("--hop", type=int, choices=[2, 3, 4], help="Run one test set")
    parser.add_argument(
        "--all", action="store_true", help="Run 2-hop, 3-hop, and 4-hop"
    )
    parser.add_argument(
        "--k", type=int, default=5, help="Number of paragraphs to retrieve"
    )
    parser.add_argument(
        "--evaluate",
        action="store_true",
        help="Evaluate predictions with scripts/evaluate.py after retrieval",
    )
    return parser.parse_args()


def main():
    args = parse_args()
    if not args.all and args.hop is None:
        raise SystemExit("Please pass either --hop {2,3,4} or --all")

    print("Loading corpus...")
    corpus = load_jsonl(DATA_DIR / "corpus.jsonl")
    print(f"Loaded {len(corpus)} corpus paragraphs")

    print("Building BM25 index...")
    bm25, corpus_ids = build_bm25(corpus)

    if args.all:
        for hop in [2, 3, 4]:
            run_for_hop(hop, bm25, corpus_ids, k=args.k, run_evaluate=args.evaluate)
    else:
        run_for_hop(args.hop, bm25, corpus_ids, k=args.k, run_evaluate=args.evaluate)


if __name__ == "__main__":
    main()
