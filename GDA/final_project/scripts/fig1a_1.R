suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(harmony)
  library(ggplot2)
})

MTX_DIR <- "data/HCC_data_VIB_CCB_portal/processed_feature_bc_matrix"
META_FILE <- "data/HCC_data_VIB_CCB_portal/meta_data.txt"
OUTPUT_DIR <- "analysis"
FIG_DIR <- "figures"
set.seed(42)
counts <- Read10X(data.dir = MTX_DIR, gene.column = 1) # 数据有问题，需要 gene.column=1
message("Matrix dimensions: ", nrow(counts), " genes x ", ncol(counts), " cells")

seu <- CreateSeuratObject(counts = counts, project = "aHCC_Tumor", min.cells = 3)

message("Reading metadata from: ", META_FILE)
meta <- read.table(META_FILE, header = TRUE, sep = "\t", row.names = 1, stringsAsFactors = FALSE)

common_cells <- intersect(rownames(meta), colnames(seu))
message("Cells with matching metadata: ", length(common_cells))

seu <- subset(seu, cells = common_cells)
seu <- AddMetaData(seu, metadata = meta[common_cells, ])

message("Metadata added successfully. Sample Preview:")
print(head(seu@meta.data))

seu[["percent.mt"]] <- PercentageFeatureSet(seu, pattern = "^MT-")
# 这里的过滤标准来自 Methods
# nFeature: 200-6000
# nCount: >400
# MT: <50%
seu <- subset(seu, subset = nFeature_RNA > 200 & 
                            nFeature_RNA < 6000 & 
                            nCount_RNA > 400 & 
                            percent.mt < 50)
message("Cells after QC: ", ncol(seu))

message("Normalizing & PCA...")
seu <- NormalizeData(seu)
seu <- FindVariableFeatures(seu, selection.method = "vst", nfeatures = 2000)
seu <- ScaleData(seu, vars.to.regress = c("percent.mt", "nCount_RNA"))
seu <- RunPCA(seu, npcs = 50, verbose = FALSE)

message("Running Harmony...")

batch_var <- "Patient_ID"
seu <- RunHarmony(seu, group.by.vars = batch_var, plot_convergence = FALSE)

message("Clustering & UMAP...")
seu <- RunUMAP(seu, reduction = "harmony", dims = 1:30)
seu <- FindNeighbors(seu, reduction = "harmony", dims = 1:30)
seu <- FindClusters(seu, resolution = 0.5)

save_path <- file.path(OUTPUT_DIR, "01_tumor_integrated.rds")
message("Saving to: ", save_path)
saveRDS(seu, save_path)

p1 <- DimPlot(seu, reduction = "umap", label = TRUE) + ggtitle("Clusters")
ggsave(file.path(FIG_DIR, "UMAP_tumor_global.png"), p1, width = 8, height = 6)