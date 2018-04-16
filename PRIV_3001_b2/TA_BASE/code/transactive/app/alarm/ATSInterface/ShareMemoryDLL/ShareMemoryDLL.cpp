// ShareMemoryDLL .cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
 
#include "ShareMemoryDLL.h"
 
#include "ISCSAlarmQueueItem.h"
#include "ATSAckQueueItem.h"
#include "ATSMmsQueueItem.h"
#include "ATSAlarm.h"
#include "ISCSAlarm.h"


// #include "ace/Log_Msg.h"
// #include "ace/streams.h"
// #include "ace/Logging_Strategy.h"
// #include "ace/Dynamic_Service.h"
// #include "ace/Service_Config.h"
// #include "ace/Log_Msg.h" 
// #include "ace/Time_Value.h" 
// #include "ace/OS.h" 
// #include "ace/Logging_Strategy.h" 
// #include "ace/ARGV.h" 
// #include "ace/Dynamic_Service.h" 
// #include "ace/Service_Config.h" 
// #include "ace/Reactor.h" 
// #include "ace/Task.h"

#ifdef ENABLE_LOG4CXX
	#include "log4cxx/logger.h"
	#include "log4cxx/PropertyConfigurator.h"
	#include "log4cxx/xml/domconfigurator.h"
	using namespace log4cxx;
	using namespace log4cxx::xml;
	using namespace log4cxx::helpers;  
#else
	#include "logger.h"  
	using namespace log4cxx;  
#endif

using namespace std;

#include "ATSAlarmInfoManager.h"
 
 

// const char *ISCS_QUEUE_ID = "iscs queue id";
// const char *ATS_MMS_QUEUE_ID = "ats mms queue id";
// const char *ATS_ACK_QUEUE_ID = "ats ack queue id";

// #define FIND_QUEUE_RETURN(QUEUE, ALLOCATOR, QUEUE_TYPE, QUEUE_ID) \
// 	if (NULL != ALLOCATOR) { \
// 		if (0 != ALLOCATOR->find(QUEUE_ID, QUEUE)) \
// 			QUEUE = QueryFactory<QUEUE_TYPE>::CreateQueue(pAlloc, QUEUE_ID);} 

IATSAlarm *g_pIATSAlarm = NULL;
IISCSAlarm *g_pISCSAlarm = NULL;

 

// ISCS_ALARM_QUEUE* g_pIscsAlarmQueue = NULL;
// ATS_MMS_QUEUE* g_pAtsMmsQueue = NULL;
// ATS_ACK_QUEUE* g_pAtsAckQueue = NULL;

// ISCS_ALARM_QUEUE* CShareMemoryDLL::m_pIscsAlarmQueue = NULL;
// ATS_MMS_QUEUE* CShareMemoryDLL::m_pAtsMmsQueue = NULL;
// ATS_ACK_QUEUE* CShareMemoryDLL::m_pAtsAckQueue = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			 //
			{
 
		 
			char modelName[200+1];
			char logName[200];
			DWORD dwSize = GetModuleFileName(NULL, modelName, 200);
			modelName[dwSize] = 0;
			std::string strAppName=modelName;
			std::string strAppPath=modelName;
			strAppName = strAppName.substr(strAppName.rfind("\\") + 1);
			strAppName = strAppName.substr(0,strAppName.rfind(".") ); 
			strAppPath = strAppPath.substr(0,strAppPath.rfind("\\") +1);
			string Property = strAppPath +strAppName + ".cfg";          
#ifdef ENABLE_LOG4CXX			 
			DOMConfigurator::configure(Property);			 
#endif
			LoggerPtr logger = Logger::getLogger("root");
			LOG4CXX_INFO(logger, (" --------------------start-----------------------------"));	 
			 
// 			 ACE_OSTREAM_TYPE *output = new ofstream (logName);
//  			 ACE_LOG_MSG->msg_ostream (output, 1);
// 			 ACE_LOG_MSG->set_flags (ACE_Log_Msg::OSTREAM);
// 			 ACE_LOG_MSG->clr_flags (ACE_Log_Msg::STDERR);
// 
//              ACE_LOG_MSG->priority_mask (LM_ERROR | LM_WARNING | LM_DEBUG |LM_INFO, ACE_Log_Msg::PROCESS);
// 			 ACE_Log_Msg::enable_debug_messages ();
// 			  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("%D, %M, [%t], Line:%l Start \n")));

// 			 ACE_Logging_Strategy *  log =  ACE_Dynamic_Service<ACE_Logging_Strategy>::instance("Logger"); 
// 			  
// 			 if (log == 0) 
// 			 { 
// 				 int result =  ACE_Service_Config::process_directive( 
// 					 ACE_DYNAMIC_SERVICE_DIRECTIVE("Logger", 
// 					 "ACE",              
// 					 "_make_ACE_Logging_Strategy", 
// 					 "") 
// 					 ); 
// 				 ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) result = %d.\n"),result)); 
// 			 } 
// 			 log = 
// 				 ACE_Dynamic_Service<ACE_Logging_Strategy>::instance("Logger"); 
// 
// 			 if (log) 
// 			 { 
// 				 ACE_ARGV args; 
// 				 args.add (__argv[0]); 
// 				 char cmdline[250] ={0}; 
// 				 sprintf(cmdline,"-s log.txt -f STDERR|OSTREAM -p DEBUG|ERROR"); 
// 				 args.add (ACE_TEXT (cmdline));     
// 				 log->init(args.argc(),args.argv()); 
// 			 } 
// 			 else 
// 			 { 
// // 				 ACE_Logging_Strategy logStrategy;
// // 				 ACE_ARGV args; 
// // 				 //args.add (__argv[0]); 
// // 				 char cmdline[250] ={0}; 
// // 				 sprintf(cmdline,"-s log.txt -f STDERR|OSTREAM -p DEBUG|ERROR"); 
// // 				 args.add (ACE_TEXT (cmdline)); 
// // 				 logStrategy.init(args.argc(),args.argv());
// 
// 			 }   
              //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) start here.\n"))); 

			  break;
			}
			
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			//CShareMemoryDLL::fini();
			//TA_Base_Core::Shmem::unmapSharedMem();
			break;
    }
    return TRUE;
}

