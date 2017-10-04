# -*- coding: utf-8 -*-
"""
@author: mk
"""
import csv

#FIXME: add command line arguments for file names 
import sys
#print sys.argv[1]

#f = open('H5N1_VN1203_DE_Probes.txt')
#f1 = open('H5N1_VN1203_UNIVERSE_Probes.txt')
#f2 = open ('KEGG_Pathway_Genes.txt')

f = open(sys.argv[1])
f1 = open(sys.argv[2])
f2 = open (sys.argv[3])
#initialize DE and Universe lists
DEs = list(csv.reader(f, delimiter='\t'))
Universe = list(csv.reader(f1, delimiter='\t'))

#Filter Unique Genes symbols for DE probes and Universe probes. Multiple probes map to 1 gene, and in this analysis we only want to know y/n if gene was DE or not.
DEs = list(set([DEs[i][1] for i in range(1,len(DEs))]))#skip column names
Universe = list(set([Universe[i][1] for i in range(1,len(Universe))]))
Universe1 = set.difference(set(Universe),set(DEs))#Universe in this case is Non-DE genes, so DE genes are removed

#Get Kegg Pathways
Kegg = list(csv.reader(f2,delimiter='\t'))
#Get each list of genes for the ith pathway
Kegg_members = [Kegg[i][2:] for i in range(1,len(Kegg))]#skip column names entry and only grab member genes([2:]) not ID or title

#Function to calculate OR for the ith pathway
def OR(pathway):
    #Calculate OR for the ith KEGG pathway
    DE_intx = len(set.intersection(set(DEs),set(pathway)))#A
    DE_dif = len(set.difference(set(DEs),set(pathway)))#C
    Universe_intx = len(set.intersection(set(Universe1),set(pathway)))#B
    Universe_dif = len(set.difference(set(Universe1),set(pathway)))#D
    #use formula provided for Odds Ratio. done both ways to make sure.
    OR = (float(DE_intx) / float(Universe_intx)) / (float(DE_dif) / float(Universe_dif))
    OR = (float(DE_intx) * float(Universe_dif)) / (float(DE_dif) * float(Universe_intx))
    #return a tuple with OR , intersecting DE genes within the pathway, non-DE genes within the pathway
    DE_pathway_genes = list(set.intersection(set(DEs),set(pathway)))
    Non_DE_pathway_genes = list(set.difference(set(pathway),set(DEs)))
    return OR,DE_pathway_genes,Non_DE_pathway_genes
    
#compute odds ratio for list of KEGG pathways. also keep track of DE genes within ith pathways and non-DE genes  
OR_list = [OR(Kegg_members[i]) for i in range(len(Kegg_members))]
OR_threshold = 1.5

#Get the KEGG IDs + Titles of pathways that pass OR threshold filtering (i+1 becuase initially skipped [0] b/c column names)
#Also retrieve list of intersecting DE_genes and list of non-DE genes within that pathway 
Pathways1 = [(Kegg[i+1][0:2],OR_list[i][1],OR_list[i][2]) for i in range(len(OR_list)) if OR_list[i][0] >= OR_threshold]

#Toll-like Receptor Signalling receptor Pathway
TLR_path = Pathways1[5]

#write the DE genes and Non-DE genes on two seperate lines
with open('TLR_genes.txt','w') as f:
    f.writelines(' '.join(TLR_path[1]) + '\n' + ' '.join(TLR_path[2]))
