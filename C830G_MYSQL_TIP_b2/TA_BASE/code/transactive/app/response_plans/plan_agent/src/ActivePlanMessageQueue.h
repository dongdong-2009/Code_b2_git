#ifndef ACTIVEPLANMESSAGEQUEUE_H_INCLUDED
#define ACTIVEPLANMESSAGEQUEUE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanMessageQueue.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2010/08/12 11:33:51 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */

namespace TA_Base_App
{
	//Class declaration
	class ActivePlanMessageQueue : public ACE_Message_Queue<ACE_MT_SYNCH>
	{
	public:
		ActivePlanMessageQueue();
		~ActivePlanMessageQueue();

        virtual int dequeue( ACE_Message_Block *&first_item, ACE_Time_Value *timeout  = 0 );
        virtual int enqueue( ACE_Message_Block *new_item, ACE_Time_Value *timeout = 0 );

        void abandon();
        void reuse();

	private:
		// Make sure these don't get called
		ActivePlanMessageQueue &operator= (const ActivePlanMessageQueue &);
		ActivePlanMessageQueue (const ActivePlanMessageQueue &);

    private:
        bool m_bAbandoned;
		mutable ACE_RW_Mutex m_mtxDeletionLock;
	}; // ActivePlanMessageQueue

    typedef boost::shared_ptr<ActivePlanMessageQueue> ActivePlanMessageQueuePtr;
}; // TA_Base_App

#endif // ACTIVEPLANMESSAGEQUEUE_H_INCLUDED
