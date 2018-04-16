/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/MasterModeDataPointWorkQueue.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/03/26 11:00:59 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#include "ace/Barrier.h"
#include "app/ecs/ECSAgent/src/MasterModeDataPointWorkQueue.h"
#include "app/ecs/ECSAgent/src/ItaEcsDatapoints.h"


//
// AbstractWorkQueueItem
//

AbstractWorkQueueItem::AbstractWorkQueueItem()
{

};


AbstractWorkQueueItem::~AbstractWorkQueueItem()
{

};


void AbstractWorkQueueItem::executeWorkItem()
{
	setDataPoint();
};


//
// ExternalStationWorkQueueItem
//

ExternalStationWorkQueueItem::ExternalStationWorkQueueItem(ItaEcsDatapoints& theDatapoints, 
									unsigned long location, long stationMode, long masterMode,
									ACE_Barrier& barrier):
m_theDatapoints(theDatapoints),
m_location(location),
m_stationMode(stationMode),
m_masterMode(masterMode),
m_barrier(barrier)
{
	
}

ExternalStationWorkQueueItem::~ExternalStationWorkQueueItem()
{

}

void ExternalStationWorkQueueItem::setDataPoint()
{

	try
	{
		m_theDatapoints.setExternalSelectedMasterMode(m_location, m_masterMode);
        m_theDatapoints.setExternalSelectedOverridingStationMode(m_location, m_stationMode);
		
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "<unknown exception>", "Unhandled exception in ExternalStationWorkQueueItem call()");
	}

	//m_barrier.wait();

}


//
// ExternalStationWorkQueueManager
//

MasterModeDataPointWorkQueueManager::MasterModeDataPointWorkQueueManager() : m_threadPoolManager (NULL)
{
	m_threadPoolManager = TA_Base_Core::ThreadPoolSingletonManager::getInstance();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MasterModeDataPointWorkQueueManager() - Thread pool upper limit=%ld",
		m_threadPoolManager->getUpperLimit());    
};


MasterModeDataPointWorkQueueManager::~MasterModeDataPointWorkQueueManager()
{
	TA_Base_Core::ThreadPoolSingletonManager::removeInstance(m_threadPoolManager);
	m_threadPoolManager = NULL;
};


int MasterModeDataPointWorkQueueManager::execute(AbstractWorkQueueItem* item)
{
	// take ownership of the task
	TA_Base_Core::IWorkItemPtr newOwner(item);

	if (m_threadPoolManager != NULL)
	{
		m_threadPoolManager->queueWorkItem(newOwner);
	}

	return 0;
}






