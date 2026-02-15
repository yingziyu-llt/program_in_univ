suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(harmony)
})
set.seed(42)

# ==============================================================================
# Helper Function: Strict Processing (unchanged)
# ==============================================================================
process_subset <- function(seu_obj, label, dims = 30, res = 0.6) {
  message(sprintf("Processing %s (%d cells)...", label, ncol(seu_obj)))
  
  # 1. Normalize & Find Variables
  seu_obj <- NormalizeData(seu_obj)
  seu_obj <- FindVariableFeatures(seu_obj, nfeatures = 2000)
  
  # 2. Remove TCR/BCR & Cell Cycle genes from Variable Features [cite: 1439]
  tcr_genes <- grep("^TR[ABGD][VDJC]", rownames(seu_obj), value = TRUE)
  cc_genes <- c(Seurat::cc.genes$s.genes, Seurat::cc.genes$g2m.genes)
  
  var_feats <- VariableFeatures(seu_obj)
  var_feats <- setdiff(var_feats, c(tcr_genes, cc_genes))
  VariableFeatures(seu_obj) <- var_feats
  
  # 3. Scale 
  if("S.Score" %in% colnames(seu_obj@meta.data)){
      seu_obj <- ScaleData(seu_obj, vars.to.regress = c("S.Score", "G2M.Score"))
  } else {
      seu_obj <- ScaleData(seu_obj)
  }
  
  # 4. PCA
  seu_obj <- RunPCA(seu_obj, npcs = dims, verbose = FALSE)
  
  # 5. Harmony Integration
  meta_cols <- colnames(seu_obj@meta.data)
  batch_var <- "orig.ident" 
  for (c in c("patient_id", "Patient_ID", "sample_id")) {
    if (c %in% meta_cols) { batch_var <- c; break }
  }
  
  message(sprintf("  Running Harmony on '%s'...", batch_var))
  seu_obj <- RunHarmony(seu_obj, group.by.vars = batch_var, dims.use = 1:dims, plot_convergence = FALSE)
  
  # 6. UMAP & Clustering
  seu_obj <- RunUMAP(seu_obj, reduction = "harmony", dims = 1:dims, seed.use = 42)
  seu_obj <- FindNeighbors(seu_obj, reduction = "harmony", dims = 1:dims)
  seu_obj <- FindClusters(seu_obj, resolution = res, random.seed = 42)
  
  return(seu_obj)
}

# ==============================================================================
# Main Workflow: Hierarchical Clustering (Updated)
# ==============================================================================

# --- Load Data ---
seu_all <- readRDS("analysis/02_tumor_annotated.rds") 
tnk_cells <- subset(seu_all, subset = cell_type_major %in% c("T Cell", "NK Cell"))
# --- Step 1: Coarse Clustering (Lineage Level) ---
# Lower resolution (0.4) is better for separating major lineages 
tnk_cells <- process_subset(tnk_cells, "Global_TNK", res = 0.6)

# Plot to identify Lineages
p1 <- DimPlot(tnk_cells, label = T, raster = FALSE) + ggtitle("Level 1: Coarse Lineages")
print(p1)

# Check markers to ID clusters manually
# CD8: CD8A, CD8B
# CD4: CD4, IL7R
# NK: GNLY, NKG7 (High), CD3E (Low)
# Proliferative: MKI67 (Exclude these from downstream if strictly following paper [cite: 53])
DotPlot(tnk_cells, features = c("CD3E", "CD4", "CD8A", "GNLY", "NKG7", "MKI67", "TOP2A", "CCR7", "SELL", "CD69", "PDCD1")) + RotatedAxis()
library(Matrix)

# 假设你的对象叫 tnk_cells (请按需替换)
target_obj <- tnk_cells 

# 1. 获取原始计数矩阵 (Assay 默认为 "RNA")
counts_matrix <- GetAssayData(target_obj, layer = "counts", assay = "RNA")

# 2. 手动计算并赋值给 meta.data
# n_genes: 每一列中大于0的元素个数
target_obj$n_genes <- Matrix::colSums(counts_matrix > 0)

# total_count: 每一列的总和
target_obj$total_count <- Matrix::colSums(counts_matrix)

