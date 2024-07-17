import os
import re

if __name__ == "__main__":
    with open("./data/data.txt", "r") as f:
        subsitute = [
            {
                "W": "W",
                "T": "T",
                "B": "B",
            },
            {
                "W": "T",
                "T": "B",
                "B": "W",
            },
            {
                "W": "B",
                "T": "W",
                "B": "T",
            },
            {
                "W": "T",
                "T": "W",
                "B": "B",
            },
            {
                "W": "B",
                "T": "T",
                "B": "W",
            },
            {
                "W": "W",
                "T": "B",
                "B": "T",
            },
        ]

        def cycle_replace(tile, str_x):
            return str_x[tile[0]] + tile[1]

        expand_data = [[] for _ in range(6)]
        for line in f:
            if re.search(r"[WTB]\d", line):
                for i in range(6):
                    expand_data[i].append(
                        re.sub(
                            r"[WTB]\d",
                            lambda x: cycle_replace(x.group(0), subsitute[i]),
                            line,
                        )
                    )
            else:
                for i in range(6):
                    expand_data[i].append(line)

        march_index = []
        for i, line in enumerate(expand_data[0]):
            if line.startswith("Match"):
                march_index.append(i)
        with open("./data/data_new.txt", "w") as f_new:
            for march_begin, march_end in zip(
                march_index, march_index[1:] + [len(expand_data[0])]
            ):
                for i in range(6):
                    f_new.writelines(expand_data[i][march_begin:march_end])
