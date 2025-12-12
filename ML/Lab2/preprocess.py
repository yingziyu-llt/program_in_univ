import argparse
import logging
from pathlib import Path
from typing import List

import torch
from module import Tokenizer
from torch.utils.data import TensorDataset
from tqdm import tqdm

logging.basicConfig(
    level=logging.INFO, format="%(asctime)s - %(name)s - %(levelname)s - %(message)s"
)
logger = logging.getLogger(__name__)


class CoupletExample(object):
    def __init__(self, seq: List[str], tag: List[str]):
        assert len(seq) == len(tag)
        self.seq = seq
        self.tag = tag


class CoupletFeatures(object):
    def __init__(self, input_ids: List[int], target_ids: List[int]):
        self.input_ids = input_ids
        self.target_ids = target_ids


def read_examples(fdir: Path):
    seqs = []
    tags = []
    with open(fdir / "in.txt", "r", encoding="utf-8") as f:
        for line in f.readlines():
            seqs.append(line.split())
    with open(fdir / "out.txt", "r", encoding="utf-8") as f:
        for line in f.readlines():
            tags.append(line.split())
    examples = [CoupletExample(seq, tag) for seq, tag in zip(seqs, tags)]
    return examples


def convert_examples_to_features(examples: List[CoupletExample], tokenizer: Tokenizer):
    features = []
    for example in tqdm(examples, desc="creating features"):
        seq_ids = tokenizer.convert_tokens_to_ids(example.seq)
        tag_ids = (
            [tokenizer.sos_id]
            + tokenizer.convert_tokens_to_ids(example.tag)
            + [tokenizer.eos_id]
        )
        features.append(CoupletFeatures(seq_ids, tag_ids))
    return features


def convert_features_to_tensors(
    features: List[CoupletFeatures], tokenizer: Tokenizer, max_seq_len: int
):
    total = len(features)
    input_ids = torch.full((total, max_seq_len), tokenizer.pad_id, dtype=torch.long)
    target_ids = torch.full(
        (total, max_seq_len + 2), tokenizer.pad_id, dtype=torch.long
    )

    masks = torch.zeros(total, max_seq_len, dtype=torch.bool)
    lens = torch.zeros(total, dtype=torch.long)

    for i, f in enumerate(tqdm(features, desc="creating tensors")):
        seq_len = min(len(f.input_ids), max_seq_len)
        input_ids[i, :seq_len] = torch.tensor(f.input_ids[:seq_len])
        masks[i, :seq_len] = 1
        lens[i] = seq_len
        tgt_len = min(len(f.target_ids), max_seq_len + 2)
        target_ids[i, :tgt_len] = torch.tensor(f.target_ids[:tgt_len])

    return (
        input_ids,
        masks,
        lens,
        target_ids,
    )


def create_dataset(fdir: Path, tokenizer: Tokenizer, max_seq_len: int):
    examples = read_examples(fdir)
    features = convert_examples_to_features(examples, tokenizer)
    tensors = convert_features_to_tensors(features, tokenizer, max_seq_len)
    dataset = TensorDataset(*tensors)
    return dataset


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", default="data/couplet", type=str)
    parser.add_argument("--output", default="dataset", type=str)
    parser.add_argument("--max_seq_len", default=32, type=int)
    args = parser.parse_args()

    input_dir = Path(args.input)
    output_dir = Path(args.output)
    output_dir.parent.mkdir(exist_ok=True, parents=True)
    vocab_file = input_dir / "vocabs"

    logger.info("creating tokenizer...")
    tokenizer = Tokenizer()
    tokenizer.build(vocab_file)

    logger.info("creating dataset...")
    train_dataset = create_dataset(input_dir / "train", tokenizer, args.max_seq_len)
    test_dataset = create_dataset(input_dir / "test", tokenizer, args.max_seq_len)

    logger.info("saving dataset...")
    tokenizer.save_pretrained(output_dir / "vocab.pkl")
    torch.save(train_dataset, output_dir / "train.pkl")
    torch.save(test_dataset, output_dir / "test.pkl")
