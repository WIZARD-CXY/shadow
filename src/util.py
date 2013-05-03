#!/usr/bin/python
#coding=utf-8

def preProcess(fileName,Label=""):
    """process the file to be svm-formated file """
    with open(fileName) as rf:
        with open(fileName+'_','w') as wf:
            content = rf.readline()
            while content:
                processedConetent = content.replace('[','').replace(']','').replace('(','').replace(')','').replace(", "," ")
                
                l=processedConetent.split(" ")

                i=1
                l2=[]
                for x in l:
                    l2.append(str(i)+":"+x)
                    i=i+1

                wf.writelines(Label+" "+(" ".join(l2)))
                content = rf.readline()

def scale(a,b,x):
    """scale the test set data using linear scaling"""
    return a*x+b

def convert(A):
    """covert original data fomat to online-svm-data format"""
    B=[]
    for i in range(7):
        for j in range(3):
            B.append(A[i][j])
    
    for i in range(4):
        B.append(A[7][i])
    for i in range(3):
        B.append(A[8][i])
    for i in range(4):
        B.append(A[9][i])

    return B

def findABfromFile():
    """find the coefficient (a and b ) of linear function 'y=ax+b' from training_set.range file"""
    """a and b will store every feature's linear coefficient"""
    a=[]
    b=[]
    fd=open("../svm/tools/training_set.range")
    #ignore first two line

    fd.readline()
    fd.readline()
    
    #start reading content from file
    content = fd.readline()
    while content:
        min = float(content.split(" ")[1])
        max = float(content.split(" ")[2])
        ai = 2/(max-min)
        bi = 1-max*ai
        a.append(ai)
        b.append(bi)
        content = fd.readline()

    return a,b

if __name__ == "__main__":

    import sys
    
    if len(sys.argv) == 1:
        print "USAGE: ",sys.argv[0]," fileName ","Label(can be omitted,default is None)"
        sys.exit(1)
    if len(sys.argv) == 2:
        preProcess(sys.argv[1])
        sys.exit(0)

    preProcess(sys.argv[1],sys.argv[2])