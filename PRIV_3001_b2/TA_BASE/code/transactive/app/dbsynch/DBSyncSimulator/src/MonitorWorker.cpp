#include "MonitorWorker.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "ConnectionActorReleaser.h"


NS_BEGIN(TA_DB_Sync)


using  TA_Base_Core::DebugUtil;

ReEntrantThreadLockable CMonitorWorker::m_singletonLock;
CMonitorWorker* CMonitorWorker::m_theClassInstance = NULL;

CMonitorWorker* CMonitorWorker::getInstance()
{
	TA_Base_Core::ThreadGuard guard(m_singletonLock);
	// If guard acquired then make sure the singleton is still NULL
	if ( m_theClassInstance == NULL )
	{
		// Create the one & only object
		m_theClassInstance = new CMonitorWorker();
		m_theClassInstance->start();
	}

	return  m_theClassInstance;
}

CMonitorWorker::CMonitorWorker(void)
{
	m_pMonitorMemoryAllocReset = new CWorkTime(PROPAGATE_TIMER_LOG_THREAD_WORK);
	m_toTerminate=false;
}

CMonitorWorker::~CMonitorWorker(void)
{
	DEF_DELETE(m_pMonitorMemoryAllocReset);
}

void CMonitorWorker::terminate()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CMonitorWorker::terminate()");

	m_toTerminate = true;
}

void CMonitorWorker::run()
{
   LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CMonitorWorker begin run");

   m_pMonitorMemoryAllocReset->workBegin();
   while (m_toTerminate==false)
   {	
	   _MonitorThreads();
	   _MonitorDebugMemory();
	   Thread::sleep(100);
   }
   LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CMonitorWorker begin run");

}

void CMonitorWorker::_MonitorThreads()
{
	WorkTimeInfo workTimeInfoGet;

	ConnectionActorReleaser::getInstance()->getWorkTimeInfo(workTimeInfoGet);

	//Monitor releaseActor time
	//The oracle client API may hang if disconnect network while doing a select/update/enqueue/dequeue,
	//so add this monitor mechanism to recovery.
	//If this thread found that the release of connctionactor spent more than 5 minutes(a parameter, 'releaseactortimeout', in config file), the thread will stop
	//the whole application, then the monitor shell script will start a new one automatically.
	if (workTimeInfoGet.m_nReleaseActorNotWork > g_system_variables.m_nReleaseActorTimeOut)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "ReleaseActorNotWork %d > %d, thread hang SynchEngine must Exit",
			workTimeInfoGet.m_nReleaseActorNotWork, g_system_variables.m_nReleaseActorTimeOut);  
		TA_Base_Core::Thread::sleep(TIME_MIN_FIVE);
		exit(0);	
	}
}

void CMonitorWorker::_MonitorDebugMemory()
{
#if defined(_WIN32)
	//(WIN32 debug version only).
	//reset debug heap every one hour
	//reset debug heap to fix problem: WIN32 Debug version SynchEngine run more than two hours 
	//will have a valid debug heap block type identifier 
	//
	int nMemoryResetNotWork = 0;
	nMemoryResetNotWork = m_pMonitorMemoryAllocReset->getNotworkTime();
	if (nMemoryResetNotWork > TIME_BASE_HOUR)
	{
		m_pMonitorMemoryAllocReset->workEnd();

		long lRequest = 0;
		long NewCrtBreakAlloc = 0;		
		char* debug_new = new char[10];
		_CrtIsMemoryBlock(debug_new, 10, &lRequest, NULL, NULL); 
		NewCrtBreakAlloc = (lRequest > -1) ? (LONG_MIN / 2):(LONG_MAX / 2);			
		_CrtSetBreakAlloc(NewCrtBreakAlloc);
		LOG1(SourceInfo, DebugUtil::DebugInfo, "MemoryAllocReset req=%d", lRequest);
		delete[] debug_new;
		debug_new = NULL;
		
		m_pMonitorMemoryAllocReset->workBegin();
	}

#else
//SOLAIS don't need to reset memory
#endif
}




NS_END(TA_DB_Sync)