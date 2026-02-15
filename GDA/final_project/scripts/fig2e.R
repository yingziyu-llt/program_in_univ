suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(ggpubr)
  library(rstatix)
  library(cowplot)
})

# 1. 设置输出目录
out_dir <- "analysis/results_abundance_paper_aligned"
dir.create(out_dir, recursive = TRUE, showWarnings = FALSE)

# 2. 定义合并映射 (保持不变)
merge_map <- c(
  "CD8_Naive" = "CD8_Tn", "CD8_Trm" = "CD8_Trm", "CD8_Tem" = "CD8_Tem",
  "CD8_Tem_IL7R" = "CD8_Tem", "CD8_Tem_GZMK" = "CD8_Tem", "CD8_Tem_Other" = "CD8_Tem",
  "CD8_Tem_GZMH" = "CD8_Tem", "CD8_Tcm" = "CD8_Tem", "CD8_Temra" = "CD8_Temra",
  "CD8_NK_like" = "CD8_NK_like", "CD8_Tex" = "CD8_Tex", "CD8_Tex_CXCL13" = "CD8_Tex",
  "CD8_ISG" = "CD8_Tex", "MAIT" = "MAIT", "gdT" = "gdT",
  "CD4_Naive" = "CD4_Tn", "CD4_Treg" = "CD4_Treg", "CD4_Treg_Resting" = "CD4_Treg",
  "CD4_Treg_Effector" = "CD4_Treg", "CD4_Th1_CXCL13" = "CD4_CXCL13", "CD4_Th17" = "CD4_IL17",
  "CD4_Tcm" = "CD4_Tcm", "CD4_Tem" = "CD4_Tem", "CD4_Th1" = "CD4_Tem",
  "CD4_Th2" = "CD4_Tem", "CD4_Memory" = "CD4_Tem", "CD4_CTL" = "CD4_Tem",
  "CD4_Activated" = "CD4_Tem", "CD4_ISG" = "CD4_Tem"
)

# 3. 准备数据函数
prepare_metadata <- function(file_cd8, file_cd4, file_nk, 
                             map_cd8, drop_cd8, 
                             map_cd4, drop_cd4, 
                             map_nk, drop_nk, label) {
  
  if(!file.exists(file_cd8)) stop(paste("File not found:", file_cd8))
  
  s_cd8 <- readRDS(file_cd8)
  s_cd4 <- readRDS(file_cd4)
  
  s_cd8 <- RenameIdents(s_cd8, map_cd8[names(map_cd8) %in% levels(Idents(s_cd8))])
  s_cd8 <- subset(s_cd8, idents = drop_cd8[drop_cd8 %in% levels(Idents(s_cd8))], invert = TRUE)
  s_cd8$lineage <- "CD8" 
  
  s_cd4 <- RenameIdents(s_cd4, map_cd4[names(map_cd4) %in% levels(Idents(s_cd4))])
  s_cd4 <- subset(s_cd4, idents = drop_cd4[drop_cd4 %in% levels(Idents(s_cd4))], invert = TRUE)
  s_cd4$lineage <- "CD4"
  
  s_global <- merge(s_cd8, y = list(s_cd4))
  
  current_idents <- Idents(s_global)
  is_cd4_ctl <- current_idents == "CD4_CTL"
  if(any(is_cd4_ctl)) s_global$lineage[is_cd4_ctl] <- "CD4"
  
  fine_names <- as.character(Idents(s_global))
  merged_names <- merge_map[fine_names]
  merged_names[is.na(merged_names)] <- fine_names[is.na(merged_names)] 
  
  s_global$cell_type_merged <- merged_names
  return(s_global@meta.data)
}

# 4. ID 定义 (保持不变)
ids_pbmc_cd8 <- c("0"="CD8_Temra","1"="gdT","2"="CD8_Naive","3"="CD8_Tem","4"="MAIT","5"="CD4_CTL","6"="CD8_Tcm","7"="CD8_Tem","8"="Doublet_HLA","9"="CD8_Tem","10"="CD8_Naive")
drop_pbmc_cd8 <- c("Doublet_HLA") 
ids_pbmc_cd4 <- c("0"="CD4_Naive","1"="CD4_Tcm","2"="CD4_Naive","3"="CD4_Th17","4"="CD4_Treg","5"="CD4_Tem","6"="CD4_Th1","7"="LowQual","8"="Doublet_CD8","9"="CD4_Th2","10"="CD4_CTL","11"="Doublet_Myeloid","12"="Doublet_CD8","13"="T_ISG")
drop_pbmc_cd4 <- c("Doublet_CD8", "Doublet_Myeloid", "LowQual")
ids_tumor_cd8 <- c("0"="CD8_Tex","1"="CD8_Tem_IL7R","2"="CD8_Tem_GZMK","3"="CD8_Temra","4"="CD8_Trm","5"="CD8_Tem_Other","7"="CD8_Tex_CXCL13","9"="CD8_Temra","10"="CD8_LowQual","6"="Doublet_Hep","8"="CD8_NK_like","11"="Doublet_Hep","12"="Doublet_Hep")
drop_tumor_cd8 <- c("Doublet_Hep", "CD8_LowQual")
ids_tumor_cd4 <- c("0"="CD4_CTL","1"="CD4_Treg_Resting","2"="Doublet_Hep","3"="Doublet_CD8","4"="CD4_Naive","5"="CD4_Treg_Effector","6"="CD4_Tcm","7"="CD4_Tem","8"="CD4_Th17","9"="CD4_Th1_CXCL13","10"="CD4_Activated","11"="Doublet_CD8")
drop_tumor_cd4 <- c("Doublet_Hep", "Doublet_CD8")
ids_nk_dummy <- c("0"="NK")
drop_nk_dummy <- c()

