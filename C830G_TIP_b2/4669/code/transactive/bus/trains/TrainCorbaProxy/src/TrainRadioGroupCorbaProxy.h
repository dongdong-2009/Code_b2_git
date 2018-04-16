/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert V
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(TrainRadioGroupCorbaProxy_H)
#define TrainRadioGroupCorbaProxy_H


#include "bus/trains/TrainCommonLibrary/src/GenericObserverManager.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainRadioGroupUpdateObserver.h"
#include "bus/trains/TrainCommonLibrary/src/RadioGroupTypes.h"

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#include <boost/shared_ptr.hpp>


namespace TA_IRS_Bus
{
    /**
     * Interface for commands and notification related to Train Radio Group Coverage
     * (a.k.a. Operating Areas).
     * This is a simple proxy, it wraps the named object, and converts parameters and
     * return types between CORBA and C++ datatypes using the conversion methods.
     * @author Robert V
     * @version 1.0
     * @created 22-Oct-2007 4:47:00 PM
     */

    class TrainRadioGroupCorbaProxy
              : public TA_IRS_Bus::GenericObserverManager<ITrainRadioGroupUpdateObserver>,
                public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {

    public:

        /**
         * Constructor that will create the reference to the remote agent interface
         *
         * @param agentName    The entity name of the train agent this proxy will contact
         * @param agentKey    The train agent entity key
         * @param locationKey    The agent location key
         */
        TrainRadioGroupCorbaProxy( const std::string& agentName,
                                   unsigned long agentKey,
                                   unsigned long locationKey );


        /**
         * destructor
         */
        ~TrainRadioGroupCorbaProxy();


        /**
         * Gets the radio groups available for the given location (the ones that cover
         * trains available to be selected from the current location).
         *
         * At a station, it will be all groups covering this station
         * At the OCC it will be all groups covering any part of the mainline
         * At the depot, it will be all groups covering the depot
         *
         * @exception AgentCommunicationException if there was a problem contacting the
         * agent
         *
         * @param location    the transactive location for which to retrieve groups
         */
        RadioGroupTypes::TrainRadioGroupMap getTrainRadioGroups( unsigned long location );


        /**
         *
         * @param payload
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload );


    protected:


        /**
         * This is a virtual method, which by default does nothing.
         * It is called when the first observer is added, and can be overridden to perform
         * some specific operation (eg subscribe to messages)
         */
        virtual void firstObserverAdded();


        /**
         * This is a virtual method, which by default does nothing.
         * It is called when last first observer is removed, and can be overridden to
         * perform some specific operation (eg unsubscribe to messages)
         */
        virtual void lastObserverRemoved();


    private:


        // to avoid including any CORBA in the proxy class header files
        // we are now using the 'pimpl' idiom
        // ive just pimpled this class out
        struct RadioGroupCorbaObject;


        /**
         * private constructor
         */
        TrainRadioGroupCorbaProxy();


        /**
         * Called to subscribe to updates from the agent
         */
        void subscribeToMessages();


        /**
         * Called to unsubscribe to updates from the agent
         */
        void unsubscribeToMessages();


        /**
         * The standard named object for access to the CORBA servant.
         */
        RadioGroupCorbaObject* m_radioGroupCorbaObject;


        /**
         * This is the entity key of the train agent, used to subscribe to comms messages
         * and state update messages.
         */
        unsigned long m_trainAgentEntityKey;


        /**
         * This is the entity name of the train agent.
         */
        std::string m_trainAgentEntityName;


        /**
         * This is the location key of the train agent entity.
         */
        unsigned long m_trainAgentLocationKey;


        /**
         * flag to indicate message subscription
         */
        bool m_subscribedToMessages;


        /**
         * Lock to protect the subscription process
         */
        TA_Base_Core::NonReEntrantThreadLockable m_subscriptionLock;

    };


    /**
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:47:00 PM
     */
    typedef boost::shared_ptr< TrainRadioGroupCorbaProxy > TrainRadioGroupCorbaProxyPtr;

}

#endif // !defined(TrainRadioGroupCorbaProxy_H)
