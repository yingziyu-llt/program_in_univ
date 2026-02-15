suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(ggplot2)
  library(ggrepel)
  library(ggpubr)
  library(patchwork)
})

# ==============================================================================
# 1. 加载数据与预处理 (Data Loading & Filtering)
# ==============================================================================
target_file <- "analysis/08_myeloid_final_clean_reintegrated.rds"
if(!file.exists(target_file)) stop("文件未找到: ", target_file)

seu_obj <- readRDS(target_file)
Idents(seu_obj) <- seu_obj$cell_type_fine
message(">>> 数据加载成功。原始细胞数: ", ncol(seu_obj))

# --- [Filter 1] 治疗组: 只保留 Atezo+Bev ---
if("Treatment" %in% colnames(seu_obj@meta.data)) {
  tx_groups <- unique(seu_obj$Treatment)
  target_tx <- grep("atezo.*bev", tx_groups, value = TRUE, ignore.case = TRUE)
  if(length(target_tx) > 0) {
    seu_obj <- subset(seu_obj, subset = Treatment %in% target_tx)
    message(">>> 已保留 'atezo+bev' 组。剩余细胞数: ", ncol(seu_obj))
  } else {
    warning("未找到 'atezo+bev' 组，跳过过滤。")
  }
}

# --- [Filter 2] Response: 只保留 R/NR ---
valid_resp <- c("Responder", "NonResponder")
if("Response" %in% colnames(seu_obj@meta.data)) {
  seu_obj <- subset(seu_obj, subset = Response %in% valid_resp)
  seu_obj$Response <- factor(seu_obj$Response, levels = valid_resp)
  message(">>> 已保留 Responder/NonResponder。剩余细胞数: ", ncol(seu_obj))
}

# --- [Filter 3] 定义 Macrophage 群体 (排除 Monocytes) ---
all_clusters <- levels(Idents(seu_obj))
# 排除掉 Monocytes, Doublets
non_mac_patterns <- c("Mono", "Hepatocyte", "Plasma")
mac_clusters <- grep(paste(non_mac_patterns, collapse="|"), all_clusters, value = TRUE, invert = TRUE)

message(">>> 定义的 Macrophage 亚群 (用于 5b/5c):")
print(mac_clusters)

# 提取 Macrophage 总集
seu_mac_total <- subset(seu_obj, idents = mac_clusters)

# ==============================================================================
# 2. 生成 Figure 5b: Volcano Plot (Responder vs Non-Responder)
# ==============================================================================
message("\n>>> 正在生成 Fig 5b (Volcano)...")

# 设置对比
Idents(seu_mac_total) <- "Response"

# 差异分析 (Cell-level Wilcoxon)
deg_res <- FindMarkers(seu_mac_total, ident.1 = "Responder", ident.2 = "NonResponder", 
                       logfc.threshold = 0.1, min.pct = 0.1)

deg_res$gene <- rownames(deg_res)
deg_res$category <- "NS"
deg_res$category[deg_res$avg_log2FC > 0.25 & deg_res$p_val_adj < 0.05] <- "Responder High"
deg_res$category[deg_res$avg_log2FC < -0.25 & deg_res$p_val_adj < 0.05] <- "NonResponder High"

# 标记论文中的关键基因 
highlight_genes <- c("CXCL9", "CXCL10", "GBP1", "STAT1", "WARS", "CD274", # Pro-inflammatory
                     "TREM2", "GPNMB", "CCL18", "FOLR2", "PPARG", "CD163") # Suppressive

deg_res$label <- ifelse(deg_res$gene %in% highlight_genes, deg_res$gene, NA)

# 绘图
p_volcano <- ggplot(deg_res, aes(x = avg_log2FC, y = -log10(p_val))) +
  geom_point(aes(color = category), size = 1.5, alpha = 0.8) +
  scale_color_manual(values = c("NonResponder High" = "#1f77b4", 
                                "Responder High" = "#d62728", 
                                "NS" = "grey90")) +
  geom_text_repel(aes(label = label), max.overlaps = 50, box.padding = 0.5, 
                  fontface = "bold", size = 3) +
  geom_vline(xintercept = c(-0.25, 0.25), linetype = "dashed", color="grey30") +
  theme_classic() +
  labs(title = "Fig 5b: Macrophage DEGs (Responder vs Non-Responder)", 
       subtitle = paste0("n=", ncol(seu_mac_total), " cells (Atezo+Bev)"),
       x = "Log2 Fold Change", y = "-Log10 P-value") +
  theme(legend.position = "top", legend.title = element_blank(),
        plot.title = element_text(hjust = 0.5))

