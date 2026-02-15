suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
})

# 输出目录
out_dir <- "analysis" 

# ==============================================================================
# 1. 定义合并映射表 (Merge Map) - 严格对齐论文 Fig 2e
# ==============================================================================
# 左边是你目前的 cell_type_fine，右边是论文的分类

merge_map <- c(
  # --- CD8 T Cells ---
  "CD8_Naive"      = "CD8_Tn",
  "CD8_Trm"        = "CD8_Trm",
  
  # Tem group
  "CD8_Tem"        = "CD8_Tem",
  "CD8_Tem_IL7R"   = "CD8_Tem",
  "CD8_Tem_GZMK"   = "CD8_Tem",
  "CD8_Tem_Other"  = "CD8_Tem",
  "CD8_Tem_GZMH"   = "CD8_Tem",
  "CD8_Tcm"        = "CD8_Tem", # CD8 Tcm 在肿瘤中很少，通常并入 Tem
  
  # Temra group (包含 NK-like 以匹配论文的高比例)
  "CD8_Temra"      = "CD8_Temra",
  "CD8_NK_like"    = "CD8_Temra", 
  
  # Tex group
  "CD8_Tex"        = "CD8_Tex",
  "CD8_Tex_CXCL13" = "CD8_Tex",
  "CD8_ISG"        = "CD8_Tex",
  
  # Other CD8
  "MAIT"           = "MAIT",
  "gdT"            = "gdT",
  
  # --- CD4 T Cells ---
  "CD4_Naive"      = "CD4_Tn",
  
  "CD4_Treg"            = "CD4_Treg", 
  "CD4_Treg_Resting"    = "CD4_Treg", 
  "CD4_Treg_Effector"   = "CD4_Treg",
  
  "CD4_Th1_CXCL13" = "CD4_CXCL13", # 论文 Fig 2e 重点
  "T_ISG"          = "CD4_CXCL13", # 假设 T_ISG 在 CD4 里多为 activated/exhausted 状态
  
  "CD4_Th17"       = "CD4_IL17",
  
  "CD4_Tcm"        = "CD4_Tcm",
  
  # Tem / Th1 / Memory / CTL 统归为 Tem
  "CD4_Tem"        = "CD4_Tem",
  "CD4_Th1"        = "CD4_Tem",
  "CD4_Th2"        = "CD4_Tem",
  "CD4_Memory"     = "CD4_Tem",
  "CD4_CTL"        = "CD4_Tem", 
  "CD4_Activated"  = "CD4_Tem",
  "CD4_ISG"        = "CD4_Tem",
  
  # --- NK Cells ---
  # 论文 Fig 2e 区分了 Cytotoxic 和 Resting
  "NK_Dim"                      = "NK_Cytotoxic",
  "NK_Cytotoxic_CD16"           = "NK_Cytotoxic",
  "NK_Cytotoxic_Transitional"   = "NK_Cytotoxic",
  "NK_Inflammatory"             = "NK_Cytotoxic",
  
  "NK_Bright"                   = "NK_Resting",
  "NK_Resident"                 = "NK_Resting",
  "NK_Resident_CXCR6"           = "NK_Resting",
  "NK_Resident_GZMK"            = "NK_Resting",
  "NK_Naive_CD56bright"         = "NK_Resting",
  
  # 其他 NK
  "NK_Proliferating"            = "NK_Proliferating",
  "NK_ISG"                      = "NK_Resting", # ISG通常偏向组织驻留/CD56bright特征
  "NK_Activated_IEG"            = "NK_Cytotoxic",
  
  # --- Proliferating (PBMC) ---
  "CD8_Proliferating" = "Proliferating",
  "CD4_Proliferating" = "Proliferating",
  "NK_Proliferating"  = "Proliferating",
  "T_Proliferating"   = "Proliferating",
  "T_Proliferating_Other" = "Proliferating",
  "Prolif_CD4" = "Proliferating",
  "Prolif_CD8" = "Proliferating",
  "Prolif_NK_gdT" = "Proliferating",
  "Prolif_Myeloid" = "Proliferating",
  "Prolif_B" = "Proliferating"
)

# ==============================================================================
# 2. 处理函数：应用映射并保存
# ==============================================================================
apply_mapping <- function(input_path, output_filename, label) {
  message(sprintf("\n>>> Processing %s...", label))
  
  if (!file.exists(input_path)) {
    warning(paste("Input file not found:", input_path))
    return(NULL)
  }
  
  seu_obj <- readRDS(input_path)
  
  # 1. 修正 Lineage (特别是 CD4_CTL)
  # 这一步是为了确保之前在 CD8 里的 Cluster 5 被正确标记为 CD4
  fine_idents <- as.character(Idents(seu_obj))
  is_cd4_ctl <- fine_idents == "CD4_CTL"
  if (any(is_cd4_ctl)) {
    message(sprintf("    Correcting lineage for %d CD4_CTL cells -> CD4", sum(is_cd4_ctl)))
    seu_obj$lineage[is_cd4_ctl] <- "CD4"
  }
  
  # 2. 应用 Merge Map
  # 获取当前的细分注释
  current_labels <- as.character(Idents(seu_obj))
  
  # 映射到大类
  merged_labels <- merge_map[current_labels]
  
  # 检查是否有未映射的标签 (NA)
  if (any(is.na(merged_labels))) {
    na_labels <- unique(current_labels[is.na(merged_labels)])
    message("    Warning: Some labels were not found in merge_map and will define as 'Unknown':")
    print(na_labels)
    merged_labels[is.na(merged_labels)] <- "Unknown"
  }
  
  # 3. 写入 Metadata
  seu_obj$cell_type_paper <- merged_labels
  
  # 4. 更新 Idents 为新的大类 (方便后续直接用)
  Idents(seu_obj) <- "cell_type_paper"
  
  message(sprintf("    Mapped %d categories to %d categories.", 
                  length(unique(current_labels)), 
                  length(unique(merged_labels))))
  
  # 5. 保存
  save_path <- file.path(out_dir, output_filename)
  message(sprintf("    Saving to %s...", save_path))
  saveRDS(seu_obj, save_path)
  
  return(invisible(NULL))
}

# ==============================================================================
# 3. 执行
# ==============================================================================

# --- Tumor Dataset ---
# 输入: 07_global_tnk_annotated.rds (Fine annotation)
# 输出: 18_tumor_final_merged.rds
apply_mapping("analysis/TNK/07_global_tnk_annotated.rds", "analysis/TNK/18_tumor_final_merged.rds", "Tumor")

# --- PBMC Dataset ---
# 输入: 14_pbmc_global_annotated.rds (Fine annotation)
# 输出: 18_pbmc_final_merged.rds
apply_mapping("analysis/TNK/14_pbmc_global_annotated.rds", "analysis/TNK/18_pbmc_final_merged.rds", "PBMC")

message("\nMerge Map Applied! Ready for Differential Expression & Trajectory.")