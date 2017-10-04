#Mark Klick
#BMI550
#HW3
#USAGE: Given any MSA, the entropy and sum-of-pairs will be calculated 
import math
import re

with open('550test.txt') as f:
    #read in MSA, not empty strings
    align = [line.rstrip() for line in f if line.rstrip() != '']

##################
#General Column Entropy Function
##################
def ColumnEntropy(MSA):
    #assumption:MSA seqs have same # of columns. extract all columns in MSA
    cols = [''.join([MSA[j][i] for j in range(len(MSA))]) for i in range(len(MSA[0]))]
    #use regex to find probability of each AA in column
    probs = [((len(re.findall(cols[i][j],cols[i]))*1.0/len(cols[i])*1.0)) for i in range(len(cols))for j in range(len(cols[i]))]
    #subset the list to match probabilities with their respective indexes in the columns
    probs = [probs[i:i+len(cols[0])]for i in range(0,len(probs),len(cols[0]))]
    #my old incorrect method was this #entropys = [((len(re.findall(cols[i][j],cols[i]))*1.0/len(cols[i])*1.0)*math.log((len(re.findall(cols[i][j],cols[i]))*1.0/len(cols[i])*1.0),2)) for i in range(len(cols))for j in range(len(cols[i]))]
    #wanted the one liner but o well, better to be correct than pythonic
    #realized I was recording the probabilities incorrectly e.g. ACCC = 0.25 0.75 0 0 where i had ACCC = 0.25 0.75 0.75 0.75 . just have to fix by checking if i+1 character has been seen yet. 
    for i in range(len(probs)):
        for j in range(len(probs[i])):
            if j==len(probs[i])-1:
                break
            elif probs[i][j] == probs[i][j+1] and cols[i][j] == cols[i][j+1]:
                probs[i][j]=0
                probs[i][j+1]=0
                break
    #calculate p*math.log(p,2) for each AA's probability in each column
    for i in range(len(probs)):
        for j in range(len(probs[i])):
            p = float(probs[i][j])
            if p != 0:
                probs[i][j] = p*math.log(p,2)
    #apply the negated summation to the previously generated values i.e. -summ(p*math.log(p,2))
    Align_Entropy = [-sum(probs[i]) for i in range(len(probs))]
    #sum column entropys
    Align_Entropy = sum(Align_Entropy)
    return Align_Entropy
    
test = ['AAA','ACC','ACG','ACT']#check with example in lecture slides
print ColumnEntropy(test)
print ColumnEntropy(align)
col_ent = ColumnEntropy(align)

#BLOSUM62 file retrieved from ftp://ftp.ncbi.nih.gov/blast/matrices/BLOSUM62
#read in blosum matrix into a dictionary of dictionaries to represent the rows and columns of BLOSUM62 for easy lookup give AA x AA
with open('blosum62.txt') as f1:
    AAs = f1.readline().rstrip().split()#grab the AA column names
    blos_mat = {}#tried to use dict comprehension but couldn't get it to work out :/
    for line in f1:
        row = line.split()
        #grab AA from row
        AA = row.pop(0)#good ol c++ vector method forgot it worked on .py lists but perfect for this dict creation e.g.pop head off vector
        blos_mat[AA] = {}
        #dict with each column name as the key and value = the BLOSUM62 substitution score for that [row][col] AA x AA sub score
        for col in AAs:
            blos_mat[AA][col] = row.pop(0)

##################
#General SOP Function 
##################
def BLOSUM62_sop(MSA):            
    #quick re.sub to match blosum * = -
    MSA = [re.sub(r'-',r'*',MSA[i])for i in range(len(MSA))]
    zipper = []
    import itertools
    listt = list(itertools.combinations(range(len(MSA)),2))
    #zip unique combos of internal pairwise alignments for sum-of-pairs scoring
    for i in range(len(listt)):
        zipper.append(zip(MSA[listt[i][0]],MSA[listt[i][1]]))
    #look up BLOSUM scores in blos_mat for each AA pair in each pairwise alignment
    SOP_scores = [int(blos_mat[zipper[i][j][0]][zipper[i][j][1]])for i in range(len(zipper))for j in range(len(zipper[i]))]#you can easily split this list on alignment length if you wanted to store a seperate SP score for each internal pairwise alignment
    #sum up all the BLOSUM pairwise scores
    SP_score = sum(SOP_scores)
    return SP_score
    
#utilize general function that takes any MSA not just the one in 550test.txt
sp_score = BLOSUM62_sop(align)

with open('550hw3output.txt','w') as f1:
    f1.write('Column Entropy: '+str(col_ent)+' SP-score: '+str(sp_score))