# 5. 核心绘图函数 (已修复 write.csv 报错)
plot_and_stats_t_cells_ref_style <- function(meta, label) {
  
  if ("Treatment" %in% colnames(meta)) {
    meta <- meta %>% filter(Treatment == "atezo+bev")
  }
  
  meta <- meta %>% 
    filter(!is.na(Response) & Response != "" & Response != "Not Evaluable" & Response != "DeathBeforeImaging")
  
  # --- 计算比例 ---
  df_prop <- meta %>%
    group_by(Patient_ID, Response, cell_type_merged) %>%
    summarise(n = n(), .groups = 'drop') %>%
    group_by(Patient_ID) %>%
    mutate(fraction = n / sum(n)) %>% 
    ungroup()
  
  # --- 排序逻辑 ---
  cell_types_unique <- unique(df_prop$cell_type_merged)
  cd4_types <- sort(cell_types_unique[grep("^CD4", cell_types_unique)])
  cd8_types <- sort(cell_types_unique[grep("^CD8", cell_types_unique)])
  other_types <- sort(setdiff(cell_types_unique, c(cd4_types, cd8_types)))
  final_order <- c(cd4_types, cd8_types, other_types)
  df_prop$cell_type_merged <- factor(df_prop$cell_type_merged, levels = final_order)
  
  # --- 统计检验 ---
  stat.test <- df_prop %>%
    group_by(cell_type_merged) %>%
    wilcox_test(fraction ~ Response) %>%
    add_significance() %>%
    add_xy_position(x = "cell_type_merged", dodge = 0.8) 
  
  # === 修复点：导出 CSV 前移除 list 类型的列 (groups) ===
  # rstatix 的 add_xy_position 会生成一个名为 'groups' 的列表列，导致 write.csv 失败
  stat_export <- stat.test %>% select(-any_of(c("groups")))
  write.csv(stat_export, file.path(out_dir, paste0("Stats_", label, "_RefStyle.csv")), row.names = FALSE)
    
  # --- 绘图 ---
  my_cols <- c("Responder" = "#2E3192", "NonResponder" = "#E41A1C")
  
  p_box <- ggboxplot(
    df_prop, 
    x = "cell_type_merged", 
    y = "fraction",
    color = "Response", 
    palette = my_cols,
    add = "jitter", 
    add.params = list(size = 1.0, alpha = 0.6),
    width = 0.8,
    bxp.errorbar = TRUE
  ) +
    labs(
      y = "Relative frequency", 
      x = NULL,
      title = paste0("Relative abundance of intra-tumoral T-cells (", label, ")"),
      subtitle = "stratified for response"
    ) +
    theme_classic() +
    theme(
      plot.title = element_text(hjust = 0.5, face = "bold", size = 16),
      plot.subtitle = element_text(hjust = 0.5, size = 12),
      axis.text.x = element_text(angle = 90, vjust = 0.5, hjust = 1, size = 12, color = "black"),
      axis.text.y = element_text(size = 12, color = "black"),
      axis.title.y = element_text(size = 14, face = "bold"),
      legend.position = c(0.15, 0.9), 
      legend.background = element_blank(),
      legend.title = element_blank()
    )
  
  p_box <- p_box + stat_pvalue_manual(
    stat.test, 
    label = "p", 
    tip.length = 0.01,
    hide.ns = TRUE
  )
  
  ggsave(file.path(out_dir, paste0("Boxplot_RefStyle_", label, ".pdf")), p_box, width = 12, height = 6)
  print(p_box)
}

# 6. 使用你的正确路径运行
meta_tumor <- prepare_metadata(
  "analysis/TNK/06_cd8_fine_clustering.rds", 
  "analysis/TNK/06_cd4_fine_clustering.rds", 
  "analysis/TNK/06_nk_fine_clustering.rds",
  ids_tumor_cd8, drop_tumor_cd8, 
  ids_tumor_cd4, drop_tumor_cd4, 
  ids_nk_dummy, drop_nk_dummy, "Tumor"
)

# 运行绘图
plot_and_stats_t_cells_ref_style(meta_tumor, "Tumor")