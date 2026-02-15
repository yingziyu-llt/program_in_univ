# ==============================================================================
# File: 03_plot_fig4_analysis.R
# Purpose: Generate Fig 4 analysis with Paper-style visualizations
# ==============================================================================

suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(slingshot)
  library(SingleCellExperiment)
  library(tradeSeq)
  library(ggplot2)
  library(gridExtra)
  library(fgsea)
  library(msigdbr)
  library(RColorBrewer)
})

out_dir <- "analysis/results_trajectory"

# 1. Load Data
message(">>> [Step 3] Loading data...")
seu_cd8 <- readRDS(file.path(out_dir, "01_seu_cd8_clean.rds"))
sce <- readRDS(file.path(out_dir, "01_sce_slingshot_orig.rds"))
sce_proj <- readRDS(file.path(out_dir, "01_sce_curve_proj.rds"))
sce_gam <- readRDS(file.path(out_dir, "02_tradeseq_model.rds"))

# ==============================================================================
# Part A: Plot Basic Trajectory
# ==============================================================================
pdf(file.path(out_dir, "Fig4_A_Trajectory_UMAP.pdf"), width = 7, height = 6)
colors <- scales::hue_pal()(length(unique(Idents(seu_cd8))))
names(colors) <- levels(Idents(seu_cd8))
cell_colors <- colors[as.character(Idents(seu_cd8))]

plot(reducedDims(sce)$UMAP, 
     col = alpha(cell_colors, 0.6), 
     pch = 16, cex = 0.5, 
     main = "CD8 Differentiation Trajectories")
lines(SlingshotDataSet(sce_proj), lwd = 3, col = 'black')
legend("topright", legend = names(colors), fill = colors, bty = "n", cex = 0.8)
dev.off()

# ==============================================================================
# Part B: Identify Lineages
# ==============================================================================
sling_curves <- SlingshotDataSet(sce)
n_curves <- length(sling_curves@curves)
lineage_map <- list()

message(">>> Identifying Lineage Endpoints:")
for(i in 1:n_curves){
  pt <- slingPseudotime(sling_curves)[,i]
  top_cells <- names(sort(pt, decreasing = TRUE)[1:30])
  major_type <- names(sort(table(seu_cd8$cell_type_paper[top_cells]), decreasing=TRUE))[1]
  lineage_map[[i]] <- major_type
  message(sprintf("    Lineage %d ends at: %s", i, major_type))
}

# ==============================================================================
# Part C: Pseudotime Density (Fig 4c)
# ==============================================================================
# (略过重复部分，如果需要可以把之前的代码贴在这里，或者直接关注 Part D)

# ==============================================================================
# Part D: Fig 4e (GSEA Barplot - Paper Style)
# ==============================================================================

# [AUTO-DETECT] 自动匹配 Lineage 编号
idx_temra <- which(sapply(lineage_map, function(x) grepl("Temra", x, ignore.case = TRUE)))[1]
idx_tex <- which(sapply(lineage_map, function(x) grepl("Tex", x, ignore.case = TRUE)))[1]

# 兜底：如果自动匹配失败，请手动修改这里
if(is.na(idx_temra)) idx_temra <- 3
if(is.na(idx_tex)) idx_tex <- 1

message(sprintf(">>> Running Comparison: Temra (L%d) vs Tex (L%d)", idx_temra, idx_tex))

# 1. 运行 diffEndTest 获取统计量 (Wald Statistic)
#    Wald Stat 是一个很好的排序指标，比单纯的 logFC 更稳健
message("    Calculating Wald statistics (diffEndTest)...")
diff_res <- diffEndTest(sce_gam, pairwise = TRUE)

# 2. 确定比较方向的列名
# 我们需要找到 "WaldStat_XvsY" 以及 "LogFC_XvsY"
# 注意：tradeSeq 的 LogFC 列名通常是 "logFC{id1}_{id2}"
stat_col_pattern <- paste0("waldStat_", min(idx_temra, idx_tex), "vs", max(idx_temra, idx_tex))
logfc_col_pattern <- paste0("logFC", min(idx_temra, idx_tex), "_", max(idx_temra, idx_tex))

