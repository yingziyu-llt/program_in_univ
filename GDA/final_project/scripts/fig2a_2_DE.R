suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(ggplot2)
  library(pheatmap)
  library(future) # 加速 DE 计算
})

# 设置并行计算 (可选，根据你电脑核心数调整)
plan("multisession", workers = 4)
options(future.globals.maxSize = 2000 * 1024^2) # 2GB 内存限制防止溢出

# ==============================================================================
# 1. 定义论文重点关注的 Marker 列表 [cite: 60, 63, 187, 417, 602]
# ==============================================================================
# 这些基因来自 Nature Comms 文章，用于区分 Temra, Tex, Tem, Naive, Treg 等

markers_dict <- list(
  # CD8: 区分 Naive, Tem (GZMK), Tex (PD1/CXCL13), Temra (FGFBP2/CX3CR1)
  CD8 = c(
    "CD8A", "CD8B", 
    "TCF7", "LEF1", "CCR7",          # Naive / Central Memory
    "GZMK", "LTB",                   # Tem (Effector Memory)
    "PDCD1", "HAVCR2", "LAG3", "TOX", "CXCL13", "CTLA4", # Tex (Exhausted)
    "FGFBP2", "CX3CR1", "GNLY", "GZMB", "PRF1", "NKG7", "ZEB2", # Temra / Cytotoxic
    "MKI67" # Proliferation
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
    "KLRC1", "XCL1", "XCL2",         # Resting / Adaptive / Tissue Resident
    "FCGR3A", "NCAM1",               # CD16, CD56
    "MKI67"
  )
)

# ==============================================================================
# 2. 通用分析函数：绘图 + DE
# ==============================================================================
run_diagnostics <- function(file_path, label, specific_markers) {
  
  message(sprintf("\n\n>>> Loading %s data from %s...", label, file_path))
  if(!file.exists(file_path)) {
    warning(sprintf("File %s not found. Skipping.", file_path))
    return(NULL)
  }
  
  seu_obj <- readRDS(file_path)
  
  # 确保 Idents 是最新的聚类结果
  # 如果之前保存时没有设置，默认取最后一列 active.ident
  message(sprintf("    Current clusters: %s", paste(levels(Idents(seu_obj)), collapse = ", ")))
  
  # --- A. Visualization Plots ---
  message("    Generating Diagnostic Plots...")
  
  # 1. UMAP
  p1 <- DimPlot(seu_obj, label = TRUE, label.size = 5, raster = FALSE) + 
    ggtitle(paste0(label, " - Clusters"))
  
  # 2. DotPlot (Paper Markers)
  # 检查基因是否存在于数据中
  valid_markers <- specific_markers[specific_markers %in% rownames(seu_obj)]
  p2 <- DotPlot(seu_obj, features = valid_markers, cols = c("lightgrey", "red"), dot.scale = 8) + 
    RotatedAxis() + 
    ggtitle(paste0(label, " - Canonical Markers"))
  
  # 3. QC Violin Plot (检查是否有低质量 Cluster)
  p3 <- VlnPlot(seu_obj, features = c("nFeature_RNA", "nCount_RNA", "percent.mt"), 
                pt.size = 0, ncol = 3, log = TRUE)
  
  # --- B. Differential Expression (DE) ---
  message("    Running FindAllMarkers (this may take a moment)...")
  
  # 寻找每个 Cluster 相比于其他 Cluster 的高表达基因
  markers_df <- FindAllMarkers(seu_obj, 
                               only.pos = TRUE, 
                               min.pct = 0.25, 
                               logfc.threshold = 0.25,
                               test.use = "wilcox")
  
  # 整理 Top 10 Markers 用于画图
  top10 <- markers_df %>% 
    group_by(cluster) %>% 
    top_n(n = 10, wt = avg_log2FC)
  
  # 4. Heatmap of Top 10 Markers
  p4 <- DoHeatmap(seu_obj, features = top10$gene) + NoLegend() + 
    ggtitle(paste0(label, " - Top 10 DE Genes per Cluster"))
  
  # --- C. Saving Outputs ---
  out_dir <- "analysis/results_annotation"
  dir.create(out_dir, showWarnings = FALSE, recursive = TRUE)
  
  # 保存图片 (多页 PDF)
  pdf_name <- file.path(out_dir, paste0("Plot_", label, "_Diagnostics.pdf"))
  pdf(pdf_name, width = 12, height = 8)
  print(p1)
  print(p2)
  print(p3)
  print(p4)
  dev.off()
  
  # 保存 Marker 表格
  csv_name <- file.path(out_dir, paste0("Table_", label, "_AllMarkers.csv"))
  write.csv(markers_df, csv_name, row.names = FALSE)
  
  message(sprintf("    [Done] Results saved to %s", out_dir))
}

# ==============================================================================
# 3. 执行分析
# ==============================================================================

# 1. 分析 CD8
run_diagnostics("analysis/06_cd8_fine_clustering.rds", "CD8", markers_dict$CD8)

# 2. 分析 CD4
run_diagnostics("analysis/06_cd4_fine_clustering.rds", "CD4", markers_dict$CD4)

# 3. 分析 NK
run_diagnostics("analysis/06_nk_fine_clustering.rds",  "NK",  markers_dict$NK)

message("\nAll finished! Check the 'analysis/results_annotation' folder.")