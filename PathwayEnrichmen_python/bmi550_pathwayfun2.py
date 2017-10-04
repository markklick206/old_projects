# -*- coding: utf-8 -*-
"""
@author: mk
"""
from Bio import Entrez

## Provide your email address
email = "klick@ohsu.edu"
Entrez.email = email

#Get DE and Non-DE pathway genes
with open('TLR_genes.txt') as fh:
    DE_genes = fh.readline().split()
    NDE_genes = fh.readline().split()
fh.close()

#for each gene in the list, make a query string for each organism, loop through each query string for DE and nonDE genes
def Seq_Fetcher(gene,species):
    ## Submit a query
    #handle = Entrez.esearch(db="nuccore", term="CCL4[Gene] AND Homo Sapiens[Orgn] AND mRNA[Filter] AND Refseq[Filter]")
    handle = Entrez.esearch(db="nuccore", term=gene+"[Gene] AND "+species+"[Orgn] AND mRNA[Filter] AND Refseq[Filter]")
    ## Entrez.read() parses XML results
    ## A dictionary is returned
    record = Entrez.read(handle)
    ids = record["IdList"]
    print gene
    if ids:
        ## Use Entrez.efetch() to get a fasta record
        handle2 = Entrez.efetch(db="nuccore", id=ids[0], rettype="fasta", retmode="text")
        ## Here we use the handle's read() method, not Entrez.read(),
        ## since the retmode parameter is text, not XML
        fasta_records1 = handle2.read()
        return fasta_records1
    else:
        return None
        
##Get the sequences for DE pathway genes
DE_seqs_human = [Seq_Fetcher(DE_genes[i],"Homo sapiens") for i in range(len(DE_genes))]
DE_seqs_mouse = [Seq_Fetcher(DE_genes[i],"Mus musculus") for i in range(len(DE_genes))]
DE_seqs_zebrafish = [Seq_Fetcher(DE_genes[i],"Danio rerio") for i in range(len(DE_genes))]
#
##Get the sequences for non DE pathway genes
NDE_seqs_human = [Seq_Fetcher(NDE_genes[i],"Homo sapiens") for i in range(len(NDE_genes))]
NDE_seqs_mouse = [Seq_Fetcher(NDE_genes[i],"Mus musculus") for i in range(len(NDE_genes))]
NDE_seqs_zebrafish = [Seq_Fetcher(NDE_genes[i],"Danio rerio") for i in range(len(NDE_genes))]

#Save 3 seqs in Fasta file for clustalw
def write_fastas(human,mouse,zebrafish,gene_list):
    gene_idxs = []#keep track of 
    for i in range(len(gene_list)):#same indexes
        if(human[i]==None or mouse[i]==None or zebrafish[i]==None):
            continue#we want genes that have a sequence for each organism
        with open("%s.fasta"%gene_list[i], "w") as fh1:
            fh1.write(human[i]+mouse[i]+zebrafish[i])#write each species fasta to a file
        gene_idxs.append(i)
    return gene_idxs

#Write 3 species sequences for DE/NDE genes in pathway to fasta for clustalw mutli-alignment
DE_gene_idxs = write_fastas(DE_seqs_human,DE_seqs_mouse,DE_seqs_zebrafish,DE_genes)
NDE_gene_idxs = write_fastas(NDE_seqs_human,NDE_seqs_mouse,NDE_seqs_zebrafish,NDE_genes)
#idxs list hold indeces of which genes have sequences across all 3 organisms and will be included in clustalw 
#text fasta record retrieved using Entrez doesn't have a gene name field easy acessible, so just store indeces of original gene lists

#immport clustal command line argument
from Bio.Align.Applications import ClustalwCommandline

#clustal wrapper function 
def clustalw_em(gene):
    command = ClustalwCommandline("clustalw2", infile=gene+".fasta")
    print command
    command.dnamatrix = 'IUB'
    ## Run the Clustalw command
    stdout, stderr = command()
    
#align for each DE gene that all organisms had sequences for
for i in range(len(DE_gene_idxs)):
    clustalw_em(DE_genes[DE_gene_idxs[i]])
#align for each Non-DE gene that all organisms had sequences for
for i in range(len(NDE_gene_idxs)):
    clustalw_em(NDE_genes[NDE_gene_idxs[i]])

#read in clustal output multi-alignents and compute edit distance (levenstein distance)
from Bio import AlignIO
import itertools
import Levenshtein#usefull because my implementation had overhead

#function to wrap align.IO and score each alignment file    
def align_reader(gene):
    alignment = AlignIO.read(gene+".aln", "clustal")
    seq_list = [seq.seq for seq in alignment]
    #for seq in alignment:
        #print seq.seq
    #generate combos of pairwise sequence comparisons
    listt = list(itertools.combinations(range(len(seq_list)),2))
    x = []
    for i in range(len(listt)):#compute the levenshtein distance between sequences
        #https://pypi.python.org/pypi/python-Levenshtein/0.12.0#id7
        x.append(Levenshtein.distance(str(seq_list[listt[i][0]]),str(seq_list[listt[i][1]])))
    x = sum(x)/len(seq_list)#makes sense to calculate average distance across sequences in alignment
    return gene,x #return a tuple with gene name and alignment score  
    #return x#return gene scores for the 2 groups

##look up each gene's .aln file and score each DE gene alignemnt
DE_align_scores = [align_reader(DE_genes[DE_gene_idxs[i]]) for i in range(len(DE_gene_idxs))]
    
##score each NDE gene alignemnt
NDE_align_scores = [align_reader(NDE_genes[NDE_gene_idxs[i]]) for i in range(len(NDE_gene_idxs))]

#Compute a Mann-Whitney U rank test 
from scipy import stats
#only need scores for U test
x1 = [DE_align_scores[i][1] for i in range(len(DE_align_scores))]
x2 = [NDE_align_scores[i][1] for i in range(len(NDE_align_scores))]
u, p = stats.mannwhitneyu(x1,x2)
#print "DE_genes =", DE_align_scores
#print "NDE_genes =", NDE_align_scores
print "U =", u
print "P-value =", p
#make boxplot and save it
import matplotlib.pyplot as plt
fig = plt.figure()
box1 = plt.boxplot([x1,x2])
plt.title("TLR Pathway Conervation")
plt.ylabel("Conservation Score (Levenshtein)", fontsize=8)
plt.xlabel("DE genes vs Non DE Genes p-value:0.42719", fontsize=8)
plt.savefig('TLR_Pathway_boxplot.png')