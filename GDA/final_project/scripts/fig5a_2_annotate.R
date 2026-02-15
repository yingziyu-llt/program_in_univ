suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(harmony)
  library(ggplot2)
})

# ==============================================================================
# 0. 定义增强版整合函数 (包含黑名单剔除 & 细胞周期回归)
# ==============================================================================
process_subset <- function(seu_obj, label, dims = 30, res = 0.6) {
  message(sprintf(">>> Processing %s (%d cells)...", label, ncol(seu_obj)))
  
  # 1. Normalize
  seu_obj <- NormalizeData(seu_obj)
  seu_obj <- FindVariableFeatures(seu_obj, nfeatures = 2000)
  
  # 2. 细胞周期评分
  # 确保 cc.genes 加载 (Seurat 内置)
  if(!exists("cc.genes")) data(cc.genes)
  s_genes <- cc.genes$s.genes
  g2m_genes <- cc.genes$g2m.genes
  seu_obj <- CellCycleScoring(seu_obj, s.features = s_genes, g2m.features = g2m_genes, set.ident = FALSE)
  
  # 3. 剔除黑名单基因 (干扰聚类的噪音)
  var_feats <- VariableFeatures(seu_obj)
  
  # 定义黑名单
  liver_genes <- c("ALB", "AFP", "TTR", "APOA1", "APOA2", "APOB", "HP", "FGA", "FGB", "CPS1")
  hb_genes <- grep("^HB[^(P)]", rownames(seu_obj), value = TRUE)
  ig_genes <- grep("^IG[HKL]", rownames(seu_obj), value = TRUE)
  tcr_genes <- grep("^TR[ABGD]", rownames(seu_obj), value = TRUE)
  mt_genes <- grep("^MT-", rownames(seu_obj), value = TRUE)
  rp_genes <- grep("^RP[SL]", rownames(seu_obj), value = TRUE)
  
  blacklist <- unique(c(liver_genes, hb_genes, ig_genes, tcr_genes, mt_genes, rp_genes))
  
  # 计算剔除数量
  removed_genes <- intersect(var_feats, blacklist)
  message("    剔除干扰高变基因数量: ", length(removed_genes))
  if(length(removed_genes) > 0) message("    (例如: ", paste(head(removed_genes, 3), collapse=","), "...)")
  
  # 更新高变基因
  var_feats_clean <- setdiff(var_feats, blacklist)
  # 同时移除细胞周期基因，避免它们直接驱动聚类
  var_feats_clean <- setdiff(var_feats_clean, c(s_genes, g2m_genes))
  VariableFeatures(seu_obj) <- var_feats_clean
  
  # 4. Scale & Regress (关键步骤：回归掉细胞周期和背景噪音)
  message("    Scaling & Regressing (CC scores, %mt, nCount)...")
  seu_obj <- ScaleData(seu_obj, vars.to.regress = c("S.Score", "G2M.Score", "percent.mt", "nCount_RNA"))
  
  # 5. PCA
  seu_obj <- RunPCA(seu_obj, npcs = dims, verbose = FALSE)
  
  # 6. Harmony Integration
  batch_var <- "Patient_ID" 
  # 容错检查
  if(!batch_var %in% colnames(seu_obj@meta.data)) {
    warning(sprintf("'%s' not found, trying 'orig.ident'", batch_var))
    batch_var <- "orig.ident"
  }
  
  message(sprintf("    Running Harmony on '%s'...", batch_var))
  seu_obj <- RunHarmony(seu_obj, group.by.vars = batch_var, dims.use = 1:dims, plot_convergence = FALSE)
  
  # 7. UMAP & Clustering
  # 增大 n.neighbors 有助于拉近 Mono 和 Mac 的距离，防止裂开
  message("    Running UMAP...")
  seu_obj <- RunUMAP(seu_obj, reduction = "harmony", dims = 1:dims, 
                     n.neighbors = 40, min.dist = 0.3, seed.use = 42)
  
  # 注意：这里虽然跑了聚类，但我们后续主要用之前的注释画图
  seu_obj <- FindNeighbors(seu_obj, reduction = "harmony", dims = 1:dims)
  seu_obj <- FindClusters(seu_obj, resolution = res, random.seed = 42)
  
  return(seu_obj)
}

# ==============================================================================
# 1. 读取原始数据 & 应用注释
# ==============================================================================
target_file <- "analysis/06_mono_mac_fine_clustering.rds" 

if(file.exists(target_file)) {
  seu_obj <- readRDS(target_file)
  message("数据加载成功！原始细胞数: ", ncol(seu_obj))
} else {
  stop("未找到文件: ", target_file)
}

