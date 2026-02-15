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
  
  cc_genes <- c(Seurat::cc.genes$s.genes, Seurat::cc.genes$g2m.genes)
  seu_obj <- CellCycleScoring(seu_obj, s.features = Seurat::cc.genes$s.genes, g2m.features = Seurat::cc.genes$g2m.genes)
  
  
  var_feats <- VariableFeatures(seu_obj)
  liver_genes <- c("ALB", "AFP", "TTR", "APOA1", "APOA2", "APOB", "HP", "FGA", "FGB", "CPS1")
  hb_genes <- grep("^HB[^(P)]", rownames(seu_obj), value = TRUE)
  ig_genes <- grep("^IG[HKL]", rownames(seu_obj), value = TRUE)
  tcr_genes <- grep("^TR[ABGD]", rownames(seu_obj), value = TRUE)
  mt_genes <- grep("^MT-", rownames(seu_obj), value = TRUE)
  rp_genes <- grep("^RP[SL]", rownames(seu_obj), value = TRUE)

  blacklist <- unique(c(liver_genes, hb_genes, ig_genes, tcr_genes, mt_genes, rp_genes))
  message("剔除干扰基因数量: ", length(intersect(var_feats, blacklist)))

  var_feats_clean <- setdiff(var_feats, blacklist)
  VariableFeatures(seu_obj) <- var_feats_clean
  var_feats <- setdiff(var_feats, cc_genes)
  VariableFeatures(seu_obj) <- var_feats
  
  # 3. Scale 
  seu_obj <- ScaleData(seu_obj, vars.to.regress = c("S.Score", "G2M.Score", "percent.mt", "nCount_RNA"))
  
  # 4. PCA
  seu_obj <- RunPCA(seu_obj, npcs = dims, verbose = FALSE)
  
  # 5. Harmony Integration
  meta_cols <- colnames(seu_obj@meta.data)
  batch_var <- "Patient_ID" 
  
  message(sprintf("  Running Harmony on '%s'...", batch_var))
  seu_obj <- RunHarmony(seu_obj, group.by.vars = batch_var, dims.use = 1:dims, plot_convergence = FALSE)
  
  # 6. UMAP & Clustering
  seu_obj <- RunUMAP(seu_obj, reduction = "harmony", dims = 1:dims, seed.use = 42)
  seu_obj <- FindNeighbors(seu_obj, reduction = "harmony", dims = 1:dims)
  seu_obj <- FindClusters(seu_obj, resolution = res, random.seed = 42)
  
  return(seu_obj)
}

# ==============================================================================
# Main Workflow: Hierarchical Clustering
# ==============================================================================

# --- Load Data ---
seu_all <- readRDS("analysis/02_tumor_annotated.rds") 
myeloid_cells <- subset(seu_all, subset = cell_type_major %in% c("Myeloid"))
# --- Step 1: Coarse Clustering (Lineage Level) ---
# Lower resolution (0.4) is better for separating major lineages 
myeloid_cells <- process_subset(myeloid_cells, "Global_Myeloid", res = 0.6)

# Plot to identify Lineages
p1 <- DimPlot(myeloid_cells, label = T, raster = FALSE) + ggtitle("Level 1: Coarse Lineages")
print(p1)
# 定义髓系细胞大类 Marker
myeloid_features <- c(  
  # --- 2. Monocytes (单核细胞) ---
  "CD14", "S100A8",       # Classical Mono (S100A8/9 在 Neu 也高，需结合 CSF3R 判断)
  "FCGR3A", "LST1",               # Non-Classical Mono (CD16+)
  
  # --- 3. Macrophages (巨噬细胞 - TME 重点) ---
  "CD68",                   # General & M2-like (CD206)
  "C1QA",                   # Complement-high TAMs (通常富集在肿瘤核心)
  "SPP1",                    # SPP1+ TAMs (促血管生成/缺氧相关)
  
  # --- 4. Dendritic Cells (DCs) ---
  "CLEC9A", "XCR1",                         # cDC1 (交叉呈递，抗肿瘤关键)
  "CD1C", "FCER1A",              # cDC2 (最常见的 cDC)
  "LILRA4",                       # pDC (浆细胞样 DC)
  "LAMP3",                 # mregDC / LAMP3+ DC (成熟/迁移态 DC，TME 中常见)
  
  # --- 5. Neutrophils (中性粒细胞) ---
  "CSF3R", "CXCR2", "FCGR3B"               # 核心特异性 Marker (区别于 Monocyte)
  )

