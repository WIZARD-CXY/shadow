#!/usr/bin/env python
#coding=utf-8

from collections import deque
import threading
import time
from svmutil import *
from util import *

#max deque size
MAX_QUEUE_SIZE =100

class myQueueWorker(threading.Thread):
    """a class derived from threading.Thread and use a thread-safe deque to store the every frame"""
    
    def __init__(self,type):
        threading.Thread.__init__(self)
        self.q=deque()
        self.Flag=True
        self.x=[]
        self.type=type
        self.start()
    
    """on-line convert the newest frame"""
    def run(self):
        if self.type not in ["train","offline-test"]:
            # do online Test
            while self.Flag:
                if len(self.q) > 0:
                    self.x.append(convert(self.q[0]))
                    time.sleep(0.034) #magic sleep time ,thread safe is guaranteed by dequeue
     
    """redefine the append method to get the deque's max size=MAX_QUEUE_SIZE"""
    def append(self,val):
        if len(self.q) < MAX_QUEUE_SIZE:
            self.q.appendleft(val)
        else:
            self.q.pop()
            self.q.appendleft(val)
    
    """extend the hand bounding box to every frame and scale"""
    def setBoundingandScaling(self,A,B):  
        a,b = findABfromFile()
        for features in self.x:
            features.extend(A)
            features.extend(B)

            for i in xrange(44):
                features[i]=scale(a[i],b[i],features[i])
    
    """use svm_predict to predict the unknown human motion"""
    def onlineTest(self,label): 
        m = svm_load_model("../svm/tools/training_set.model")
        p_label,p_acc,p_val = svm_predict([label]*len(self.x),self.x,m)
        print p_label
        print 
        print
    
    """ control the thread """    
    def setRunningFlag(self,Flag):
        self.Flag=Flag
