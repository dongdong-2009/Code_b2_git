#!/usr/local/bin/python

'''
Version      : 1.0
Author       : xia xinsong
Created      : 2013-03-20
Main Function:
    wrapped module about log collector, process state collector and system state collector
'''


import sys
import signal
import time
import process_stat
import system_perf
import log_collector
import common_pub

myLogger = common_pub.getLogger()

def signalHandler( sigNum, frame ):
    myLogger.info( "receive signal:%d " % sigNum )
    stopMain()

def stopMain():
    print "iscs server monitor script need stop"    
    pass

if __name__ == "__main__":
    try:
        signal.signal( signal.SIGTERM, signalHandler )
        
        currModuleName = "iscs_agent_monitor"
        #currModuleName = sys.modules[__name__]
        
        #print currModuleName
        
        processExist = common_pub.checkProcessExist(currModuleName)
        #print processExist
        
        if processExist:
            print "maybe another process (%s) has been started" %currModuleName
            #exit(0)
        
        system_perf.startSystemPerfWorker()
        process_stat.startProcessStatWorker()

        log_collector.startLogCollectionWorker()

        while True:
            time.sleep(60)
            
    except Exception:
        print "iscs server monitor script terminated, reason is %s"  %str(sys.exc_info())  
        myLogger.exception("iscs server monitor script terminated, reason is %s" %str(sys.exc_info()) )  

    except KeyboardInterrupt:
        print "iscs server monitor script terminated due to CTRL^C, reason is %s" %str(sys.exc_info())    
        myLogger.exception( "iscs server monitor script terminated due to CTRL^C, reason is %s" %str(sys.exc_info()) )  
    except:
        print "iscs server monitor script terminated due to unknown exception, reason is %s" %str(sys.exc_info())    
        myLogger.exception( "iscs server monitor script terminated due to unknown exception, reason is %s"%str(sys.exc_info()) )    

    else:
        stopMain()
                
    sys.exit()    
    