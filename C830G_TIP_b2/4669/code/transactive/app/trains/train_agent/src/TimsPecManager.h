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
#if !defined(TimsPecManager_H)
#define TimsPecManager_H


#include "app/trains/train_agent/src/IAtsChangeProcessor.h"
#include "app/trains/train_agent/src/ICallEndObserver.h"
#include "app/trains/train_agent/src/ITimsPecManager.h"

#include "bus/trains/TrainProtocolLibrary/src/ChangeoverStatusEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecActivatedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecReadyForConversationEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecRequestForResetEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecResetEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecSelectedByDriverEvent.h"
#include "bus/trains/TrainTransactionModel/src/ITrainEventProcessor.h"
#include "bus/trains/TrainTransactionModel/src/ITrainStateUpdateListener.h"
//#include "bus/managed_agent/src/IOperationStateCallback.h"

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
    class ITrainStatusManager;
    class IOperationModeManager;
    class IMessageSender;
    class IRightsChecker;
    class IAuditSender;
    class IRadioAgentInterface;
    class RadioMessageProcessor;
    class AtsMessageProcessor;
    class ITimsCctvManager;
}

namespace TA_IRS_Bus
{
    class ITrainTransactionManager;
}

namespace TA_Base_Core
{
    class ThreadPoolSingletonManager;
}


namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:12 PM
     */
    class TimsPecManager : public IAtsChangeProcessor,
                           public ITimsPecManager,
                           public ICallEndObserver,
                           public TA_IRS_Bus::ITrainStateUpdateListener,
                           public TA_IRS_Bus::ITrainEventProcessor
    {

    public:

        /**
         * Constructor
         *
         * @param configuration    used to access agent configuration
         * @param statusManager    used to set and get train state
         * @param transactionManager    used to send and register for messages from trains
         * @param operationModeManager    used to query and register for operation mode changes
         * @param messageSender    used to send client updates
         * @param rightsChecker    used to check rights for operator actions
         * @param auditSender    used to send audit messages for operator actions
         * @param radioAgent    used to perform radio operations
         * @param radioMessageProcessor    used to subscribe to Radio Updates
         * @param atsMessageProcessor    used to subscribe for ATS messages
         * @param cctvManager    used to activate PEC cameras
         */
        TimsPecManager( ITrainAgentConfiguration& configuration,
                        ITrainStatusManager& statusManager,
                        TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                        IOperationModeManager& operationModeManager,
                        IMessageSender& messageSender,
                        IRightsChecker& rightsChecker,
                        IAuditSender& auditSender,
                        IRadioAgentInterface& radioAgent,
                        RadioMessageProcessor& radioMessageProcessor,
                        AtsMessageProcessor& atsMessageProcessor,
                        ITimsCctvManager& cctvManager );


        /**
         * Destructor
         */
        virtual ~TimsPecManager();


        /**
         * FlexibleTimeoutNamedObject calls this method to calculate the timoeut value based on the current configuration
         */
        virtual unsigned short getFlexibleTimeOutValue();


        /**
         * Gets all active PEC for all trains, and their status.
         *
         * @return all active PEC for all trains managed by this agent
         */
        virtual TA_IRS_Bus::PecTypes::PecUpdateList getAllActivePec();


        /**
         * This will start the PEC answer sequence, and make the audio call.
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous (ie if the given PEC is not active or not valid)
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
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
        virtual void answerPec( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                const std::string& sessionId );


        /**
         * This will activate the correct CCTV image for a PEC on a train.
         * It follows all the standard CCTV conflict management rules.
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous (ie if the given PEC is not active or not valid)
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train that the PEC is in progress on
         * @param car    The car the PEC is active in
         * @param pec    The PEC that is active
         * @param sessionId    The operator performing the action
         */
        virtual TA_IRS_Bus::CctvTypes::TrainCameraSequence activatePecCamera( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                                              TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                                                              TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                                                              const std::string& sessionId );


        /**
         * This will activate the correct CCTV image for a PEC on a train.
         * It follows all the standard CCTV conflict management rules.
         * If this train is delocalised, it allows the operator to select the location of the train manually.
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous (ie if the given PEC is not active or not valid)
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train that the PEC is in progress on
         * @param car    The car the PEC is active in
         * @param pec    The PEC that is active
         * @param trainLocation    Than manually determined location of the train
         * @param sessionId    The operator performing the action
         */
        virtual TA_IRS_Bus::CctvTypes::TrainCameraSequence activatePecCameraDelocalisedTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                                                              TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                                                                              TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                                                                              unsigned long trainLocation,
                                                                                              const std::string& sessionId );


        /**
         * Resets the PEC on the train.
         * If this PEC is in progress, the PEC call will be ended and the PEC reset. The PEC will be considered reset once the command executes successfully.
         * If this PEC is not valid, then the actual reset will not be sent. The PEC will simply be removed from the list of active PEC.
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous (ie if the given PEC is not active or not valid)
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception PecException if there was an error performing a PEC operatio
         *
         * @param trainId    The train to reset the PEC on
         * @param car    The car
         * @param pec    The PEC number
         * @param sessionId    The operator performing the reset
         */
        virtual void resetPec( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                               TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                               TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                               const std::string& sessionId );


        /**
         * This is the same as resetPec except it applies to all PEC on a particular train.
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception PecException if there was an error performing a PEC operatio
         *
         * @param trainId    The train to reset the PEC on
         * @param sessionId    The operator doing the reset
         */
        virtual void resetAllPec( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                  const std::string& sessionId );


        /**
         * This handles a call end event.
         * It can do any required processing to clean up on unexpected call termination.
         * This is called in its own thread, so processing can be done.
         *
         * @param callEnd    The details of the call that ended
         * @param trainList    The trains involved in the radio call
         */
        virtual void processCallEndEvent( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callEnd,
                                          const TA_IRS_Bus::CommonTypes::TrainIdList& trainList );


        /**
         * Implemented by the concrete class to handle state changes.
         *
         * @param state
         */
        virtual void processTrainStateChanged( TA_IRS_Bus::TrainStatePtr state );


        /**
         * This will process the changes between the new and old ats information
         *
         * @param oldInformation    The previous information
         * @param newInformation    The latest updated information
         */
        virtual void processAtsUpdate( const AtsTrainInformation& oldInformation,
                                       const AtsTrainInformation& newInformation );


        /**
         * The operation mode of the agent has changed.
         *
         * When going to control for the first time, current state must be sent to clients
         *
         * @param operationState    The new operation mode
         */
        virtual void notifyOperationStateChange( TA_Base_Core::EOperationMode operationState );


        /**
         * The event handler for incoming train events this object is registered for
         *
         * @param event    PecActivatedEventPtr
         */
        virtual void processEvent( TA_IRS_Bus::TrainEventPtr event );


    private:


        /**
         * Private default constructor
         */
        TimsPecManager();


        /**
         * Processes incoming PEC Activated events
         *
         * @param event    The incoming event
         */
        void processPecActivatedEvent( TA_IRS_Bus::PecActivatedEventPtr event );


        /**
         * Processes incoming PEC ready for conversation events
         *
         * @param event    The incoming event
         */
        void processPecReadyForConversationEvent( TA_IRS_Bus::PecReadyForConversationEventPtr event );


        /**
         * Processes incoming PEC request fir reset events
         *
         * @param event    The incoming event
         */
        void processPecRequestForResetEvent( TA_IRS_Bus::PecRequestForResetEventPtr event );


        /**
         * Processes incoming PEC reset events
         *
         * @param event    The incoming event
         */
        void processPecResetEvent( TA_IRS_Bus::PecResetEventPtr event );


        /**
         * Processes incoming PEC answered by driver events
         *
         * @param event    The incoming event
         */
        void processPecSelectedByDriverEvent( TA_IRS_Bus::PecSelectedByDriverEventPtr event );


        /**
         * Processes incoming MPU changeover events
         *
         * @param event    The incoming event
         */
        void processChangeoverStatusEvent( TA_IRS_Bus::ChangeoverStatusEventPtr event );


        /**
         * This will perform checks to ensure that the operator action is allowed
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the agent isnt in control of this train
         *
         * @param trainId    The train to check
         */
        void performPreOperatorActionChecks( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                             const std::string& sessionId );


        /**
         * This checks to see if the train is within this agents control area, or if it is communicating with this agent.
         *
         * @return if either condition is met
         *
         * @param trainId    The train to check
         */
        bool checkPecValidity( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This checks to see if the train is communicating with this agent.
         *
         * @return if the train is still communicating with this agent
         *
         * @param trainId    The train to check
         */
        bool checkTrainCommunicationValidity( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This will invalidate all PEC on the given train
         *
         * @param trainId    The train to invalidate
         */
        void invalidatePecForTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * ends the radio call for a PEC if it was made
         */
        void endRadioCall( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                             TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                             TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                             const std::string& sessionId );


        /**
         * Checks if the number of retries has been exceeded and throws the exception
         * if they have, otherwise just increments the retry count
         */
        template< typename ExceptionType >
        void checkRetries( unsigned long& retries, ExceptionType& e );


        /**
        * Used to access agent configuration
        */
        ITrainAgentConfiguration& m_configuration;


        /**
         * Used to set and get train state
         */
        ITrainStatusManager& m_statusManager;


        /**
         * Used to send and register for messages from trains
         */
        TA_IRS_Bus::ITrainTransactionManager& m_transactionManager;


        /**
         * Used to query and register for operation mode changes
         */
        IOperationModeManager& m_operationModeManager;


        /**
         * Used to send client updates
         */
        IMessageSender& m_messageSender;


        /**
         * Used to check rights for operator actions
         */
        IRightsChecker& m_rightsChecker;


        /**
         * Used to send audit messages for operator actions
         */
        IAuditSender& m_auditSender;


        /**
         * Used to perform radio operations
         */
        IRadioAgentInterface& m_radioAgent;


        /**
         * Used to subscribe for ATS messages
         */
        AtsMessageProcessor& m_atsMessageProcessor;


        /**
         * Used to subscribe to Radio Updates
         */
        RadioMessageProcessor& m_radioMessageProcessor;


        /**
         * Used to schedule work items
         */
        TA_Base_Core::ThreadPoolSingletonManager& m_threadPool;


        /**
         * Used to activate PEC cameras
         */
        ITimsCctvManager& m_cctvManager;

    };

}
#endif // !defined(TimsPecManager_H)
