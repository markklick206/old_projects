source("https://bioconductor.org/biocLite.R")
biocLite("TCGAbiolinks")
library(TCGAbiolinks)
library(SummarizedExperiment)
library(WGCNA)
library(flashClust)

# query <- GDCquery(project = "TCGA-SARC",
#                   data.category = "Transcriptome Profiling",
#                   data.type = "Gene Expression Quantification", 
#                   workflow.type = "HTSeq - Counts")
# 
# GDCdownload(query)
# data <- GDCprepare(query)

# use TCGA biolinks protocol for data download from GDC
query1 <- GDCquery(project = "TCGA-BRCA", 
                  data.category = "Gene expression",
                  data.type = "Gene expression quantification",
                  experimental.strategy = "RNA-Seq",
                  platform = "Illumina HiSeq",
                  file.type = "results", 
                  legacy = TRUE)
GDCdownload(query1)
data1 <- GDCprepare(query1)

# pull out TCGA barcodes trying to get tcgabiolinks to work

# selection of normal samples "NT"
samplesNT <- TCGAquery_SampleTypes(barcode = colnames(data1),
                                   typesample = c("NT"))
length(samplesNT)
# [1] 112

# selection of tumor samples "TP"
samplesTP <- TCGAquery_SampleTypes(barcode = colnames(data1), 
                                   typesample = c("TP"))
length(samplesTP)
# [1] 1093
# for no reason at all take a random subset to equal the same number of samples as the NT
# NOTE: I had a thought to build wgcna network with 1000 tumor samples then see how well cancer modules
# are preserved in the normal tissue -- not sure how much computation this requires at the moment.
# it seems like a good idea to use cancer network as control and normal as treatment.
samplesTP <- sample(samplesTP,length(samplesNT),replace = FALSE)

# grab the NT normal samples  
query_NT <- GDCquery(project = "TCGA-BRCA", 
                  data.category = "Gene expression",
                  data.type = "Gene expression quantification",
                  experimental.strategy = "RNA-Seq",
                  platform = "Illumina HiSeq",
                  file.type = "results",
                  barcode = samplesNT, 
                  legacy = TRUE)
GDCdownload(query_NT)
data_NT <- GDCprepare(query_NT)

# grab the TP tumor samples
query_TP <- GDCquery(project = "TCGA-BRCA", 
                     data.category = "Gene expression",
                     data.type = "Gene expression quantification",
                     experimental.strategy = "RNA-Seq",
                     platform = "Illumina HiSeq",
                     file.type = "results",
                     barcode = samplesTP, 
                     legacy = TRUE)
GDCdownload(query_TP)
data_TP <- GDCprepare(query_TP)

# %$^& why do people publish code that is broken 
# exprs_data_NT <- assay(data_NT,"raw_counts") # looked inside summarized_experiment and error 
# stemmed from an index error 

# access rnaseq exprs data directly, which is nice just took some debugging of R pckgs :)
# make gene(rows) sample(cols) expression matrix for NT and TP samples
exprs_data_NT <- assays(data_NT)[[1]]
exprs_data_TP <- assays(data_TP)[[1]]

####################################################

# normalize and perform quantile filtering of genes
exprs_data_NT_norm <- TCGAanalyze_Normalization(tabDF = exprs_data_NT, geneInfo =  geneInfo)
exprs_data_NT_norm_filt <- TCGAanalyze_Filtering(tabDF = exprs_data_NT_norm,
                                  method = "quantile", 
                                  qnt.cut =  0.25)

exprs_data_TP_norm <- TCGAanalyze_Normalization(tabDF = exprs_data_TP, geneInfo =  geneInfo)
exprs_data_TP_norm_filt <- TCGAanalyze_Filtering(tabDF = exprs_data_TP_norm,
                                                 method = "quantile", 
                                                 qnt.cut =  0.25)

