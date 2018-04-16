/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\DataProcessorThread.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible handling the data processing thread
  */

#include "bus/ats/ats_agent/CommonATS/src/DataProcessorThread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_IRS_Bus
{
	DataProcessorThread::DataProcessorThread(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: m_atsEntityData(EntityData),
		  m_newDataNotif(0),
		  m_keepRunning(true),
		  m_isProcessSuccess(false)
	{
		FUNCTION_ENTRY("DataProcessorThread");
		FUNCTION_EXIT;
	}

	DataProcessorThread::~DataProcessorThread()
	{
		FUNCTION_ENTRY("~DataProcessorThread");
		FUNCTION_EXIT;
	}

	void DataProcessorThread::terminate()
	{
		FUNCTION_ENTRY("terminate");

		terminateAndWait();
		m_keepRunning = false;
		m_newDataNotif.post();

		FUNCTION_EXIT;
	}

	void DataProcessorThread::addItem(DataNotificationPtr dataNotif, bool latestRecord )
	{
		FUNCTION_ENTRY("addItem");

		TA_Base_Core::ThreadGuard guard(m_msgCacheLock);
		//This is to make sure that we only keep and handle the latest record
		if (latestRecord)  m_messageCache.clear();
		m_messageCache.push_back(dataNotif);

		m_newDataNotif.post();
		FUNCTION_EXIT;
	}

	void DataProcessorThread::run()
	{
		FUNCTION_ENTRY("run");

		while(m_keepRunning)
		{
			m_newDataNotif.wait();
			{
				{
					TA_Base_Core::ThreadGuard guard(m_msgCacheLock);
					DataNotificationVecPtrIter iter = m_messageCache.begin();
					for(iter; iter != m_messageCache.end(); iter++)
					{
						m_messageQueue.push((*iter));
					}
					m_messageCache.clear();
				}
				
				//while there are items in m_messageQueue
				while(m_messageQueue.empty() == false)
				{
					DataNotificationPtr atsDataNotif = m_messageQueue.front();
					processItem(atsDataNotif);
					m_messageQueue.pop();
				}
			}
			
		}
		FUNCTION_EXIT;
	}
}