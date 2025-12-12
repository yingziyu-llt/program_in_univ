import argparse
import logging
import re  # 【新增】用于解析文件名中的 epoch
import time
from pathlib import Path

import torch
import torch.nn as nn
import torch.optim as optim

# 确保 module/__init__.py 中已经可以正确 import 新的 Seq2Seq 模型
from module import Tokenizer, init_model_by_key

# 保持原有的 metric 引用
from module.metric import calc_bleu, calc_rouge_l
from torch.utils.data import DataLoader
from torch.utils.tensorboard import SummaryWriter

logging.basicConfig(
    level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s"
)
logger = logging.getLogger(__name__)


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--epochs", default=20, type=int)
    # Seq2Seq 模型参数量大，建议调小 batch_size
    parser.add_argument("--batch_size", default=64, type=int)
    parser.add_argument("--max_seq_len", default=32, type=int)
    parser.add_argument("--lr", default=0.001, type=float)
    parser.add_argument("--no_cuda", action="store_true")
    # 默认模型改为 seq2seq
    parser.add_argument("-m", "--model", default="seq2seq", type=str)
    parser.add_argument("--fp16", action="store_true")
    parser.add_argument("--fp16_opt_level", default="O1", type=str)
    parser.add_argument("--max_grad_norm", default=3.0, type=float)
    parser.add_argument("--dir", default="dataset", type=str)
    parser.add_argument("--output", default="output", type=str)
    parser.add_argument("--logdir", default="runs", type=str)

    # 【新增】接续训练参数
    parser.add_argument(
        "--resume", default="", type=str, help="Path to checkpoint to resume from"
    )

    # 模型超参数
    parser.add_argument("--embed_dim", default=128, type=int)
    parser.add_argument("--n_layer", default=2, type=int)  # Seq2Seq 通常 2 层效果更好
    parser.add_argument("--hidden_dim", default=256, type=int)
    parser.add_argument("--ff_dim", default=512, type=int)
    parser.add_argument("--n_head", default=8, type=int)

    parser.add_argument("--test_epoch", default=1, type=int)
    parser.add_argument("--save_epoch", default=10, type=int)

    parser.add_argument("--embed_drop", default=0.2, type=float)
    parser.add_argument("--hidden_drop", default=0.1, type=float)
    return parser.parse_args()


def clean_seq(seq_ids, tokenizer):
    """
    辅助函数：清理生成的序列 ID
    1. 遇到 EOS 停止
    2. 去除 SOS, PAD, UNK 等特殊字符
    """
    res = []
    for idx in seq_ids:
        # 如果是 EOS，直接结束
        if hasattr(tokenizer, "eos_id") and idx == tokenizer.eos_id:
            break
        # 如果是 SOS，跳过
        if hasattr(tokenizer, "sos_id") and idx == tokenizer.sos_id:
            continue
        # 如果是 PAD，跳过
        if idx == tokenizer.pad_id:
            continue

        res.append(idx)
    return res


def auto_evaluate(model, testloader, tokenizer):
    bleus = []
    rls = []
    device = next(model.parameters()).device
    model.eval()

    for step, batch in enumerate(testloader):
        # 注意：这里解包也要对应 preprocess.py 的修改
        # input_ids, masks, lens, target_ids
        batch = tuple(t.to(device) for t in batch)
        input_ids, masks, lens, target_ids = batch

        with torch.no_grad():
            # 验证阶段：trg=None，触发自回归生成
            logits = model(input_ids, lens, trg=None)
            # logits: [batch, max_len, vocab_size]
            _, preds = torch.max(logits, dim=-1)

        for seq, tag in zip(preds.tolist(), target_ids.tolist()):
            # 清洗预测结果 (Prediction)
            seq_clean = clean_seq(seq, tokenizer)
            # 清洗真实标签 (Reference)
            tag_clean = clean_seq(tag, tokenizer)

            # 如果生成为空，给个极低分防止报错
            if not seq_clean:
                bleus.append(0.0)
                rls.append(0.0)
                continue

            bleu = calc_bleu(seq_clean, tag_clean)
            rl = calc_rouge_l(seq_clean, tag_clean)
            bleus.append(bleu)
            rls.append(rl)

    if len(bleus) == 0:
        return 0.0, 0.0
    return sum(bleus) / len(bleus), sum(rls) / len(rls)