####################################################
# pre-process following the WGCNA tutorial 1 protocol (female liver)
gsg_NT = goodSamplesGenes(t(exprs_data_NT_norm_filt), verbose = 3)
gsg_NT$allOK

gsg_TP = goodSamplesGenes(t(exprs_data_TP_norm_filt), verbose = 3)
gsg_TP$allOK

# check the samples for outliers
NT_sampleTree = hclust(dist(t(exprs_data_NT_norm_filt)), method = "average")
# Plot the sample tree
sizeGrWindow(12,9)
#pdf(file = "NT_sampleClustering.pdf", width = 12, height = 9)
par(cex = 0.6)
par(mar = c(0,4,2,0))
plot(NT_sampleTree, main = "Sample clustering to detect outliers", sub="", xlab="", cex.lab = 1.5,
     cex.axis = 1.5, cex.main = 2)
abline(h = 1700000, col = "red")
dev.off()

# remove the outlier sample using wgcna cut tree method based on the visually observed tree height
NT_clust = cutreeStatic(NT_sampleTree, cutHeight = 1700000, minSize = 10)
table(NT_clust)
# clust 1 contains the samples we want to keep.
NT_keepSamples = (NT_clust==1)
exprs_data_NT_norm_filt = exprs_data_NT_norm_filt[,NT_keepSamples]
# number of genes and samples
ncol(exprs_data_NT_norm_filt)#samples 110
nrow(exprs_data_NT_norm_filt)#genes 13202

# check the samples for outliers
TP_sampleTree = hclust(dist(t(exprs_data_TP_norm_filt)), method = "average")
# Plot the sample tree
sizeGrWindow(12,9)
#pdf(file = "TP_sampleClustering.pdf", width = 12, height = 9)
par(cex = 0.6)
par(mar = c(0,4,2,0))
plot(TP_sampleTree, main = "Sample clustering to detect outliers", sub="", xlab="", cex.lab = 1.5,
     cex.axis = 1.5, cex.main = 2)
abline(h = 2500000, col = "red")
dev.off()

# remove the outlier sample using wgcna cut tree method based on the visually observed tree height
TP_clust = cutreeStatic(TP_sampleTree, cutHeight = 2500000, minSize = 10)
table(TP_clust)
# clust 1 contains the samples we want to keep.
TP_keepSamples = (TP_clust==1)
exprs_data_TP_norm_filt = exprs_data_TP_norm_filt[,TP_keepSamples]
# number of genes and samples
ncol(exprs_data_TP_norm_filt)#samples 111
nrow(exprs_data_TP_norm_filt)#genes 13202

# subset to common genes shared between datasets
# NOTE: JMiller suggests doing this so that every row in the matrices corresponds to same gene
common_genes <- intersect(rownames(exprs_data_TP_norm_filt),rownames(exprs_data_NT_norm_filt))
exprs_data_NT_norm_filt <- exprs_data_NT_norm_filt[common_genes,]
exprs_data_TP_norm_filt <- exprs_data_TP_norm_filt[common_genes,]

####################################################

# back to wgcna tutorial for advice on choosing the soft-thresholding power

# Choose a set of soft-thresholding powers
powers = c(c(1:10), seq(from = 12, to=20, by=2))
# Call the network topology analysis function
NT_sft = pickSoftThreshold(t(exprs_data_NT_norm_filt), powerVector = powers, verbose = 5)
# Plot the results:
sizeGrWindow(9, 5)
par(mfrow = c(1,2))
cex1 = 0.9
# Scale-free topology fit index as a function of the soft-thresholding power
plot(NT_sft$fitIndices[,1], -sign(NT_sft$fitIndices[,3])*NT_sft$fitIndices[,2],
     xlab="Soft Threshold (power)",ylab="Scale Free Topology Model Fit,signed R^2",type="n",
     main = paste("Scale independence"));
text(NT_sft$fitIndices[,1], -sign(NT_sft$fitIndices[,3])*NT_sft$fitIndices[,2],
     labels=powers,cex=cex1,col="red");
