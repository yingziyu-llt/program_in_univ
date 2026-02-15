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
tnk_cells <- readRDS("analysis/10_pbmc_coarse.rds") 

ids_cd8 <- c(3,5,6,8)   # REPLACE with your CD8 cluster IDs
ids_cd4 <- c(0,2,4,7,12,13)   # REPLACE with your CD4 cluster IDs
ids_nk  <- c(1,9,10,14)      # REPLACE with your NK cluster IDs
ids_prolif <- c(11)        # Proliferating cells cluster ID

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
  
  saveRDS(seu_cd8, "analysis/11_PBMC_cd8_fine_clustering.rds")
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
  
  saveRDS(seu_cd4, "analysis/11_PBMC_cd4_fine_clustering.rds")
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
  
  saveRDS(seu_nk, "analysis/11_PBMC_nk_fine_clustering.rds")
}

if (length(ids_prolif) > 0) {
  seu_prolif <- subset(tnk_cells, idents = ids_prolif)
  seu_prolif <- process_subset(seu_prolif, "Proliferating_Only", res = 0.4)
  
  # Validation Markers for Proliferating Cells
  markers_prolif <- c("MKI67", "TOP2A", "PCNA", "CDK1")
  print(DotPlot(seu_prolif, features = markers_prolif) + RotatedAxis() + ggtitle("Proliferating Cells Markers"))
  
  saveRDS(seu_prolif, "analysis/11_PBMC_proliferating_fine_clustering.rds")
}

message("Hierarchy processing complete!")