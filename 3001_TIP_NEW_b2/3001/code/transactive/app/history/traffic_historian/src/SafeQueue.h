/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/history/traffic_historian/src/SafeQueue.h $
  * @author  Katherine Thomson
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: 
  *
  * This class provides a thread safe queue that stores traffic station
  * statistical information.
  *
  */

#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include <queue>
#include "MiThreadGuard.h"
#include "TdDeviceDmIf.h"

class SafeQueue : public MiThreadLockable 
{
public:

    // A data structure that holds the statistical information for a traffic 
    // station received in an event.

    struct StatsEvent
    {
        // The entity key of the traffic station.
        
        long entityKey;     

        // The information about the traffic station.
        
        its::TdDeviceDmIf::TrafficDetectorDevice device;
    };
    
    typedef struct StatsEvent t_StatsEvent;


    // The standard constructor and destructor.

    SafeQueue(){}
	virtual ~SafeQueue(){}


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
	void push(const t_StatsEvent& statsEvent);


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
	bool pop(t_StatsEvent& statsEvent);

private:

	// The queue that stores the statistical information.

	std::queue<t_StatsEvent> m_queue;
};

#endif // !defined(SAFE_QUEUE_H)
