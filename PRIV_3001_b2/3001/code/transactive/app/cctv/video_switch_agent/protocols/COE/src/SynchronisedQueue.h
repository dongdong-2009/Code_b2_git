#if !defined(SYNCHRONISEDQUEUE__H)
#define SYNCHRONISEDQUEUE__H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/SynchronisedQueue.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  *	Template to wrap std::queue in synchronous access.
  *
  * Note - if using multi-threaded libraries, this is possibly redundant.
  */

#include <queue>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


namespace TA_IRS_App
{
    /////////////////////////////////////////////////////////////////////////////
    // SynchronisedQueue

    template < typename QueueItemType >
    class SynchronisedQueue
    {
    // Operations
    public:
		/** 
		  * Constructor
		  *
		  */
        SynchronisedQueue();

		/** 
		  * Destructor
		  *
		  */
 	    ~SynchronisedQueue();

		/** 
		  * empty
		  *
		  * empty the Queue
		  */
        bool empty();

		/** 
		  * front
		  *
		  * Return the front item of the Queue
		  *
		  * @return	ItemType&	-	Reference to Const <typename ItemType> on front of the
		  *							Queue
		  */
        const QueueItemType &front();

		/** 
		  * pop
		  *
		  * Remove front item from Queue
		  */
        void pop();

		/** 
		  * push
		  *
		  * Place item on the back of the Queue
		  *
		  * @param	const ItemType &item	-	Reference to Const <typename ItemType> to 
		  *										be added to Queue
		  */
        void push(const QueueItemType &item);

		/** 
		  * size
		  *
		  * Return the number of items in the Queue
		  *
		  * @return	long	-	The number of items on the Queue
		  */
        long size();

    private:
		// hide the default copy and assignment constructors
        SynchronisedQueue( const SynchronisedQueue< QueueItemType >& synchronisedQueue );
        SynchronisedQueue& operator=( const SynchronisedQueue< QueueItemType >& synchronisedQueue );
        

    // Attributes
    private:
        TA_Base_Core::NonReEntrantThreadLockable m_queueLock;

		// the aggregated std::queue
        std::queue< QueueItemType > m_queue;
    };

} // namespace TA_Base_Bus

#include "SynchronisedQueue.inl"

#endif // !defined(AFX_SYNCHRONISEDQUEUE_H__D55846AF_42F5_4157_9ED0_D2AD9E93A208__INCLUDED_)
