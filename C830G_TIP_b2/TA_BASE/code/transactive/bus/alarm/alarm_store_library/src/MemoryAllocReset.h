// MemoryAllocReset.h: interface for the MemoryAllocReset class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYALLOCRESET_H__542D6CD3_F30B_4F93_8D69_224888C8390C__INCLUDED_)
#define AFX_MEMORYALLOCRESET_H__542D6CD3_F30B_4F93_8D69_224888C8390C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
namespace TA_Base_Bus
{
	class MemoryAllocReset : public TA_Base_Core::Thread
	{
	public:		
		virtual ~MemoryAllocReset();
		virtual void run();	 
		virtual void terminate();
		void Initialize();
		static MemoryAllocReset* getInstance();
		static void release();
	private:
		bool m_isInitialized;
		MemoryAllocReset();
		bool m_IsWorking;
		int  m_ticketCount;
		static MemoryAllocReset*  m_Instance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
        TA_Base_Core::NonReEntrantThreadLockable m_initilizedLock;
	};
}
#endif // !defined(AFX_MEMORYALLOCRESET_H__542D6CD3_F30B_4F93_8D69_224888C8390C__INCLUDED_)
