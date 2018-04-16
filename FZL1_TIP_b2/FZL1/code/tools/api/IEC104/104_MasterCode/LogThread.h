/*@$***************************************************************************
* File Name        : LogThread.h
*
* the thread is used to record  log 
*******************************************************************************/

#ifndef __LOG_THREAD__H__
#define __LOG_THREAD__H__

#include "Threads.h"
using namespace esca;

#include<iostream>
#include<fstream>


/*
*class LogThread 
*this class is used to record log.
*Considering the big time comsuming of IO operation,we use one thread class to do
*Any data which need to be recorded  are put into the queue of this class
*When thread is running,there are some data in the queue, the thread need to write those data to the log file
*
*/
#ifdef _DEBUG
#define   debugPrint(fmt, ...)  LogThread ::instance()->printStringWithTime(fmt,__VA_ARGS__)
#else
#define   debugPrint(fmt, ...)   
#endif

#define _SHWTM

class LogThread : public esca::CLogQThread
{
public:
//return the only instance of this class
	static LogThread * instance();
	static void init();
	
	~LogThread();
//at the beginging of thread runing,it will run the Main() function
#ifdef WIN32
	virtual DWORD Main();
#else
	virtual b_ui64 Main();
#endif
//writing the data into the queue
	int LogWrite(const unsigned int size,const char * buffer);
	void  showSystemTime(std::string &timeStr);
	void  printStringWithTime(const char* fmt, ...);
	b_ui32 getThreadID();
//open the record file
	void openFile(char *fileName,int flag);
	void mylogRecord(int level,std::string &str);
private:
	
	LogThread();
	std::ofstream m_logFile;
	static esca::CMutex *m_mutex;
	static LogThread* m_log;

};

#endif