// This is the constructor of a class that has been exported.
// see ShareMemoryDLL.h for the class definition
SHAREMEMORYDLL_API IATSAlarm*  getATSInterface()
{
	if(!g_pIATSAlarm)
	{
		g_pIATSAlarm = new CATSAlarm();
	}
	return g_pIATSAlarm;
}

SHAREMEMORYDLL_API IISCSAlarm*  getISCSInterface()
{
	if(!g_pISCSAlarm)
	{
		g_pISCSAlarm = new CISCSAlarm();
	}

	return g_pISCSAlarm;
}



//std::string CShareMemoryDLL::m_appName("");
CShareMemoryDLL::CShareMemoryDLL() 
{
	// init();
}

CShareMemoryDLL::~CShareMemoryDLL()
{
	// fini();	
}

void CShareMemoryDLL::init()
{
	/*
	HANDLE ShareMem = CreateMutex(NULL, FALSE, SHARE_MEM_MUTEX);
	WaitForSingleObject(ShareMem, INFINITE);
	
	TA_Base_Core::ALLOCATOR *pAlloc = TA_Base_Core::Shmem::getAllocator();

	FIND_QUEUE_RETURN( ((void *&)g_pIscsAlarmQueue), pAlloc, ISCSAlarmQueueItem, ISCS_QUEUE_ID);
 	FIND_QUEUE_RETURN( ((void *&)g_pAtsMmsQueue), pAlloc, ATSMmsQueueItem, ATS_MMS_QUEUE_ID);
 	FIND_QUEUE_RETURN( ((void *&)g_pAtsAckQueue), pAlloc, ATSAckQueueItem, ATS_ACK_QUEUE_ID);
	g_pIscsAlarmQueue->setACE_Allocator(pAlloc);
	g_pAtsMmsQueue->setACE_Allocator(pAlloc);
	g_pAtsAckQueue->setACE_Allocator(pAlloc);


 //	void *pBuf = TA_Base_Core::Shmem::alloc(sizeof(CATSAlarm));
 //	g_pIATSAlarm = new (pBuf) CATSAlarm(*m_pIscsAlarmQueue, *m_pAtsMmsQueue, *m_pAtsAckQueue);

 //	pBuf = TA_Base_Core::Shmem::alloc(sizeof(CISCSAlarm));
 //	g_pISCSAlarm = new (pBuf) CISCSAlarm(*m_pIscsAlarmQueue, *m_pAtsMmsQueue, *m_pAtsAckQueue);

	ReleaseMutex(ShareMem);
	CloseHandle(ShareMem);
	*/
}
// void CShareMemoryDLL::initNew()
// {
// 	/*
// 	HANDLE ShareMem = CreateMutex(NULL, FALSE, SHARE_MEM_MUTEX);	
// 	WaitForSingleObject(ShareMem, INFINITE);
// 
// 	TA_Base_Core::ALLOCATOR *pAlloc = TA_Base_Core::Shmem::getAllocator();
// 
// 	FIND_QUEUE_RETURN( ((void *&)g_pIscsAlarmQueue), pAlloc, ISCSAlarmQueueItem, ISCS_QUEUE_ID);
//  	FIND_QUEUE_RETURN( ((void *&)g_pAtsMmsQueue), pAlloc, ATSMmsQueueItem, ATS_MMS_QUEUE_ID);
//  	FIND_QUEUE_RETURN( ((void *&)g_pAtsAckQueue), pAlloc, ATSAckQueueItem, ATS_ACK_QUEUE_ID);
// 
// 
// // 	g_pIscsAlarmQueue->setACE_Allocator(pAlloc);
// // 	g_pAtsMmsQueue->setACE_Allocator(pAlloc);
// // 	g_pAtsAckQueue->setACE_Allocator(pAlloc);
// 
//  
// 
// 	ReleaseMutex(ShareMem);
// 	CloseHandle(ShareMem);
// 	*/
// }

// void CShareMemoryDLL::setAppName(std::string appName)
// {
// 	m_appName=appName.c_str();
// }
// 
// std::string CShareMemoryDLL::getAppName( )
// {
// 	return m_appName.c_str();
// }
void CShareMemoryDLL::fini()
{
	/*
	HANDLE ShareMem = CreateMutex(NULL, FALSE, SHARE_MEM_MUTEX);
	WaitForSingleObject(ShareMem, INFINITE);

	TA_Base_Core::Shmem::dealloc(g_pIATSAlarm);
	TA_Base_Core::Shmem::dealloc(g_pISCSAlarm);
	TA_Base_Core::Shmem::dealloc(m_pIscsAlarmQueue);
	TA_Base_Core::Shmem::dealloc(m_pAtsMmsQueue);
	TA_Base_Core::Shmem::dealloc(m_pAtsAckQueue);

	ReleaseMutex(ShareMem);
	CloseHandle(ShareMem);
	*/
}
