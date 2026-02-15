suppressPackageStartupMessages({
  library(Seurat)
  library(tidyverse)
  library(harmony)
  library(ggplot2)
  library(cowplot)
})

# Set random seed
set.seed(42)

# Output directory
out_dir <- "analysis/results_PBMC_final_annotation"
dir.create(out_dir, recursive = TRUE, showWarnings = FALSE)

# ==============================================================================
# 1. Helper Function: Load -> Annotate -> Clean -> Plot
# ==============================================================================
process_and_annotate <- function(file_path, label, rename_map, drop_idents) {
  message(sprintf("\n>>> Processing %s...", label))
  
  if (!file.exists(file_path)) stop(paste("File not found:", file_path))
  seu_obj <- readRDS(file_path)
  
  # 1. Rename IDs
  current_ids <- levels(Idents(seu_obj))
  valid_map <- rename_map[names(rename_map) %in% current_ids]
  
  if (length(valid_map) > 0) {
    seu_obj <- RenameIdents(seu_obj, valid_map)
  }
  
  # 2. Store new names in metadata
  seu_obj$cell_type_fine <- Idents(seu_obj)
  seu_obj$lineage <- label # Default lineage based on file origin
  
  # 3. Drop contamination clusters
  if (!is.null(drop_idents)) {
    to_remove <- drop_idents[drop_idents %in% levels(Idents(seu_obj))]
    if (length(to_remove) > 0) {
      message(sprintf("    Removing clusters: %s", paste(to_remove, collapse = ", ")))
      seu_obj <- subset(seu_obj, idents = to_remove, invert = TRUE)
    }
  }
  
  # 4. Plot individual object
  p <- DimPlot(seu_obj, label = TRUE, repel = TRUE, label.size = 3, raster = FALSE) + 
       ggtitle(paste0(label, " Annotated")) + 
       theme(legend.position = "bottom")
  
  pdf_file <- file.path(out_dir, paste0("UMAP_", label, "_Annotated.pdf"))
  ggsave(pdf_file, p, width = 8, height = 8)
  
  return(seu_obj)
}

# ==============================================================================
# 2. Define Annotation Maps
# ==============================================================================

# --- CD8 Mapping ---
ids_cd8 <- c(
  "0"  = "CD8_Temra",       
  "1"  = "gdT",             
  "2"  = "CD8_Naive",       
  "3"  = "CD8_Tem_GZMH",    
  "4"  = "MAIT",            
  "5"  = "CD4_CTL",         # 注意：这里命名为 CD4_CTL，稍后会修正其 Lineage
  "6"  = "CD8_Tcm",         
  "7"  = "CD8_Tem",         
  "8"  = "Doublet_HLA",     
  "9"  = "CD8_Tem",         
  "10" = "CD8_Naive"        
)
# 注意：drop_cd8 中去掉了 "CD4_CTL" (原 Cluster 5)，因为我们要保留它并归入 CD4
drop_cd8 <- c("Doublet_HLA") 

# --- CD4 Mapping ---
ids_cd4 <- c(
  "0"  = "CD4_Naive",            
  "1"  = "CD4_Tcm",              
  "2"  = "CD4_Naive",            
  "3"  = "CD4_Th17",             
  "4"  = "CD4_Treg",             
  "5"  = "CD4_Tem",              
  "6"  = "CD4_Th1",              
  "7"  = "LowQual",          
  "8"  = "Doublet_CD8",      
  "9"  = "CD4_Th2",              
  "10" = "CD4_CTL",              
  "11" = "Doublet_Myeloid",  
  "12" = "Doublet_CD8",      
  "13" = "T_ISG"             
)
drop_cd4 <- c("Doublet_CD8", "Doublet_Myeloid", "LowQual")

# --- NK Mapping ---
ids_nk <- c(
  "0" = "NK_Dim",           
  "1" = "NK_Dim",           
  "2" = "Doublet_T",        
  "3" = "NK_Bright",        
  "4" = "NK_Bright",        
  "5" = "NK_LowQual",       
  "6" = "NK_Dim",           
  "7" = "Doublet_T",        
  "8" = "NK_ISG",           
  "9" = "NK_Proliferating"  
)
drop_nk <- c("Doublet_T", "NK_LowQual")

# --- Proliferating Mapping ---
ids_prolif <- c(
  "0" = "Prolif_CD8",       
  "1" = "Prolif_NK_gdT",    
  "2" = "Prolif_CD4",       
  "3" = "LowQual",          
  "4" = "Prolif_T_Mix",     
  "5" = "Prolif_S_Phase",   
  "6" = "LowQual",          
  "7" = "Prolif_B",         
  "8" = "Prolif_G2M_Phase", 
  "9" = "Prolif_Myeloid"    
)
drop_prolif <- c("LowQual", "Doublet_B", "Doublet_Myeloid", "Prolif_G2M_Phase", "Prolif_S_Phase", "Prolif_T_Mix")

