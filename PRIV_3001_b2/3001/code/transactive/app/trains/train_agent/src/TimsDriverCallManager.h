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
#if !defined(TimsDriverCallManager_H)
#define TimsDriverCallManager_H

#include "app/trains/train_agent/src/ICallEndObserver.h"
#include "app/trains/train_agent/src/ITimsDriverCallManager.h"
#include "app/trains/train_agent/src/ITimsDriverCallManagerTaskCallback.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"
#include "app/trains/train_agent/src/RadioAccess.h"

#include "bus/trains/TrainProtocolLibrary/src/ChangeoverStatusEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/EndOfOccCallEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/RequestForOccCallEvent.h"
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
    class IRadioAgentInterface;
    class RadioMessageProcessor;
    class IRadioGroupCoordinator;
    class IAuditSender;
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
     * @created 01-Apr-2008 2:48:08 PM
     */
    class TimsDriverCallManager : public ITimsDriverCallManager,
                                  public TA_IRS_Bus::ITrainEventProcessor,
                                  public ICallEndObserver,
                                  public TA_IRS_Bus::ITrainStateUpdateListener,
                                  public ITimsDriverCallManagerTaskCallback
    {

    public:

        /**
         * Constructor
         *
         * @param configuration
         * @param statusManager
         * @param transactionManager
         * @param operationModeManager
         * @param messageSender    Used to send client updates
         * @param rightsChecker
         * @param auditSender
         * @param radioAgent    Used to perform radio functions (calls, DGNA etc)
         * @param radioMessageProcessor    Used to subscribe to radio updates
         * @param radioGroupCoordinator    Used to lock and unlock GTSI groups for group train calls
         */
        TimsDriverCallManager( ITrainAgentConfiguration& configuration,
                               ITrainStatusManager& statusManager,
                               TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                               IOperationModeManager& operationModeManager,
                               IMessageSender& messageSender,
                               IRightsChecker& rightsChecker,
                               IAuditSender& auditSender,
                               IRadioAgentInterface& radioAgent,
                               RadioMessageProcessor& radioMessageProcessor,
                               IRadioGroupCoordinator& radioGroupCoordinator,
							   RadioAccess& radioAccess);


        /**
         * Destructor
         */
        virtual ~TimsDriverCallManager();


        /**
         * FlexibleTimeoutNamedObject calls this method to calculate the
         * timeout value based on the current configuration
         */
        unsigned short getFlexibleTimeOutValue();


        /**
         * This will retrieve all active OCC call requests.
         *
         * @return a sequence of OCC call requests
         */
        TA_IRS_Bus::CallTypes::OccCallRequestList getAllOccCallRequests();


        /**
         * This will answer the outstanding OCC call request for the given train.
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or there is no outstanding call request for this train
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainRadioException if the radio call failed
         *
         * @param trainId    The train whos OCC call request we are going to answer.
         * @param sessionId    The session ID of the operator answering the call
         */
        virtual void answerOccCallRequest( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                           const std::string& sessionId );


        /**
         * Sets up a call to a train or a group of trains.
         * This will set up a call the given trains, and make a DGNA containing the
         * TSI of each successfully set up train, and also the individual subscribers given.
         * It will then call the resulting group.
         *
         * If only one train is given, or only one train succeeds in the setup
         * (and there are no individual subscribers) a direct call will be made to the train.
         *
         * @return the overall result of the operation, including the result of each specific train
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainRadioException if the DGNA or radio call failed
         *
         * @param trainList    A list of trains that the call will go to
         * @param individualSubscribers    These are the non-train subscribers that will be included in the DGNA.
         * @param override    Whether to send a request or an override
         * @param callType    The type of radio call to make (eg acknowledged/unacknowledged)
         *                    This can be overridden by the train agent if the given type does not
         *                    match the call that will be made (ie Single call is given and a group will be called)
         * @param callPriority    The call priority for the radio call
         * @param sessionId    The operator making the call
         */
        virtual TA_IRS_Bus::CallTypes::DriverCallResult makeDriverCall( const TA_IRS_Bus::CommonTypes::TrainIdList& trainList,
                                                                        const TA_IRS_Bus::CallTypes::ItsiList& individualSubscribers,
                                                                        bool override,
                                                                        TA_IRS_Bus::CallTypes::ECallType callType,
                                                                        TA_IRS_Bus::CallTypes::ECallPriority callPriority,
                                                                        const std::string& sessionId );


        /**
         * Sets up a call to a train or a group of trains.
         * This will set up a call the given trains, and when successful call the group tsi given.
         *
         * @return the overall result of the operation, including the result of each specific train
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception GroupInUseException if the given group TSI is in use
         * @exception TrainRadioException if the DGNA or radio call failed
         *
         * @param trainList    A list of trains that the call will go to
         * @param groupTsi    This is the TSI of the group to call.
         *                    If this is not specified, and more than one train is given then a DGNA will be used.
         * @param override    Whether to send a request or an override
         * @param callType    The type of radio call to make (eg acknowledged/unacknowledged)
         *                    This can be overridden by the train agent if the given type does not
         *                    match the call that will be made (ie Single call is given and a group will be called)
         * @param callPriority    The call priority for the radio call
         * @param sessionId    The operator making the call
         */
        virtual TA_IRS_Bus::CallTypes::DriverCallResult makeDriverCall( const TA_IRS_Bus::CommonTypes::TrainIdList& trainList,
                                                                        const std::string& groupTsi,
                                                                        bool override,
                                                                        TA_IRS_Bus::CallTypes::ECallType callType,
                                                                        TA_IRS_Bus::CallTypes::ECallPriority callPriority,
                                                                        const std::string& sessionId );


        /**
         * Thsi will process incoming train events this object is subscribed to
         *
         * @param event
         */
        virtual void processEvent( TA_IRS_Bus::TrainEventPtr event );


        /**
         * Receives changes to train states this object is subscribed to
         *
         * @param state
         */
        virtual void processTrainStateChanged( TA_IRS_Bus::TrainStatePtr state );


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
         * The operation mode of the agent has changed.
         *
         * When going to control for the first time, current state must be sent to clients
         *
         * @param operationState    The new operation mode
         */
        virtual void notifyOperationStateChange( TA_Base_Core::EOperationMode operationState );


    private:


        /**
         * Private default constructor
         */
        TimsDriverCallManager();


        /**
         * This is a handler for incoming Request for OCC Call events
         *
         * @param event    The incoming event from the train
         */
        void processRequestForOccCallEvent( TA_IRS_Bus::RequestForOccCallEventPtr event );


        /**
         * This is a handler for incoming End of OCC Call events
         *
         * @param event    The incoming event from the train
         */
        void processEndOfOccCallEvent( TA_IRS_Bus::EndOfOccCallEventPtr event );


        /**
         * This is a handler for incoming MPU changeover events
         *
         * @param event    The incoming event from the train
         */
        void processChangeoverStatusEvent( TA_IRS_Bus::ChangeoverStatusEventPtr event );


        /**
         * This will perform checks to ensure that the operator action is allowed (either calling or answering a train).
         *
         * @exception InvalidTrainException if the train ID given is not known or there is no outstanding call request for this train
         *
         * @param trainId    The train to check
         */
        void performPreCallChecks( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This will end the call for a given train.
         * It does not deal with any TIMS protocol, it just considers the train to be reset in terms of call functionality.
         * It clears the state of any calls or incoming calls.
         * In the case of a call in progress, it will end the call only if the given train is the only train involved in the call.
         *
         * @param trainId    The train to end the call for
         * @param timestamp    The optional timestamp of the triggering event
         */
        void endRadioCallForSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
										 bool bEndDueToFailure,
                                         time_t timestamp = 0 );


        /**
         * This will schedule a task to reset the call on a train.
         * The task will call performTimsCallReset
         *
         * @param trainId    The train to schedule the task for
         */
        void scheduleCallResetTask( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This is called to send the required command to TIMS once a call to a train has ended.
         * The call has already ended, so there is no command sent to the radio to end the call
         * It clears the state of any calls or incoming calls.
         * It will then send the end of voice call command to the train
         *
         * This is either called internally or from the CallResetTask,
         * it can be declared private here so only those with a pointer to
         * the interface declaring this public can call this.
         *
         * @param trainId    The train to end the call for
         */
        virtual void performTimsCallReset( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This is called to send the required command to TIMS to set up a call to a train.
         * The command is sent to the train, and the response processed.
         * The given request object is updated with the status.
         *
         * This is either called internally or from the CallResetTask,
         * it can be declared private here so only those with a pointer to
         * the interface declaring this public can call this.
         *
         * @param voiceCallRequest    The object containing all information for the request,
         *                            and which will also be used to store the result.
         */
        virtual void performTimsCallSetup( TrainVoiceCallRequestPtr voiceCallRequest );


        /**
         * This will send the commands to all trains to be involved in the call and wait for the results.
         *
         * @return the result of the command sent to all trains
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trainList    The list of trains to call
         * @param override    Whether a normal request or override
         * @param groupCall    Whether this call will be a group or a single call
         * @param sessionId    The operator making the call
         */
        TrainVoiceCallRequestList setupDriverCall( const TA_IRS_Bus::CommonTypes::TrainIdList& trainList,
                                                   bool override,
                                                   bool groupCall,
                                                   const std::string& sessionId );
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
         * Used to subscribe to Radio Updates
         */
        RadioMessageProcessor& m_radioMessageProcessor;


        /**
         * Used to schedule work items
         */
        TA_Base_Core::ThreadPoolSingletonManager& m_threadPool;


        /**
         * Used to lock and unlock GTSI groups for group train calls
         */
	     IRadioGroupCoordinator& m_radioGroupCoordinator;

		 //IRadioAgentInterface& m_radioAgent;
		 RadioAccess& m_radioAccess;

    };

} // TA_IRS_App

#endif // !defined(TimsDriverCallManager_H)
