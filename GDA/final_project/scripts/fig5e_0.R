suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(CellChat)
  library(patchwork)
})

# ==============================================================================
# 1. 加载数据
# ==============================================================================
# Myeloid (刚刚做好的)
seu_myeloid <- readRDS("analysis/08_myeloid_final_clean_reintegrated.rds")
# 确保 Idents 是细分注释
Idents(seu_myeloid) <- seu_myeloid$cell_type_fine

# Tumor T cells (您提供的路径)
file_t_tumor <- "analysis/TNK/18_tumor_final_merged.rds"
if(!file.exists(file_t_tumor)) stop("T细胞文件未找到: ", file_t_tumor)
seu_t <- readRDS(file_t_tumor)

# 假设 T 细胞的注释在 active.ident 或某个 meta 列，这里先统一一下
# 如果 T 细胞还没细分注释，您可能需要先确认 T 细胞的 Idents 是 "CD8 Tem", "CD8 Tex" 等
# 这里我们假设 seu_t 的 Idents 已经是最终注释
message("Myeloid 细胞数: ", ncol(seu_myeloid))
message("Tumor T 细胞数: ", ncol(seu_t))

# ==============================================================================
# 2. 数据合并 (Merge)
# ==============================================================================
# 为了避免 metadata 冲突，我们只保留核心列
cols_to_keep <- c("Patient_ID", "Response", "Treatment", "orig.ident")

# 检查列是否存在
common_cols <- intersect(colnames(seu_myeloid@meta.data), colnames(seu_t@meta.data))
message("共有的 Metadata 列: ", paste(common_cols, collapse=", "))

# 合并
# add.cell.ids 用于防止 barcode 重复
seu_combined <- merge(seu_myeloid, y = seu_t, add.cell.ids = c("Myeloid", "TNK"))

# 修复 Idents: merge 后 Idents 可能会丢失或变回 cluster ID
# 我们需要把 Myeloid 的 fine annotation 和 T 的 annotation 合并到一列
# 提取原始注释
anno_myeloid <- as.character(Idents(seu_myeloid))
names(anno_myeloid) <- colnames(seu_myeloid)

anno_t <- as.character(Idents(seu_t))
names(anno_t) <- colnames(seu_t)

# 在合并后的对象里重建注释
# 注意：merge 后 colnames 会加上前缀，需要对应
current_names <- colnames(seu_combined)
# 构造新的注释向量
new_idents <- c(paste0("Myeloid_", names(anno_myeloid)), paste0("TNK_", names(anno_t)))
# 这里有个 trick: merge 的顺序是 x then y, 且加了前缀
# 我们可以利用 add.cell.ids 自动加的前缀来匹配
cell_types <- rep(NA, ncol(seu_combined))
names(cell_types) <- colnames(seu_combined)

# 填入 Myeloid 注释
myeloid_cells <- grep("^Myeloid_", colnames(seu_combined), value = T)
# 去掉前缀匹配回原名
myeloid_orig_names <- gsub("^Myeloid_", "", myeloid_cells)
cell_types[myeloid_cells] <- anno_myeloid[myeloid_orig_names]

# 填入 T 注释
t_cells <- grep("^TNK_", colnames(seu_combined), value = T)
t_orig_names <- gsub("^TNK_", "", t_cells)
cell_types[t_cells] <- anno_t[t_orig_names]

# 赋值给 meta.data
seu_combined$cell_type_interaction <- cell_types
Idents(seu_combined) <- seu_combined$cell_type_interaction

message("合并完成！总细胞数: ", ncol(seu_combined))
table(Idents(seu_combined))

# ==============================================================================
# 3. 拆分 Responder / NonResponder (为 CellChat 准备)
# ==============================================================================
# 同样只保留 atezo+bev
target_tx <- grep("atezo.*bev", unique(seu_combined$Treatment), value = T, ignore.case = T)
if(length(target_tx) > 0) seu_combined <- subset(seu_combined, subset = Treatment %in% target_tx)

# 拆分
seu_resp <- subset(seu_combined, subset = Response == "Responder")
seu_nonresp <- subset(seu_combined, subset = Response == "NonResponder")

message("Responder 细胞数: ", ncol(seu_resp))
message("NonResponder 细胞数: ", ncol(seu_nonresp))

saveRDS(list(R = seu_resp, NR = seu_nonresp), "analysis/09_cellchat_input_objects.rds")