# ==============================================================================
# 3. 生成 Figure 5c: Boxplots (Pseudo-bulk PD-L1)
# ==============================================================================
message("\n>>> 正在生成 Fig 5c (Pseudo-bulk)...")

# --- 通用绘图函数 (稳健版) ---
plot_pseudobulk_robust <- function(object, target_gene, title_text) {
  
  # 1. 计算全基因组 Pseudo-bulk (防止单基因丢失)
  message(paste0("   正在计算: ", title_text))
  avg_list <- AverageExpression(object, group.by = "Patient_ID", assay = "RNA", features = NULL)
  
  if("RNA" %in% names(avg_list)) mat <- avg_list$RNA else mat <- avg_list
  
  # 2. 提取目标基因
  if(!target_gene %in% rownames(mat)) {
    warning(paste(target_gene, "不在矩阵中，跳过。"))
    return(NULL)
  }
  
  df <- data.frame(
    Patient_ID = colnames(mat),
    Expression = as.numeric(mat[target_gene, ])
  )
  
  # 3. 合并临床信息
  meta_uniq <- unique(object@meta.data[, c("Patient_ID", "Response")])
  plot_data <- merge(df, meta_uniq, by = "Patient_ID")
  plot_data <- plot_data %>% filter(Response %in% valid_resp)
  
  if(nrow(plot_data) < 3) return(NULL)
  
  # 4. 绘图
  ggplot(plot_data, aes(x = Response, y = Expression, fill = Response)) +
    geom_boxplot(outlier.shape = NA, alpha = 0.8, width = 0.6) +
    geom_jitter(width = 0.1, size = 2.5, alpha = 0.7, shape = 21, color = "black") +
    stat_compare_means(method = "wilcox.test", label = "p.format", 
                       comparisons = list(valid_resp)) +
    scale_fill_manual(values = c("Responder" = "#d62728", "NonResponder" = "#1f77b4")) +
    theme_classic(base_size = 12) +
    labs(title = title_text, 
         y = paste("Avg", target_gene), x = "") +
    theme(legend.position = "none", plot.title = element_text(hjust = 0.5))
}

# --- 生成上图 (Top): Total Macrophages ---
# 使用 seu_mac_total (已排除 Monocytes)
p_box_top <- plot_pseudobulk_robust(seu_mac_total, "CD274", "Fig 5c Top: Total Macrophages")

# --- 生成下图 (Bottom): Macro CXCL10 Only ---
if("Macro CXCL10" %in% levels(Idents(seu_obj))) {
  seu_cxcl10 <- subset(seu_obj, idents = "Macro CXCL10")
  p_box_bottom <- plot_pseudobulk_robust(seu_cxcl10, "CD274", "Fig 5c Bottom: Macro CXCL10")
} else {
  p_box_bottom <- NULL
  warning("未找到 Macro CXCL10 群")
}

# ==============================================================================
# 4. 整合保存 (Saving)
# ==============================================================================
out_dir <- "figures"
if(!dir.exists(out_dir)) dir.create(out_dir)

# 保存 Fig 5b
ggsave(file.path(out_dir, "Fig5b_Volcano_Final.pdf"), p_volcano, width = 6, height = 6)

# 保存 Fig 5c (拼图)
if(!is.null(p_box_top) && !is.null(p_box_bottom)) {
  p_combined <- p_box_top / p_box_bottom
  ggsave(file.path(out_dir, "Fig5c_Boxplot_Final.pdf"), p_combined, width = 4, height = 8)
  message(">>> 成功！Fig 5b 和 5c 已保存至 analysis/figures 文件夹。")
} else {
  message(">>> 警告：Fig 5c 拼图失败，尝试单独保存。")
  if(!is.null(p_box_top)) ggsave(file.path(out_dir, "Fig5c_Top.pdf"), p_box_top)
  if(!is.null(p_box_bottom)) ggsave(file.path(out_dir, "Fig5c_Bottom.pdf"), p_box_bottom)
}