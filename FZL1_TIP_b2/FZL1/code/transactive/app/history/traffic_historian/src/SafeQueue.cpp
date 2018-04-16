/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/history/traffic_historian/src/SafeQueue.cpp $
  * @author  Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: 
  *
  * This class provides a thread safe queue that stores traffic station
  * statistical information.
  *
  */

// Ignore STL warnings on Windows.

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include "SafeQueue.h"

/** 
  * push
  *
  * Push the StatsEvent onto the back of the queue.  
  * This method is thread safe.
  *
  * @param An event structure containing the key of traffic station and
  *        the detector information including the statistical data
  *        that the Historian monitors.
  *
  */
void SafeQueue::push(const t_StatsEvent& statsEvent)
{
	//Make it all thread safe
	MiThreadGuard guard(*this);

	m_queue.push(statsEvent);
}

/** 
  * pop
  *
  * Pops the StatsEvent off the front of the queue.
  * This method is thread safe.
  * 
  * @param An event structure to receive the popped StatsEvent.
  *
  * @return A bool which is true if an event was popped and false
  *         if the queue was empty.
  *
  */
bool SafeQueue::pop(t_StatsEvent& statsEvent)
{
	//Make it all thread safe
	MiThreadGuard guard(*this);

	if (m_queue.empty())
	{
		return false;
	}
	else
	{
		//Do the copy to avoid getting a reference to the object in the queue
		t_StatsEvent temp = m_queue.front();
		statsEvent = temp;
		m_queue.pop();
		return true;
	}
}