# ==============================================================================
# 3. Execute Annotation and Cleaning
# ==============================================================================

# 处理 CD8 (默认 lineage 为 "CD8")
seu_cd8 <- process_and_annotate("analysis/11_PBMC_cd8_fine_clustering.rds", "CD8", ids_cd8, drop_cd8)

# 【关键修改】修正 Cluster 5 (CD4_CTL) 的 lineage 为 "CD4"
message(">>> Correcting Lineage for misclassified CD4 cells in CD8 object...")
seu_cd8$lineage[seu_cd8$cell_type_fine == "CD4_CTL"] <- "CD4"

# 处理其他对象
seu_cd4 <- process_and_annotate("analysis/11_PBMC_cd4_fine_clustering.rds", "CD4", ids_cd4, drop_cd4)
seu_nk  <- process_and_annotate("analysis/11_PBMC_nk_fine_clustering.rds",  "NK",  ids_nk,  drop_nk)

# Process Proliferating Cells
prolif_file <- "analysis/11_PBMC_proliferating_fine_clustering.rds"
seu_prolif <- NULL
if(file.exists(prolif_file)) {
  seu_prolif <- process_and_annotate(prolif_file, "Proliferating", ids_prolif, drop_prolif)
}

# ==============================================================================
# 4. Merge and Re-run Global UMAP
# ==============================================================================
message("\n>>> Merging objects into Global PBMC T/NK map...")

# Build merge list
merge_list <- list(seu_cd4, seu_nk)
if (!is.null(seu_prolif)) {
  merge_list <- c(merge_list, list(seu_prolif))
}

# Merge
seu_global <- merge(seu_cd8, y = merge_list)

# Re-run Standard Process
message("    Normalizing and finding variable features...")
seu_global <- NormalizeData(seu_global)
seu_global <- FindVariableFeatures(seu_global, nfeatures = 2000)

# Remove TCR and Cell Cycle genes from variable features
tcr_genes <- grep("^TR[ABGD][VDJC]", rownames(seu_global), value = TRUE)
cc_genes <- c(Seurat::cc.genes$s.genes, Seurat::cc.genes$g2m.genes)
VariableFeatures(seu_global) <- setdiff(VariableFeatures(seu_global), c(tcr_genes, cc_genes))

message("    Scaling and PCA...")
if(!"S.Score" %in% colnames(seu_global@meta.data)) {
  seu_global <- CellCycleScoring(seu_global, s.features = cc.genes$s.genes, g2m.features = cc.genes$g2m.genes)
}
seu_global <- ScaleData(seu_global, vars.to.regress = c("S.Score", "G2M.Score"))
seu_global <- RunPCA(seu_global, npcs = 30, verbose = FALSE, seed.use = 42)

message("    Running Harmony (Global integration)...")
batch_var <- "orig.ident" 
for (c in c("Patient_ID", "patient_id", "sample_id")) {
  if (c %in% colnames(seu_global@meta.data)) {
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
# 5. Final Plots
# ==============================================================================
message("\n>>> Generating final plots...")

Idents(seu_global) <- "cell_type_fine"

# Plot 1: Fine Annotation
p_fine <- DimPlot(seu_global, reduction = "umap", label = TRUE, repel = TRUE, 
                  pt.size = 0.5, label.size = 3, raster = FALSE) + 
          ggtitle("PBMC Global T/NK (Fine Annotation)") +
          theme(legend.position = "bottom")

# Plot 2: Lineage (现在 CD4_CTL 会正确显示为 CD4 颜色)
p_lineage <- DimPlot(seu_global, reduction = "umap", group.by = "lineage", 
                     cols = c("CD8"="#377EB8", "CD4"="#4DAF4A", "NK"="#E41A1C", "Proliferating"="#984EA3"),
                     pt.size = 0.5, raster = FALSE) + 
             ggtitle("PBMC Global T/NK (Lineage)")

# Combine
p_final <- plot_grid(p_lineage, p_fine, ncol = 2, rel_widths = c(0.8, 1.2))

# Save
ggsave(file.path(out_dir, "PBMC_Global_TNK_Final_UMAP.pdf"), p_final, width = 18, height = 8)

# ==============================================================================
# 6. Save Final Object
# ==============================================================================
save_path <- "analysis/14_pbmc_global_annotated.rds"
message(sprintf(">>> Saving final object to %s...", save_path))
saveRDS(seu_global, save_path)

message("Done! PBMC Annotation pipeline complete.")