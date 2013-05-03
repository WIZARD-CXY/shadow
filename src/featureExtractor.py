#!/usr/bin/env python  
#coding=utf-8

import time
import roslib
import rospy
import tf
from myQueueWorker import myQueueWorker
import sys

joints = ['/head_1','/neck_1','/left_shoulder_1','/left_elbow_1','/left_hand_1','/right_shoulder_1','/right_elbow_1','/right_hand_1','/left_hip_1'
,'/right_hip_1']

if __name__ == '__main__':
    
    if len(sys.argv) != 3:
        print "USAGE: ",sys.argv[0],"train (training mode ) / test (test mode)" ,"3 (training/testing duration unit is sec)"
        sys.exit(1)
    
    rospy.init_node('tf_listener')

    listener = tf.TransformListener()
    rate = rospy.Rate(30.0)

    duration = int(sys.argv[2])
    
    if sys.argv[1] not in ["train","offline-test"]:
        print "===Start==="
        print 
    
    while True:
        if sys.argv[1] not in ["train","offline-test"]:
           input=raw_input("Hit \" label(1 to 7) + ENTER\" to start !\nOr \"q + ENTER\" to exit  ")
           """ online test jump out"""
           if input == 'q':
               break

        # Instantiate myQueue 
        FramesQueue = myQueueWorker(sys.argv[1])

        startTime = time.time()
        #Hand BoundingBox will record xmin,xmax,ymin,ymax,zmin,zmax of the hand movement 
        leftHandBoundingBox=[10,-10,10,-10,10,-10]
        rightHandBoundingBox=[10,-10,10,-10,10,-10]

        while not rospy.is_shutdown() and ((time.time()-startTime)<= duration):
            try:
                Frame=[]

                for joint in joints:

                    (trans,rot) = listener.lookupTransform('/torso_1', joint, rospy.Time(0))
                    
                    # add left_hip and right hip orientation feature
                    if joint in ["/left_hip_1","/right_hip_1"]:
                        Frame.append(trans)
                        Frame.append(rot)
                        continue

                    # for left hand and right hand ,we only record their bounding box at the end 
                    if joint == "/left_hand_1":
                        for i in xrange(3):
                            leftHandBoundingBox[2*i] = min(trans[i],leftHandBoundingBox[2*i])
                            leftHandBoundingBox[2*i+1] = max(trans[i],leftHandBoundingBox[2*i+1])
                        continue

                    if joint == "/right_hand_1":
                        for i in xrange(3):
                            rightHandBoundingBox[2*i] = min(trans[i],rightHandBoundingBox[2*i])
                            rightHandBoundingBox[2*i+1] = max(trans[i],rightHandBoundingBox[2*i+1])
                        continue

                    Frame.append(trans)
            
                # add to FrameQueue
                FramesQueue.append(Frame)
        
            except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException):
                continue

            rate.sleep()
        
        if sys.argv[1] in ["train","offline-test"]:
            for x in FramesQueue.q:
                x.append(leftHandBoundingBox)
                x.append(rightHandBoundingBox)
                print x
            break
        
        else:
            #Kill the other thread
            FramesQueue.setRunningFlag(False)
            #In the end append boundingBox to each Frame
            FramesQueue.setBoundingandScaling(leftHandBoundingBox,rightHandBoundingBox)
          
            FramesQueue.onlineTest(int(input))