#!/bin/bash
# This script is a automatic tool for the svm training data file collection procedure
# It will use tfListener.py to collect the skeletal information to a file 
# And use preProcess.py to make the file appropriate for svm trainning use.
# pose list 
# #1 stand straight 
# #2 sitting 
# #3 bow 
# #4 stand with arm crossing 
# #5 drink water 
# #6 hand shaking 
# #7 wave goodbye

echo "============================= W E L C O M E =================================="

for i in 1 2 3 4 5 6 7 ;do

   echo "Hit \"Enter\" key to start training DATA COLLECTION,default training time is 3 secs"
   read 
   python featureExtractor.py train 3 > ../trainingSet/$i
   python util.py ../trainingSet/$i $i

   echo "$i training DATA COLLECTION complete "
   echo 

done

for i in 1 2 3 4 5 6 7 ;do
	cat ../trainingSet/"$i"_  >> ../trainingSet/training_set
done

echo "\"training_set\" file is available under \"../trainingSet\" directory for svm use"      