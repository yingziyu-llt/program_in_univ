import jieba.posseg as pseg
from collections import Counter

# 输入文件路径
txt_file_name = './data/红楼梦.txt'

# 打开文件，读入文字
try:
    with open(txt_file_name, 'r', encoding='utf-8') as txt_file:
        text = txt_file.read()
except Exception as e:
    print(f"读取文件时发生错误：{e}")
    exit(1)

# 分词并提取人名
word_gen = pseg.cut(text)
name_list = [word for word, flag in word_gen if flag == 'nr' and len(word) > 1]

# 统计人名出现次数
name_counter = Counter(name_list)

# 输出高频人名
print("高频人名：")
for name, count in name_counter.most_common(100):  # 提取100个高频人名
    print(f"{name} {count} nr")