# this line corresponds to using an R^2 cut-off of h
abline(h=0.90,col="red")
# Mean connectivity as a function of the soft-thresholding power
plot(NT_sft$fitIndices[,1], NT_sft$fitIndices[,5],
     xlab="Soft Threshold (power)",ylab="Mean Connectivity", type="n",
     main = paste("Mean connectivity"))
text(NT_sft$fitIndices[,1], NT_sft$fitIndices[,5], labels=powers, cex=cex1,col="red")

# Call the network topology analysis function
TP_sft = pickSoftThreshold(t(exprs_data_TP_norm_filt), powerVector = powers, verbose = 5)
# Plot the results:
sizeGrWindow(9, 5)
par(mfrow = c(1,2))
cex1 = 0.9
# Scale-free topology fit index as a function of the soft-thresholding power
plot(TP_sft$fitIndices[,1], -sign(TP_sft$fitIndices[,3])*TP_sft$fitIndices[,2],
     xlab="Soft Threshold (power)",ylab="Scale Free Topology Model Fit,signed R^2",type="n",
     main = paste("Scale independence"));
text(TP_sft$fitIndices[,1], -sign(TP_sft$fitIndices[,3])*TP_sft$fitIndices[,2],
     labels=powers,cex=cex1,col="red");
# this line corresponds to using an R^2 cut-off of h
abline(h=0.90,col="red")
# Mean connectivity as a function of the soft-thresholding power
plot(TP_sft$fitIndices[,1], TP_sft$fitIndices[,5],
     xlab="Soft Threshold (power)",ylab="Mean Connectivity", type="n",
     main = paste("Mean connectivity"))
text(TP_sft$fitIndices[,1], TP_sft$fitIndices[,5], labels=powers, cex=cex1,col="red")

# WGCNA FAQ suggests for over 60 samples a soft thresholding power of 12
# visual scale free topology model fit agrees with this choice for NT
# however, the network topology visual for TP samples suggests a soft thresholding power of 14 
NT_softpower <- 12
TP_softpower <- 14
########################################################

# JMiller method to assess comparability of the two data sets I've aquired
rankExpr_NT= rank(rowMeans(exprs_data_NT_norm_filt))
rankExpr_TP= rank(rowMeans(exprs_data_TP_norm_filt))
random5000= sample(common_genes,5000)
rankConn_NT= rank(softConnectivity(t(exprs_data_NT_norm_filt[random5000,]),type="signed",power=NT_softpower))
rankConn_TP= rank(softConnectivity(t(exprs_data_TP_norm_filt[random5000,]),type="signed",power=NT_softpower))

pdf("NT_vs_TP_generalNetworkProperties.pdf", height=10, width=9) 
par(mfrow=c(2,1))
verboseScatterplot(rankExpr_NT,rankExpr_TP, xlab="Ranked Expression NT",
                   ylab="Ranked Expression TP")
verboseScatterplot(rankConn_NT,rankConn_TP, xlab="Ranked Connectivity NT",
                   ylab="Ranked Connectivity TP")
dev.off() 

# Two things on the comparability of these two data sets: positive correlations and low p-values,
# p-values are better for expression than connectivity.
#cor=0.87, p<1e???200
#cor=0.66, p<1e???200

###############################################
WGCNAnThreads()
enableWGCNAThreads(nThreads = 6)#leave two for all my tabs open in chrome :P
#disableWGCNAThreads()

# run WGCNA!!!!!!
adjacency_NT = adjacency(t(exprs_data_NT_norm_filt),power=NT_softpower,type="signed")
diag(adjacency_NT)=0
dissTOM_NT = 1-TOMsimilarity(adjacency_NT, TOMType="signed")#10-15-20mins
geneTree_NT = flashClust(as.dist(dissTOM_NT), method="average")

