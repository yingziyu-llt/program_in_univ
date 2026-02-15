suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(ggplot2)
  library(pheatmap)
  library(future) 
})

# 设置并行计算
plan("multisession", workers = 4)
options(future.globals.maxSize = 3000 * 1024^2) 

# ==============================================================================
# 1. 定义 Tumor Myeloid 重点关注的 Marker 列表
# 基于 Nature Communications 论文 Fig 5, Fig 6 及 Methods 
# ==============================================================================
markers_dict <- list(
  # --- 核心大类区分 ---
  General = c("CD68", "CD14", "FCGR3A"), 
  
  # --- Monocytes (单核细胞) ---
  # Classical Mono: S100A8/9 high, VCAN high [cite: 1046]
  Classical_Mono = c("S100A8", "S100A9", "VCAN", "FCN1"),
  # Non-classical Mono: CD16+, LST1+ [cite: 1046]
  NonClassical_Mono = c("LST1", "CDKN1C"), 
  
  # --- Macrophages: Pro-inflammatory / Gatekeepers (论文核心) ---
  # [cite: 702] High expression of CXCL9, CXCL10, STAT1, GBP1
  # [cite: 1649] Signature also includes IDO1, WARS
  Macro_CXCL10 = c("CXCL10", "CXCL9", "STAT1", "GBP1", "WARS"),
  
  # --- Macrophages: Angiogenic / Hypoxic ---
  # [cite: 1052] SPP1+ TAMs
  Macro_SPP1 = c("SPP1", "VEGFA", "ADM"),
  
  # --- Macrophages: Immunosuppressive / Resident-like ---
  # [cite: 709, 719] TREM2+, GPNMB+, FOLR2+, C1Q+
  Macro_Suppressive = c("TREM2", "GPNMB", "FOLR2", "C1QA", "C1QB", "CCL18"),
  
  # --- Key Checkpoint (论文结论) ---
  # [cite: 723, 1071] PD-L1 expression linked to response
  Checkpoint = c("CD274") 
)

# 将列表展平用于 DotPlot，去重
flat_markers_myeloid <- unique(unlist(markers_dict))

# ==============================================================================
# 2. 通用分析函数：绘图 + DE (保持原逻辑不变)
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
    ggtitle(paste0(label, " - Clusters (Harmony Integrated)"))
  
  # 2. DotPlot (Paper Markers)
  valid_markers <- specific_markers[specific_markers %in% rownames(seu_obj)]
  p2 <- DotPlot(seu_obj, features = valid_markers, cols = c("lightgrey", "red"), dot.scale = 8) + 
    RotatedAxis() + 
    ggtitle(paste0(label, " - Canonical Markers (Paper Fig.5)"))
  
  # 3. QC Violin Plot
  if(!"nFeature_RNA" %in% colnames(seu_obj@meta.data)) seu_obj$nFeature_RNA <- seu_obj$nFeature_RNA
  
  p3 <- VlnPlot(seu_obj, features = c("nFeature_RNA", "nCount_RNA", "percent.mt"), 
                pt.size = 0, ncol = 3, log = TRUE)
  
  # --- B. Differential Expression (DE) ---
  message("    Running FindAllMarkers (this may take a moment)...")
  
  # 寻找每个 Cluster 的特征基因
  # 注意：这里 min.pct 和 logfc 保持适中，以免漏掉弱表达的关键基因
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
  seu_subset <- seu_obj
  if(ncol(seu_obj) > 2000) {
    seu_subset <- subset(seu_obj, downsample = 200)
  }
  
  p4 <- DoHeatmap(seu_subset, features = top10$gene) + NoLegend() + 
    ggtitle(paste0(label, " - Top 10 DE Genes"))
  
  # --- C. Saving Outputs ---
  dir.create(output_subdir, showWarnings = FALSE, recursive = TRUE)
  
  # 保存 PDF
  pdf_name <- file.path(output_subdir, paste0("Plot_", label, "_Diagnostics.pdf"))
  pdf(pdf_name, width = 14, height = 10) # 稍微调宽一点以容纳更多 Marker
  print(p1)
  print(p2)
  print(p3)
  print(p4)
  dev.off()
  
  # 保存 CSV
  csv_name <- file.path(output_subdir, paste0("Table_", label, "_AllMarkers.csv"))
  write.csv(markers_df, csv_name, row.names = FALSE)
  
  message(sprintf("    [Done] Results saved to %s", output_subdir))
}

# ==============================================================================
# 3. 执行分析
# ==============================================================================

# 定义 Tumor Myeloid 专用的输出目录
OUT_DIR <- "analysis/results_Tumor_Myeloid_annotation"

# 分析 Mono/Mac
# 输入文件来自上一轮生成的 "06_mono_mac_fine_clustering.rds"
target_file <- "analysis/06_mono_mac_fine_clustering.rds"

run_diagnostics(target_file, "Mono_Mac", flat_markers_myeloid, OUT_DIR)

message("\nTumor Myeloid analysis finished! Check the folder: ", OUT_DIR)