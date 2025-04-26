"""
Date: 2023.03.12
Author: Justin

要点说明：
用第三方库pyecharts绘制关系图（Graph），带分类节点
使用前面程序生成的《红楼梦》人物关系数据
数据有手工调整，增加了分类信息
"""

from pyecharts import options as opts
from pyecharts.charts import Graph

##--- 第0步：准备工作
# 输入文件
node_file_name = './output/红楼梦-人物节点-分类.csv'  # 包含分类信息的节点文件
link_file_name = './output/红楼梦-人物连接.csv'       # 连接文件
# 输出文件
out_file_name = './output/关系图-分类-红楼人物.html'

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
# 类别列表，用于给节点分成不同系列，会自动用不同颜色表示
categories = [
    {},  # 默认类别
    {'name': '贾府'},  # 贾府成员
    {'name': '王家'},  # 王家成员
    {'name': '史家'},
    {'name': '薛家'},  # 薛家成员
    {'name': '仆人'},  # 仆人
    {'name': '其他'},   # 其他角色
]

node_in_graph = []
for one_line in node_line_list:
    one_line = one_line.strip('\n')
    one_line_list = one_line.split(',')
    name, weight, category = one_line_list[0], int(one_line_list[1]), int(one_line_list[2])
    symbol_size = max(10, min(30, int(weight) / 20))  # 动态调整节点尺寸
    node_in_graph.append(opts.GraphNode(
        name=name,
        value=weight,
        symbol_size=symbol_size,
        category=category
    ))

link_in_graph = []
for one_line in link_line_list:
    one_line = one_line.strip('\n')
    one_line_list = one_line.split(',')
    source, target, value = one_line_list[0], one_line_list[1], int(one_line_list[2])
    link_in_graph.append(opts.GraphLink(
        source=source,
        target=target,
        value=value
    ))

##--- 第3步：画图
c = Graph(init_opts=opts.InitOpts(
    width="100%",  # 设置画布宽度为100%
    height="800px",  # 设置画布高度为800px
    bg_color="#f5f5f5"  # 背景颜色
))
c.add(
    "",
    node_in_graph,
    link_in_graph,
    edge_length=[20, 100],  # 调整边长范围
    repulsion=3000,  # 增大斥力，避免节点重叠
    categories=categories,  # 使用分类
    layout="force",  # 使用力引导布局
    is_draggable=True,  # 允许拖拽节点
    label_opts=opts.LabelOpts(is_show=True, position="right")  # 显示标签
)
c.set_global_opts(
    title_opts=opts.TitleOpts(title="关系图-红楼人物-分类"),
    legend_opts=opts.LegendOpts(is_show=True)  # 显示图例
)
c.render(out_file_name)

print(f"已生成文件：{out_file_name}")