adjacency_TP = adjacency(t(exprs_data_TP_norm_filt),power=NT_softpower,type="signed")
diag(adjacency_TP)=0
dissTOM_TP = 1-TOMsimilarity(adjacency_TP, TOMType="signed")#10-15-20mins
geneTree_TP = flashClust(as.dist(dissTOM_TP), method="average") 

##########################################################
# NOTE: SAVED IMAGE HERE TO THE_DATA_TCGA_BRCA_NT_vs_TP
##########################################################

# plot the NT and TP networks
pdf("NT_TP_dendrogram.pdf",height=6,width=16)
par(mfrow=c(1,2))
plot(geneTree_NT,xlab="",sub="",main="Gene clustering on TOM-based dissimilarity NT",
     labels=FALSE,hang=0.04)
plot(geneTree_TP,xlab="",sub="",main="Gene clustering on TOM-based dissimilarity TP",
     labels=FALSE,hang=0.04)
dev.off()

# this plot looks good because there are a lot of distinct branches. less in TP.

############################################

# determine modules in NT data set by visual displaying module sizes using different values for deepSplit 
# this will determine the module size i.e. lots of small modules or several big modules
# art or science thing right here! How do you know what you want? In this case with the 
# large number of genes 13k I'm looking for lots of small modules, especially when comparing the
# TP network! 

NT_mColorh=NULL
for (ds in 0:3){
  tree = cutreeHybrid(dendro = geneTree_NT, pamStage=FALSE,
                      minClusterSize = (30-3*ds), cutHeight = 0.99,
                      deepSplit = ds, distM = dissTOM_NT)
  NT_mColorh=cbind(NT_mColorh,labels2colors(tree$labels));
}
pdf("Module_choices.pdf", height=10,width=25);
plotDendroAndColors(geneTree_NT, NT_mColorh, paste("dpSplt =",0:3), main = "",dendroLabels=FALSE);
dev.off()
modules_NT = NT_mColorh[,4] # (Chosen based on plot below) 

# deepSplit 3 has a lot of small modules! 74 modules
# PCA for visualization. PCs1_NT holds all the module Eigengenes i.e. represents the highest percent of variance
# for all genes in a given module
PCs1_NT = moduleEigengenes(t(exprs_data_NT_norm_filt), colors=modules_NT)
ME_1_NT = PCs1_NT$eigengenes
distPC1_NT = 1-abs(cor(ME_1_NT,use="p"))
distPC1_NT = ifelse(is.na(distPC1_NT), 0, distPC1_NT)
pcTree1_NT = hclust(as.dist(distPC1_NT),method="a")
MDS_1_NT = cmdscale(as.dist(distPC1_NT),2)
colors_NT = names(table(modules_NT))
pdf("NT_ModuleEigengeneVisualizations.pdf",height=6,width=6)
par(mfrow=c(1,1), mar=c(0, 3, 1, 1) + 0.1, cex=1)
plot(pcTree1_NT, xlab="",ylab="",main="",sub="")
plot(MDS_1_NT, col= colors_NT, main="MDS plot", cex=2, pch=19)
ordergenes = geneTree_NT$order
plotMat(scale(log(exprs_data_NT_norm_filt[ordergenes,])) , rlabels= modules_NT[ordergenes], clabels=
           colnames(exprs_data_NT_norm_filt), rcols=modules_NT[ordergenes]) 
for (which.module in names(table(modules_NT))){
  ME = ME_1_NT[, paste("ME",which.module, sep="")]
  barplot(ME, col=which.module, main="", cex.main=2,
          ylab="eigengene expression",xlab="array sample")
}
dev.off()

# plot eigen gene network for NT
sizeGrWindow(5,7.5)
par(cex = 0.9)
plotEigengeneNetworks(ME_1_NT, "", marDendro = c(0,4,1,2), marHeatmap = c(3,4,1,2), cex.lab = 0.8, xLabelsAngle= 90)

