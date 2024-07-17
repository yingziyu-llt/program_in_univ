import re
from concurrent.futures import ProcessPoolExecutor


# 定义字符替换的函数
def cycle_replace(match, subsitute):
    tile = match.group(0)
    return subsitute[tile[0]] + tile[1]


# 处理单行的函数
def process_line(line, subsitutes):
    expanded_lines = []
    if re.search(r"[WTB]\d", line):
        for subsitute in subsitutes:
            expanded_lines.append(
                re.sub(
                    r"[WTB]\d",
                    lambda x: cycle_replace(x, subsitute),
                    line,
                )
            )
    else:
        for _ in subsitutes:
            expanded_lines.append(line)
    return expanded_lines


# 处理单个比赛块的函数
def process_match_block(match_block, subsitutes):
    expanded_block = [[] for _ in range(6)]
    for line in match_block:
        processed_lines = process_line(line, subsitutes)
        for i in range(6):
            expanded_block[i].append(processed_lines[i])
    return expanded_block


# 辅助函数，用于并行处理比赛块
def process_match_block_helper(args):
    match_block, subsitutes = args
    return process_match_block(match_block, subsitutes)


# 主要的文件处理函数
def process_file(file_path):
    subsitutes = [
        {"W": "W", "T": "T", "B": "B"},
        {"W": "T", "T": "B", "B": "W"},
        {"W": "B", "T": "W", "B": "T"},
        {"W": "T", "T": "W", "B": "B"},
        {"W": "B", "T": "T", "B": "W"},
        {"W": "W", "T": "B", "B": "T"},
    ]

    with open(file_path, "r") as f:
        lines = f.readlines()

    # 找出每个比赛块的起始索引
    match_index = [i for i, line in enumerate(lines) if line.startswith("Match")]
    match_blocks = [
        lines[match_index[i] : match_index[i + 1] if i + 1 < len(match_index) else None]
        for i in range(len(match_index))
    ]

    # 使用 ProcessPoolExecutor 并行处理每个比赛块
    with ProcessPoolExecutor() as executor:
        results = list(
            executor.map(
                process_match_block_helper,
                [(block, subsitutes) for block in match_blocks],
            )
        )

    # 将结果写入新文件
    with open("./data/data_new.txt", "w") as f_new:
        for result in results:
            for i in range(6):
                f_new.writelines(result[i])


if __name__ == "__main__":
    process_file("./data/sample.txt")
