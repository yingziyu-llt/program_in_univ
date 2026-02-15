suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(ggplot2)
  library(ggrepel)
})

# 输出目录
out_dir <- "analysis/results_DE"
dir.create(out_dir, recursive = TRUE, showWarnings = FALSE)

# ==============================================================================
# 1. 加载数据并准备
# ==============================================================================
# 使用 Tumor 数据
seu_tumor <- readRDS("analysis/18_tumor_final_merged.rds")

# 提取 CD8 T 细胞
# 这里的 idents 对应你最终确认的 CD8 亚群名称
# 如果你用了 merge_map 后的名字，请相应调整
# 这里我们假设用 cell_type_fine 里的 CD8 相关亚群
target_clusters <- c("CD8_Tex", "CD8_Tem", "CD8_Temra", "CD8_Trm", "CD8_Naive", "CD8_Tem_IL7R", "CD8_Tem_GZMK")
# 确保只选存在的亚群
target_clusters <- target_clusters[target_clusters %in% unique(seu_tumor$cell_type_fine)]

message("Subsetting CD8 T cells...")
seu_cd8 <- subset(seu_tumor, subset = cell_type_fine %in% target_clusters)

# 过滤掉无效的 Response
seu_cd8 <- subset(seu_cd8, subset = Response %in% c("Responder", "NonResponder"))

# ==============================================================================
# 2. 执行差异分析 (Responder vs NonResponder)
# ==============================================================================
message("Running Differential Expression (Responder vs NonResponder)...")

# 设置对比组
Idents(seu_cd8) <- "Response"
de_markers <- FindMarkers(seu_cd8, 
                          ident.1 = "Responder", 
                          ident.2 = "NonResponder",
                          logfc.threshold = 0.01, # 稍微放宽一点以便画图
                          min.pct = 0.01,
                          test.use = "wilcox")

# 添加基因名列
de_markers$gene <- rownames(de_markers)

# 保存表格
write.csv(de_markers, file.path(out_dir, "DE_CD8_Responder_vs_NonResponder.csv"))

# ==============================================================================
# 3. 绘制火山图 (Volcano Plot) - 仿 Fig 2f
# ==============================================================================
plot_volcano <- function(markers, title) {
  # 定义显著性分类
  markers <- markers %>%
    mutate(
      status = case_when(
        p_val_adj < 0.05 & avg_log2FC > 0.25 ~ "Up (Responder)",
        p_val_adj < 0.05 & avg_log2FC < -0.25 ~ "Down (NonResponder)",
        TRUE ~ "Not Sig"
      ),
      # 用于画图的 logP
      logP = -log10(p_val_adj + 1e-300) # 防止 Inf
    )
  
  # 挑选重点基因进行标记 (参考论文 Fig 2f)
  # R侧: GZMB, GNLY, PRF1, GZMH, FGFBP2, CX3CR1, NKG7
  # NR侧: GZMK, FOS, JUN, DUSP1, IL7R
  highlight_genes <- c("GZMB", "GNLY", "PRF1", "GZMH", "FGFBP2", "CX3CR1", "NKG7", "FCGR3A",
                       "GZMK", "FOS", "JUN", "IL7R", "LTB", "CCR7", "DUSP1")
  
  markers$label <- ifelse(markers$gene %in% highlight_genes & markers$status != "Not Sig", markers$gene, NA)
  
  # 绘图
  p <- ggplot(markers, aes(x = avg_log2FC, y = logP, color = status)) +
    geom_point(alpha = 0.6, size = 1.5) +
    scale_color_manual(values = c("Up (Responder)" = "#E41A1C", 
                                  "Down (NonResponder)" = "#377EB8", 
                                  "Not Sig" = "grey80")) +
    geom_vline(xintercept = c(-0.25, 0.25), linetype = "dashed", color = "grey50") +
    geom_hline(yintercept = -log10(0.05), linetype = "dashed", color = "grey50") +
    geom_text_repel(aes(label = label), max.overlaps = 20, box.padding = 0.5) +
    theme_classic() +
    labs(title = title, 
         x = "log2 Fold Change (Resp / NonResp)", 
         y = "-log10 Adjusted P-value") +
    theme(legend.position = "top")
  
  return(p)
}

p_vol <- plot_volcano(de_markers, "Intra-tumoural CD8 T cells: Resp vs NonResp")
ggsave(file.path(out_dir, "Volcano_CD8_Global.pdf"), p_vol, width = 8, height = 7)

message("DE Analysis done. Check 'analysis/results_DE'.")