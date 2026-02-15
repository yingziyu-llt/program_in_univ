# scripts/01_env_setup.R

check_pkg <- function(pkg) {
  if (requireNamespace(pkg, quietly = TRUE)) {
    ver <- packageVersion(pkg)
    cat(sprintf("[OK]   %-20s version: %s\n", pkg, ver))
    return(TRUE)
  } else {
    cat(sprintf("[FAIL] %-20s NOT installed!\n", pkg))
    return(FALSE)
  }
}

if (requireNamespace("NMF", quietly = TRUE) && packageVersion("NMF") >= "0.23.0") {
  message("NMF >= 0.23.0 is already installed.")
} else {
  message("Installing NMF from CRAN (compilation required)...")
  if (!require("BiocManager", quietly = TRUE)) install.packages("BiocManager", repos = "https://cloud.r-project.org")
  BiocManager::install("Biobase", update = FALSE, ask = FALSE)
  install.packages("NMF", repos = "https://cloud.r-project.org")
}

if (!requireNamespace("CellChat", quietly = TRUE)) {
  tryCatch({
    remotes::install_github("sqjin/CellChat", 
                            ref = "v1.5.0", 
                            upgrade = "never",
                            repos = "https://cloud.r-project.org")
  }, error = function(e) {
  })
} else {
  message("CellChat is installed.")
}

if (!requireNamespace("GenomeInfoDbData", quietly = TRUE)) {
  install.packages("GenomeInfoDbData", 
                   repos = "https://bioconductor.org/packages/3.14/data/annotation", 
                   type = "source")
}

check_and_load <- function(pkg) {
  cat(sprintf("Checking %-15s ... ", pkg))
  res <- try(library(pkg, character.only = TRUE), silent = TRUE)
  if (inherits(res, "try-error")) {
    cat("[FAIL]\n")
    message(attr(res, "condition")$message)
  } else {
    cat(sprintf("[OK] v%s\n", packageVersion(pkg)))
  }
}

pkgs <- c("Seurat", "harmony", "slingshot", "tradeSeq", 
          "CellChat", "infercnv", "survival", 
          "ComplexHeatmap", "NMF", "circlize", "ineq")

message("--- Loading Diagnostics ---")
invisible(lapply(pkgs, check_and_load))