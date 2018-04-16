/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(TrainStateUpdateCorbaProxy_H)
#define TrainStateUpdateCorbaProxy_H


#include "bus/trains/TrainCommonLibrary/src/GenericObserverManager.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainStateUpdateObserver.h"
#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"

#include <boost/shared_ptr.hpp>


namespace TA_IRS_Bus
{
    /**
     * Interface for elements and methods used for State Update. For the TrainAgent,
     * State updates are broadcast notifications, ie. to every trainagent.
     * This is a simple proxy, it wraps the named object, and converts parameters and
     * return types between CORBA and C++ datatypes using the conversion methods.
     * @author Robert van Hugten
     * @version 1.0
     * @created 22-Oct-2007 4:47:01 PM
     */

    class TrainStateUpdateCorbaProxy
              : public TA_IRS_Bus::GenericObserverManager<ITrainStateUpdateObserver>,
                public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>,
                public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>
    {

    public:


        /**
         * Constructor that will create the reference to the remote agent interface
         *
         * @param agentName    The entity name of the train agent this proxy will contact
         * @param agentKey    The train agent entity key
         * @param locationKey    The agent location key
         */
        TrainStateUpdateCorbaProxy( const std::string& agentName,
                                    unsigned long agentKey,
                                    unsigned long locationKey );


        /**
         * Constructor that will create the reference to the remote agent interface
         * This will not allow subscription to messages.
         *
         * @param agentName    The entity name of the train agent this proxy will contact
         */
        TrainStateUpdateCorbaProxy( const std::string& agentName );


        /**
         * Destructor
         */
        ~TrainStateUpdateCorbaProxy();


        /**
         * This queries this agent and returns all local state held by this agent. The
         * information it returns is relevant only to the peer of this agent (ie this is
         * only used between control/monitor pairs).
         *
         * @return the full local state of this agent
         *
         * @exception AgentCommunicationException if there was a problem contacting the
         * agent
         */
        StateUpdateTypes::FullLocalState getLocalState();


        /**
         * This queries this agent and returns all global state held by this agent. The
         * information it returns is relevant to all train agents throughout the system.
         *
         * @return the full global state of this agent
         *
         * @exception AgentCommunicationException if there was a problem contacting the
         * agent
         */
        StateUpdateTypes::FullGlobalState getGlobalState();


        /**
         *
         * @param payload
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload );


        /**
         *
         * @param payload
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::StateUpdateMessageCorbaDef& payload );


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
        struct StateUpdateCorbaObject;


        TrainStateUpdateCorbaProxy();


        /**
         * Called to subscribe to updates from the agent
         */
        void subscribeToMessages();


        /**
         * Called to unsubscribe to updates from the agent
         */
        void unsubscribeToMessages();


        /**
         * object for access to the CORBA servant
         */
        StateUpdateCorbaObject* m_stateUpdateCorbaObject;


        /**
         * flag to indicate message subscription
         */
        bool m_subscribedToMessages;


        /**
         * Lock to protect the subscription process
         */
        TA_Base_Core::NonReEntrantThreadLockable m_subscriptionLock;


        /**
         * This is the entity name of the train agent.
         */
        std::string m_trainAgentEntityName;


        /**
         * This is the entity key of the train agent, used to subscribe to comms messages
         * and state update messages.
         */
        unsigned long m_trainAgentEntityKey;


        /**
         * This is the location key of the train agent entity.
         */
        unsigned long m_trainAgentLocationKey;

    };

    /**
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:47:01 PM
     */
    typedef boost::shared_ptr< TrainStateUpdateCorbaProxy > TrainStateUpdateCorbaProxyPtr;

}

#endif // !defined(TrainStateUpdateCorbaProxy_H)
