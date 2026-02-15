suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(harmony)
  library(ggplot2)
  library(cowplot)
})

# 设置随机种子
set.seed(42)

# 输出目录
out_dir <- "analysis/results_final_annotation"
dir.create(out_dir, recursive = TRUE, showWarnings = FALSE)

# ==============================================================================
# 1. 辅助函数：加载 -> 注释 -> 清洗 -> 绘图
# ==============================================================================
process_and_annotate <- function(file_path, label, rename_map, drop_idents) {
  message(sprintf("\n>>> Processing %s...", label))
  
  if (!file.exists(file_path)) stop(paste("File not found:", file_path))
  seu_obj <- readRDS(file_path)
  
  # 1. 重命名 ID
  # 只重命名当前对象中存在的 ID
  current_ids <- levels(Idents(seu_obj))
  valid_map <- rename_map[names(rename_map) %in% current_ids]
  
  if(length(valid_map) > 0) {
    seu_obj <- RenameIdents(seu_obj, valid_map)
  }
  
  # 2. 将新名字存入 metadata
  seu_obj$cell_type_fine <- Idents(seu_obj)
  seu_obj$lineage <- label # 标记大类来源
  
  # 3. 剔除污染群
  if (!is.null(drop_idents)) {
    to_remove <- drop_idents[drop_idents %in% levels(Idents(seu_obj))]
    if (length(to_remove) > 0) {
      message(sprintf("    Removing clusters: %s", paste(to_remove, collapse = ", ")))
      seu_obj <- subset(seu_obj, idents = to_remove, invert = TRUE)
    }
  }
  
  # 4. 绘图 (保存单个亚群的注释图)
  p <- DimPlot(seu_obj, label = TRUE, repel = TRUE, label.size = 4, raster = FALSE) + 
       ggtitle(paste0(label, " Annotated (Cleaned)")) + 
       theme(legend.position = "bottom")
  
  pdf_file <- file.path(out_dir, paste0("UMAP_", label, "_Annotated.pdf"))
  ggsave(pdf_file, p, width = 8, height = 8)
  
  return(seu_obj)
}

# ==============================================================================
# 2. 定义注释映射表 (Updated based on your input)
# ==============================================================================

# --- CD8 Mapping ---
# 重点更新: 9->Temra, 8->NK_like
ids_cd8 <- c(
  "0" = "CD8_Tex",          
  "1" = "CD8_Tem_IL7R",     
  "2" = "CD8_Tem_GZMK", 
  "3" = "CD8_Temra",        
  "4" = "CD8_Trm",          
  "5" = "CD8_Tem_Other",
  "7" = "CD8_Tex_CXCL13",   
  "9" = "CD8_Temra",        # [Updated] Expanded Temra definition
  "10" = "CD8_LowQual",
  "6" = "Doublet_Hep",      
  "8" = "CD8_NK_like",      # [Updated] Kept as NK-like
  "11" = "Doublet_Hep", 
  "12" = "Doublet_Hep"
)
drop_cd8 <- c("Doublet_Hep", "CD8_LowQual")

# --- CD4 Mapping ---
ids_cd4 <- c(
  "0" = "CD4_CTL",
  "1" = "CD4_Treg_Resting",
  "2" = "Doublet_Hep",
  "3" = "Doublet_CD8",
  "4" = "CD4_Naive",
  "5" = "CD4_Treg_Effector",
  "6" = "CD4_Tcm",
  "7" = "CD4_Tem",
  "8" = "CD4_Th17",
  "9" = "CD4_Th1_CXCL13",
  "10" = "CD4_Activated",
  "11" = "Doublet_CD8"
)
drop_cd4 <- c("Doublet_Hep", "Doublet_CD8")

# --- NK Mapping ---
ids_nk <- c(
  "0" = "NK_Cytotoxic_CD16",
  "1" = "NK_Resident_CXCR6",
  "2" = "NK_Cytotoxic_Transitional",
  "3" = "NK_Resident_GZMK",
  "4" = "Doublet_T_Cell",
  "5" = "NK_LowQual",
  "6" = "NK_Inflammatory",
  "7" = "NK_Activated_IEG",
  "8" = "NK_Naive_CD56bright",
  "9" = "Doublet_Hep",
  "10" = "NK_ISG",
  "11" = "Doublet_Myeloid"
)
drop_nk <- c("Doublet_T_Cell", "Doublet_Hep", "Doublet_Myeloid", "NK_LowQual")

