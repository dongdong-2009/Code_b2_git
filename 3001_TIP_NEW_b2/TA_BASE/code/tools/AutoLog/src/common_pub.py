#!/usr/local/bin/python

'''
Version      : 1.0
Author       : xia xinsong
Created      : 2013-03-20
Main Function:
   provide public functions used by other modules

   Note: for necessary info, print it to stdin and log it in file
'''


import sys,os
import time
import re
import shutil
from subprocess import Popen, PIPE, STDOUT
import logging, logging.config
import threading

LoggerSingleton = None
LoggerLock = threading.Lock()


__all__ = ["getLogger", "splitLine", "stopLogger", "openFile", "outputStringArray", "grepfiles",  "gepFilesByTimeSort", "get_oldest_file_info", "get_latest_file_info",
           "autoExtendFile", "rotateFile", "getSocketCount", "createDump", "exccuteShellCommand","outputTimeHeaderToFile", "getValueByNamePattern"]

#example:  reBuild = re.compile( r'^\[Stack=(?P<stack>\d+)\]' )
#          retValue = getValueByPattern(reBuild, "[Stack=15]", "stack")

def getValueByNamePattern(reBuild, line, catptureName):
    matchObj = reBuild.match(line);
    if matchObj is None:
        return None
    
    matchValue = matchObj.group(catptureName)
    #print "match: ", matchValue
    return matchValue
    
def splitLine(line, pattern="\s+", maxSplit=0):
    p = re.compile(pattern)
    retList = p.split(line, maxSplit)
    return retList

def checkProcessExist(processname):
    strCount = 0
    try:
        cmd1 = Popen( ['/usr/bin/ps',  '-ef'], stdout = PIPE )
        #fileObj = TemporaryFile()
        cmd2 = Popen( ['grep', processname], stdin = cmd1.stdout, stdout = PIPE )
        cmd3 = Popen( ['grep', '-v', "grep"], stdin = cmd2.stdout, stdout = PIPE )
        cmd4 = Popen( ['wc', '-l'], stdin = cmd3.stdout, stdout = PIPE, stderr = PIPE )        
        (strCount, strErr) = cmd4.communicate()
    except Exception:
        print "checkProcessExist( %s ) failed, exception: %s" %(processname, str(sys.exc_info()) )
        myLogger.exception( "checkProcessExist( %s ) failed, exception: %s" %(processname, str(sys.exc_info()) ) )

    processCount = int(strCount)
    print "processCount: ", processCount
    assert(processCount >= 1) # include self instance
    if processCount > 1:
        return True
    else:
        return False
    
def getLogger():
    logging.config.fileConfig( "./pythonLogger.conf" )

    with LoggerLock:
        global LoggerSingleton
        if not LoggerSingleton:
            LoggerSingleton = logging.getLogger( "myLogger" )
            if LoggerSingleton is None:
                print "myLogger initialize failed!"
            else:
                LoggerSingleton.info( "\n\n------------------- log start -------------------" )
    return LoggerSingleton

def stopLogger():
    if LoggerSingleton:
        LoggerSingleton.info( "------------------- log finish -------------------" )
        logging.shutdown()

myLogger = getLogger()


def outputStringArray(strArray, seq=','):
    arrayLen = len(strArray)
    if arrayLen == 0:
        return ""
    
    index = 0;
    retStr = ""
    for item in strArray:
        retStr += str(item)
        index +=1
        if index < arrayLen:
            retStr += seq 
    
    return retStr    

def grepfiles( fileDir, filepattern, bRecursiveSearch = False ):
    """
    Get file list match file pattern in directory
    bRecursiveSearch = True will search subdirectory too
    """
    try:
        fileReg = re.compile(filepattern)
        #print "filedir: %s, pattern: %s" % (fileDir, filepattern)
        filelist = os.listdir( fileDir )
    except Exception:
        print "os.listdir failed: mydir is %s, exception is %s" %(fileDir, str( sys.exc_info() ) )
        myLogger.error( "os.listdir failed: mydir is %s, exception is %s" %(fileDir, str( sys.exc_info() ) ) )
        return []
    
    ret_file_list = []
    for filename in filelist:
        filefullname = fileDir + '/' + filename
        if os.path.isfile( filefullname ):
            matchobj = fileReg.search( filename )
            #print "matching filename: %s, full name is %s" %(filename, filefullname)
            if matchobj:
                ret_file_list.append( filefullname )
                #print "has been matched: filename: %s, full name is %s" %(filename, filefullname)
        elif os.path.isdir( filefullname ) and bRecursiveSearch:
            tmpfilelist = grepfiles( filefullname, filepattern, bRecursiveSearch )
            if len( tmpfilelist ) > 0:
                ret_file_list += tmpfilelist
    
    return ret_file_list