def predict_demos(model, tokenizer: Tokenizer):
    demos = [
        "马齿草焉无马齿",
        "天古天今，地中地外，古今中外存天地",
        "笑取琴书温旧梦",
        "日里千人拱手划船，齐歌狂吼川江号子",
        "我有诗情堪纵酒",
        "我以真诚溶冷血",
        "三世业岐黄，妙手回春人共赞",
    ]

    # [FIX] 如果是多卡并行模式，解包出单卡模型进行预测
    if isinstance(model, nn.DataParallel):
        infer_model = model.module
    else:
        infer_model = model

    infer_model.eval()
    device = next(infer_model.parameters()).device

    sents = [torch.tensor(tokenizer.encode(sent)) for sent in demos]

    for i, sent in enumerate(sents):
        # 构造 Batch=1
        input_tensor = sent.unsqueeze(0).to(device)
        len_tensor = torch.tensor(
            [len(sent)], dtype=torch.long
        )  # 长度放在 CPU 即可，或 .to(device) 亦可

        with torch.no_grad():
            # 使用 infer_model 而不是 model
            logits = infer_model(input_tensor, len_tensor, trg=None)

        pred_ids = logits.argmax(dim=-1).squeeze(0).tolist()
        pred_clean_ids = clean_seq(pred_ids, tokenizer)
        pred_str = tokenizer.decode(pred_clean_ids)

        logger.info(f"上联：{demos[i]}。 预测的下联：{pred_str}")


def save_model(filename, model, args, tokenizer):
    info_dict = {"model": model.state_dict(), "args": args, "tokenzier": tokenizer}
    torch.save(info_dict, filename)