# # determine modules based on the TP data 
# mColorh=NULL
# for (ds in 0:3){
#   tree = cutreeHybrid(dendro = geneTree_TP, pamStage=FALSE,
#                       minClusterSize = (30-3*ds), cutHeight = 0.99,
#                       deepSplit = ds, distM = dissTOM_NT)
#   mColorh=cbind(mColorh,labels2colors(tree$labels));
# }
# pdf("TP_Module_choices.pdf", height=10,width=25);
# plotDendroAndColors(geneTree_TP, mColorh, paste("dpSplt =",0:3), main = "",dendroLabels=FALSE);
# dev.off()
# modules_TP = mColorh[,4] # (Chosen based on plot below)

# # determine MEs in TP network
TP_mColorh=NULL
for (ds in 0:3){
  tree = cutreeHybrid(dendro = geneTree_TP, pamStage=FALSE,
                      minClusterSize = (30-3*ds), cutHeight = 0.99,
                      deepSplit = ds, distM = dissTOM_TP)
  TP_mColorh=cbind(TP_mColorh,labels2colors(tree$labels));
}
pdf("TP_Module_choices.pdf", height=10,width=25);
plotDendroAndColors(geneTree_TP, TP_mColorh, paste("dpSplt =",0:3), main = "",dendroLabels=FALSE);
dev.off()
modules_TP = TP_mColorh[,4] # (Chosen based on plot below) 

# lets pick deepSplit 3 again like NT data because I want lots of small modules#115
# PCA for visualization. PCs1_NT holds all the module Eigengenes i.e. represents the highest percent of variance
# for all genes in a given module
PCs1_TP = moduleEigengenes(t(exprs_data_TP_norm_filt), colors=modules_TP)
ME_1_TP = PCs1_TP$eigengenes
distPC1_TP = 1-abs(cor(ME_1_TP,use="p"))
distPC1_TP = ifelse(is.na(distPC1_TP), 0, distPC1_TP)
pcTree1_TP = hclust(as.dist(distPC1_TP),method="a")
MDS_1_TP = cmdscale(as.dist(distPC1_TP),2)
colors_TP = names(table(modules_TP))
pdf("TP_ModuleEigengeneVisualizations.pdf",height=6,width=6)
par(mfrow=c(1,1), mar=c(0, 3, 1, 1) + 0.1, cex=1)
plot(pcTree1_TP, xlab="",ylab="",main="",sub="")
plot(MDS_1_TP, col= colors_TP, main="MDS plot", cex=2, pch=19)
ordergenes_TP = geneTree_TP$order
plotMat(scale(log(exprs_data_TP_norm_filt[ordergenes_TP,])) , rlabels= modules_TP[ordergenes_TP], clabels=
           colnames(exprs_data_TP_norm_filt), rcols=modules_TP[ordergenes_TP]) 
for (which.module in names(table(modules_TP))){
  ME = ME_1_TP[, paste("ME",which.module, sep="")]
  barplot(ME, col=which.module, main="", cex.main=2,
          ylab="eigengene expression",xlab="array sample")
}
dev.off()

######################################################

# qualitative assessment of how well my modules in NT network are preserved in TP network
pdf("Final_modules.pdf",height=8,width=12)
plotDendroAndColors(geneTree_NT, modules_NT, "Modules", dendroLabels=FALSE, hang=0.03, addGuide=TRUE,
                    guideHang=0.05, main="Gene dendrogram and module colors NT")
plotDendroAndColors(geneTree_TP, modules_NT, "Modules", dendroLabels=FALSE, hang=0.03, addGuide=TRUE,
                    guideHang=0.05, main="Gene dendrogram and module colors TP")
dev.off() 

# NOTE: 'it is often not possible to see an obvious grouping of model labels in the second data set, 
# even if there is significant module preservation This function assesses how well a module in one study is preserved in another study using
# a number of strategies and (for the purposes of this tutorial) outputs a single Z-score summary '
# 'In general, the higher the value of "Zsummary.pres" the more preserved the module is between data sets:
# 5<Z<10 indicates moderate preservation, while Z>10 indicates high preservation'

