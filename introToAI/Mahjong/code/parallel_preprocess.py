# parallel_preprocess.py

import multiprocessing
import os
import json


def find_match_lines(file_path):
    match_lines = []
    total_lines = -1
    with open(file_path, "r") as f:
        for i, line in enumerate(f):
            if line.startswith("Match"):
                match_lines.append(i)
            total_lines = i
    return match_lines, total_lines


def parallel_process(file_path, start_line, end_line, offset, cpu_id):
    os.system(
        f"python preprocess-new.py {file_path} {start_line} {end_line} {offset} {cpu_id}"
    )


if __name__ == "__main__":
    file_path = "data/data.txt"
    match_lines, total_lines = find_match_lines(file_path)
    total_matches = len(match_lines)
    print(len(match_lines))
    num_cpus = multiprocessing.cpu_count()
    # num_cpus = 20
    match_per_process = 100
    chunk_size = num_cpus * match_per_process  # 每个进程处理200场比赛

    processes = []
    total_chunks = len(match_lines) // chunk_size + (
        1 if len(match_lines) % chunk_size != 0 else 0
    )

    for chunk_id in range(total_chunks):
        start_index = chunk_id * chunk_size

        for i in range(num_cpus):
            if start_index + i * match_per_process >= total_matches:
                break
            start_line = match_lines[start_index + i * match_per_process]
            end_line = (
                match_lines[start_index + (i + 1) * match_per_process] - 1
                if (start_index + (i + 1) * match_per_process) < total_matches
                else total_lines
            )

            offset = start_index + i * match_per_process

            p = multiprocessing.Process(
                target=parallel_process,
                args=(file_path, start_line, end_line, offset, chunk_id * num_cpus + i),
            )
            processes.append(p)
            p.start()

        for p in processes:
            p.join()
        processes.clear()  # 清除进程列表，准备下一轮

    # 合并所有进程生成的 JSON 文件
    total_out = []
    for chunk_id in range(total_chunks):
        for i in range(num_cpus):
            try:
                with open(f"data/count-{chunk_id * num_cpus + i}.json", "r") as f:
                    total_out += json.load(f)
            except FileNotFoundError:
                continue

    with open("data/count.json", "w") as f:
        json.dump(total_out, f)

    # 校验 count.json 和 count-backup.json 是否一致
    with open("data/count.json", "r") as f:
        count = json.load(f)
    with open("data/count-back.json", "r") as f:
        count_backup = json.load(f)

    print(len(count), len(count_backup))
    # 检验两个列表从第几个数开始不一样了
    for i in range(len(count)):
        if count[i] != count_backup[i]:
            print(i)
            print(count[i], count_backup[i])
            break