def run():
    args = get_args()
    fdir = Path(args.dir)
    tb = SummaryWriter(args.logdir)
    device = torch.device(
        "cuda" if torch.cuda.is_available() and not args.no_cuda else "cpu"
    )
    output_dir = Path(args.output)
    output_dir.mkdir(exist_ok=True, parents=True)

    logger.info(args)
    logger.info("loading vocab...")
    tokenizer = Tokenizer.from_pretrained(fdir / "vocab.pkl")

    logger.info("loading dataset...")
    # 确保 preprocess.py 重新运行过，生成的 pkl 包含新的数据结构
    train_dataset = torch.load(fdir / "train.pkl")
    test_dataset = torch.load(fdir / "test.pkl")

    train_loader = DataLoader(train_dataset, batch_size=args.batch_size, shuffle=True)
    test_loader = DataLoader(test_dataset, batch_size=args.batch_size)

    logger.info("initializing model...")
    model = init_model_by_key(args, tokenizer)
    model.to(device)

    # 【新增】接续训练逻辑
    start_epoch = 0
    if args.resume:
        if Path(args.resume).exists():
            logger.info(f"Loading checkpoint from {args.resume}...")
            checkpoint = torch.load(args.resume, map_location=device)

            # 1. 加载模型权重
            state_dict = checkpoint["model"]
            # 如果checkpoint是DataParallel保存的（key以module.开头），但当前model还没wrap，需要去掉前缀
            from collections import OrderedDict

            new_state_dict = OrderedDict()
            for k, v in state_dict.items():
                name = k[7:] if k.startswith("module.") else k
                new_state_dict[name] = v
            model.load_state_dict(new_state_dict)

            # 2. 从文件名解析起始 epoch
            # 假设文件名格式为 {model}_{epoch}.bin，例如 seq2seq_10.bin
            try:
                fname = Path(args.resume).stem
                match = re.search(r"_(\d+)$", fname)
                if match:
                    # 文件名中的数字通常代表已完成的 epoch 数
                    # 例如 seq2seq_10.bin 表示完成了前10个epoch (0-9)，下一个从10开始
                    start_epoch = int(match.group(1))
                    logger.info(
                        f"Detected completed epoch {start_epoch}, resuming from epoch {start_epoch}"
                    )
                else:
                    logger.warning(
                        "Could not parse epoch from filename. Starting from epoch 0."
                    )
            except Exception as e:
                logger.warning(f"Error parsing epoch: {e}. Starting from epoch 0.")
        else:
            logger.error(
                f"Checkpoint file {args.resume} not found! Starting from scratch."
            )

    # Loss 忽略 PAD
    loss_function = nn.CrossEntropyLoss(ignore_index=tokenizer.pad_id)
    optimizer = optim.Adam(model.parameters(), lr=args.lr)

    # FP16 处理 (保持原样)
    if args.fp16:
        try:
            from apex import amp

            amp.register_half_function(torch, "einsum")
            model, optimizer = amp.initialize(
                model, optimizer, opt_level=args.fp16_opt_level
            )
        except ImportError:
            raise ImportError("Please install apex to use fp16 training.")

    if torch.cuda.device_count() > 1:
        model = torch.nn.DataParallel(model)

    scheduler = optim.lr_scheduler.ReduceLROnPlateau(optimizer, mode="min")
    logger.info(f"num gpu: {torch.cuda.device_count()}")

    global_step = start_epoch * len(train_loader)  # 修正 global_step 计数

    # 【关键修改】从 start_epoch 开始循环
    for epoch in range(start_epoch, args.epochs):
        logger.info(f"***** Epoch {epoch} *****")
        model.train()
        t1 = time.time()
        accu_loss = 0.0

        for step, batch in enumerate(train_loader):
            optimizer.zero_grad()
            batch = tuple(t.to(device) for t in batch)

            # 获取 target_ids
            # input_ids, masks, lens, target_ids
            input_ids, masks, lens, target_ids = batch

            # Forward: 传入 target_ids 进行 Teacher Forcing 训练
            logits = model(input_ids, lens, target_ids)
            output_preds = logits[:, 1:, :].contiguous()
            output_labels = target_ids[:, 1:].contiguous()

            loss = loss_function(
                output_preds.view(-1, tokenizer.vocab_size), output_labels.view(-1)
            )

            if torch.cuda.device_count() > 1:
                loss = loss.mean()
            accu_loss += loss.item()

            if args.fp16:
                with amp.scale_loss(loss, optimizer) as scaled_loss:
                    scaled_loss.backward()
                torch.nn.utils.clip_grad_norm_(
                    amp.master_params(optimizer), args.max_grad_norm
                )
            else:
                loss.backward()
                torch.nn.utils.clip_grad_norm_(model.parameters(), args.max_grad_norm)

            optimizer.step()

            if step % 100 == 0:
                tb.add_scalar("loss", loss.item(), global_step)
                logger.info(f"[epoch]: {epoch}, [batch]: {step}, [loss]: {loss.item()}")
            global_step += 1

        scheduler.step(accu_loss)
        t2 = time.time()
        logger.info(f"epoch time: {t2 - t1:.5}, accumulation loss: {accu_loss:.6}")

        if (epoch + 1) % args.test_epoch == 0:
            predict_demos(model, tokenizer)
            bleu, rl = auto_evaluate(model, test_loader, tokenizer)
            logger.info(f"BLEU: {round(bleu, 9)}, Rouge-L: {round(rl, 8)}")

        if (epoch + 1) % args.save_epoch == 0:
            # 保持原来的保存逻辑：文件名使用 epoch + 1
            filename = f"{args.model}_{epoch + 1}.bin"
            filename = output_dir / filename
            save_model(filename, model, args, tokenizer)


if __name__ == "__main__":
    run()
