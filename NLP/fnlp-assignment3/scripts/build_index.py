import argparse
import json
import multiprocessing as mp
import shutil
import time
from pathlib import Path

import numpy as np
import torch
from sentence_transformers import SentenceTransformer

DATA_DIR = Path(__file__).resolve().parent.parent / "data"
MODEL_NAME = "BAAI/bge-large-en-v1.5"


def load_jsonl(path):
    with open(path, "r", encoding="utf-8") as f:
        return [json.loads(line) for line in f if line.strip()]


def split_evenly(items, num_chunks):
    chunk_size = (len(items) + num_chunks - 1) // num_chunks
    return [
        items[i * chunk_size : min((i + 1) * chunk_size, len(items))]
        for i in range(num_chunks)
    ]


def encode_chunk(worker_id, device, texts, batch_size, output_path):
    print(f"[worker {worker_id}] Loading model: {MODEL_NAME} (device={device})")

    model = SentenceTransformer(MODEL_NAME, device=device)
    model.max_seq_length = 512

    t0 = time.time()
    embeddings = model.encode(
        texts,
        batch_size=batch_size,
        show_progress_bar=True,
        normalize_embeddings=True,
    ).astype(np.float32)
    np.save(output_path, embeddings)

    print(f"[worker {worker_id}] Saved {embeddings.shape} in {time.time() - t0:.1f}s")
    return worker_id, embeddings.shape


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--batch-size", type=int, default=32)
    parser.add_argument(
        "--devices",
        type=str,
        default="cuda:0,cuda:1,cuda:2,cuda:3",
    )
    args = parser.parse_args()

    emb_path = DATA_DIR / "corpus_embeddings.npy"
    if emb_path.exists():
        print(f"[skip] Embeddings exist: {emb_path}")
        embeddings = np.load(emb_path)
    else:
        print("Loading corpus...")
        corpus = load_jsonl(DATA_DIR / "corpus.jsonl")
        corpus_texts = [f"{c['title'].strip()}: {c['text'].strip()}" for c in corpus]

        devices = [d.strip() for d in args.devices.split(",") if d.strip()]

        part_dir = DATA_DIR / "corpus_embeddings.parts"
        if part_dir.exists():
            shutil.rmtree(part_dir)
        part_dir.mkdir(parents=True)

        chunks = split_evenly(corpus_texts, len(devices))
        tasks = [
            (i, d, c, args.batch_size, part_dir / f"part_{i:02d}.npy")
            for i, (d, c) in enumerate(zip(devices, chunks))
            if c
        ]

        t0 = time.time()
        ctx = mp.get_context("spawn")
        with ctx.Pool(processes=len(tasks)) as pool:
            for worker_id, shape in pool.starmap(encode_chunk, tasks):
                pass

        embeddings = np.concatenate(
            [np.load(part_dir / f"part_{i:02d}.npy") for i in range(len(tasks))],
            axis=0,
        )
        np.save(emb_path, embeddings)
        shutil.rmtree(part_dir)

    index_path = DATA_DIR / "corpus.faiss"
    if index_path.exists():
        return

    import faiss

    dim = embeddings.shape[1]
    index = faiss.IndexFlatIP(dim)
    index.add(embeddings)
    faiss.write_index(index, str(index_path))


if __name__ == "__main__":
    main()
