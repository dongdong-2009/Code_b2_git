/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanMessageQueue.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2010/08/12 11:33:51 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

namespace TA_Base_App
{
	ActivePlanMessageQueue::ActivePlanMessageQueue() : ACE_Message_Queue<ACE_MT_SYNCH>(),
    m_bAbandoned( false )
    {
    }

	ActivePlanMessageQueue::~ActivePlanMessageQueue()
	{
        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Delete message queue (%d)", this );
	}

    int ActivePlanMessageQueue::dequeue( ACE_Message_Block *&first_item, ACE_Time_Value *timeout )
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_mtxDeletionLock);

        if ( m_bAbandoned )
        {
            TA_THROW( ActivePlanMessageException( "ACE_Message_Queue is not used any more when dequeue" ) );
        }

        return ACE_Message_Queue<ACE_MT_SYNCH>::dequeue( first_item, timeout );
    }

    int ActivePlanMessageQueue::enqueue( ACE_Message_Block *new_item, ACE_Time_Value *timeout )
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_mtxDeletionLock);

        if ( m_bAbandoned )
        {
            TA_THROW( ActivePlanMessageException( "ACE_Message_Queue is not used any more when enqueue" ) );
        }

        return ACE_Message_Queue<ACE_MT_SYNCH>::enqueue( new_item, timeout );
    }

    void ActivePlanMessageQueue::abandon()
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_mtxDeletionLock);
        m_bAbandoned = true;
    }

    void ActivePlanMessageQueue::reuse()
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_mtxDeletionLock);
        m_bAbandoned = false;
    }
} // TA_Base_App
