/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(AtsAgentObserver_H)
#define AtsAgentObserver_H


#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"


namespace TA_IRS_App
{
    class ITrainAgentConfiguration;

    class IAtsAgentInterface;

    class IAtsMessageProcessor;
}


namespace TA_IRS_App
{

    /**
     * This receives messages from the ATS agent and passes them onto the observer.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:16 PM
     */
    class AtsAgentObserver : public TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >
    {

    public:

        /**
         * Constructor with minimum required information
         *
         * @param configuration    The agent configuration
         * @param atsMessageProcessor    handler for incoming ats events
         * @param atsAgentInterface    Interface used to query the ATS agent for initial state
         */
        AtsAgentObserver( ITrainAgentConfiguration& configuration,
                          IAtsMessageProcessor& atsMessageProcessor,
                          IAtsAgentInterface& atsAgentInterface );


        /**
         * Destructor
         */
        virtual ~AtsAgentObserver();


        /**
         * Receives the ATS update messages
         *
         * @param payload
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload );


    private:


        /**
         * Private default constructor
         */
        AtsAgentObserver();


        /**
         * This loads the initial train information state from the ATS agent.
         */
        void loadInitialState();


        /**
         * Subscribe to radio agent updates
         *
         * @param configuration    The agent configuration
         */
        void subscribeToMessages( ITrainAgentConfiguration& configuration );


        /**
         * Unsubscribe to radio agent updates
         */
        void unsubscribeToMessages();


        /**
         * Interface to the ats agent
         */
        IAtsAgentInterface& m_atsAgentInterface;


        /**
         * This is the handler for all incoming ats comms messages
         */
        IAtsMessageProcessor& m_atsMessageProcessor;

    };

}// TA_IRS_App

#endif // !defined(AtsAgentObserver_H)
