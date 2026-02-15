suppressPackageStartupMessages({
  library(Seurat)
  library(CellChat)
  library(tidyverse)
  library(patchwork)
  library(ComplexHeatmap)
})

# ==============================================================================
# 1. 加载数据
# ==============================================================================
input_file <- "analysis/09_cellchat_input_objects.rds"
if(!file.exists(input_file)) stop("输入文件不存在")
inputs <- readRDS(input_file)
seu_r <- inputs$R
seu_nr <- inputs$NR

message("数据加载完毕。")

# ==============================================================================
# 2. 定义【绝对鲁棒】的计算函数
# ==============================================================================
run_cellchat_robust <- function(seu_obj, label) {
  message(paste0("\n>>> [Robust Calc] 处理: ", label))
  
  # 1. 创建对象
  cc <- createCellChat(object = seu_obj, group.by = "cell_type_interaction")
  cc@DB <- CellChatDB.human # 使用全库
  
  # 2. 预处理
  cc <- subsetData(cc) 
  cc <- identifyOverExpressedGenes(cc)
  cc <- identifyOverExpressedInteractions(cc)
  
  # 3. 推断通讯 (关键修改)
  # type="truncatedMean": 更稳健的方法 (针对低表达基因)
  # trim=0.05: 排除极少数离群值
  # population.size=TRUE: 考虑细胞比例 (免疫互作通常需要)
  cc <- computeCommunProb(cc, type = "truncatedMean", trim = 0.05, 
                          population.size = TRUE, nboot = 20) 
  
  # 4. 过滤 (!!! 核心修改 !!!)
  # min.cells = 0: 强制保留所有群，哪怕只有几个细胞。
  # 这样可以避免 liftCellChat 带来的 bug。
  cc <- filterCommunication(cc, min.cells = 0)
  
  # 5. Pathway level
  cc <- computeCommunProbPathway(cc)
  cc <- aggregateNet(cc)
  
  return(cc)
}

# ==============================================================================
# 3. 执行计算 (确保两组细胞类型完全一致)
# ==============================================================================
# 在计算前，为了保险，我们先统一一下 Seurat 对象的 levels
# 这样 CellChat 创建时就会有相同的 groups
all_levels <- union(levels(Idents(seu_r)), levels(Idents(seu_nr)))

# 强制给 Seurat 对象补齐 levels (即使该组没这个细胞，也会在 table 里显示为 0)
# 注意：Seurat 会自动丢弃 0 细胞的 level，但 createCellChat 有时会读取 meta.data 的 factor levels
seu_r$cell_type_interaction <- factor(seu_r$cell_type_interaction, levels = all_levels)
seu_nr$cell_type_interaction <- factor(seu_nr$cell_type_interaction, levels = all_levels)
Idents(seu_r) <- seu_r$cell_type_interaction
Idents(seu_nr) <- seu_nr$cell_type_interaction

# 运行计算
cc_r <- run_cellchat_robust(seu_r, "Responder")
cc_nr <- run_cellchat_robust(seu_nr, "NonResponder")

# 检查是否还需要 lift (理论上 min.cells=0 后不需要，但以防万一)
group.new <- union(levels(cc_r@idents), levels(cc_nr@idents))

if(!identical(levels(cc_r@idents), levels(cc_nr@idents))) {
  message(">>> 警告：Groups 仍不一致，尝试 lift (通常 min.cells=0 后不会发生)...")
  cc_r <- liftCellChat(cc_r, group.new)
  cc_nr <- liftCellChat(cc_nr, group.new)
} else {
  message(">>> 检测通过：两组细胞类型完全一致，无需 lift。")
}

# 合并
cc_list <- list(Responder = cc_r, NonResponder = cc_nr)
cc_merged <- mergeCellChat(cc_list, add.names = names(cc_list))

# ==============================================================================
# 4. 绘图 (带诊断)
# ==============================================================================
out_dir <- "analysis/figures"
if(!dir.exists(out_dir)) dir.create(out_dir)

pathway_target <- "CXCL"

# --- 诊断：CXCL 是否存在？ ---
check_pathway <- function(cc, label) {
  pws <- cc@netP$pathways
  if(pathway_target %in% pws) {
    message(paste0("[OK] ", label, " 中包含 CXCL 通路。"))
    return(TRUE)
  } else {
    message(paste0("[FAIL] ", label, " 中未检测到 CXCL 通路！(可能表达太低)"))
    # 打印前5个存在的通路看看
    message("   可用的通路样例: ", paste(head(pws, 5), collapse=", "))
    return(FALSE)
  }
}

r_has_cxcl <- check_pathway(cc_r, "Responder")
nr_has_cxcl <- check_pathway(cc_nr, "NonResponder")

if(!r_has_cxcl && !nr_has_cxcl) {
  stop("两组都没有检测到 CXCL 信号！可能是数据中相关基因 (CXCL9, CXCL10, CXCR3) 表达量几乎为0。")
}

# --- Fig 5e: 手动矩阵绘图 (绕过 igraph 报错) ---
message("正在绘制 Fig 5e...")

pdf(file.path(out_dir, "Fig5e_CXCL_Network_Robust.pdf"), width = 10, height = 5)
par(mfrow = c(1, 2), xpd = TRUE)

# 计算最大权重用于统一标尺
# 手动提取 netP 中的 CXCL 矩阵
get_net_matrix <- function(cc, pathway) {
  if(pathway %in% cc@netP$pathways) {
    return(cc@netP$prob[,,pathway])
  } else {
    # 如果该组没有 CXCL，返回全 0 矩阵
    mat <- matrix(0, nrow=length(group.new), ncol=length(group.new))
    rownames(mat) <- colnames(mat) <- group.new
    return(mat)
  }
}

mat_r <- get_net_matrix(cc_r, pathway_target)
mat_nr <- get_net_matrix(cc_nr, pathway_target)
weight.max <- max(max(mat_r), max(mat_nr))

# 绘图函数
plot_circle_manual <- function(mat, title, max_w) {
  if(sum(mat) == 0) {
    plot(1, type="n", axes=F, xlab="", ylab="")
    text(1, 1, paste(title, "\n(No Signal)"))
  } else {
    netVisual_circle(mat, vertex.weight = rowSums(mat), weight.scale = T, 
                     edge.weight.max = max_w, title.name = title)
  }
}

plot_circle_manual(mat_r, "Responder - CXCL", weight.max)
plot_circle_manual(mat_nr, "NonResponder - CXCL", weight.max)

dev.off()

# --- Fig 5f ---
message("正在绘制 Fig 5f...")

# Top: Contribution
pdf(file.path(out_dir, "Fig5f_Top_CXCL_Contribution.pdf"), width = 6, height = 4)
# 只要有一组有信号，就可以画
if(r_has_cxcl || nr_has_cxcl) {
  try(netAnalysis_contribution(cc_merged, signaling = pathway_target))
}
dev.off()

# Bottom: Info Flow
pdf(file.path(out_dir, "Fig5f_Bottom_CXCL_InfoFlow.pdf"), width = 5, height = 4)
if(r_has_cxcl || nr_has_cxcl) {
  # 注意：如果某一组全空，rankNet 可能会报 warning，但不应报错停止
  try(rankNet(cc_merged, mode = "comparison", stacked = TRUE, do.stat = TRUE, signaling = pathway_target))
}
dev.off()

saveRDS(cc_merged, "analysis/09_cellchat_final_merged_robust.rds")
message("完成！请检查 analysis/figures/Fig5e_CXCL_Network_Robust.pdf")