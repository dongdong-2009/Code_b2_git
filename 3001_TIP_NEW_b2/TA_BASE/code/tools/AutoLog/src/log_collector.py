#!/usr/local/bin/python

'''
Version      : 1.0
Author       : xia xinsong
Created      : 2013-03-20
Main Function:
   collect and compress ISCS agent logger automatically   
   
   Note: the lated log file could not zipped
       
'''

import os
import zipfile
import worker_thread
import common_pub

MaxZipFileSize = 1000*1000*20 #maximum size of zip file  is 10M
MaxZipFileCount = 20
WorkTimeInternal = 10


#LogFileTraitList = ["OccNotificationServiceAgent"]
LogFileTraitList = ["SystemController","OccNotificationServiceAgent", "OccAtsAgent"]

myLogger = common_pub.getLogger() 



class FileTraitStruct:
    pass

class LogCollector( worker_thread.BaseActionObject ):
    '''
    wrapper for ISCS agent log collector
    '''
    def __init__( self, logfiletraitlist = LogFileTraitList, logdir = '/u01/transactive/log', zipdir='./data' ):
        '''
        constructor
        '''
        worker_thread.BaseActionObject.__init__(self)
        self._logdir = logdir
        self._zipdir = zipdir
        self._fileTraitDict = {}
        assert(self._logdir != self._zipdir)
        
        assert(len(logfiletraitlist) > 0)
        
        for filetrait_name in logfiletraitlist:
            fileTraitStruct = FileTraitStruct()
            fileTraitStruct._name = filetrait_name
            fileTraitStruct._zippedFilelist = []
            fileTraitStruct._latestFileName = ""
            fileTraitStruct._latestFileTime = 0
            fileTraitStruct._archiveFileName = self._zipdir + "/" + filetrait_name + "_autocol.zip"
            fileTraitStruct._archiveFilePattern = filetrait_name + "_autocol"
            fileTraitStruct._zipFileIndex = 1;

             
            self._fileTraitDict[fileTraitStruct._name] = fileTraitStruct
            #delete old zip file after init
            if os.path.exists( fileTraitStruct._archiveFileName ):
                os.remove( fileTraitStruct._archiveFileName )
        
    def do_action( self ):
        #print "LogCollector do action"
        #print ".",
        for fileTraitName in self._fileTraitDict:

            fileTraitStruct = self._fileTraitDict[fileTraitName]
            assert(fileTraitStruct is not None)
             
            properFileList = common_pub.grepfiles( self._logdir, fileTraitName )
            
            if len( properFileList ) == 0:
                #myLogger.debug( "file trait (%s) do not have any log file" % fileTraitName )
                continue

            # get file list max modify time
            (fileTraitStruct._latestFileTime, fileTraitStruct._latestFileName) = common_pub.get_latest_file_info( properFileList )
            #print "latest file:  %s, %s"  %( str(fileTraitStruct._latestFileTime), fileTraitStruct._latestFileName )
           
            #remove the newest file
            newFileList = []
            properFileList.remove(fileTraitStruct._latestFileName)
            for fileName in properFileList:
                if fileName not in fileTraitStruct._zippedFilelist:
                    newFileList.append(fileName)
                
            fileTraitStruct._zippedFilelist += newFileList
            
            #print "properFileList: ", properFileList
            #print "fileTraitStruct._zippedFilelist", fileTraitStruct._zippedFilelist
            
            self.zipArchiveOldFiles( newFileList, fileTraitStruct )

        
    def zipArchiveOldFiles( self, filelist, fileTraitStruct):
        if len( filelist ) == 0:  
            #myLogger.debug( "fileList size = 0, do not backup file to archive file:%s" % archive_file_name )
            return
        
        #(fRet, is_zipfile_exist, currFileFullName) = common_pub.rotateFile("./data", archiveFilePattern, "zip", MaxZipFileCount, MaxZipFileSize)
        (fRet, is_zipfile_exist, currZipFileFullName, fileTraitStruct._zipFileIndex) = common_pub.autoExtendFile(self._zipdir, fileTraitStruct._archiveFilePattern, "zip", MaxZipFileCount, MaxZipFileSize, fileTraitStruct._zipFileIndex) 
        assert(currZipFileFullName == fileTraitStruct._archiveFileName)
        #print (fRet, is_zipfile_exist, currFileFullName)
        
        if not fRet:
            myLogger.exception("rotate zip file %s failed" %currZipFileFullName)
            
        try:
            if is_zipfile_exist:
                zipFile = zipfile.ZipFile( currZipFileFullName, 'a', zipfile.ZIP_DEFLATED ) #need not keep old zip content
            else:
                myLogger.info("create new zip file: %s" % currZipFileFullName)
                zipFile = zipfile.ZipFile( currZipFileFullName, 'w', zipfile.ZIP_DEFLATED )
        except Exception:
            print "open zip file %s failed, dup file flag = [%d]" %(currZipFileFullName, is_zipfile_exist)
            myLogger.exception("open zip file %s failed, dup file flag = [%d]" %(currZipFileFullName, is_zipfile_exist) )
          
        zipped_file_list = zipFile.namelist()
        
        #print "zipped file list: ", zipped_file_list
        
        new_zip_file_list = []
        
        for filefullName in filelist:
            trippedFileName = filefullName[1:]
            if not trippedFileName in zipped_file_list:
                new_zip_file_list.append( filefullName )
                #print "add file name: %s" %filefullName
            else:
                myLogger.warning("file %s has been zipped in archived file %s" %(filefullName, currZipFileFullName) )
        
        #print "new_zip_file_list: %s" %new_zip_file_list    
        for filename in new_zip_file_list:
            try:
                zipFile.write( filename )
                myLogger.info("file %s zip into archived file %s" %(filename, currZipFileFullName) )
            except Exception:
                print "Write file %s to archive file %s Failed" % (filename, currZipFileFullName )
                myLogger.exception( "Write file %s to archive file %s Failed" % (filename, currZipFileFullName ) )
    
        zipFile.close()

def startLogCollectionWorker():
    logCollector = LogCollector()
    #systemPerfDict = systemPerfMgr.collect_system_info()
    mythread = worker_thread.TimerThread(WorkTimeInternal, logCollector)
    
    print "start log file collector"
    mythread.start()
    
if __name__ == "__main__":
    
    startLogCollectionWorker()