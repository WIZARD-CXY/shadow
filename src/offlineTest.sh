#!/bin/bash 
# This shell script will be used as automatically off-line test 

if [ $# == 0 ];then
     echo "USAGE: $0 testfileName(must exist (relative path),normally end with \"_\")"
     exit
fi

python ../svm/tools/checkdata.py "$1"
if [ $? != 0 ];then
	echo "testset data format error.Please check.Now EXIT"
	exit
fi

cd ../svm/
#Scaling testing data
./svm-scale -r tools/training_set.range ../testSet/"$1" > ../testSet/"$1".scale

#Test
./svm-predict ../testSet/"$1".scale tools/training_set.model ../testSet/"$1".predict