# 检查列名是否存在
avail_cols <- colnames(diff_res)
target_stat_col <- avail_cols[grepl(paste0("waldStat.*", idx_tex, ".*", idx_temra), avail_cols) | 
                              grepl(paste0("waldStat.*", idx_temra, ".*", idx_tex), avail_cols)][1]

if(is.na(target_stat_col)){
  stop("Could not find WaldStat column. Check `colnames(diff_res)` manually.")
}

message(sprintf("    Using Statistic Column: %s", target_stat_col))

# 3. 构建排序列表 (Ranked List)
# Ranking Metric = sign(LogFC) * WaldStat
# 这里的 LogFC 符号很重要。我们需要确认方向：Positive = Temra, Negative = Tex

# 为了确保方向正确，我们手动算一下预测值的 LogFC 符号
yhat <- predictCells(sce_gam, gene = rownames(diff_res))
w <- slingCurveWeights(sce)
cells_temra <- which(w[, idx_temra] > 0.7)
cells_tex <- which(w[, idx_tex] > 0.7)
mean_temra <- rowMeans(yhat[, cells_temra, drop=FALSE])
mean_tex <- rowMeans(yhat[, cells_tex, drop=FALSE])
manual_logfc <- log2(mean_temra + 1) - log2(mean_tex + 1)

# 构建最终 Metric
# 如果 manual_logfc > 0 (Temra高), metric > 0
# 如果 manual_logfc < 0 (Tex高), metric < 0
# 使用 diff_res 里的 waldStat (因为它代表显著性程度)
rank_metric <- sign(manual_logfc) * diff_res[[target_stat_col]]
names(rank_metric) <- rownames(diff_res)
rank_metric <- sort(rank_metric, decreasing = TRUE)

# 4. 运行 FGSEA
message("    Running FGSEA...")
m_df <- msigdbr(species = "Homo sapiens", category = "C5", subcategory = "GO:BP")
fgsea_sets <- split(x = m_df$gene_symbol, f = m_df$gs_name)

fgseaRes <- fgsea(pathways = fgsea_sets, stats = rank_metric, minSize = 15, maxSize = 500)

# 5. 筛选 Top Pathways (模仿论文)
# 选出 Temra (NES > 0) 的 Top 10 和 Tex (NES < 0) 的 Top 10
top_temra <- fgseaRes %>% 
  filter(NES > 0, pval < 0.05) %>% 
  arrange(desc(NES)) %>% 
  head(10) %>%
  mutate(Group = "Temra")

top_tex <- fgseaRes %>% 
  filter(NES < 0, pval < 0.05) %>% 
  arrange(NES) %>% 
  head(10) %>%
  mutate(Group = "Tex")

plot_data <- bind_rows(top_temra, top_tex)

# 6. 绘制条形图 (Paper Style)
if(nrow(plot_data) > 0) {
  message("    Generating Bar Plot...")
  
  # 清理 Pathway 名称 (去掉 GOBP_ 前缀，换成小写方便阅读)
  plot_data$pathway_clean <- gsub("GOBP_", "", plot_data$pathway)
  plot_data$pathway_clean <- gsub("_", " ", plot_data$pathway_clean)
  
  # 排序：让 Temra 在上，Tex 在下
  plot_data$pathway_clean <- factor(plot_data$pathway_clean, levels = unique(plot_data$pathway_clean[order(plot_data$NES)]))
  
  p <- ggplot(plot_data, aes(x = NES, y = pathway_clean, fill = Group)) +
    geom_col() +
    scale_fill_manual(values = c("Temra" = "#E7B800", "Tex" = "#2E9FDF")) + # 论文常用配色
    labs(x = "Normalized Enrichment Score (NES)", 
         y = "GO Biological Process",
         title = "Pathways driving trajectory divergence (Temra vs Tex)") +
    theme_classic() +
    theme(axis.text.y = element_text(size = 8),
          legend.position = "right") +
    geom_vline(xintercept = 0, linetype = "dashed", color = "gray")
  
  ggsave(file.path(out_dir, "Fig4_E_Barplot_PaperStyle.pdf"), plot = p, width = 10, height = 8)
  message(">>> Success! Check 'Fig4_E_Barplot_PaperStyle.pdf'.")
  
} else {
  message(">>> Warning: No significant pathways found (pval < 0.05). Check if model fits correctly.")
}