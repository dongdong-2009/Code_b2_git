/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/history/traffic_historian/src/EventConsumer.cpp $
  * @author Craig Grant
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: Katherine Thomson
  *
  * The EventConsumer accepts and processes events pushed from the 
  * CORBA event service.  It listens for any new Traffic Detector data and 
  * passes it to the appropriate TrafficDetector object.
  *
  */

// Ignore STL warnings on Windows.

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include "DebugUtil.h"
#include "EventListener.h"
#include "ReceivedEvent.h"
#include "EventConsumer.h"
#include "TdDeviceDmIf.h"
#include "TDMap.h"


/** 
  * run
  * 
  * This method continuously checks the queue to see if there are
  * any events to be processed.  If there are events on the queue,
  * they are popped off the queue and processed.  Otherwise the 
  * thread will sleep momentarily and then try again.  This method
  * implements the virtual method from MiThread.
  *
  */
void CEventConsumer::run()
{
    m_done = false;
    while ( !m_done )
    {
        //MiThreadGuard threadGuard( m_securityGuard );
        if ( !ProcessStatsEvent() )
        {
            // The minimum interval that the Traffic Detector agent will
            // poll the traffic stations is 10 seconds.  Assume that if
            // there was not an event on the queue last time that it is ok
            // to sleep for a bit.  If there was an event to be processed,
            // there is a good chance that there will be more events on
            // the queue, so don't sleep.
            
            sleep( 5000 );
        }
    }
}


/** 
  * terminate
  * 
  * This method tells the EventConsumer thread to stop processing events, so
  * that the thread can be terminated. It implements the virtual method 
  * from MiThread.
  *
  */
void CEventConsumer::terminate()
{
    //MiThreadGuard threadGuard( m_securityGuard );
    m_done = true;
}


/** 
  * callback
  *
  * This method handles events coming in from the Current event channel.
  * Events relating to traffic detector statistics are pushed onto a queue
  * for further processing. It implements a virtual method from CallbackUser.
  *
  * @param The received event information.
  * 
  * @exception A CORBA::SystemException is thrown if the CORBA related
  *            processing of the string fails.
  *
  */
void CEventConsumer::callback(const CallbackSubmission* submission)
    throw( CORBA::SystemException)
{
    // Cast the event into something we can start to work with.
    
    const ReceivedEvent* re = dynamic_cast<const ReceivedEvent*>(submission);

    if (re == NULL)
    {
        return;
    }

	// If the received data is not an event, or we couldn't get
	// the event, then we ignore it.
	
    nexus::EventData* eventData = 0;
    CORBA::Any any = *(re->getData());
    if (!(any >>= eventData))
    {
        LOG( DebugUtil::DebugWarn, "Event contains unexpected data type...Ignoring" );
        return;
    }
	if( 0 == eventData )
	{
        LOG( DebugUtil::DebugWarn, "Couldn't extract event data...Ignoring" );
        return;
	}

	
	// Log the type of event that was received.
	
	char lvBuffer[ 257 ];
	CORBA::String_var lvEventType = CORBA::string_dup( eventData->eventType );
	sprintf( lvBuffer, "Event received.  EventType = '%s'", lvEventType.in() );
	LOG( DebugUtil::DebugDebug, lvBuffer );

	
    // Extract the device data.
	
    its::TdDeviceDmIf::TrafficDetectorDevice* deviceData = NULL;
    if (!(eventData->state >>= deviceData))
    {
        LOG( DebugUtil::DebugDebug, "Event does not contain TrafficDetector Data" );
        return;
    }

    // Extract the entity key and device data from the event and
    // push it onto the queue
    
    SafeQueue::t_StatsEvent statsEvent;
    statsEvent.entityKey = eventData->entityKey;
    statsEvent.device = *deviceData;
    m_eventQueue.push( statsEvent );
}


/**
  * ProcessStatsEvent
  *
  * This method processes the first event on the queue if there
  * is one.  It then pops the event off the front of the queue.
  *
  * @return A bool which is true if an event was processed.
  *
  */
bool CEventConsumer::ProcessStatsEvent()
{
    SafeQueue::t_StatsEvent statsEvent;
    if ( m_eventQueue.pop(statsEvent) )
    {
        TDMap::GetInstance()->WriteStatistics( statsEvent.entityKey, statsEvent.device );
        return true;    
    }
    return false;
}

