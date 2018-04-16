#!/usr/local/bin/python

'''
Version      : 1.0
Author       : xia xinsong
Created      : 2013-03-20
Main Function:
   collect process related information, including PID, Memory, CPU, time, Command Parameter, Thread Count, File Count
   could dump core file based on memory, file Count, ThreadCount   
   
   Note: the data should be extracted further before analyzing
       
Revision     : 
   2013-03-25:  could dump based on memory limit
'''


import sys
import os
import re
import time
import worker_thread
import common_pub

NoNeedBaseDump = 1;

myLogger = common_pub.getLogger() 

MaxLogFileCount = 20
MaxLogFileSize = 1000*1000*10
WorkTimeInternal = 120
DumpBaseCoreTime = 60*30 #after monitor 30 minutes, will try to dump base core file as base line

FILE_HEADER = "\n\n###time,timestamp,usr,pid,ppid,nlwp,vmem,pmem,cpu,starttime,runtime,processtime,command,args,socket\n"

#process pattern
ProcessStatFileTraits = ["Agent","SystemController"] 

#set the alarm condition: dumpMaxCount, alarmTimeInternal(second), threadLimit, socketLimit, memoryLimit(M)
ProcessAlarmConditionDict = {
                              'OccNotificationServiceAgent':[5, 60*30, 5000,7000,5000],
                              'OccAlarmAgent':[5, 60*30, 5000,7000,5000]
                            }


ProcessStateOutputFileName = "./data/iscs_process_info.csv"
OtherDumpFileName = "./data/otherdumpinfo.log"

#command to retiver process info 
UcbPsCmd = '/usr/ucb/ps '
UcbPsCmdParam = '-axww' # PID,TT,S,TIME,COMMAND
NormalPsCmd = '/usr/bin/ps '
#                         0  , 1 , 2  , 3  ,4  , 5 , 6  , 7   ,  8 ,  9  , 10 
NormalPsCmdParam = '-efo user,pid,ppid,nlwp,vsz,rss,pcpu,stime,etime,time,comm'


#processor state key column
COLUMN_TIME_STR = 0
COLUMN_TIMESTAMP = 1
COLUMN_USER = 2
COLUMN_PID = 3
COLUMN_PPID = 4
COLUMN_LWP_COUNT = 5
COLUMN_VIRTUAL_MEM = 6
COULMN_PHY_MEM = 7
COLUMN_CPU_RATE = 8
COLUMN_START_TIME = 9
COLUMN_RUN_TIME = 10
COLUMN_VALID_PROCESS_TIME = 11
COLUMN_CMD = 12
COLUMN_ARGS = 13
COLUMN_PFILES = 14

MaxItemCount = 15

#should be sun os
if sys.platform.find( 'sunos' ) < 0:
    print "the script should be run on Solaris 9 or above!"
    sys.exit(-1)

class ProcessAlarmInfoStruct:
    '''it has following member:
       entityname, pid, lwpCountLimit, fileCountLimit, latestAlarmTimeStamp, currentAlarmCount, raisedAlarmCountLimit
    '''
    def __init__(self):
        self._uniquename = ""
        self._currentpid = 0
        self._lwpCountLimit = 0
        self._socketCountLimit = 0
        self._memoryLimit = 0
        self._latestAlarmTimeStamp = 0
        self._currentAlarmCount = 0
        self._raisedAlarmCountLimit = 0
        self._alarmTimeInternal = 0
        self._startMonitorTime = 0
        if  NoNeedBaseDump:
            self._dumpBaseCore = True
        else: 
            self._dumpBaseCore = False   


