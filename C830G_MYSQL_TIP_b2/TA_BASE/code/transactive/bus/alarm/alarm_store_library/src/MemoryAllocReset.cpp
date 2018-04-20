// MemoryAllocReset.cpp: implementation of the MemoryAllocReset class.
//
//////////////////////////////////////////////////////////////////////

#include "MemoryAllocReset.h"
#include "core/utilities/src/DebugUtil.h"
#include <crtdbg.h>

using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{


	MemoryAllocReset* MemoryAllocReset::m_Instance=NULL;
	TA_Base_Core::NonReEntrantThreadLockable MemoryAllocReset::m_singletonLock;
    MemoryAllocReset* MemoryAllocReset::getInstance()
	{
		if (m_Instance!=NULL)
		{
			return m_Instance;
		}		
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance==NULL)
		{
			m_Instance=new MemoryAllocReset();
		}
		return m_Instance;
	}
	void MemoryAllocReset::release()
	{
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance!=NULL)
		{
			m_Instance->terminateAndWait();
			delete m_Instance;
			m_Instance=NULL;
		}		 
	}
	MemoryAllocReset::MemoryAllocReset()
	{
		m_IsWorking=TRUE;
		m_ticketCount=0;
		m_isInitialized=FALSE;
	}
	MemoryAllocReset::~MemoryAllocReset()
	{

	}

	void MemoryAllocReset::Initialize()
	{	
		if(m_isInitialized)
		{
			return;
		}
		TA_Base_Core::ThreadGuard guard(m_initilizedLock);
		if(m_isInitialized)
		{
			return;
		}        
		start();		 
		m_isInitialized=TRUE;
	}

	void MemoryAllocReset::run()
	{
	   LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MemoryAllocReset::run in");
	   while(m_IsWorking)
	   {
			this->sleep(500);
			m_ticketCount++;
		    if (m_ticketCount>7000)
			{
				m_ticketCount=0;
				long lRequest;
				long NewCrtBreakAlloc;		
				char* debug_new = new char[10];
				_CrtIsMemoryBlock(debug_new, 10, &lRequest,  NULL,   NULL); 
				NewCrtBreakAlloc   = (lRequest > -1) ? (LONG_MIN / 2):(LONG_MAX / 2);			
				_CrtSetBreakAlloc(NewCrtBreakAlloc);
				LOG1(SourceInfo, DebugUtil::DebugInfo, "MemoryAllocReset req=%d", lRequest);
				delete[] debug_new;
			}
	   }
	   LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MemoryAllocReset::run out");

	}
    void MemoryAllocReset::terminate()
	{
		m_IsWorking=FALSE;
	}
}
