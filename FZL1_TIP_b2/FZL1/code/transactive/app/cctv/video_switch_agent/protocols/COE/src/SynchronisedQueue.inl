/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/cctv/video_switch_agent/protocols/COE/src/Attic/SynchronisedQueue.inl,v $
  * @author:  Robert Young
  * @version: $Revision: 1.1.2.2 $
  *
  * Last modification: $Date: 2005/01/24 09:18:32 $
  * Last modified by:  $Author: stuartw $
  *
  *	Template to wrap std::queue in synchronous access.
  *
  * If using multi-threaded libraries, this is possibly redundant.
  */

#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_IRS_App
{
    template < typename QueueItemType >
    SynchronisedQueue< QueueItemType >::SynchronisedQueue()
    {
    }


    template < typename QueueItemType >
    SynchronisedQueue< QueueItemType >::~SynchronisedQueue()
    {
    }

 
    template < typename QueueItemType >
    bool SynchronisedQueue< QueueItemType >::empty()
    {
        TA_Base_Core::ThreadGuard accessGuard( m_queueLock );

        return m_queue.empty();
    }

   
    template < typename QueueItemType >
    const QueueItemType &SynchronisedQueue< QueueItemType >::front()
    {
        TA_Base_Core::ThreadGuard accessGuard( m_queueLock );

        return m_queue.front();
    }


    template < typename QueueItemType >
    void SynchronisedQueue< QueueItemType >::pop()
    {
        TA_Base_Core::ThreadGuard accessGuard( m_queueLock );

        m_queue.pop();
    }        


    template < typename QueueItemType >
    void SynchronisedQueue< QueueItemType >::push(const QueueItemType& item)
    {
        TA_Base_Core::ThreadGuard accessGuard( m_queueLock );

        m_queue.push(item);
    }


    template < typename QueueItemType >
    long SynchronisedQueue< QueueItemType >::size()
    {
        TA_Base_Core::ThreadGuard accessGuard( m_queueLock );

        return m_queue.size();
    }

} // namespace TA_Base_Bus