# ==============================================================================
# 3. 执行注释与清洗
# ==============================================================================

# 注意：如果之前你做了Naive寻找并保存了新文件，请在这里修改文件名
# 默认读取 06_...
seu_cd8 <- process_and_annotate("analysis/06_cd8_fine_clustering_with_naive.rds", "CD8", ids_cd8, drop_cd8)
seu_cd4 <- process_and_annotate("analysis/06_cd4_fine_clustering.rds", "CD4", ids_cd4, drop_cd4)
seu_nk  <- process_and_annotate("analysis/06_nk_fine_clustering.rds",  "NK",  ids_nk,  drop_nk)

# ==============================================================================
# 4. 合并与重跑全局 UMAP
# ==============================================================================
message("\n>>> Merging objects into Global T/NK map...")

# 4.1 合并对象
seu_global <- merge(seu_cd8, y = list(seu_cd4, seu_nk))

# 4.2 重新跑标准流程
message("    Normalizing and finding variable features...")
seu_global <- NormalizeData(seu_global)
seu_global <- FindVariableFeatures(seu_global, nfeatures = 2000)

# [关键] 移除 TCR 和 Cell Cycle 基因，防止它们主导 UMAP 结构
tcr_genes <- grep("^TR[ABGD][VDJC]", rownames(seu_global), value = TRUE)
cc_genes <- c(Seurat::cc.genes$s.genes, Seurat::cc.genes$g2m.genes)
VariableFeatures(seu_global) <- setdiff(VariableFeatures(seu_global), c(tcr_genes, cc_genes))

message("    Scaling and PCA...")
# 计算周期评分用于回归
if(!"S.Score" %in% colnames(seu_global@meta.data)) {
  seu_global <- CellCycleScoring(seu_global, s.features = cc.genes$s.genes, g2m.features = cc.genes$g2m.genes)
}
seu_global <- ScaleData(seu_global, vars.to.regress = c("S.Score", "G2M.Score"))
seu_global <- RunPCA(seu_global, npcs = 30, verbose = FALSE, seed.use = 42)

message("    Running Harmony (Global integration)...")
# 自动检测 Batch 变量
batch_var <- "orig.ident"
for(c in c("Patient_ID", "patient_id", "sample_id")) {
  if(c %in% colnames(seu_global@meta.data)) {
    batch_var <- c
    break
  }
}
message(sprintf("    Using batch variable: %s", batch_var))

set.seed(42)
seu_global <- RunHarmony(seu_global, group.by.vars = batch_var, dims.use = 1:30, 
                         plot_convergence = FALSE, max.iter.harmony = 20)

message("    Running UMAP...")
seu_global <- RunUMAP(seu_global, reduction = "harmony", dims = 1:30, seed.use = 42)

# ==============================================================================
# 5. 最终绘图
# ==============================================================================
message("\n>>> Generating final plots...")

# 设置 Idents 为我们刚才注释好的细分类型
Idents(seu_global) <- "cell_type_fine"

# 图1: 细分类型 (Fine Annotation)
p_fine <- DimPlot(seu_global, reduction = "umap", label = TRUE, repel = TRUE, 
                  pt.size = 0.5, label.size = 3, raster = FALSE) + 
          ggtitle("Global T/NK Landscape (Fine Annotation)") +
          theme(legend.position = "bottom")

# 图2: 粗大类 (Major Lineage) - 来自之前的 lineage 列
p_lineage <- DimPlot(seu_global, reduction = "umap", group.by = "lineage", 
                     cols = c("CD8"="#377EB8", "CD4"="#4DAF4A", "NK"="#E41A1C"),
                     pt.size = 0.5, raster = FALSE) + 
             ggtitle("Global T/NK Landscape (Lineage)")

# 组合图
p_final <- plot_grid(p_lineage, p_fine, ncol = 2, rel_widths = c(0.8, 1.2))

# 保存图片
ggsave(file.path(out_dir, "Global_TNK_Final_UMAP.pdf"), p_final, width = 18, height = 8)

# ==============================================================================
# 6. 保存最终对象
# ==============================================================================
save_path <- "analysis/07_global_tnk_annotated.rds"
message(sprintf(">>> Saving final object to %s...", save_path))
saveRDS(seu_global, save_path)

message("Done! Annotation pipeline complete.")