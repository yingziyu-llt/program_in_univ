"""
Date: 2023.03.11
Author: Justin

要点说明：
统计人名“共现”情况
基于前面的程序示例，对《红楼梦》全文进行统计
"""

import jieba
import jieba.posseg as pseg
from tqdm import tqdm  # 使用 tqdm 显示进度条


##--- 第0步：准备工作，重要变量的声明

# 输入文件
txt_file_name = './data/红楼梦.txt'
# 输出文件
node_file_name = './output/红楼梦-人物节点.csv'
link_file_name = './output/红楼梦-人物连接.csv'

# 忽略词列表
ignore_set = {
    '宝玉曰', '黛玉曰', '宝钗曰', '凤姐曰', '贾母曰', '太太',
    '奶奶', '丫鬟', '小厮', '众人', '小姐', '夫人', '老爷',
    '明白', '言语', '冷笑', '老婆子', '那丫头', '老祖宗',
    '大老爷', '宝兄弟', '凤丫头', '贾政听', '贾母笑','胡涂','丫鬟'
}
# 人名合并规则
name_mapping = {
    '宝玉': '贾宝玉',
    '黛玉': '林黛玉',
    '林妹妹': '林黛玉',
    '宝钗': '薛宝钗',
    '凤姐': '王熙凤',
    '凤姐儿': '王熙凤',
    '老太太': '史太君',
    '贾母': '史太君',
    '贾母王': '史太君',
    '老祖宗': '史太君',
    '林姑娘': '林黛玉',
    '宝二爷': '贾宝玉',
    '宝姐姐': '薛宝钗',
    '宝琴': '薛宝琴',
    '宝钗笑': '薛宝钗',
    '贾政道': '贾政',
    '贾政听': '贾政',
    '雨村': '贾雨村',
    '和尚': '妙玉',
    '刘老老': '刘姥姥',
    '巧姐': '贾巧姐',
    '北静王': '北静王',
    '冯紫英': '冯紫英',
    '包勇': '包勇',
    '鲍二': '鲍二',
    '司棋': '司棋',
    '秋纹': '秋纹',
    '雪雁': '雪雁',
    '金桂': '夏金桂',
    '迎春': '贾迎春',
    '惜春': '贾惜春',
    '探春': '贾探春',
    '元妃': '贾元春',
    '赵姨娘': '赵姨娘',
    '邢夫人': '邢夫人',
    '尤氏': '尤氏',
    '薛蟠': '薛蟠',
    '周瑞': '周瑞',
    '林之孝': '林之孝',
    '秦氏': '秦可卿',
    '贾环': '贾环',
    '贾兰': '贾兰',
    '贾瑞': '贾瑞',
    '士隐': '甄士隐',
    '贾蓉': '贾蓉',
    '贾芸': '贾芸',
    '贾珍': '贾珍',
    '平儿': '平儿',
    '紫鹃': '紫鹃',
    '小红': '小红',
    '旺儿': '旺儿',
    '小丫头': '丫鬟',
    '贾政': '贾政',
    '王夫人': '王夫人',
    '薛姨妈': '薛姨妈',
    '妙玉': '妙玉',
    '湘云': '史湘云',
    '云儿': '史湘云',
    '史大姑娘': '史湘云',
}

# 统计限制条件
name_cnt_limit = 50  # 只统计出现次数达到此值及以上的人物
link_cnt_limit = 5   # 只导出共现次数达到此值及以上的连接

# 加载用户字典
jieba.load_userdict('./data/userdict.txt')  # 红楼梦专用用户字典


##--- 第1步：生成基础数据（一个列表，一个字典）

# 打开文件，读入文字
try:
    with open(txt_file_name, 'r', encoding='utf-8') as txt_file:
        line_list = txt_file.readlines()
except FileNotFoundError:
    print(f"文件未找到：{txt_file_name}")
    exit(1)
except Exception as e:
    print(f"读取文件时发生错误：{e}")
    exit(1)

line_name_list = []  # 每个段落出现的人物列表
name_cnt_dict = {}   # 统计人物出现次数

print('正在分段统计……')
for line in tqdm(line_list, desc="处理段落", unit="行"):
    word_gen = pseg.cut(line)  # 分词结果，“生成器”类型
    current_names = []

    for one in word_gen:
        word, flag = one.word, one.flag

        # 跳过单字词或不在人名词性范围内的词
        if len(word) == 1 or flag != 'nr':
            continue

        # 跳过忽略词
        if word in ignore_set:
            continue

        # 合并别名
        word = name_mapping.get(word, word)

        # 记录当前段落中的人物
        current_names.append(word)

        # 更新人物出现次数
        name_cnt_dict[word] = name_cnt_dict.get(word, 0) + 1

    line_name_list.append(current_names)

print('基础数据处理完成')


##--- 第2步：统计人名“共现”数量

relation_dict = {}

print('正在统计共现关系……')
for line_name in tqdm(line_name_list, desc="统计共现", unit="段"):
    # 过滤掉出现次数不足的人物
    filtered_names = [name for name in line_name if name_cnt_dict[name] >= name_cnt_limit]

    # 统计共现关系
    for i, name1 in enumerate(filtered_names):
        if name1 not in relation_dict:
            relation_dict[name1] = {}

        for name2 in filtered_names[i + 1:]:
            relation_dict[name1][name2] = relation_dict[name1].get(name2, 0) + 1

print('共现统计完成，仅统计出现次数达到{}及以上的人物'.format(name_cnt_limit))


##--- 第3步：输出统计结果

# 字典转成列表，按出现次数排序
item_list = sorted(name_cnt_dict.items(), key=lambda x: x[1], reverse=True)

## 导出节点文件
try:
    with open(node_file_name, 'w', encoding='utf-8') as node_file:
        node_file.write('Name,Weight\n')  # 写入表头
        node_cnt = 0

        for name, cnt in item_list:
            if cnt >= name_cnt_limit:
                node_file.write(f"{name},{cnt}\n")
                node_cnt += 1

    print(f'人物数量：{node_cnt}')
    print(f'已写入文件：{node_file_name}')
except Exception as e:
    print(f"写入节点文件时发生错误：{e}")

## 导出连接文件
try:
    with open(link_file_name, 'w', encoding='utf-8') as link_file:
        link_file.write('Source,Target,Weight\n')  # 写入表头
        link_cnt = 0

        for name1, link_dict in relation_dict.items():
            for name2, link in link_dict.items():
                if link >= link_cnt_limit:
                    link_file.write(f"{name1},{name2},{link}\n")
                    link_cnt += 1

    print(f'连接数量：{link_cnt}')
    print(f'已写入文件：{link_file_name}')
except Exception as e:
    print(f"写入连接文件时发生错误：{e}")