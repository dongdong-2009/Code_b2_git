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
#if !defined(TrainDriverCallCorbaProxy_H)
#define TrainDriverCallCorbaProxy_H


#include "bus/trains/TrainCommonLibrary/src/GenericObserverManager.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainDriverCallUpdateObserver.h"
#include "bus/trains/TrainCommonLibrary/src/CallTypes.h"

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#include <boost/shared_ptr.hpp>


namespace TA_IRS_Bus
{

    /**
     * Interface for commands used by Trainborne Radio operations.
     * This is a simple proxy, it wraps the named object, and converts parameters and
     * return types between CORBA and C++ datatypes using the conversion methods.
     * @author Robert van Hugten
     * @version 1.0
     * @created 22-Oct-2007 4:46:54 PM
     */

    class TrainDriverCallCorbaProxy
                : public TA_IRS_Bus::GenericObserverManager<ITrainDriverCallUpdateObserver>,
                  public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {

    public:


        /**
         * Constructor that will create the reference to the remote agent interface
         *
         * @param agentKey    The train agent entity key
         * @param agentName    The entity name of the train agent this proxy will contact
         * @param locationKey    The agent location key
         */
        TrainDriverCallCorbaProxy( const std::string& agentName,
                                   unsigned long agentKey,
                                   unsigned long locationKey );

        /**
         * Destructor
         */
        ~TrainDriverCallCorbaProxy();


        /**
         * This will retrieve all active OCC call requests.
         *
         * @return a sequence of OCC call requests
         *
         * @exception AgentCommunicationException if there was a problem contacting the
         * agent
         */
        CallTypes::OccCallRequestList getAllOccCallRequests();


        /**
         * This will answer the outstanding OCC call request for the given train.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or there is no outstanding call request for this train
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainRadioException if the radio call failed
         *
         * @param trainId    The train whos OCC call request we are going to answer.
         * @param sessionId    The session ID of the operator answering the call
         */
        void answerOccCallRequest( CommonTypes::TrainIdType trainId, const std::string& sessionId );


        /**
         * Sets up a call to a train or a group of trains.
         * This will set up a call the given trains, and make a DGNA containing the TSI of each
         * successfully set up train, and also the individual subscribers given.
         * It will then call the resulting group.
         *
         * If only one train is given, or only one train succeeds in the setup
         * (and there are no individual subscribers) a direct call will be made to the train.
         *
         * @return the overall result of the operation, including the result of each specific train
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainRadioException if the DGNA or radio call failed
         *
         * @param trainList    A list of trains that the call will go to
         * @param individualSubscribers    These are the non-train subscribers that will be included in the DGNA.
         * @param override    Whether to send a request or an override
         * @param callType    The type of radio call to make (eg acknowledged/unacknowledged)
         *                    This can be overridden by the train agent if the given type does not match
         *                    the call that will be made (ie Single call is given and a group will be called)
         * @param callPriority    The call priority for the radio call
         * @param sessionId    The operator making the call
         */
        CallTypes::DriverCallResult makeDriverCall( const CommonTypes::TrainIdList& trainList,
                                                    const CallTypes::ItsiList& individualSubscribers,
                                                    bool override,
                                                    CallTypes::ECallType callType,
                                                    CallTypes::ECallPriority callPriority,
                                                    const std::string& sessionId );


        /**
         * Sets up a call to a train or a group of trains.
         * This will set up a call the given trains, and when successful call the group tsi given.
         *
         * @return the overall result of the operation, including the result of each specific train
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception GroupInUseException if the given group TSI is in use
         * @exception TrainRadioException if the DGNA or radio call failed
         *
         * @param trainList    A list of trains that the call will go to
         * @param groupTsi    This is the TSI of the group to call. If this is not specified,
         *                    and more than one train is given then a DGNA will be used.
         * @param override    Whether to send a request or an override
         * @param callType    The type of radio call to make (eg acknowledged/unacknowledged)
         *                    This can be overridden by the train agent if the given type does not match
         *                    the call that will be made (ie Single call is given and a group will be called)
         * @param callPriority    The call priority for the radio call
         * @param sessionId    The operator making the call
         */
        CallTypes::DriverCallResult makeDriverCall( const CommonTypes::TrainIdList& trainList,
                                                    const std::string& groupTsi,
                                                    bool override,
                                                    CallTypes::ECallType callType,
                                                    CallTypes::ECallPriority callPriority,
                                                    const std::string& sessionId );


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
        struct DriverCallCorbaObject;


        TrainDriverCallCorbaProxy();


        /**
         * Called to subscribe to updates from the agent
         */
        void subscribeToMessages();


        /**
         * Called to unsubscribe to updates from the agent
         */
        void unsubscribeToMessages();


        /**
         * object for access to the CORBA servant.
         */
        DriverCallCorbaObject* m_driverCallCorbaObject;


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
     * @created 22-Oct-2007 4:46:54 PM
     */
    typedef boost::shared_ptr< TrainDriverCallCorbaProxy > TrainDriverCallCorbaProxyPtr;

}

#endif // !defined(TrainDriverCallCorbaProxy_H)
