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
#if !defined(TrainPecCorbaProxy_H)
#define TrainPecCorbaProxy_H


#include "bus/trains/TrainCommonLibrary/src/GenericObserverManager.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainPecUpdateObserver.h"
#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#include <boost/shared_ptr.hpp>

namespace TA_IRS_Bus
{
    /**
     * Interface for commands used by Trainborne PEC operations.
     * This is a simple proxy, it wraps the named object, and converts parameters and
     * return types between CORBA and C++ datatypes using the conversion methods.
     * @author Robert van Hugten
     * @version 1.0
     * @created 22-Oct-2007 4:46:59 PM
     */

    class TrainPecCorbaProxy
                : public TA_IRS_Bus::GenericObserverManager<ITrainPecUpdateObserver>,
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
        TrainPecCorbaProxy( const std::string& agentName,
                            unsigned long agentKey,
                            unsigned long locationKey );

        /**
         * destructor
         */
        ~TrainPecCorbaProxy();


        /**
         * Gets all active PEC for all trains, and their status.
         *
         * @return all active PEC for all trains managed by this agent
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         */
        PecTypes::PecUpdateList getAllActivePec();


        /**
         * This will start the PEC answer sequence, and make the audio call.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *                                      (ie if the given PEC is not active or not valid)
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception PecException if there was an error performing a PEC operation
         * @exception TrainRadioException if the radio call failed
         *
         * @param trainId    The train the PEC is on
         * @param car    The PEC car
         * @param pec    The PEC within the car
         * @param sessionId    The operator answering the PEC
         */
        void answerPec( CommonTypes::TrainIdType trainId,
                        ProtocolPecTypes::EPecCarNumber car,
                        ProtocolPecTypes::EPecNumber pec,
						const std::string& sessionId, PecTypes::EPecTrainSource pecTrainSource );


        /**
         * This will activate the correct CCTV image for a PEC on a train.
         * It follows all the standard CCTV conflict management rules.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *                                      (ie if the given PEC is not active or not valid)
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception TrainCctvConflictException if the train can not be activated due to conflict management rules
         * @exception TrainCctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train that the PEC is in progress on
         * @param car    The car the PEC is active in
         * @param pec    The PEC that is active
         * @param sessionId    The operator performing the action
         */
        CctvTypes::TrainCameraSequence activatePecCamera( CommonTypes::TrainIdType trainId,
                                                          ProtocolPecTypes::EPecCarNumber car,
                                                          ProtocolPecTypes::EPecNumber pec,
                                                          const std::string& sessionId );


        /**
         * This will activate the correct CCTV image for a PEC on a train.
         * It follows all the standard CCTV conflict management rules.
         * If this train is delocalised, it allows the operator to select the location of the train manually.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *                                      (ie if the given PEC is not active or not valid)
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception TrainCctvConflictException if the train can not be activated due to conflict management rules
         * @exception TrainCctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train that the PEC is in progress on
         * @param car    The car the PEC is active in
         * @param pec    The PEC that is active
         * @param trainLocation    Than manually determined location of the train
         * @param sessionId    The operator performing the action
         */
        CctvTypes::TrainCameraSequence activatePecCameraDelocalisedTrain( CommonTypes::TrainIdType trainId,
                                                                          ProtocolPecTypes::EPecCarNumber car,
                                                                          ProtocolPecTypes::EPecNumber pec,
                                                                          unsigned long trainLocation,
                                                                          const std::string& sessionId );


        /**
         * Resets the PEC on the train.
         * If this PEC is in progress, the PEC call will be ended and the PEC reset.
         * The PEC will be considered reset once the command executes successfully.
         * If this PEC is not valid, then the actual reset will not be sent.
         * The PEC will simply be removed from the list of active PEC.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *                                      (ie if the given PEC is not active or not valid)
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception PecException if there was an error performing a PEC operation
         *
         * @param trainId    The train to reset the PEC on
         * @param car    The car
         * @param pec    The PEC number
         * @param sessionId    The operator performing the reset
         */
        void resetPec( CommonTypes::TrainIdType trainId,
                       ProtocolPecTypes::EPecCarNumber car,
                       ProtocolPecTypes::EPecNumber pec,
					   const std::string& sessionId, PecTypes::EPecTrainSource pecTrainSource,
					   bool bCallTerminated = false);


        /**
         * This is the same as resetPec except it applies to all PEC on a particular train.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *                                      (ie if the given PEC is not active or not valid)
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception PecException if there was an error performing a PEC operation
         *
         * @param trainId    The train to reset the PEC on
         * @param sessionId    The operator doing the reset
         */
        void resetAllPec( CommonTypes::TrainIdType trainId,
						  const std::string& sessionId, 
						  PecTypes::EPecTrainSource pecTrainSource,
						  bool bCallTerminated = false);


        /**
         * Hold the PEC on the train.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *                                      (ie if the given PEC is not active or not valid)
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception PecException if there was an error performing a PEC operation
         *
         * @param trainId    The train to hold the PEC on
         * @param car    The car
         * @param pec    The PEC number
         * @param sessionId    The operator performing the hold
         */
        void holdPec( CommonTypes::TrainIdType trainId,
                       ProtocolPecTypes::EPecCarNumber car,
                       ProtocolPecTypes::EPecNumber pec,
					   const std::string& sessionId, PecTypes::EPecTrainSource pecTrainSource );


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
        struct PecCorbaObject;


        /**
         *  private constructor
         */
        TrainPecCorbaProxy();


        /**
         * Called to subscribe to updates from the agent
         */
        void subscribeToMessages();


        /**
         * Called to unsubscribe to updates from the agent
         */
        void unsubscribeToMessages();


        /**
         * standard named object for access to the CORBA servant.
         */
        PecCorbaObject* m_pecCorbaObject;


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
     * @created 22-Oct-2007 4:46:59 PM
     */
    typedef boost::shared_ptr< TrainPecCorbaProxy > TrainPecCorbaProxyPtr;

}

#endif // !defined(TrainPecCorbaProxy_H)
