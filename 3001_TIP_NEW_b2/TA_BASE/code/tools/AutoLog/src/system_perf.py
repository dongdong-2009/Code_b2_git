#!/usr/local/bin/python

'''
Version      : 1.0
Author       : xia xinsong
Created      : 2013-03-20
Main Function:
   collect system overall information, including Memory, CPU, IO
   Note: the data should be extracted further before analyzing
       
Revision     : 

'''


import sys
import os
import time
import worker_thread
import common_pub

MaxLogFileCount = 20
MaxLogFileSize = 1000*1000*10
WorkTimeInternal = 120

myLogger = common_pub.getLogger() 

sys.path.append( sys.argv[0] )
__all__ = ["SystemStatCollector"]

FILE_HEADER = "\n\n###time,timestamp,vmstat,iostat,uptime\n"

ProcessStateOutputFileName = "./data/iscs_system_info.csv"

COLUMN_TIME_STR = 0
COLUMN_TIME_STAMP = 1
COLUMN_VMSTAT = 2
COLUMN_IOSTAT = 3
COLUMN_UPTIME_STAT = 4

#should be sun os
if sys.platform.find( 'sunos' ) < 0:
    print "the script should be run on Solaris 9 or above!"
    myLogger.error( "the script should be run on Solaris 9 or above!" )
    #sys.exit(-1)

class SystemPerfCollector(worker_thread.BaseActionObject):
    '''Collecct overall state of whole system
    '''
    def __init__(self):
        worker_thread.BaseActionObject.__init__(self)
        self._outputfilename = ProcessStateOutputFileName
        self._logFileIndex = 1
               
        self._outfile = common_pub.openFile(self._outputfilename,'a+')
        if self._outfile is not None:
            self._outfile.write(FILE_HEADER)
        else:
            sys.exit(-1)
    
    def __del__(self):
        if self._outfile is not None:
            self._outfile.flush()
            self._outfile.close()
    
    def getMemoryStat(self):
        vmstatCmd = "vmstat 1 1"
        alllines = os.popen( vmstatCmd ).readlines()
        for line in alllines[2:]:  # skip two line
            line = line.strip()
            if len(line) > 0:
                return line

    def getIOStat(self):
        iostatCmd = "iostat 1 1"
        alllines = os.popen( iostatCmd ).readlines()
        for line in alllines[2:]:  # skip two line
            line = line.strip()
            if len(line) > 0:
                return line;
    
    def getUptimeStat(self):
        uptimeCmd = "uptime"
        line = os.popen( uptimeCmd ).readline()
        line = line.strip()
        return line
                    
    def output_system_info(self, systemInfoItemArray):
        line = common_pub.outputStringArray(systemInfoItemArray)
        line += "\n"
        
        self._outfile.write(line) 
        self._outfile.flush() 
        
        if os.stat( self._outputfilename ).st_size >= MaxLogFileSize:
            self._outfile.close()
        
            #(fRet, is_zipfile_exist, currFileFullName) = common_pub.rotateFile("./data", "iscs_system_info", "csv", MaxLogFileCount, MaxLogFileSize)
            (fRet, is_zipfile_exist, currFileFullName, self._logFileIndex) = common_pub.autoExtendFile("./data", "iscs_system_info", "csv", MaxLogFileCount, MaxLogFileSize, self._logFileIndex)
            if not fRet:
                print "rotate iscs_system_info.csv failed" 
                myLogger.error( "rotate iscs_system_info.csv failed" )
            
            assert(currFileFullName == self._outputfilename)
            self._outfile = common_pub.openFile(self._outputfilename,'a+')
            if self._outfile is not None:
                self._outfile.write(FILE_HEADER)
                  
          
 
    def collect_system_info(self):
        tempStampNow = time.time()
        timeStrNow = time.strftime("%Y-%m-%d %H:%M:%S")
        subItemArray = [];
        subItemArray.append(timeStrNow)
        subItemArray.append(tempStampNow)

        line = self.getMemoryStat()
        subItemArray.append(line)
        
        line = self.getIOStat()
        subItemArray.append(line)
        
        #line = self.getUptimeStat()
        #subItemArray.append(line)
        
        self.output_system_info(subItemArray)
        
        
    def do_action(self):
        try:
            self.collect_system_info()
        except Exception:
            print "exception: %s" %str(sys.exc_info())
            myLogger.exception( "exception: %s" %str(sys.exc_info()) )
    

def startSystemPerfWorker():
    system_perf_action_object = SystemPerfCollector()
    #systemPerfDict = systemPerfMgr.collect_system_info()
    mythread = worker_thread.TimerThread(WorkTimeInternal, system_perf_action_object)
    print "start system performance worker"
    mythread.start()
         
if __name__ == "__main__":
    myLogger.info("start system perf")
    startSystemPerfWorker()