# WGCNA modulePreservation 
multiExpr = list(NT=list(data=t(exprs_data_NT_norm_filt)),TP=list(data=t(exprs_data_TP_norm_filt)))
multiColor = list(NT = modules_NT)
mp=modulePreservation(multiExpr,multiColor,referenceNetworks=1,verbose=3,networkType="signed",
                      nPermutations=30,maxGoldModuleSize=100,maxModuleSize=400)
stats = mp$preservation$Z$ref.NT$inColumnsAlsoPresentIn.TP
stats[order(-stats[,2]),c(1:2)] 

##########################################################
# calculate KMEs (correlation btween each gene and ME) and use it to compare networks

geneModuleMembership1 = signedKME(t(exprs_data_NT_norm_filt), ME_1_NT)
colnames(geneModuleMembership1)=paste("PC",colors_NT,".cor",sep="");
MMPvalue1=corPvalueStudent(as.matrix(geneModuleMembership1),dim(exprs_data_NT_norm_filt)[[2]]);
colnames(MMPvalue1)=paste("PC",colors_NT,".pval",sep="");
Gene = rownames(exprs_data_NT_norm_filt)
kMEtable1 = cbind(Gene,Gene,modules_NT)
for (i in 1:length(colors_NT))
  kMEtable1 = cbind(kMEtable1, geneModuleMembership1[,i], MMPvalue1[,i])
colnames(kMEtable1)=c("PSID","Gene","Module",sort(c(colnames(geneModuleMembership1),
                                                    colnames(MMPvalue1))))
write.csv(kMEtable1,"kMEtable_NT.csv",row.names=FALSE)

# First calculate MEs for A2, since we haven't done that yet
PCs2A = moduleEigengenes(t(exprs_data_TP_norm_filt), colors=modules_NT)
ME_2A = PCs2A$eigengenes
geneModuleMembership2 = signedKME(t(exprs_data_TP_norm_filt), ME_2A)
colnames(geneModuleMembership1)=paste("PC",colors_NT,".cor",sep="");
MMPvalue2=corPvalueStudent(as.matrix(geneModuleMembership2),dim(exprs_data_TP_norm_filt)[[2]]);
colnames(MMPvalue2)=paste("PC",colors_NT,".pval",sep="");
kMEtable2 = cbind(Gene,Gene,modules_NT)
for (i in 1:length(colors_NT))
  kMEtable2 = cbind(kMEtable2, geneModuleMembership2[,i], MMPvalue2[,i])
colnames(kMEtable2)=colnames(kMEtable1)
write.csv(kMEtable2,"kMEtable_TP.csv",row.names=FALSE)

##########################################################
# plot the kme values 
#+ and - correlated genes
pdf("all_kMEtable_NT_vs_kMEtable_TP.pdf",height=8,width=8)
for (c in 1:length(colors_NT)){
  verboseScatterplot(geneModuleMembership2[,c],geneModuleMembership1[,c],main=colors_NT[c],
                     xlab="kME in TP",ylab="kME in NT")
}
dev.off()
# in module genes asseses hub gene conservation 'Hub genes are genes that show significant correlation with MEs 
# and high within-module connectivity'
pdf("inModule_kMEtable_TP_vs_kMEtable_NT.pdf",height=8,width=8)
for (c in 1:length(colors_NT)){
  inMod = modules_NT== colors_NT[c]
  verboseScatterplot(geneModuleMembership2[inMod,c],geneModuleMembership1[inMod,c],main=colors_NT[c],
                     xlab="kME in A2",ylab="kME in A1")
} 
dev.off()

