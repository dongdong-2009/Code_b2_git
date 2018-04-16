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
#if !defined(RadioAgentObserver_H)
#define RadioAgentObserver_H

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

// forward declarations
namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
    class IRadioAgentInterface;
    class IRadioMessageProcessor;
}

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:58 PM
     */
    class RadioAgentObserver : public TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >
    {

    public:

        /**
         * Constructor with minimum required information
         *
         * @param configuration    The agent configuration
         * @param radioAgentInterface    The interface to the radio agent
         * @param radioMessageProcessor    handler for incoming radio events
         */
        RadioAgentObserver( ITrainAgentConfiguration& configuration,
                            IRadioAgentInterface& radioAgentInterface,
                            IRadioMessageProcessor& radioMessageProcessor );


        /**
         * Destructor
         */
        virtual ~RadioAgentObserver();


        /**
         * Receive the comms message
         *
         * @param message    The message data
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message );


    private:

        /**
         * Private default constructor
         */
        RadioAgentObserver();


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
         * An interface to query the radio agent
         */
        IRadioAgentInterface& m_radioAgentInterface;


        /**
         * This is the handler for all incoming radio comms messages
         */
        IRadioMessageProcessor& m_radioMessageProcessor;

    };

} // TA_IRS_App

#endif // !defined(RadioAgentObserver_H)
