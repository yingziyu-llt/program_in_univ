suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(harmony)
})

set.seed(42)

# ==============================================================================
# 1. 配置路径
# ==============================================================================
MTX_DIR <- "data/PBMC/PBMC_scRNAseq_NKT"
META_FILE <- "data/PBMC/PBMC_meta_data.txt"
OUTPUT_DIR <- "analysis"

# ==============================================================================
# 2. 读取与合并 (类似 Tumor 的逻辑)
# ==============================================================================
message("Reading PBMC matrix...")
# 这里同样加入自动容错，防止列数问题
counts <- tryCatch(
  { Read10X(data.dir = MTX_DIR) },
  error = function(e) { Read10X(data.dir = MTX_DIR, gene.column = 1) }
)

seu_pbmc <- CreateSeuratObject(counts = counts, project = "aHCC_PBMC", min.cells = 3)

# ==============================================================================
# 3. 加载元数据
# ==============================================================================
message("Reading PBMC metadata...")
meta <- read.table(META_FILE, header = TRUE, sep = "\t", row.names = 1, stringsAsFactors = FALSE)

# 匹配 Barcode
common_cells <- intersect(rownames(meta), colnames(seu_pbmc))
if (length(common_cells) == 0) stop("PBMC Metadata mismatch! Check barcodes.")

seu_pbmc <- subset(seu_pbmc, cells = common_cells)
seu_pbmc <- AddMetaData(seu_pbmc, metadata = meta[common_cells, ])

# ==============================================================================
# 4. 简易质控与处理 (PBMC 质量通常较好)
# ==============================================================================
seu_pbmc[["percent.mt"]] <- PercentageFeatureSet(seu_pbmc, pattern = "^MT-")
# 沿用文章标准
seu_pbmc <- subset(seu_pbmc, subset = nFeature_RNA > 200 & nFeature_RNA < 6000 & percent.mt < 50)

seu_pbmc <- NormalizeData(seu_pbmc) %>% 
  FindVariableFeatures(nfeatures = 2000) %>% 
  ScaleData() %>% 
  RunPCA(verbose = FALSE)

# ==============================================================================
# 5. Harmony 去批次
# ==============================================================================
# 自动检测 Patient ID 列
batch_var <- "Patient_ID" 
if (!batch_var %in% colnames(seu_pbmc@meta.data)) {
   # 尝试找 patient id 列
   possible <- c("patient_id", "sample_id", "orig.ident", "patient")
   found <- possible[possible %in% colnames(seu_pbmc@meta.data)]
   if(length(found)>0) batch_var <- found[1]
}
message("PBMC Batch variable: ", batch_var)

seu_pbmc <- RunHarmony(seu_pbmc, group.by.vars = batch_var, plot_convergence = FALSE)

# ==============================================================================
# 6. 保存
# ==============================================================================
# 因为文件夹名带有 NKT，我们假设它已经是 T/NK 为主，暂时不做粗分，直接保存用于细分
saveRDS(seu_pbmc, file.path(OUTPUT_DIR, "04_pbmc_integrated.rds"))
message("PBMC data prepared.")