############################################################
# determine hub genes 
topGenesKME = NULL
for (c in 1:length(colors_NT)){
  kMErank1 = rank(-geneModuleMembership1[,c])
  kMErank2 = rank(-geneModuleMembership2[,c])
  maxKMErank = rank(apply(cbind(kMErank1,kMErank2+.00001),1,max))
  topGenesKME = cbind(topGenesKME,Gene[maxKMErank<=10])
}
colnames(topGenesKME) = colors_NT
topGenesKME 

#######################################################
# export modules to visANT using the function provided by JMiller: connectivy.csv has genes in modules
# sorted by intramodule connectivity (k-in) and mean expression. 
# visantOverall.csv has pairwise gene topological overlap (TO) and correlation
source("tutorialFunctions.R")
# export NT modules 
for (co in colors_NT[colors_NT!="grey"])
  visantPrepOverall(modules_NT, co, t(exprs_data_NT_norm_filt), rownames(exprs_data_NT_norm_filt), 500, NT_softpower, TRUE)

# export TP modules
for (co in colors_NT[colors_NT!="grey"])
  visantPrepOverall(modules_NT, co, t(exprs_data_TP_norm_filt), rownames(exprs_data_TP_norm_filt), 500, NT_softpower, TRUE)

########################################################
# write the gene lists for the NT determined modules to gain some context via enrichment analysis (DAVID)
# or plug the module gene lists into ReactomeFIViz
folder = "geneLists_NT/"
for (c in colors_NT){
  fn = paste(folder, c, ".txt",sep="")
  write.geneList(Gene[modules_NT==c], fn)
}
write(Gene,paste(folder,"all.txt",sep="")) 

#########################################################
# if we wanted to look at expression levels of intra-modular hub genes and module eigengenes 
# between NT and TP. this might be of use for visuals (matches the)
boxplot(exprs_data_NT_norm_filt["BUB1",],main="BUB1 expression NT -", las=2, xlab="NT samples", ylab="")
boxplot(exprs_data_TP_norm_filt["BUB1",],main="BUB1 expression TP -", las=2, xlab="TP samples", ylab="")
boxplot(ME_1_NT[,"MEdarkgreen"],main="Green ME expr. NT -", las=2, xlab="NT samples", ylab="")
boxplot(ME_2A[,"MEdarkgreen"],main="Green ME expr. TP -", las=2, xlab="TP samples", ylab="") 

########################################################

# compare our two networks how I originally wanted to i.e. constructing different modules for NT and TP
modulesTP_new = matchModules(Gene, modules_NT, Gene, modules_TP) 
# user enrichment function compares modules btwn NT and TP and finds signficant overlap
enrichments_TP_NT = userListEnrichment(Gene,modules_TP,"kMEtable_NT.csv","NT","enrichment_of_TPmods_NTmods.csv")
enrichments_TP_NT$sigOverlaps

# plot the two network dendograms and see how the modules compare!
pdf("NT_TP_dendrogram_plots.pdf",height=5,width=15)
plotDendroAndColors(geneTree_NT, modules_NT, "NT", dendroLabels=FALSE, hang=0.03,
                    addGuide=TRUE, guideHang=0.05, main="NT dendrogram")
plotDendroAndColors(geneTree_TP, modulesTP_new, "TP_new", dendroLabels=FALSE, hang=0.03,
                    addGuide=TRUE, guideHang=0.05, main="TP dendrogram")
dev.off()

########################################################
# export the new TP network info to visant and lets do a few enrichment comparisons via DAVID
# export the TP modules 
for (co in colors_TP[colors_TP!="grey"])
  visantPrepOverall(modules_TP, co, t(exprs_data_TP_norm_filt), rownames(exprs_data_TP_norm_filt), 500, NT_softpower, TRUE)

# write the gene lists for the mapped TP determined modules to gain some context via enrichment analysis (DAVID)
# or plug the module gene lists into ReactomeFIViz
folder = "geneLists_TP/"
for (c in colors_TP){
  fn = paste(folder, c, ".txt",sep="")
  write.geneList(Gene[modules_TP==c], fn)
}
write(Gene,paste(folder,"all.txt",sep=""))