# if the value is -1, will move to the front
def cmpFileCretedTime(filename1, filename2):
    try:
        tmptime1 = os.stat( filename1 ).st_mtime
        tmptime2 = os.stat( filename2 ).st_mtime
        if (tmptime1 <= tmptime2):
            return -1
        else:
            return 1
    except OSError:
        print "compile file create time failed: %s <==> %s" % (filename1, filename2)
        myLogger.error( "compile file create time failed: %s <==> %s" % (filename1, filename2) )
    return -1            
 

def gepFilesByTimeSort(fileDir, filepattern, bRecursiveSearch = False):
    filelist = grepfiles(fileDir, filepattern, bRecursiveSearch)
    filelist.sort(cmpFileCretedTime, None, False)
    #print filelist
    return filelist
            
def get_oldest_file_info(filelist):
    min_mod_time = time.time()
    oldest_file_name = ""
    tmptime = 0
    
    for filename in filelist:
        try:
            if not os.path.exists( filename ):
                continue
            tmptime = os.stat( filename ).st_mtime
 
            if tmptime < min_mod_time:
                min_mod_time = tmptime
                oldest_file_name = filename
        except OSError:
            print "get file (%s) stat failed" % filename
            myLogger.error( "get file (%s) stat failed" % filename )
   
    return (min_mod_time, oldest_file_name)

def get_latest_file_info(filelist):
    max_mod_time = 0
    newest_file_name = ""
    tmptime = 0
    for filename in filelist:
        try:
            if not os.path.exists( filename ):
                continue
            tmptime = os.stat( filename ).st_mtime

            if tmptime > max_mod_time:
                max_mod_time = tmptime
                newest_file_name = filename
        except OSError:
            print "get file (%s) stat failed" % filename
    
    return (max_mod_time, newest_file_name)
  

def autoExtendFile(fileDir, fileNamePrefix, ext, maxFileNumer, maxFileSize, currFileIndex):
    is_file_exist = False
    curFileFullName = fileDir +"/" + fileNamePrefix + '.' + ext
    fRet = True
    try:
        if os.path.exists( curFileFullName ):
            is_file_exist = True
            if os.stat( curFileFullName ).st_size == 0:
                os.remove( curFileFullName )
                is_file_exist = False
                
            elif os.stat( curFileFullName ).st_size >= maxFileSize:
                renameFileName = fileDir + "/" + fileNamePrefix + "." + str(currFileIndex) + "." + ext
                if maxFileNumer > 0 and currFileIndex > maxFileNumer:
                    delFileIndex = currFileIndex - maxFileNumer
                    delFileName = fileDir + "/" + fileNamePrefix + "." + str(delFileIndex) + "." + ext
                    myLogger.info("reached to max file count %d, currentindex = %d, delete file %s" %(maxFileNumer, currFileIndex, delFileName) )
                    os.remove(delFileName)
                        
                #print "renameFileName: ", renameFileName
                myLogger.info("renaming current file <%s> to new file <%s>" %(curFileFullName, renameFileName) )
                shutil.move( curFileFullName, renameFileName )
                currFileIndex = currFileIndex+1
                is_file_exist = False 
    except IOError:
        if is_file_exist:
            try:
                os.remove( curFileFullName )
                is_file_exist = False
            except  Exception:
                print 'Remove archive file %s failed' % curFileFullName
                fRet = False
    except Exception:
        print "exception: %s" %str(sys.exc_info())
        myLogger.exception( "exception: %s" %str(sys.exc_info()) )
                    
    return (fRet, is_file_exist, curFileFullName, currFileIndex)

def rotateFile(fileDir, fileNamePrefix, ext, maxFileNumer, maxFileSize):
    is_file_exist = False
    curFileFullName = fileDir +"/" + fileNamePrefix + '.' + ext
    fRet = True
    try:
        if os.path.exists( curFileFullName ):
            is_file_exist = True
            if os.stat( curFileFullName ).st_size == 0:
                os.remove( curFileFullName )
                is_file_exist = False
                
            elif os.stat( curFileFullName ).st_size >= maxFileSize:
                oldfiles = grepfiles( fileDir, fileNamePrefix + "\.\d+" + "\." + ext ) #abc.1.zip  
                myLogger.info("file size is more larger, need rotate, old file list is: %s" %oldfiles)
                
                filecount = len( oldfiles )
                filecount += 1
                
                if maxFileNumer > 0 and filecount >= maxFileNumer:
                    (minModTime, oldestFileName ) = get_oldest_file_info(oldfiles)
                    #print "oldest file:  %s, %s"  %( str(minModTime), oldestFileName )

                    myLogger.info("reached to max file count %d, delete the file %s" %(maxFileNumer, oldestFileName) )
                    os.remove(oldestFileName)
                    renameFileName = oldestFileName
                else:
                    renameFileName = fileDir + "/" + fileNamePrefix + "." + str(filecount) + "." + ext

                #print "renameFileName: ", renameFileName
                myLogger.info("renaming current file <%s> to new file <%s>" %(curFileFullName, renameFileName) )
                shutil.move( curFileFullName, renameFileName )
                is_file_exist = False
                
    except IOError:
        if is_file_exist:
            try:
                os.remove( curFileFullName )
                is_file_exist = False
            except  Exception:
                print 'Remove archive file %s failed' % curFileFullName
                fRet = False
    return (fRet, is_file_exist, curFileFullName)
    
 
