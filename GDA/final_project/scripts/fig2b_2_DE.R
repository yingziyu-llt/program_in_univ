suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(ggplot2)
  library(pheatmap)
  library(future) 
})

# 设置并行计算
plan("multisession", workers = 4)
options(future.globals.maxSize = 3000 * 1024^2) # 稍微调大一点内存限制

# ==============================================================================
# [cite_start]1. 定义论文重点关注的 Marker 列表 [cite: 60, 63, 187, 417, 602]
# ==============================================================================
markers_dict <- list(
  # CD8: 区分 Naive, Tem (GZMK), Tex (PD1/CXCL13), Temra (FGFBP2/CX3CR1)
  CD8 = c(
    "CD8A", "CD8B", 
    "TCF7", "LEF1", "CCR7", "SELL",  # Naive / Central Memory
    "GZMK", "LTB",                   # Tem (Effector Memory)
    "PDCD1", "HAVCR2", "LAG3", "TOX", "CXCL13", "CTLA4", # Tex (Exhausted)
    "FGFBP2", "CX3CR1", "GNLY", "GZMB", "PRF1", "NKG7", "ZEB2", # Temra / Cytotoxic
    "MKI67" 
  ),
  
  # CD4: 区分 Naive, Treg, Th1/Exhausted
  CD4 = c(
    "CD4", 
    "TCF7", "CCR7", "SELL",          # Naive / Central Memory
    "FOXP3", "IL2RA", "IKZF2",       # Treg
    "CXCL13", "IFNG", "PDCD1",       # Th1 / Helper / Exhausted-like
    "IL7R", "S100A4",                # Memory
    "MKI67"
  ),
  
  # NK: 区分 Cytotoxic vs Resting
  NK = c(
    "GNLY", "NKG7", "PRF1", "GZMB",  # Cytotoxic
    "KLRC1", "XCL1", "XCL2",         # Resting / Adaptive
    "FCGR3A", "NCAM1",               # CD16, CD56
    "CX3CR1", "FGFBP2",              # Mature NK markers
    "MKI67"
  ),

  # Proliferating: 细胞周期相关
  Prolif = c(
    "MKI67", "TOP2A", "PCNA", "CDK1", "MCM6", 
    "CD8A", "CD4", "GNLY" # 查看是哪种细胞在增殖
  )
)

# ==============================================================================
# 2. 通用分析函数：绘图 + DE
# ==============================================================================
run_diagnostics <- function(file_path, label, specific_markers, output_subdir) {
  
  message(sprintf("\n\n>>> Loading %s data from %s...", label, file_path))
  if(!file.exists(file_path)) {
    warning(sprintf("File %s not found. Skipping.", file_path))
    return(NULL)
  }
  
  seu_obj <- readRDS(file_path)
  
  # 打印当前 Cluster 情况
  message(sprintf("    Current clusters: %s", paste(levels(Idents(seu_obj)), collapse = ", ")))
  
  # --- A. Visualization Plots ---
  message("    Generating Diagnostic Plots...")
  
  # 1. UMAP
  p1 <- DimPlot(seu_obj, label = TRUE, label.size = 5, raster = FALSE) + 
    ggtitle(paste0(label, " (PBMC) - Clusters"))
  
  # 2. DotPlot (Paper Markers)
  valid_markers <- specific_markers[specific_markers %in% rownames(seu_obj)]
  p2 <- DotPlot(seu_obj, features = valid_markers, cols = c("lightgrey", "red"), dot.scale = 8) + 
    RotatedAxis() + 
    ggtitle(paste0(label, " - Canonical Markers"))
  
  # 3. QC Violin Plot
  # 检查是否有 Cluster 质量显著偏低 (可能是 Doublet 或死细胞)
  # 确保 meta.data 里有这些列，没有则尝试计算
  if(!"nFeature_RNA" %in% colnames(seu_obj@meta.data)) seu_obj$nFeature_RNA <- seu_obj$nFeature_RNA
  
  p3 <- VlnPlot(seu_obj, features = c("nFeature_RNA", "nCount_RNA", "percent.mt"), 
                pt.size = 0, ncol = 3, log = TRUE)
  
  # --- B. Differential Expression (DE) ---
  message("    Running FindAllMarkers (this may take a moment)...")
  
  # 寻找每个 Cluster 的特征基因
  markers_df <- FindAllMarkers(seu_obj, 
                               only.pos = TRUE, 
                               min.pct = 0.25, 
                               logfc.threshold = 0.25,
                               test.use = "wilcox")
  
  # 整理 Top 10 Markers
  top10 <- markers_df %>% 
    group_by(cluster) %>% 
    top_n(n = 10, wt = avg_log2FC)
  
  # 4. Heatmap
  # 如果细胞太多，Heatmap 可能会画得很慢或报错，这里做个简单的下采样用于绘图
  seu_subset <- seu_obj
  if(ncol(seu_obj) > 2000) {
    seu_subset <- subset(seu_obj, downsample = 200)
  }
  
  p4 <- DoHeatmap(seu_subset, features = top10$gene) + NoLegend() + 
    ggtitle(paste0(label, " - Top 10 DE Genes"))
  
  # --- C. Saving Outputs ---
  # 这里的 output_subdir 允许我们将 PBMC 结果和 Tumor 结果分开放
  dir.create(output_subdir, showWarnings = FALSE, recursive = TRUE)
  
  # 保存 PDF
  pdf_name <- file.path(output_subdir, paste0("Plot_PBMC_", label, "_Diagnostics.pdf"))
  pdf(pdf_name, width = 12, height = 8)
  print(p1)
  print(p2)
  print(p3)
  print(p4)
  dev.off()
  
  # 保存 CSV
  csv_name <- file.path(output_subdir, paste0("Table_PBMC_", label, "_AllMarkers.csv"))
  write.csv(markers_df, csv_name, row.names = FALSE)
  
  message(sprintf("    [Done] Results saved to %s", output_subdir))
}

# ==============================================================================
# 3. 执行分析
# ==============================================================================

# 定义 PBMC 专用的输出目录
OUT_DIR <- "analysis/results_PBMC_annotation"

# 1. 分析 PBMC CD8
run_diagnostics("analysis/11_PBMC_cd8_fine_clustering.rds", "CD8", markers_dict$CD8, OUT_DIR)

# 2. 分析 PBMC CD4
run_diagnostics("analysis/11_PBMC_cd4_fine_clustering.rds", "CD4", markers_dict$CD4, OUT_DIR)

# 3. 分析 PBMC NK
run_diagnostics("analysis/11_PBMC_nk_fine_clustering.rds",  "NK",  markers_dict$NK, OUT_DIR)

# 4. 分析 PBMC Proliferating (如果有生成的话)
run_diagnostics("analysis/11_PBMC_proliferating_fine_clustering.rds", "Proliferating", markers_dict$Prolif, OUT_DIR)

message("\nAll PBMC DE analysis finished! Check the 'analysis/results_PBMC_annotation' folder.")