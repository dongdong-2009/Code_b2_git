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

#include "app/scada/ModbusServerAgent/src/ModbusPointThread.h"
#include "app/scada/ModbusServerAgent/src/ModbusPointAgentProxy.h"
#include "app/scada/ModbusServerAgent/src/ModbusPointManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_Base_App
{	
	ModbusPointThread::ModbusPointThread(ModbusPointManager* pManager) 
		: m_bTerminate(false), m_updateCnt(0), m_semaphore(0), m_head(NULL), m_tail(NULL), m_pMBPManager(pManager)
	{
		start();
	}
	
	ModbusPointThread::~ModbusPointThread()
	{
		terminateAndWait();
	}

	void ModbusPointThread::terminate()
	{
		m_bTerminate = true;
		m_semaphore.post();
	}

	void ModbusPointThread::run()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ModbusPointThread start %u", getId());

		DataPointUpdateData * item = NULL;
		
		while(!m_bTerminate)
		{
			item = getItem();

			if (item != NULL)
			{
				try
				{
					m_pMBPManager->processDataItem(*item);
					++m_updateCnt;
				}
				catch (...)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception happened when processProxyUpdate %u", getId());
				}
			}
			
			m_semaphore.wait();
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ModbusPointThread stop %u", getId());
	}

	DataPointUpdateData * ModbusPointThread::getItem()
	{
		TA_Base_Core::ThreadGuard guard(m_queueLockThread);

		DataPointUpdateData * item = NULL;

		if (m_head != NULL)
		{
			item = m_head;
			m_head = item->m_next;
			if (m_head == NULL)
			{
				m_tail = NULL;
			}
			item->m_next = NULL;
		}

		return item;
	}

	void ModbusPointThread::insertUniqueItem(DataPointUpdateData* item)
	{
		TA_Base_Core::ThreadGuard guard(m_queueLockThread);

		if (item->m_next == NULL && item != m_tail)
		{
			if (m_head == NULL)
			{
				m_head = item;
			}
			else
			{
				m_tail->m_next = item;
			}

			m_tail = item;
			m_semaphore.post();
		}
	}

	unsigned int ModbusPointThread::getUpdateCnt()
	{
		return m_updateCnt;
	}
}

