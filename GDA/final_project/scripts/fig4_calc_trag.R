# ==============================================================================
# File: 01_trajectory_inference.R
# Purpose: Data cleaning, Harmony integration, and Slingshot trajectory inference
# ==============================================================================

suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(slingshot)
  library(SingleCellExperiment)
  library(harmony)
  library(scales)
})

# Output directory
out_dir <- "analysis/results_trajectory"
dir.create(out_dir, recursive = TRUE, showWarnings = FALSE)

# 1. Load Data
message(">>> [Step 1] Loading raw data...")
seu_tumor <- readRDS("analysis/18_tumor_final_merged.rds")
target_clusters <- c("CD8_Tn", "CD8_Tem", "CD8_Temra", "CD8_Tex", "CD8_Trm")
seu_cd8 <- subset(seu_tumor, idents = target_clusters)
seu_cd8 <- subset(seu_cd8, subset = Response %in% c("Responder", "NonResponder"))

# 2. Strict Preprocessing (Removing TCR & Cell Cycle)
message(">>> [Step 1] Preprocessing: Removing TCR & Cell Cycle genes...")
seu_cd8 <- NormalizeData(seu_cd8)
seu_cd8 <- FindVariableFeatures(seu_cd8, nfeatures = 2000)

# Identify TCR and Cycle genes
tcr_genes <- grep("^TR[ABGD][VDJC]", rownames(seu_cd8), value = TRUE)
cc_genes <- c(Seurat::cc.genes$s.genes, Seurat::cc.genes$g2m.genes)
var_feats <- VariableFeatures(seu_cd8)
var_feats <- setdiff(var_feats, c(tcr_genes, cc_genes)) # The Critical Step
VariableFeatures(seu_cd8) <- var_feats

# Scale and Regress
if("S.Score" %in% colnames(seu_cd8@meta.data)){
  seu_cd8 <- ScaleData(seu_cd8, vars.to.regress = c("S.Score", "G2M.Score"))
} else {
  seu_cd8 <- ScaleData(seu_cd8)
}

# 3. Dimension Reduction (PCA -> Harmony -> UMAP)
message(">>> [Step 1] Running PCA & Harmony...")
seu_cd8 <- RunPCA(seu_cd8, npcs = 30, verbose = FALSE)

# Auto-detect batch variable
meta_cols <- colnames(seu_cd8@meta.data)
batch_var <- "orig.ident"
for (c in c("patient_id", "Patient_ID", "sample_id")) {
  if (c %in% meta_cols) { batch_var <- c; break }
}
seu_cd8 <- RunHarmony(seu_cd8, group.by.vars = batch_var, dims.use = 1:30, plot_convergence = FALSE)

message(">>> [Step 1] Running UMAP on Harmony...")
set.seed(42)
seu_cd8 <- RunUMAP(seu_cd8, reduction = "harmony", dims = 1:30, seed.use = 42)

# Check for Start Cluster
seu_cd8$cell_type_paper <- trimws(as.character(seu_cd8$cell_type_paper))
if (!"CD8_Tn" %in% unique(seu_cd8$cell_type_paper)) stop("Error: 'CD8_Tn' not found!")

# 4. Run Slingshot
message(">>> [Step 1] Calculating Trajectories (Slingshot)...")
sce <- as.SingleCellExperiment(seu_cd8)
# Use Harmony for calculation
sce <- slingshot(sce, clusterLabels = 'cell_type_paper', reducedDim = 'HARMONY', start.clus = 'CD8_Tn')

# 5. Project Curves to UMAP (for plotting)
message(">>> [Step 1] Projecting curves to UMAP space...")
sce_curve_proj <- embedCurves(sce, "UMAP")

# 6. Save Results
saveRDS(seu_cd8, file.path(out_dir, "01_seu_cd8_clean.rds"))
saveRDS(sce, file.path(out_dir, "01_sce_slingshot_orig.rds"))
saveRDS(sce_curve_proj, file.path(out_dir, "01_sce_curve_proj.rds")) # For visualization

message(">>> Step 1 Complete. Files saved to ", out_dir)