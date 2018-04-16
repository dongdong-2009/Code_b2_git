/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/ModbusServerAgent/src/ModbusPointThread.h $
  * @author:	Dhanshri
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/04/23 09:01:51 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description: Class to address performance issue of huge DP updates 
  */


#ifndef MBP_THREAD_H
#define MBP_THREAD_H

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"

namespace TA_Base_App
{
	class DataPointUpdateData;
	class ModbusPointManager;
	
	class ModbusPointThread : public TA_Base_Core::Thread
	{
	public:
		ModbusPointThread(ModbusPointManager *);
		~ModbusPointThread();

		void run();

		void terminate();

		void insertUniqueItem(DataPointUpdateData *);

		unsigned int getUpdateCnt();

	private:
		DataPointUpdateData *  getItem();

		bool										m_bTerminate;
		unsigned int								m_updateCnt;
		TA_Base_Core::NonReEntrantThreadLockable	m_queueLockThread;
		TA_Base_Core::Semaphore						m_semaphore;

		DataPointUpdateData * 				m_head;
		DataPointUpdateData * 				m_tail;
		ModbusPointManager*						m_pMBPManager;
	};
}

#endif
