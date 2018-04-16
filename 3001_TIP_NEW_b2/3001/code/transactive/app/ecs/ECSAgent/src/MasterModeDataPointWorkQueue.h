/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/MasterModeDataPointWorkQueue.h $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/03/26 11:00:59 $
  * Last modified by:  $Author: qi.huang $
  *
  */
#if !defined(ExternalStationWorkQueue__INCLUDED_)
#define ExternalStationWorkQueue__INCLUDED_

#ifdef __WIN32__
#pragma warning(disable:4503)
#endif

#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/threads/src/IWorkItem.h"
#include "core/utilities/src/DebugUtil.h"
#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"


class ItaEcsDatapoints;
class ACE_Barrier;

//
// AbstractWorkQueueItem
//
class AbstractWorkQueueItem : public TA_Base_Core::IWorkItem
{
public:
	AbstractWorkQueueItem();
	virtual ~AbstractWorkQueueItem();
	virtual void setDataPoint()=0;
	void executeWorkItem();	

};


//
// ExternalStationWorkQueueItem
//
class ExternalStationWorkQueueItem : public AbstractWorkQueueItem
{
public:
	ExternalStationWorkQueueItem(ItaEcsDatapoints& theDatapoints, unsigned long location, 
							long stationMode, long masterMode, ACE_Barrier& barrier);
	virtual ~ExternalStationWorkQueueItem();
	virtual void setDataPoint();

protected:
	ItaEcsDatapoints&		m_theDatapoints;
	unsigned long			m_location;
	long					m_stationMode;
	long					m_masterMode;			
	ACE_Barrier&            m_barrier;

};


//
// MasterModeDataPointWorkQueueManager
//
class MasterModeDataPointWorkQueueManager
{

public:
	MasterModeDataPointWorkQueueManager();
	virtual ~MasterModeDataPointWorkQueueManager();
	int execute(AbstractWorkQueueItem * item);

private:
	TA_Base_Core::ThreadPoolSingletonManager* m_threadPoolManager; 

};



#endif


