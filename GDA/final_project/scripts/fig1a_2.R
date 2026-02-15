library(Seurat)
library(tidyverse)
library(ggplot2)
library(viridis)

seu <- readRDS("analysis/01_tumor_integrated.rds")
Idents(seu) <- "seurat_clusters"

markers_paper <- list(
  "HCC (Normal-like)" = c("ALB", "HP", "FGA", "FGB"),
  "HCC (Tumor-assoc)" = c("AFP", "SPINK1", "GPC3"),
  "T Cells" = c("CD3D", "CD3E", "IL7R"),
  "NK Cells" = c("KLRF1", "KLRD1", "NKG7", "GNLY"),
  "B Cells" = c("CD79A", "MS4A1"),
  "Plasma Cells" = c("JCHAIN", "MZB1"),
  "Myeloid (Macro/Mono)" = c("LYZ", "CD68", "CST3"),
  "cDC" = c("FCER1A", "CD1C", "CLEC9A", "HLA-DQA1"),
  "pDC" = c("LILRA4", "CLEC4C", "TCL1A"),
  "Endothelial" = c("PECAM1", "VWF", "PLVAP"),
  "Fibroblasts" = c("ACTA2", "COL1A1", "DCN"),
  "Proliferation" = c("MKI67", "TOP2A")
)

genes_to_plot <- unique(unlist(markers_paper))

message("Reordering clusters based on extended markers...")
avg_exp <- AverageExpression(seu, features = genes_to_plot, assays = "RNA")$RNA
cluster_dist <- dist(t(avg_exp))
cluster_tree <- hclust(cluster_dist)
ordered_levels <- cluster_tree$labels[cluster_tree$order]
seu$seurat_clusters <- factor(seu$seurat_clusters, levels = ordered_levels)
Idents(seu) <- seu$seurat_clusters

p <- DotPlot(seu, features = markers_paper, dot.scale = 6) + 
  RotatedAxis() + 
  scale_color_gradientn(colours = c("white", "#FFE6E6", "#FF0000", "#8B0000"), # Reds 色系
                        values = c(0, 0.1, 0.6, 1)) +
  labs(title = "Major Cell Types (Inc. DC/pDC)", y = "Cluster ID", x = "") +
  theme(
    panel.background = element_rect(fill = "white", colour = "black", size = 1),
    panel.grid.major = element_line(colour = "grey95", linetype = "dashed"),
    axis.text.x = element_text(size = 10, face = "bold"),
    axis.text.y = element_text(size = 9),
    plot.background = element_rect(fill = "white")
  )
ggsave("figures/DotPlot_markers_paper_v2.pdf", p, width = 16, height = 10)
message("Saved to figures/DotPlot_markers_paper_v2.pdf")
library(patchwork)
message("Generating Feature Plots (UMAP) for all markers...")
pdf_file <- "figures/FeaturePlot_markers_Reds.pdf"

pdf(pdf_file, width = 12, height = 10) 
red_cols <- c("grey95", "#D73027") 

for (cell_type in names(markers_paper)) {
  genes <- markers_paper[[cell_type]]
  valid_genes <- genes[genes %in% rownames(seu)]
  if (length(valid_genes) > 0) {
    message(sprintf("  Plotting markers for: %s", cell_type))
    p <- FeaturePlot(seu, features = valid_genes, 
                     cols = red_cols, 
                     pt.size = 0.5, 
                     order = TRUE, 
                     raster = TRUE,
                     raster.dpi = c(300, 300),
                     ncol = 3)
    p_final <- p + plot_annotation(title = paste("Markers:", cell_type),
                                   theme = theme(plot.title = element_text(size = 16, face = "bold")))
    print(p_final)
  }
}

dev.off()

message(sprintf("Done! Compact PDF saved to %s", pdf_file))
cluster_assignments <- c(
  "28" = "HCC",
  "1" = "HCC",
  "13" = "HCC",
  "0" = "HCC",
  "21" = "HCC",
  "19" = "HCC",
  "25" = "HCC",
  "22" = "HCC",
  "29" = "T Cell",
  "10" = "T Cell",
  "2" = "T Cell",
  "31" = "Endothelial",
  "15" = "B cell",
  "26" = "pDC",
  "6" = "Endothelial",
  "5" = "T Cell",
  "27" = "T Cell",
  "8" = "Endothelial",
  "11" = "Plasma Cell",
  "14" = "Fibroblasts",
  "12" = "Myeloid",
  "4" = "Myeloid",
  "20" = "cDC",
  "18" = "Fibroblasts",
  "30" = "NK Cell",
  "3" = "NK Cell",
  "9" = "HCC",
  "7" = "Proliferation",
  "16" = "HCC",
  "17" = "HCC",
  "24" = "HCC",
  "23" = "HCC"
)

seu$seurat_clusters_numeric <- seu$seurat_clusters
seu <- RenameIdents(seu, cluster_assignments)
seu$cell_type_major <- Idents(seu)
p_anno <- DimPlot(seu, reduction = "umap", group.by = "cell_type_major", 
                  label = TRUE, label.size = 5, pt.size = 0.5) +
  ggtitle("Major Cell Types (Annotated)") +
  theme_void() +
  theme(plot.title = element_text(hjust = 0.5))
ggsave("figures/UMAP_annotated_major.png", p_anno, width = 10, height = 7)
p_check <- DotPlot(seu, features = markers_paper, group.by = "cell_type_major", dot.scale = 6) + 
  RotatedAxis() + 
  scale_color_gradientn(colours = c("white", "#FFE6E6", "#FF0000", "#8B0000"), values = c(0, 0.1, 0.6, 1)) +
  labs(title = "Annotation Check", y = "", x = "")

ggsave("figures/DotPlot_annotation_check.pdf", p_check, width = 14, height = 8)
message("Saving annotated object...")
saveRDS(seu, "analysis/02_tumor_annotated.rds")