def getSocketCount(pid):
    try:
        #cmd1 = Popen( ['/usr/bin/pfiles',  str(pid)], stdout = PIPE )
        cmd1 = Popen( ['/usr/local/bin/lsof',  "-p", str(pid)], stdout = PIPE )
        #fileObj = TemporaryFile()
        cmd2 = Popen( ['wc', '-l'], stdin = cmd1.stdout, stdout = PIPE, stderr = PIPE )
        (strCount, strErr) = cmd2.communicate()
    except Exception:
        print "getSocketCount( %d ) failed, exception: %s" %(pid, str(sys.exc_info()) )
        myLogger.exception( "getSocketCount( %d ) failed, exception: %s" %(pid, str(sys.exc_info()) ) )
        return 0
    else:
        return int(strCount)
 

def openFile(fileName, flag):
    try:
        retFile = open(fileName,flag)
        
    except Exception:
            print "file <%s> open failed!" % fileName
            myLogger.exception("file <%s> open failed!" % fileName )
    
    return retFile    

def createDump( fileTrait, processId, otherDumpFileName, isbase=False ):
    time.sleep(2)
    outputTimeHeaderToFile(otherDumpFileName, "netstat all")
    exccuteShellCommand("netstat", "-na -f inet >>" + otherDumpFileName )

    time.sleep(2)
    outputTimeHeaderToFile(otherDumpFileName, "pfiles " + str(processId))
    exccuteShellCommand("pfiles", str(processId) + " >>" + otherDumpFileName )        
   
    curTime = time.localtime( time.time() )
    dumpFileName = "./data/%s.%04d%02d%02d%02d%02d" % ( fileTrait, \
        curTime.tm_year, curTime.tm_mon, curTime.tm_mday, \
        curTime.tm_hour, curTime.tm_min )
    
    if isbase:
        dumpFileName += ".basedmp"
    else:
        dumpFileName += ".dmp"
    
    args = "-o " + dumpFileName + " " + str(processId)
    
    time.sleep(2)
    exccuteShellCommand("gcore", args)
    
    time.sleep(1)
    exccuteShellCommand("gzip", dumpFileName+"."+str(processId))
    

def outputTimeHeaderToFile(filename, otherTitle):
    tempStampNow = time.time()
    timeStrNow = time.strftime("%Y-%m-%d %H:%M:%S")
    
    headerStr = "\n----------------- [" + otherTitle + "] " + timeStrNow + " (" + str(tempStampNow) + ")"   + " -----------------\n"
    fileObj = openFile(filename, "a+")
    if fileObj is not None:
        fileObj.write(headerStr)
        fileObj.close()     
    
def exccuteShellCommand(command, args):
    strLines = ""
    try:
        strLines = os.popen( command + " " + args ).readlines()
    except Exception:
        print "execute %s  %s failed, exception: %s" % ( command, args, str(sys.exc_info() ) )
        myLogger.exception( "execute %s  %s failed, exception: %s" % ( command, args, str(sys.exc_info() ) ) )
    
    myLogger.info( "execute %s  %s successful" % ( command, args) )
    return strLines
                    
if __name__ == "__main__":
    #myLogger.info("do nothing")
    #stopLogger()
    #getSocketCount(12131)
    #createDump("nsa_agnet", 12080 )
    #createDump("OccNotification", 12080, "./data/otherdumpinfo.log")
    #filelist = gepFilesByTimeSort("/u01/transactive/log", "memitem_output")
    #filelist = splitLine("1365910335   0  56263                       free:entry 0x1aa8df8")
    #print "filelist is: %s" %filelist
    
    #reBuild = re.compile( r'^\[Stack=(?P<stack>\d+)\]' )
    #retValue = getValueByNamePattern(reBuild, "[Stack=15]", "stack")
    print getSocketCount(27188)
