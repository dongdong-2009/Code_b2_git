#!/usr/local/bin/python

'''
Version      : 1.0
Author       : xia xinsong
Created      : 2013-03-20
Main Function:
   a. provide time based thread class    
   b. provide condition-based thread class
       
'''

import threading
import Queue
import time
import sys
import common_pub

myLogger = common_pub.getLogger() 

class QueueTask(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self._isterminate = False
        self._queue = Queue.Queue()
    
    def __del__(self):
        pass
    
    def put_action(self, actionObj):
        self._queue.put(actionObj)
        
    def terminate(self):
        self._isterminate = True
        
    def run(self):
        processActionCount = 0
        try:
            while not self._isterminate:
                actionObj = self._queue.get()
                actionObj.do_action()
                self._queue.task_done()
                processActionCount += 1
                #print "process action %d, %s, queue count = %d" % processActionCount, time.localtime(), self._queue.qsize()
        except Exception:
            self._isterminate = True
            print "uncaught exception: ", str(sys.exc_info())
            myLogger.exception( "uncaught exception: ", str(sys.exc_info()) )
        
class TimerThread(threading.Thread):
    def __init__(self, sleeptime, actionObject):
        threading.Thread.__init__(self) 
        self._sleeptime = sleeptime
        self._actionObject = actionObject
        self._isterminate = False
    
    def __del__(self):
        pass
    
    def terminate(self):
        self._isterminate = True
        
    def run(self):
        try:
            while not self._isterminate:
                self._actionObject.do_action()
                time.sleep(self._sleeptime)
        except Exception:
            self._isterminate = True
            print "uncaught exception: ", str(sys.exc_info())
            myLogger.exception( "uncaught exception: ", str(sys.exc_info()) )


class ConditionWorker(TimerThread):
    def __init__(self, sleeptime, actionObject):
        TimerThread.__init__(self,sleeptime,actionObject) 
        #self._sleeptime = sleeptime
        #self._actionObject = actionObject
        #self._isterminate = False
        self._condition = threading.Condition()

    def terminate(self):
        self._isterminate = True
        self.signal_()
        
    def signal_(self):
        self._condition.acquire()
        self._condition.notify()
        self._condition.release()
        
    def run(self):
        try:
            #signal.signal( signal.SIGTERM, signalHandler )
            while not self._isterminate:
                self._condition.acquire()
                self._actionObject.do_action()
                self._condition.wait(self._sleeptime)
                self._condition.release()
        except Exception:
            self._isterminate = True
            print "uncaught exception: ", str(sys.exc_info())
            myLogger.exception( "uncaught exception: ", str(sys.exc_info()) )
    
            
class BaseActionObject:
    def __init__(self):
        pass

    def __del__(self):
        pass
                   
    def do_action(self):
        print "do action"
        
class DumpAction(BaseActionObject):
    def __init__(self, uniquename, processId, otherInfoFileName, isBaseDump):
        self._uniquename = uniquename
        self._processid = processId
        self._otherInfoFileName = otherInfoFileName
        self._isBaseDump = isBaseDump 
        
    def do_action(self):
        common_pub.createDump(self._uniquename, self._processid, self._otherInfoFileName, self._isBaseDump)
        
                
if __name__ == "__main__":
    action_object = BaseActionObject()
    mythread = TimerThread(5, action_object)
    mythread.start()