# 3. 绘图验证
# 小提琴图
p_vln_clusters <- VlnPlot(target_obj, 
                          features = c("n_genes", "total_count"), 
                          group.by = "seurat_clusters", 
                          pt.size = 0,    # 不显示散点，画面更干净
                          ncol = 1)       # 竖着排，方便对比

print(p_vln_clusters)

p_vln_box <- VlnPlot(target_obj, 
                     features = c("n_genes", "total_count"), 
                     group.by = "seurat_clusters", 
                     pt.size = 0, 
                     ncol = 1) + 
             geom_boxplot(width = 0.1, fill = "white", outlier.shape = NA)

print(p_vln_box)
# 如果确认无误，可以将改动存回原对象变量
tnk_cells <- target_obj

# 假设你的对象叫 tnk_cells (或者 seu_cd8, seu_cd4 等)
# group.by 参数可以改为 "seurat_clusters" 或 "patient_id" 等

# ==============================================================================
# !!! MANUAL INPUT REQUIRED HERE !!!
# Look at the DotPlot above and fill in the cluster IDs for each lineage.
# Example IDs are provided (e.g., c(0, 2)), replace them with YOUR data's IDs.
# ==============================================================================

ids_cd8 <- c(1, 5, 8, 9)   # REPLACE with your CD8 cluster IDs
ids_cd4 <- c(0, 3, 4, 10, 12)   # REPLACE with your CD4 cluster IDs
ids_nk  <- c(2, 6, 11)      # REPLACE with your NK cluster IDs

# ==============================================================================
# Step 2: Sub-clustering Loop
# ==============================================================================

# --- A. CD8 T Cells (Fine Clustering) ---
if (length(ids_cd8) > 0) {
  seu_cd8 <- subset(tnk_cells, idents = ids_cd8)
  # High resolution (0.6-0.8) to separate Temra vs Tex [cite: 1441]
  seu_cd8 <- process_subset(seu_cd8, "CD8_Only", res = 0.8)
  
  # Validation Markers for CD8 [cite: 1515, 365]
  # Temra: FGFBP2+, CX3CR1+, PDCD1-
  markers_cd8 <- c("CD8A", "PDCD1", "HAVCR2", "LAG3", "GZMK", "FGFBP2", "CX3CR1", "GNLY", "TCF7")
  print(DotPlot(seu_cd8, features = markers_cd8) + RotatedAxis() + ggtitle("CD8 Subtypes"))
  
  saveRDS(seu_cd8, "analysis/06_cd8_fine_clustering.rds")
}

# --- B. CD4 T Cells (Fine Clustering) ---
if (length(ids_cd4) > 0) {
  seu_cd4 <- subset(tnk_cells, idents = ids_cd4)
  seu_cd4 <- process_subset(seu_cd4, "CD4_Only", res = 0.6)
  
  # Validation Markers for CD4 [cite: 63, 64]
  # Treg: FOXP3, IL2RA
  # Exhausted/Th1 (CD4-CXCL13): CXCL13, PDCD1, IFNG
  # Naive/CM: CCR7, TCF7, IL7R
  markers_cd4 <- c("CD4", "FOXP3", "IL2RA", "CXCL13", "IFNG", "PDCD1", "CCR7", "TCF7")
  print(DotPlot(seu_cd4, features = markers_cd4) + RotatedAxis() + ggtitle("CD4 Subtypes"))
  
  saveRDS(seu_cd4, "analysis/06_cd4_fine_clustering.rds")
}

# --- C. NK Cells (Fine Clustering) ---
if (length(ids_nk) > 0) {
  seu_nk <- subset(tnk_cells, idents = ids_nk)
  # NK heterogeneity is usually lower, res 0.4-0.6 is often enough
  seu_nk <- process_subset(seu_nk, "NK_Only", res = 0.5)
  
  # Validation Markers for NK [cite: 214, 220]
  # Cytotoxic: GNLY, PRF1, FCGR3A (CD16)
  # Resting/Res: KLRC1 (NKG2A), XCL1, XCL2
  markers_nk <- c("GNLY", "NKG7", "FCGR3A", "KLRC1", "XCL1", "XCL2", "MKI67")
  print(DotPlot(seu_nk, features = markers_nk) + RotatedAxis() + ggtitle("NK Subtypes"))
  
  saveRDS(seu_nk, "analysis/06_nk_fine_clustering.rds")
}

message("Hierarchy processing complete!")