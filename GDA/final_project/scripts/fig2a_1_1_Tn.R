suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(harmony)
  library(ggplot2)
  library(patchwork)
})

# 1. 加载当前的 CD8 对象
# 假设你使用的是之前细分好但还没 Merge Map 的对象 (06_cd8_fine_clustering.rds)
# 或者如果是已经 Merge 过的，请加载那个并调整 idents
seu_cd8 <- readRDS("analysis/06_cd8_fine_clustering.rds")

# 检查 Cluster 1 (或者是你看到的那个混杂群的 ID)
target_cluster_id <- "1" # 请根据实际情况修改这个ID
message(sprintf("Subsetting Cluster %s for re-analysis...", target_cluster_id))

seu_sub <- subset(seu_cd8, idents = target_cluster_id)

# ==============================================================================
# 2. 子聚类流程 (Standard Re-processing)
# ==============================================================================
# 关键：必须重新寻找高变基因，这样才能捕捉到 Tn 和 Tem 的细微差别
seu_sub <- NormalizeData(seu_sub)
seu_sub <- FindVariableFeatures(seu_sub, nfeatures = 1000)

# 剔除 TCR/CellCycle (沿用之前的逻辑)
tcr_genes <- grep("^TR[ABGD][VDJC]", rownames(seu_sub), value = TRUE)
cc_genes <- c(Seurat::cc.genes$s.genes, Seurat::cc.genes$g2m.genes)
VariableFeatures(seu_sub) <- setdiff(VariableFeatures(seu_sub), c(tcr_genes, cc_genes))

seu_sub <- ScaleData(seu_sub) # 这里不需要 regress cell cycle 了，通常影响不大
seu_sub <- RunPCA(seu_sub, verbose = FALSE)

# 检查是否需要 Harmony (如果这些细胞来自多个病人)
if (length(unique(seu_sub$Patient_ID)) > 1) {
  seu_sub <- RunHarmony(seu_sub, group.by.vars = "Patient_ID", verbose = FALSE)
  reduction_use <- "harmony"
} else {
  reduction_use <- "pca"
}

seu_sub <- RunUMAP(seu_sub, reduction = reduction_use, dims = 1:20)
seu_sub <- FindNeighbors(seu_sub, reduction = reduction_use, dims = 1:20)

# 尝试较高的分辨率，强行把它们拆开
seu_sub <- FindClusters(seu_sub, resolution = 0.8) 

# ==============================================================================
# 3. 诊断绘图：是不是分开了？
# ==============================================================================
# 重点检查 Naive Marker vs Effector Marker
# Naive: CCR7, TCF7, LEF1, SELL
# Tem: GZMK, GZMB, CCL5
markers_check <- c("CCR7", "TCF7", "LEF1", "SELL", "GZMK", "CCL5", "GZMB")

p1 <- DimPlot(seu_sub, label = TRUE) + ggtitle("Sub-clustering Result")
p2 <- DotPlot(seu_sub, features = markers_check) + RotatedAxis()

p_combine <- p1 | p2
ggsave("analysis/Debug_Naive_Search.pdf", p_combine, width = 12, height = 6)

message("Check 'analysis/Debug_Naive_Search.pdf'. Do you see a cluster with high CCR7/LEF1 and low GZMK?")

# ==============================================================================
# 4. 如果找到了，如何回填？(手动执行部分)
# ==============================================================================
# 假设你在 PDF 里看到 新的 Cluster 2 是 Naive (高 CCR7, 低 GZMK)
# 下面代码演示如何把它合回去

# 定义哪些是 Naive (请根据 PDF 结果修改这个 list)
naive_sub_clusters <- c("4") 

if (exists("naive_sub_clusters")) {
  # 1. 获取 Naive 细胞的 Barcode
  naive_cells <- colnames(seu_sub)[Idents(seu_sub) %in% naive_sub_clusters]
  
  # 2. 在原对象中标记这些细胞
  # 先把原对象转成 character 方便修改
  new_idents <- as.character(Idents(seu_cd8))
  names(new_idents) <- colnames(seu_cd8)
  
  # 将找到的细胞改名为 "CD8_Naive"
  new_idents[naive_cells] <- "CD8_Naive"
  
  # 更新原对象
  Idents(seu_cd8) <- factor(new_idents)
  
  # 保存更新后的对象
  saveRDS(seu_cd8, "analysis/06_cd8_fine_clustering_with_naive.rds")
  message("Updated object saved with CD8_Naive annotated.")
}