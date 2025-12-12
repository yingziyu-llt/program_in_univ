import argparse
from collections import OrderedDict

import torch
from module import init_model_by_key


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


def run():
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--path", required=True, type=str, help="模型路径")
    parser.add_argument("-s", "--stop_flag", default="q", type=str, help="退出标志")
    parser.add_argument("-c", "--cuda", action="store_true", help="使用GPU")
    args = parser.parse_args()

    print("loading model...")
    device = torch.device("cuda" if torch.cuda.is_available() and args.cuda else "cpu")

    # 加载模型信息，注意 map_location 防止 GPU 模型在 CPU 机器上报错
    model_info = torch.load(args.path, map_location=device)

    tokenizer = model_info["tokenzier"]
    saved_args = model_info["args"]

    # 初始化模型结构
    model = init_model_by_key(saved_args, tokenizer)

    # 安全加载权重（处理 DataParallel 保存带来的 module. 前缀）
    state_dict = model_info["model"]
    new_state_dict = OrderedDict()
    for k, v in state_dict.items():
        name = k[7:] if k.startswith("module.") else k
        new_state_dict[name] = v
    model.load_state_dict(new_state_dict)

    model.to(device)
    model.eval()

    print(f"Model loaded. 输入 '{args.stop_flag}' 退出.")

    while True:
        question = input("上联：")
        if question.lower() == args.stop_flag.lower():
            print("Thank you!")
            break

        # 1. 编码输入
        input_seq = tokenizer.encode(question)

        # 2. 准备 Tensor 和 Length
        # Seq2Seq Encoder 需要真实的长度信息
        input_ids = torch.tensor(input_seq).unsqueeze(0).to(device)
        src_len = torch.tensor(
            [len(input_seq)], dtype=torch.long
        )  # 长度放在 CPU 即可，部分 PyTorch 版本传给 pack_padded_sequence 最好在 CPU

        # 3. 推理 (trg=None 触发自回归生成)
        with torch.no_grad():
            # 注意：这里的调用签名必须匹配你在 model.py 中定义的 forward
            logits = model(input_ids, src_len, trg=None)

        # 4. 解码结果
        # logits: [1, max_len, vocab_size] -> 取最大概率的索引
        pred_ids = logits.argmax(dim=-1).squeeze(0).tolist()

        # 5. 清洗特殊 Token (SOS, EOS, PAD)
        pred_clean_ids = clean_seq(pred_ids, tokenizer)
        pred_str = tokenizer.decode(pred_clean_ids)

        print(f"下联：{pred_str}")


if __name__ == "__main__":
    run()
