#!/bin/bash
# This shell script will using easy.py to simply get the svm model
# first is to do is to check trainingset file format is valid whether or not.
# second is do the data scaling and then do parameter selection

python ../svm/tools/checkdata.py ../trainingSet/training_set

if [ $? != 0 ];then
	echo "trainingset data format error.Please check.Now EXIT"
	exit
fi

cd ../svm/tools/ && python easy.py ../../trainingSet/training_set 
