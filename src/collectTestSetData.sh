#!/bin/bash
# This shell script will automatically collect skeletal infomation of a human as a testset.
# It will use tfListener.py to get skeletal infomation to a file and preProcessed by preProcess.py
# in order to be used by svm module

echo "============================= W E L C O M E =================================="

if [ $# == 0 ];then
     echo "USAGE: $0 testfileName(must be meaningful)"
     exit
fi

python featureExtractor.py offline-test 3 > ../testSet/"$1"

# TO-DO Label the test file "0" or specific a number
python util.py ../testSet/"$1" 0

echo "\""$1"_\" is available under \"../testSet\" directory for use"