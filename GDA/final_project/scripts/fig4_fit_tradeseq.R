# ==============================================================================
# File: 02_fit_tradeseq.R (Fast Version: Parallel + Top Genes)
# Purpose: Fit GAMs with Multi-core support to speed up calculation
# ==============================================================================

suppressPackageStartupMessages({
  library(tradeSeq)
  library(SingleCellExperiment)
  library(Seurat)
  library(slingshot)
  library(BiocParallel) # [新增] 用于并行计算
})

out_dir <- "analysis/results_trajectory"

# 1. Load Data
message(">>> [Step 2] Loading Step 1 results...")
if (!file.exists(file.path(out_dir, "01_sce_slingshot_orig.rds"))) stop("Run Step 1 first!")

sce <- readRDS(file.path(out_dir, "01_sce_slingshot_orig.rds"))
seu_cd8 <- readRDS(file.path(out_dir, "01_seu_cd8_clean.rds"))

# 2. Prepare Counts Matrix
counts_matrix <- GetAssayData(seu_cd8, assay = "RNA", slot = "counts")

# 3. Filter Genes (Optimized for Speed)
message(">>> [Step 2] Selecting genes...")

# [优化 1]：只取 Top 800 高变基因作为背景 (原先是 2000，减少计算量)
# 800 个基因足够作为 GSEA 的背景噪音分布了
hvg_all <- VariableFeatures(seu_cd8)
if(length(hvg_all) > 500) {
  hvg_subset <- hvg_all[1:500] 
} else {
  hvg_subset <- hvg_all
}

# [关键]：必须包含的 Cytotoxic / Tex Signature (确保 GSEA 能富集到)
full_cytotoxic_signature <- c(
  "GZMH", "GNLY", "NKG7", "FGFBP2", "GZMB", "CST7", "CCL5", "PRF1", 
  "CX3CR1", "CTSW", "GZMA", "KLRD1", "GZMM", "CD3D", "CD8A", "CD52", 
  "PTPRC", "CD3G", "HCST", "CD3E", "PLEK", "KLRG1", "RAC2", "LCK", 
  "CD247", "HOPX", "KLRK1", "BIN2", "S100A4", "CORO1A", "IL2RG", 
  "ITGB2", "IFITM1", "EMP3", "TRBC1", "FLNA",
  "PDCD1", "CTLA4", "HAVCR2", "LAG3", "TIGIT", "TOX", "ENTPD1", 
  "LAYN", "CXCL13", "GZMK",
  "ZNF683", "ITGAE", "IL7R", "TCF7", "LEF1"
)

# 合并基因列表
target_genes <- unique(c(hvg_subset, full_cytotoxic_signature))
# 确保基因在矩阵中存在
final_genes <- intersect(target_genes, rownames(counts_matrix))

counts_sub <- counts_matrix[final_genes, ]

# 4. Configure Parallelization (并行设置)
# [优化 2]：设置并行核心数
# 如果你在 Linux 服务器上，workers 可以设为 10-20 (取决于你的 CPU 核数)
# 如果是个人电脑，设为 4-8 比较安全
n_cores <- 16
BPPARAM <- MulticoreParam(workers = n_cores)

# 如果是在 Windows 上运行，请取消下面这行的注释，并注释掉上面一行：

message(sprintf(">>> [Step 2] Fitting model for %d genes using %d cores...", length(final_genes), n_cores))

# 5. Fit GAM with Parallel
set.seed(42)
# 注意：parallel = TRUE 和 BPPARAM 参数
sce_tradeseq <- fitGAM(counts = counts_sub, 
                       pseudotime = slingPseudotime(sce, na = FALSE), 
                       cellWeights = slingCurveWeights(sce),
                       nknots = 6, 
                       parallel = TRUE, 
                       BPPARAM = BPPARAM)

# 6. Save Model
saveRDS(sce_tradeseq, file.path(out_dir, "02_tradeseq_model.rds"))
message(">>> Step 2 Complete. Model saved.")
