"""
Date: 2023.03.12
Author: Justin

要点说明：
用第三方库pyecharts绘制关系图（Graph）
使用前面程序生成的《红楼梦》人物关系数据
"""

from pyecharts import options as opts
from pyecharts.charts import Graph

##--- 第0步：准备工作
# 输入文件
node_file_name = './output/红楼梦-人物节点.csv'
link_file_name = './output/红楼梦-人物连接.csv'
# 输出文件
out_file_name = './output/关系图-红楼人物.html'


##--- 第1步：从文件读入节点和连接信息
try:
    with open(node_file_name, 'r', encoding='utf-8') as node_file:
        node_line_list = node_file.readlines()
    with open(link_file_name, 'r', encoding='utf-8') as link_file:
        link_line_list = link_file.readlines()
except FileNotFoundError as e:
    print(f"文件未找到：{e}")
    exit(1)

# 删除标题行
del node_line_list[0]
del link_line_list[0]


##--- 第2步：解析读入的信息，存入列表
# 动态调整节点尺寸
max_weight = max(int(line.split(',')[1]) for line in node_line_list)  # 获取最大权重
min_node_size = 10  # 最小节点尺寸
max_node_size = 30  # 最大节点尺寸

node_in_graph = []
for one_line in node_line_list:
    one_line = one_line.strip('\n')
    name, weight = one_line.split(',')
    weight = int(weight)
    # 动态计算节点尺寸，避免过大或过小
    symbol_size = min_node_size + (weight / max_weight) * (max_node_size - min_node_size)
    node_in_graph.append(opts.GraphNode(
        name=name,
        value=weight,
        symbol_size=symbol_size
    ))

link_in_graph = []
for one_line in link_line_list:
    one_line = one_line.strip('\n')
    source, target, value = one_line.split(',')
    link_in_graph.append(opts.GraphLink(
        source=source,
        target=target,
        value=int(value)
    ))


##--- 第3步：画图
c = Graph(init_opts=opts.InitOpts(
))
c.add(
    "",
    node_in_graph,
    link_in_graph,
    edge_length=[20, 100],  # 调整边长范围
    repulsion=1000,  # 增大斥力，避免节点重叠
    layout="force",  # 使用力引导布局
    is_draggable=True,  # 允许拖拽节点
    label_opts=opts.LabelOpts(is_show=True, position="right")  # 显示标签
)
c.set_global_opts(
    title_opts=opts.TitleOpts(title="关系图-红楼人物"),
    legend_opts=opts.LegendOpts(is_show=False)  # 隐藏图例
)
c.render(out_file_name)

print(f"已生成文件：{out_file_name}")