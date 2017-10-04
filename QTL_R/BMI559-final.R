#mk
setwd("c:/users/i7/documents/R")
library(DOQTL)

K = kinship.probs(founder.probs)

addcovar = matrix(pheno$Study)
rownames(addcovar) = rownames(pheno)
colnames(addcovar) = "Study"

qtl = scanone(pheno = pheno, pheno.col = "prop.bm.MN.RET", probs = founder.probs, 
              K = K, addcovar = addcovar, snps = muga_snps)

qtl.plot(qtl, main = "prop.bm.MN.RET")

perms = scanone.perm(pheno = pheno, pheno.col = "prop.bm.MN.RET", probs = founder.probs, 
                     K = K, addcovar = addcovar, snps = muga_snps, nperm = 1000)

thr = quantile(perms, c(0.95, 0.9, 0.37))
qtl.plot(qtl, sig.thr = thr, sig.col = c("red", "orange", "goldenrod"), main = "prop.bm.MN.RET")

#####Q3
#chromsome 10 contained peak
coef.plot(qtl, chr = 10, main = "prop.bm.MN.RET")

#####Q4
interval = bayesint(qtl$lod$A, chr = 10)

#####Q5
effect.plot(pheno = pheno, col = "prop.bm.MN.RET", founder.probs = founder.probs, 
            snp.id = interval[2, 1], snps = muga_snps)

#####Q6
cand.snps = variant.plot(var.file = "cc.snps.NCBI38.txt.gz", mgi.file = "MGI.20130703.sorted.txt.gz", 
                         chr = 10, start = interval[1, 3], end = interval[3, 3], pattern = "CAST/EiJ", 
                         qtl = qtl$lod$A[, c(2, 3, 7)])

#####Q7
cand.snps = variant.plot(var.file = "cc.indels.NCBI38.txt.gz", mgi.file = "MGI.20130703.sorted.txt.gz", 
                         chr = 10, start = interval[1, 3], end = interval[3, 3], type = "indel", 
                         pattern = "CAST/EiJ", qtl = qtl$lod$A[, c(2, 3, 7)])

#####Q8
cand.snps = variant.plot(var.file = "cc.sv.NCBI38.txt.gz", mgi.file = "MGI.20130703.sorted.txt.gz", 
                         chr = 10, start = interval[1, 3], end = interval[3, 3], type = "sv", pattern = "CAST/EiJ", 
                         qtl = qtl$lod$A[, c(2, 3, 7)])
