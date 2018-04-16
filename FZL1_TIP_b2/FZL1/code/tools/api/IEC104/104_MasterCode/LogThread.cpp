

#include"LogThread.h"


//initialize of static variables
	CMutex *LogThread::m_mutex= NULL;  //there is no initializing
	LogThread* LogThread::m_log =NULL;

//static variables are initialized at here
void LogThread::init()
{
	m_mutex =new esca::CMutex(false);

}


LogThread * LogThread::instance()
{
	if(m_log==NULL)
	{
		CLock lock(m_mutex);
		if(m_log == NULL)
			m_log = new LogThread();

	}
	return m_log;
}


LogThread::LogThread()
{
	
}
LogThread::~LogThread()
{
}

int LogThread::LogWrite(const unsigned int size,const char * buffer)
{
	CLogQThread::PostMessage(size, (void *)buffer);
	return 1;
}
//open the log recording file
void LogThread::openFile(char *fileName,int flag=0)
{
	//m_logFile.std::ofstream::open(fileName,std::ios::out);
	m_logFile.open(fileName,std::ios::out|std::ios::app);
	m_logFile<<std::endl<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
	m_logFile<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<std::endl;
}

//functions are handled by thread, can not read data and write in the log
#ifdef WIN32
   DWORD LogThread::Main()
#else
	b_ui64 LogThread::Main()
#endif
{
	CLogMessage msg(0,0);
	while(1)
	{
		GetMessage(msg);
		m_logFile<<(int)msg.m_year<<"年"<<(int)msg.m_mon<<"月"<<(int)msg.m_day<<"日";
		m_logFile<<(int)msg.m_hour<<"点"<<(int)msg.m_min<<"分"<<(int)msg.m_sec<<"秒"<<msg.m_msec<<" : ";
		for(int i=0;i<msg.m_len;i++)
			m_logFile<<msg.m_data[i];
		m_logFile.flush();
	}
	return 1;
}

void LogThread::printStringWithTime(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char buffer[512];
#ifdef _WIN32
	_vsnprintf(buffer, sizeof(buffer)-1, fmt, ap);
#else
	vsprintf(buffer, fmt, ap);
#endif
	va_end(ap);
	buffer[sizeof(buffer)-1] = '\0';
#ifdef _SHWTM
	//showSystemTime();
#endif
}

void  LogThread::showSystemTime(std::string &timeStr)
{
	b_ui8 m_year,m_mon,m_day,m_hour,m_min,m_sec;b_ui16 m_msec;
#ifdef WIN32
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	m_year = systime.wYear-208;
	m_mon = (b_ui8)systime.wMonth ;
	m_day  = (b_ui8)systime.wDay ;
	m_hour = (b_ui8)systime.wHour ;
	m_min = (b_ui8)systime.wMinute;
	m_sec = (b_ui8)systime.wSecond ;
	m_msec = systime.wMilliseconds;
#else
	time_t ltime;
	time( &ltime );
	struct tm *tcurrent;
	if(tcurrent = localtime(&ltime))
	{
		m_year = tcurrent->tm_year-100;
		m_mon = tcurrent->tm_mon+1;
		m_day  = tcurrent->tm_mday;
		m_hour = tcurrent->tm_hour;
		m_min = tcurrent->tm_min;
		m_sec = tcurrent->tm_sec;
	}
	struct timeval curTime;
	gettimeofday(&curTime,NULL);
	m_msec =(curTime.tv_usec/1000) ;
#endif
	char tmpStr[100];
	sprintf(tmpStr,"[%d-%dh:%dm:%ds:%d]",m_day,m_hour,m_min,m_sec,m_msec);
	timeStr = tmpStr;

}

b_ui32 LogThread::getThreadID()
{

    #ifdef WIN32

        return GetCurrentThreadId();

    #else

        return pthread_self();

    #endif

}

void LogThread::mylogRecord(int level,std::string &str)
{

	/*std::string timeString;
	this->showSystemTime(timeString);
	CLock lock(m_mutex);
	m_logFile<<timeString<<"<"<<getThreadID()<<">"<<str;
	m_logFile.flush();*/
	
	char tmp[100];
	sprintf(tmp,"<%d>  ",getThreadID());
	str=tmp+str;
	LogWrite(str.length(),str.c_str());
	
}