# 定义注释映射 (Cluster ID -> Cell Type)
new_cluster_ids <- c(
  "4"  = "Classical Mono",       
  "5"  = "Non-classical Mono",   
  "6"  = "Intermediate Mono",    
  
  "9"  = "Macro CXCL10",         
  "14" = "Macro CXCL10",         
  "18" = "Macro CXCL10",         
  "12" = "Macro CXCL10",         
  
  "7"  = "Macro SPP1",           
  "10" = "Macro SPP1",           
  
  "3"  = "Macro Suppressive",    
  "13" = "Macro Suppressive",    
  
  "2"  = "Macro FOLR2",          
  "8"  = "Kupffer Cells",        
  
  "1"  = "TAM C1QA",             
  
  "16" = "TAM CCL3+",            
  "17" = "TAM CCL3+",            
  
  "15" = "Stressed Mac (MT+)",   
  
  # 待剔除
  "0"  = "Macrophage LowQ",   
  "11" = "Hepatocyte Doublet"    
)

# 应用注释并保存到 meta.data
seu_obj <- RenameIdents(seu_obj, new_cluster_ids)
seu_obj$cell_type_fine <- Idents(seu_obj)

# ==============================================================================
# 2. 清洗数据 (Subset)
# ==============================================================================
seu_clean <- subset(seu_obj, idents = "Hepatocyte Doublet", invert = TRUE)
message("剔除 Doublet 后细胞数: ", ncol(seu_clean))

# ==============================================================================
# 3. 重新整合 (Re-integration)
# ==============================================================================
# 这一步会更新 UMAP 坐标，修复裂痕
seu_clean <- process_subset(seu_clean, "Myeloid_Cleaned", dims = 30, res = 0.6)

# process_subset 会重置 Idents 为数字 Cluster，我们需要恢复为细胞类型标签
Idents(seu_clean) <- seu_clean$cell_type_fine

# ==============================================================================
# 4. 绘图准备
# ==============================================================================
# 设置绘图顺序
my_levels <- c(
  "Classical Mono", "Intermediate Mono", "Non-classical Mono",
  "Macro CXCL10", "Macro SPP1", "Macro Suppressive", "Macro FOLR2",
  "Kupffer Cells",
  "TAM C1QA", "TAM CCL3+", "Stressed Mac (MT+)", "Macrophage LowQ"
)

# 确保只保留数据中存在的 levels
existing_levels <- intersect(my_levels, unique(Idents(seu_clean)))
Idents(seu_clean) <- factor(Idents(seu_clean), levels = existing_levels)

# 定义配色
my_cols <- c(
  "Classical Mono" = "#1f77b4", "Intermediate Mono" = "#aec7e8", "Non-classical Mono" = "#ff7f0e",
  "Macro CXCL10" = "#d62728", # 红色强调关键群
  "Macro SPP1" = "#9467bd", 
  "Macro Suppressive" = "#8c564b",
  "Macro FOLR2" = "#e377c2", 
  "Kupffer Cells" = "#bcbd22",
  "TAM C1QA" = "#7f7f7f", 
  "TAM CCL3+" = "#17becf",
  "Stressed Mac (MT+)" = "#c7c7c7",
  "Macrophage LowQ" = "lightgrey"
)

# ==============================================================================
# 5. 生成最终图表
# ==============================================================================
# UMAP
p_umap <- DimPlot(seu_clean, reduction = "umap", 
                  label = TRUE, label.size = 4, repel = TRUE, 
                  cols = my_cols, raster = FALSE) + 
          ggtitle("Annotated Myeloid Clusters (Re-integrated)") +
          theme(plot.title = element_text(hjust = 0.5))

# DotPlot
plot_features <- c(
  "S100A8", "VCAN",           # Classical
  "LST1", "FCGR3A",           # Non-classical
  "CXCL10", "CXCL9",          # Macro CXCL10
  "SPP1", "FABP4",            # Macro SPP1
  "GPNMB", "TREM2", "CCL18",  # Macro Suppressive
  "FOLR2", "MRC1",            # Macro FOLR2
  "CD5L", "MARCO",            # Kupffer
  "C1QA", "HLA-DQA1",         # TAM C1QA
  "CCL3", "CCL4"              # TAM CCL3+
)
# 确保基因存在
plot_features <- plot_features[plot_features %in% rownames(seu_clean)]

p_dot <- DotPlot(seu_clean, features = plot_features, 
                 cols = c("lightgrey", "#b30000"), 
                 dot.scale = 6) + 
         RotatedAxis() +
         theme(axis.text.x = element_text(angle = 45, hjust = 1)) +
         labs(x = "Marker Genes", y = "Cell Type")

# ==============================================================================
# 6. 保存结果
# ==============================================================================
out_dir <- "figures"
if(!dir.exists(out_dir)) dir.create(out_dir, recursive = TRUE)

ggsave(file.path(out_dir, "UMAP_Myeloid_Final_Reintegrated.pdf"), p_umap, width = 10, height = 7)
ggsave(file.path(out_dir, "DotPlot_Myeloid_Final_Reintegrated.pdf"), p_dot, width = 12, height = 6)

saveRDS(seu_clean, "analysis/08_myeloid_final_clean_reintegrated.rds")

message("全部完成！新整合的图表已保存至 analysis/figures。")