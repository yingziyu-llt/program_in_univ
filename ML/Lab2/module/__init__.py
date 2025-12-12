import argparse

from .model import *
from .tokenizer import Tokenizer


def init_model_by_key(args, tokenizer: Tokenizer):
    key = args.model.lower()
    if key == "seq2seq":
        model = Seq2Seq(
            tokenizer.vocab_size, args.embed_dim, args.hidden_dim, args.n_layer
        )
    else:
        raise KeyError(f"Model `{args.model}` does not exist")
    return model
