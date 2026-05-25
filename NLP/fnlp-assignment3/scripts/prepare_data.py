"""
Download MuSiQue dataset from HuggingFace and prepare:
  1. Global corpus (all unique paragraphs from validation set)
  2. Test sets: 200 sampled questions each for 2-hop, 3-hop, 4-hop
"""

import json
import os
import random
from collections import defaultdict
from pathlib import Path

from huggingface_hub import hf_hub_download

REPO_ID = "dgslibisey/MuSiQue"
DEV_FILE = "musique_ans_v1.0_dev.jsonl"
SEED = 42
SAMPLES_PER_HOP = 200

DATA_DIR = Path(__file__).resolve().parent.parent / "data"


def download_dataset():
    """Download the MuSiQue dev JSONL from HuggingFace."""
    local_path = hf_hub_download(
        repo_id=REPO_ID,
        filename=DEV_FILE,
        repo_type="dataset",
        local_dir=DATA_DIR / "raw",
    )
    return local_path


def load_jsonl(path):
    with open(path, "r", encoding="utf-8") as f:
        return [json.loads(line) for line in f if line.strip()]


def build_corpus(samples):
    """
    Build a global corpus from all paragraphs across all validation samples.
    De-duplicate by (title, paragraph_text). Returns:
      - corpus: list of {"corpus_id": int, "title": str, "text": str}
      - key_to_id: mapping (title, text) -> corpus_id
    """
    key_to_id = {}
    corpus = []

    for sample in samples:
        for para in sample["paragraphs"]:
            key = (para["title"], para["paragraph_text"])
            if key not in key_to_id:
                cid = len(corpus)
                key_to_id[key] = cid
                corpus.append({
                    "corpus_id": cid,
                    "title": para["title"],
                    "text": para["paragraph_text"],
                })
    return corpus, key_to_id


def build_test_sample(sample, key_to_id):
    """
    Convert a raw MuSiQue sample into a test record with gold_corpus_ids.
    """
    gold_corpus_ids = []
    for para in sample["paragraphs"]:
        if para["is_supporting"]:
            key = (para["title"], para["paragraph_text"])
            gold_corpus_ids.append(key_to_id[key])

    return {
        "id": sample["id"],
        "question": sample["question"],
        "answer": sample["answer"],
        "answer_aliases": sample.get("answer_aliases", []),
        "gold_corpus_ids": sorted(set(gold_corpus_ids)),
        "num_hops": len(sample["question_decomposition"]),
    }


def save_jsonl(data, path):
    with open(path, "w", encoding="utf-8") as f:
        for item in data:
            f.write(json.dumps(item, ensure_ascii=False) + "\n")


def main():
    DATA_DIR.mkdir(parents=True, exist_ok=True)

    print("Downloading MuSiQue dataset from HuggingFace...")
    dev_path = download_dataset()
    print(f"Downloaded to: {dev_path}")

    print("Loading validation data...")
    all_samples = load_jsonl(dev_path)
    print(f"Total validation samples: {len(all_samples)}")

    print("Building global corpus...")
    corpus, key_to_id = build_corpus(all_samples)
    corpus_path = DATA_DIR / "corpus.jsonl"
    save_jsonl(corpus, corpus_path)
    print(f"Corpus size: {len(corpus)} unique paragraphs -> {corpus_path}")

    # Group by hop count
    by_hops = defaultdict(list)
    for sample in all_samples:
        nhops = len(sample["question_decomposition"])
        by_hops[nhops].append(sample)

    print("Hop distribution in validation set:")
    for nhops in sorted(by_hops.keys()):
        print(f"  {nhops}-hop: {len(by_hops[nhops])} questions")

    # Sample test sets
    random.seed(SEED)
    for nhops in [2, 3, 4]:
        pool = by_hops[nhops]
        if len(pool) < SAMPLES_PER_HOP:
            print(f"Warning: only {len(pool)} {nhops}-hop questions available, "
                  f"using all of them instead of {SAMPLES_PER_HOP}")
            sampled = pool
        else:
            sampled = random.sample(pool, SAMPLES_PER_HOP)

        test_records = [build_test_sample(s, key_to_id) for s in sampled]
        out_path = DATA_DIR / f"test_{nhops}hop.jsonl"
        save_jsonl(test_records, out_path)
        print(f"Saved {len(test_records)} {nhops}-hop test questions -> {out_path}")


if __name__ == "__main__":
    main()