DotPlot(myeloid_cells, features = myeloid_features) + RotatedAxis()
library(Matrix)

target_obj <- myeloid_cells 

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
myeloid_cells <- target_obj

# 假设你的对象叫 myeloid_cells (或者 seu_cd8, seu_cd4 等)
# group.by 参数可以改为 "seurat_clusters" 或 "patient_id" 等

# ==============================================================================
# !!! MANUAL INPUT REQUIRED HERE !!!
# Look at the DotPlot above and fill in the cluster IDs for each lineage.
# Example IDs are provided (e.g., c(0, 2)), replace them with YOUR data's IDs.
# ==============================================================================

# ==============================================================================
# !!! MANUAL INPUT REQUIRED HERE !!!
# 根据你的 DotPlot 结果，填入对应的数字 ID
# ==============================================================================

# 1. 识别并剔除残留的中性粒细胞 (Neutrophils)
# 如果没有 CSF3R+ 的群，留空 c()
ids_neu <- c(12) 

# 2. 定义 Mono 和 Mac (这两者我们要放在一起做轨迹)
# 找出所有表达 CD14, S100A8, FCGR3A, C1QA, CD68 的群
# 基本上除了 Neu 和 垃圾群(低UMI)，剩下的都是
ids_myeloid_core <- setdiff(levels(myeloid_cells), ids_neu)

# ==============================================================================
# Step 2: Sub-clustering Loop (Corrected for Mono/Mac)
# ==============================================================================

# 论文策略：将 Mono 和 Mac 放在一起重聚类，以保留分化轨迹 [cite: 1040, 1043]
if (length(ids_myeloid_core) > 0) {
  
  message("Subsetting Monocytes and Macrophages for integrated analysis...")
  
  # 1. 提取核心 Mono/Mac 群 (去掉了 Neu 和潜在的垃圾群)
  seu_mono_mac <- subset(myeloid_cells, idents = ids_myeloid_core)
  
  # 2. 重新跑标准流程 (Re-process)
  # 注意：这里 resolution 可以稍微调高 (0.6 - 0.8) 以区分 TAM 亚群
  seu_mono_mac <- process_subset(seu_mono_mac, "Mono_Mac_Integrated", res = 1.2)
  
  # 3. 定义最终的亚群 Markers 用于注释 (参照论文 Fig 5 这里的基因)
  markers_final <- c(
    # Monocytes
    "S100A8", "S100A9", "VCAN",           # Classical Mono
    "FCGR3A", "LST1", "CDKN1C",           # Non-classical Mono
    
    # Macrophage General
    "C1QA", "C1QB", "CD68",               # Pan-Mac
    
    # 论文重点关注的亚群 [cite: 702, 709, 720]
    "CXCL10", "CXCL9", "STAT1",           # Macro-CXCL10 (Response相关的 pro-inflammatory)
    "SPP1",                               # Macro-SPP1 (Angiogenic/Hypoxic)
    "TREM2",                              # Macro-TREM2 (Immunosuppressive)
    "CCL2",                               # Mono-attracting
    "CD274"                               # PD-L1 (Check expression)
  )
  
  # 4. 绘图验证
  print(DimPlot(seu_mono_mac, label = T, raster = FALSE) + ggtitle("Mono/Mac Refined Clustering"))
  print(DotPlot(seu_mono_mac, features = markers_final) + RotatedAxis() + ggtitle("Mono/Mac Subtypes"))
  
  # 5. 保存最终对象
  saveRDS(seu_mono_mac, "analysis/06_mono_mac_fine_clustering.rds")
}

message("Myeloid fine processing complete!")