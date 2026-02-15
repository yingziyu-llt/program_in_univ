suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(harmony)
  library(Matrix)
  library(ggplot2)
})

# 设置随机种子
set.seed(42)

# ==============================================================================
# 1. 路径与配置
# ==============================================================================
# 根据你提供的信息设置路径
DATA_DIR <- "data/PBMC"
MTX_DIR  <- file.path(DATA_DIR, "PBMC_scRNAseq_NKT")
META_FILE <- file.path(DATA_DIR, "PBMC_meta_data.txt")

OUTPUT_DIR <- "analysis"
FIG_DIR <- "figures"
if(!dir.exists(OUTPUT_DIR)) dir.create(OUTPUT_DIR)
if(!dir.exists(FIG_DIR)) dir.create(FIG_DIR)

# ==============================================================================
# 2. 核心函数：Strict Processing (复用)
# ==============================================================================
process_subset <- function(seu_obj, label, dims = 30, res = 0.6) {
  message(sprintf("Processing %s (%d cells)...", label, ncol(seu_obj)))
  
  # 1. Normalize & Find Variables
  seu_obj <- NormalizeData(seu_obj)
  seu_obj <- FindVariableFeatures(seu_obj, nfeatures = 2000)
  
  # 2. Remove TCR/BCR & Cell Cycle genes [cite: 1439]
  tcr_genes <- grep("^TR[ABGD][VDJC]", rownames(seu_obj), value = TRUE)
  cc_genes <- c(Seurat::cc.genes$s.genes, Seurat::cc.genes$g2m.genes)
  
  var_feats <- VariableFeatures(seu_obj)
  var_feats <- setdiff(var_feats, c(tcr_genes, cc_genes))
  VariableFeatures(seu_obj) <- var_feats
  
  # 3. Scale 
  # 如果尚未计算周期评分，先计算一遍以便回归
  if(!"S.Score" %in% colnames(seu_obj@meta.data)){
      message("  Calculating cell cycle scores for regression...")
      seu_obj <- CellCycleScoring(seu_obj, s.features = Seurat::cc.genes$s.genes, g2m.features = Seurat::cc.genes$g2m.genes)
  }
  seu_obj <- ScaleData(seu_obj, vars.to.regress = c("S.Score", "G2M.Score", "percent.mt"))
  
  # 4. PCA
  seu_obj <- RunPCA(seu_obj, npcs = dims, verbose = FALSE)
  
  # 5. Harmony Integration
  meta_cols <- colnames(seu_obj@meta.data)
  batch_var <- "orig.ident" 
  # 优先检测 Patient_ID，因为 PBMC 通常来自多病人
  for (c in c("Patient_ID", "patient_id", "sample_id", "orig.ident")) {
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
# 3. 读取数据 (Robust Reading)
# ==============================================================================
message("Reading 10X matrix from: ", MTX_DIR)
counts <- tryCatch(
  { Read10X(data.dir = MTX_DIR) },
  error = function(e) {
    message("  Standard Read10X failed, retrying with gene.column=1...")
    Read10X(data.dir = MTX_DIR, gene.column = 1)
  }
)

# 创建初始对象
seu_pbmc <- CreateSeuratObject(counts = counts, project = "PBMC_NKT", min.cells = 3)
message("Initial matrix: ", ncol(seu_pbmc), " cells")

# 读取元数据
message("Reading metadata from: ", META_FILE)
# 注意：meta头文件有引号，read.table默认会处理
meta <- read.table(META_FILE, header = TRUE, sep = "\t", row.names = 1, stringsAsFactors = FALSE)

# --- 关键：Barcode 匹配检查与修复 ---
common_cells <- intersect(colnames(seu_pbmc), rownames(meta))

if (length(common_cells) == 0) {
  message("Warning: No direct barcode matches found. Checking format differences...")
  head_seu <- head(colnames(seu_pbmc), 1)
  head_meta <- head(rownames(meta), 1)
  message("  Seurat example: ", head_seu)
  message("  Meta example:   ", head_meta)
  
  # 尝试修复：如果 Meta 有后缀 "_Sample_1" 但 Seurat 没有
  if (grepl("_", head_meta) && !grepl("_", head_seu)) {
    message("  Attempting to match by removing suffix from metadata rownames...")
    # 创建临时的清洗后的行名用于匹配
    clean_meta_rownames <- sub("_.*", "", rownames(meta)) # 去掉 _ 及其后面所有内容
    # 这里有点风险，因为不同 sample 可能有相同的 barcode 序列。
    # 更安全的方法是修改 Seurat 的名字去匹配 Meta（如果有 Sample 信息）
    # 但由于我们还没加载 Sample 信息到 Seurat，我们反向操作：
    # 假设 matrix 里的 barcode 是唯一的 (cellranger aggr output)，我们需要找到映射关系
    
    # 另一种常见情况：Matrix 的 barcode 带 "-1"，但 Meta 带 "_Sample_X"
    # 我们尝试仅匹配 "barcode序列-1" 部分
    
    # 简易修复：强制取交集 (如果 matrix 是单个 sample 的聚合，barcode 应该是唯一的)
    # 让我们假设 matrix 的列名其实就是 meta 行名的一部分
    # 这里为了稳妥，我们打印错误并停止，除非你能确认 pattern
    stop("Barcode mismatch! Please check the output above. You may need to modify rownames(meta) or colnames(seu_pbmc).")
  }
} else {
  message("  Matched ", length(common_cells), " cells with metadata.")
}

# 过滤并添加元数据
seu_pbmc <- subset(seu_pbmc, cells = common_cells)
seu_pbmc <- AddMetaData(seu_pbmc, metadata = meta[common_cells, ])

# ==============================================================================
# 4. QC (PBMC Specific) 
# ==============================================================================
seu_pbmc[["percent.mt"]] <- PercentageFeatureSet(seu_pbmc, pattern = "^MT-")

message("Applying PBMC QC thresholds (Paper Methods):")
message("  nFeature: 200 - 6000")
message("  nCount:   > 400")
message("  MT:       < 15%") # 论文 PBMC 阈值为 15%

seu_pbmc <- subset(seu_pbmc, subset = nFeature_RNA > 200 & 
                                     nFeature_RNA < 6000 & 
                                     nCount_RNA > 400 & 
                                     percent.mt < 15)
message("Cells after QC: ", ncol(seu_pbmc))

# ==============================================================================
# 5. 粗聚类 (Coarse Clustering)
# ==============================================================================
# 使用 process_subset 直接跑完 Norm -> Scale -> PCA -> Harmony -> UMAP
# res = 0.4 用于区分大类 (CD4/CD8/NK)
seu_pbmc <- process_subset(seu_pbmc, "PBMC_Global", res = 0.8)

# ==============================================================================
# 6. 可视化与保存
# ==============================================================================
# UMAP 图
p1 <- DimPlot(seu_pbmc, label = TRUE, raster = FALSE) + ggtitle("PBMC Coarse Clusters")

# 谱系 Marker (辅助你下一步手动分群)
lineage_markers <- c("CD3D", "CD3E", "CD4", "CD8A", "CD8B", "GNLY", "NKG7", "MKI67", "TOP2A")
p2 <- DotPlot(seu_pbmc, features = lineage_markers) + RotatedAxis() + ggtitle("Lineage Markers")

# 计算 QC 指标 (用于检查是否有低质量群)
counts_matrix <- GetAssayData(seu_pbmc, layer = "counts", assay = "RNA")
seu_pbmc$n_genes <- Matrix::colSums(counts_matrix > 0)
seu_pbmc$total_count <- Matrix::colSums(counts_matrix)

p3 <- VlnPlot(seu_pbmc, features = c("n_genes", "total_count"), group.by = "seurat_clusters", pt.size = 0, ncol = 1)

# 保存图
ggsave(file.path(FIG_DIR, "PBMC_Coarse_UMAP.pdf"), p1, width = 8, height = 6)
ggsave(file.path(FIG_DIR, "PBMC_Coarse_DotPlot.pdf"), p2, width = 10, height = 6)
ggsave(file.path(FIG_DIR, "PBMC_Coarse_QC.pdf"), p3, width = 8, height = 8)

# 保存中间对象
save_path <- file.path(OUTPUT_DIR, "10_pbmc_coarse.rds")
message("Saving coarse PBMC object to: ", save_path)
saveRDS(seu_pbmc, save_path)

message("PBMC preprocessing complete! Please check 'figures/PBMC_Coarse_DotPlot.pdf' to identify cluster IDs for Step 2.")