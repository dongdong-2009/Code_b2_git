#if !defined(DATAPOINT_STATE_UPDATE_SUBSCRIBER_H)
#define DATAPOINT_STATE_UPDATE_SUBSCRIBER_H

/**
  * DataPointStateUpdateSubscriber.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/scada_historian/src/DataPointStateUpdateSubscriber.h $
  * @author:  A. Lee
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class subscribes to AllDatapointMessages and processes 
  * the DataPointStateUpdate messages.
  *
  */

#include <string>
#include <time.h>
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"


// NOTE: The Scada_Historian used to also listen for a "DataPointUpdated" event.
// This event was sent by the Configuration Editor when a data point updated its
// trending state. If this event is still being used, then will need to write 
// another class for it.

using namespace TA_Core;

namespace TA_App
{
	//
	// Forward declaration
	//
    class ScadaHistorianAgentProcess;

    class DataPointStateUpdateSubscriber :  public SpecialisedMessageSubscriber<CommsMessageCorbaDef>
    {

    public:

		/**
		  * DataPointStateUpdateSubscriber
		  * 
		  * Constructor.
		  *
		  */
        DataPointStateUpdateSubscriber(ScadaHistorianAgentProcess* parent, unsigned long entitykey);

		/**
		  * ~DataPointStateUpdateSubscriber
		  * 
		  * Destructor.
		  *
		  */
        virtual ~DataPointStateUpdateSubscriber();

        /**
		  * receiveSpecialisedMessage
		  * 
		  * Receives and processes the message.
		  *
		  * @param message The message
		  *
		  */
        virtual void receiveSpecialisedMessage(const CommsMessageCorbaDef& message); 

        void unsubscribe();

        unsigned long getEntitykey()
        {
            return m_entitykey;
        }

    private:

        DataPointStateUpdateSubscriber();

        ScadaHistorianAgentProcess* m_parentProcess;

        unsigned long m_entitykey;

        unsigned long m_locationkey;

    };

} // TA_App


#endif // !defined(DATAPOINT_STATE_UPDATE_SUBSCRIBER_H)