class ProcessesStatCollector(worker_thread.BaseActionObject): 
    '''collect specific process state according to name traits
       the name traits is case insensitive
    '''
    def __init__(self, nametraitsarray, queueTask = None):
        worker_thread.BaseActionObject.__init__(self)
        self._nametraitsarray = nametraitsarray
        self._outputfilename = ProcessStateOutputFileName
        self._queueTask = queueTask
        self._processAlarmInfoDict = {}
        self._logFileIndex = 1
        
        for processname in ProcessAlarmConditionDict.keys():
            tmpArray = ProcessAlarmConditionDict[processname]
            #print "tempArray: %d, %d, %d, %d" %( tmpArray[0], tmpArray[1], tmpArray[2], tmpArray[3] )
            assert(tmpArray is not None)
            processAlarmInfoStruct = ProcessAlarmInfoStruct()
            processAlarmInfoStruct._uniquename = processname
            processAlarmInfoStruct._raisedAlarmCountLimit = tmpArray[0]
            
            processAlarmInfoStruct._alarmTimeInternal = tmpArray[1]
            if processAlarmInfoStruct._alarmTimeInternal == 0:
                processAlarmInfoStruct._alarmTimeInternal = 60*2
                
            processAlarmInfoStruct._lwpCountLimit = tmpArray[2]
            processAlarmInfoStruct._socketCountLimit = tmpArray[3]
            processAlarmInfoStruct._memoryLimit = tmpArray[4]*1000
            
            processAlarmInfoStruct._startMonitorTime = time.time()
            processAlarmInfoStruct._latestAlarmTimeStamp = time.time()
            
            assert(not self._processAlarmInfoDict.has_key(processname))
            self._processAlarmInfoDict[processname] = processAlarmInfoStruct
        
        
        self._outfile = common_pub.openFile(self._outputfilename,'a+'); 

        if self._outfile is not None:
            self._outfile.write( FILE_HEADER)
            self._outfile.flush()
        else:
            sys.exit(-1)
    
    def __del__(self):
        if self._outfile is not None:
            self._outfile.flush()
            self._outfile.close()
    
    def output_process_info(self, psProcInfoDict):
        pids = psProcInfoDict.keys()
        pids.sort()
        for temppid in pids:
            subItemArray = psProcInfoDict[temppid]
            assert(subItemArray is not None) 
            line = common_pub.outputStringArray(subItemArray)
            line += "\n"
            #print line
            self._outfile.write(line)
             
        self._outfile.flush()
        
        if os.stat( self._outputfilename ).st_size >= MaxLogFileSize:
            self._outfile.close()
        
            #(fRet, is_zipfile_exist, currFileFullName) = common_pub.rotateFile("./data", "iscs_process_info", "csv", MaxLogFileCount, MaxLogFileSize)
            (fRet, is_zipfile_exist, currFileFullName, self._logFileIndex) = common_pub.autoExtendFile("./data", "iscs_process_info", "csv", MaxLogFileCount, MaxLogFileSize, self._logFileIndex)
            if not fRet:
                print "rotate iscs_process_info.csv failed" 
                myLogger.error( "rotate iscs_process_info.csv failed" )
            
            assert(currFileFullName == self._outputfilename)
            
            self._outfile = common_pub.openFile(self._outputfilename,'a+')
            if self._outfile is not None:
                self._outfile.write( FILE_HEADER)
         
        
    def is_needed_process(self, processcmd):
        #if trains empty, then always return true
        if len(self._nametraitsarray) == 0:
            return True
        
        for filetrait in self._nametraitsarray:
            lowerCmd = processcmd.lower()
            lowfiletrait = filetrait.lower()
            if lowfiletrait in lowerCmd:
                return True
        
        return False
    
    
    def collect_all_process_info(self):
        try:
            '''
            use UcbPsCmd and NormalPsCmd get process info
            '''
            ucbProcInfoList = os.popen( UcbPsCmd + UcbPsCmdParam ).readlines()
            psProcInfoList = os.popen( NormalPsCmd  + NormalPsCmdParam ).readlines()
            
            # parse PsCmd info
            psProcInfoDict = {}
            isFirstLine = True
            tempStampNow = time.time()
            timeStrNow = time.strftime("%Y-%m-%d %H:%M:%S")
            if len( psProcInfoList ) > 1:  #skip the first line
                for line in psProcInfoList:
                    line = line.strip()
                    if isFirstLine: #skip the first line
                        isFirstLine = False
                        continue
                    else:
                        subItemArray = []
                        subItemArray = re.split(r'\s+', line)  
                        temppid = int(subItemArray[1])
                        process_command = subItemArray[10]
                        #print line
                        #print process_command
                        if not self.is_needed_process(process_command):
                            continue
                        
                        subItemArray.insert(0,tempStampNow)
                        subItemArray.insert(0,timeStrNow)
                        
                        #append args
                        subItemArray.append("no arg")
                        
                        #append openFileCount
                        subItemArray.append(0)
                        
                        
                        psProcInfoDict[temppid] = subItemArray
            
            isFirstLine = True
            # /usr/ucb/ps can get full command line of process
            if len( ucbProcInfoList ) > 1:
                for line in ucbProcInfoList:
                    line = line.strip()
                    if isFirstLine: #skip the first line
                        isFirstLine = False
                        continue
                    ucbSubItemArray = re.split(r'\s+', line)
                    
                    # PID,TT,S,TIME,COMMAND
                    temppid = int(ucbSubItemArray[0])
                    tempArg = common_pub.outputStringArray(ucbSubItemArray[4:],' ')
                    #print temppid, tempArg
                    if psProcInfoDict.has_key(temppid):
                        subItemArray = psProcInfoDict[temppid]
                        subItemArray[COLUMN_ARGS] = tempArg
        

            self.checkProcessAlarmCondition(psProcInfoDict)
            self.output_process_info(psProcInfoDict)     
        except Exception:
            print "checkProcessAlarmCondition throw exception, reason is %s"  %str(sys.exc_info())  
            myLogger.exception("checkProcessAlarmCondition throw exception, reason is %s"  %str(sys.exc_info()) )  
            
           
        
    def checkProcessAlarmCondition(self, psProcInfoDict):
        assert (self._queueTask is not None)
            
        for pid in psProcInfoDict.keys():
            subItemArray = psProcInfoDict[pid]
            assert(len(subItemArray) == MaxItemCount) #so far not include socketcount
            commandParam = subItemArray[COLUMN_ARGS]
            nlwpCount = int( subItemArray[COLUMN_LWP_COUNT] )
            memoryCount = int ( subItemArray[COLUMN_VIRTUAL_MEM] )
            
            
            for uniquename in self._processAlarmInfoDict:
                if (uniquename in commandParam):
                    processAlarmInfo = self._processAlarmInfoDict[uniquename]
                    
                    if processAlarmInfo._currentpid != pid: #reset the alarm information
                        if  NoNeedBaseDump:
                            processAlarmInfo._dumpBaseCore = True
                        else: 
                            processAlarmInfo._dumpBaseCore = False   

                        #processAlarmInfo._dumpBaseCore = False
                        processAlarmInfo._startMonitorTime = time.time()
                        processAlarmInfo._currentpid = pid
                        processAlarmInfo._latestAlarmTimeStamp = time.time()
                        processAlarmInfo._currentAlarmCount = 0
                    else:
                        processAlarmInfo._currentpid = pid
                    
                    socketCount = common_pub.getSocketCount(pid)
                    #print "Socket: ", socketCount
                    subItemArray[COLUMN_PFILES] = socketCount
                    
                    shouldDoDump = False
                    
                    #print 5
                    
                    if processAlarmInfo._socketCountLimit > 0 and socketCount >= processAlarmInfo._socketCountLimit:
                        print "process %s(%d) need dump, socket count (%d) exceed the limit (%d) " %(uniquename, pid, socketCount, processAlarmInfo._socketCountLimit )
                        myLogger.info("process %s(%d) need dump, socket count (%d) exceed the limit (%d) " %(uniquename, pid, socketCount, processAlarmInfo._socketCountLimit ) ) 
                        shouldDoDump = True
                        
                    if processAlarmInfo._lwpCountLimit > 0 and processAlarmInfo._lwpCountLimit < nlwpCount:
                        print "process %s(%d) need dump, lwp count (%d) exceeed limit (%d)" %(uniquename, pid, nlwpCount, processAlarmInfo._lwpCountLimit)
                        myLogger.info("process %s(%d) need dump, lwp count (%d) exceeed limit (%d)" %(uniquename, pid, nlwpCount, processAlarmInfo._lwpCountLimit) )
                        shouldDoDump = True
                    
                    if processAlarmInfo._memoryLimit > 0 and processAlarmInfo._memoryLimit < memoryCount:
                        print "process %s(%d) need dump, memory (%d) exceed the limit (%d) " %(uniquename, pid, memoryCount, processAlarmInfo._memoryLimit )
                        myLogger.info("process %s(%d) need dump, memory (%d) exceed the limit (%d) " %(uniquename, pid, memoryCount, processAlarmInfo._memoryLimit ) )
                        shouldDoDump = True
                    
                    doBaseCoreDump = False
                    if  not processAlarmInfo._dumpBaseCore and time.time() > processAlarmInfo._startMonitorTime + DumpBaseCoreTime:
                        doBaseCoreDump = True
                        shouldDoDump = True
                    
                    if shouldDoDump:
                        if not self.canRaiseAlarm(processAlarmInfo):
                            continue
                        
                        dumpAction = worker_thread.DumpAction(uniquename, pid, OtherDumpFileName, doBaseCoreDump)
                        
                        #xinsong TODO: asynchronours or synchronous
                        #self._queueTask.put_action(dumpAction)
                        dumpAction.do_action()
                        
                        processAlarmInfo._latestAlarmTimeStamp = time.time()
                        
                        if doBaseCoreDump:
                            processAlarmInfo._dumpBaseCore = True
                        else:
                            processAlarmInfo._currentAlarmCount += 1
        
    def canRaiseAlarm(self, processAlarmInfo):
        nowTime = time.time()
        
        if processAlarmInfo._latestAlarmTimeStamp + processAlarmInfo._alarmTimeInternal > nowTime:
            return False
        
        if processAlarmInfo._raisedAlarmCountLimit == 0:
            return True
        if processAlarmInfo._raisedAlarmCountLimit > processAlarmInfo._currentAlarmCount:
            return True
        return False
            
    def do_action(self):
        try:
            self.collect_all_process_info()
        except:
            print "uncaught exception: ", str(sys.exc_info())
            myLogger.exception( "uncaught exception: ", str(sys.exc_info()) )               

def startProcessStatWorker():
    queueTask = worker_thread.QueueTask()
    prstat_action_object = ProcessesStatCollector(ProcessStatFileTraits, queueTask)
    mythread = worker_thread.TimerThread(WorkTimeInternal, prstat_action_object)
    
    print "start process state worker"
    mythread.start()
        
 
if __name__ == "__main__":
    startProcessStatWorker()