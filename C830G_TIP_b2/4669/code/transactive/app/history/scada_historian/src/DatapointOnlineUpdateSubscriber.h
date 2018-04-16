#if !defined(DATAPOINT_ONLINE_UPDATE_SUBSCRIBER_H)
#define DATAPOINT_ONLINE_UPDATE_SUBSCRIBER_H

/**
  * DataPointOnlineUpdateSubscriber.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/scada_historian/src/DatapointOnlineUpdateSubscriber.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class listens for datapoint online updates.
  *
  */

#include <string>
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"


using TA_Core::ConfigUpdateDetails;


namespace TA_App
{
	//
	// Forward declaration
	//
    class ScadaHistorianAgentProcess;

    class DataPointOnlineUpdateSubscriber :  public TA_Core::IOnlineUpdatable
    {

    public:

		/**
		  * DataPointOnlineUpdateSubscriber
		  * 
		  * Constructor.
		  *
		  */
        DataPointOnlineUpdateSubscriber(ScadaHistorianAgentProcess* parent);

		/**
		  * ~DataPointOnlineUpdateSubscriber
		  * 
		  * Destructor.
		  *
		  */
        virtual ~DataPointOnlineUpdateSubscriber();

        /**
		  * receiveSpecialisedMessage
		  * 
		  * Receives and processes the message.
		  *
		  * @param message The message
		  *
		  */
        virtual void processUpdate(const ConfigUpdateDetails& updateEvent);

        void addInterestedKey(unsigned int entitykey);

        void removeInterestedKey(unsigned int entitykey);

        void removeAllInterestedKeys();

    private:

        DataPointOnlineUpdateSubscriber();

        void handleUpdateEvent(const ConfigUpdateDetails& updateEvent);

        void handleCreateEvent(const ConfigUpdateDetails& updateEvent);

        void handleDeleteEvent(const ConfigUpdateDetails& updateEvent);

        ScadaHistorianAgentProcess* m_parentProcess;

    };

} // TA_App


#endif // !defined(DATAPOINT_ONLINE_UPDATE_SUBSCRIBER_H)



