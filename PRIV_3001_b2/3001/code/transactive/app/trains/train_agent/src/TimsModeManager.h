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
#if !defined(TimsModeManager_H)
#define TimsModeManager_H

#include "app/trains/train_agent/src/IAtsChangeProcessor.h"
#include "app/trains/train_agent/src/ITimsModeManager.h"
#include "app/trains/train_agent/src/IOperationModeObserver.h"
#include "app/trains/train_agent/src/ITimsModeManagerInternal.h"
#include "app/trains/train_agent/src/ITimsModeManagerTaskCallback.h"
#include "app/trains/train_agent/src/TimsModeTasks.h"

#include "bus/trains/TrainTransactionModel/src/ITrainEventProcessor.h"
#include "bus/trains/TrainProtocolLibrary/src/AtcEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/Oa1Event.h"
//#include "bus/managed_agent/src/IOperationStateCallback.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

// forward declarations
namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
    class ITrainStatusManager;
    class IOperationModeManager;
    class IAlarmUtility;
    class IRightsChecker;
    class IAuditSender;
    class AtsMessageProcessor;
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
     * This manages changing trains betwwen Depot
     * and Mainline mode and between normal and degraded modes.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:09 PM
     */
    class TimsModeManager : public IAtsChangeProcessor,
                            public ITimsModeManager,
                            public IOperationModeObserver,
                            public ITimsModeManagerInternal,
                            public TA_IRS_Bus::ITrainEventProcessor,
                            public ITimsModeManagerTaskCallback
    {

    public:

        /**
         * Constructor
         *
         * @param configuration
         * @param statusManager
         * @param transactionManager
         * @param operationModeManager
         * @param alarms
         * @param rightsChecker
         * @param auditSender
         * @param atsMessageProcessor
         */
        TimsModeManager( ITrainAgentConfiguration& configuration,
                         ITrainStatusManager& statusManager,
                         TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                         IOperationModeManager& operationModeManager,
                         IAlarmUtility& alarms,
                         IRightsChecker& rightsChecker,
                         IAuditSender& auditSender,
                         AtsMessageProcessor& atsMessageProcessor );


        /**
         * Destructor
         */
        virtual ~TimsModeManager();


        /**
         * This will manually force train communications over to this train agent.
         * It is only to be used in failure scenarios, and incorrect use could result
         * in the situation automatically normalising (or attempting to normalise)
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         *
         * @param trainId    The train ID to manually take
         * @param sessionId    The operator taking control
         */
        virtual void manuallyTakeTrainControl( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                               const std::string& sessionId );


        /**
         * This will signal the train agent to take control of this train by sending a change mode command to it.
         *
         * @param trainId    The train ID to manually take
         */
        virtual void signalChangeMode( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This will process the changes between the new and old ats information
         *
         * @param oldInformation    The previous information
         * @param newInformation    The latest updated information
         */
        virtual void processAtsUpdate( const AtsTrainInformation& oldInformation,
                                       const AtsTrainInformation& newInformation );


        /**
         * This is called when the train agent has acquired the control of a location.
         * All trains within the location are now under this agent's control.
         *
         * @param locationKey    This is the location key of the newly acquired location
         */
        virtual void locationControlAcquired( unsigned long locationKey );


        /**
         * This is called when the train agent has released the control of a location.
         * All trains within the location are now under another agent's control.
         *
         * @param locationKey    This is the location key of the recently released location
         */
        virtual void locationControlReleased( unsigned long locationKey );


        /**
         * This is received when a train at some location is not communicating with the correct train agent.
         * This agent will check if it should be the agent that the train is communicating with,
         * and it it is, it will spawn off a task to make the train communicate with this agent instead.
         *
         * @param event    The event to process
         */
        void processNonDutyTrainTSI( const TA_IRS_Bus::StateUpdateTypes::NonDutyTrainTSI& event );


        /**
         * The operation mode of the agent has changed.
         *
         * @param operationState    The new operation mode
         */
        virtual void notifyOperationStateChange( TA_Base_Core::EOperationMode operationState );


        /**
         * This is used to close radio mode alarms once a train starts to communicate with this server again.
         * It will also start any waiting change area tasks.
         *
         * @param event
         */
        virtual void processEvent( TA_IRS_Bus::TrainEventPtr event );


    private:


        /**
         * Private default constructor
         */
        TimsModeManager();


        /**
         * This will send a change mode command to all trains not communicating with this agent
         * within all locations under the control of this agent (if one is not already in progress)
         */
        void takeControlOfTrainsAtAllOwnedLocations();


        /**
         * This will send a change mode command to all trains not communicating with this agent
         * within the given location (if one is not already in progress)
         *
         * @param location    The location to take control of
         */
        void takeControlOfTrainsAtLocation( unsigned long location );


        /**
         * This will send a change mode command to all trains not communicating with this agent
         * within the given location set (if one is not already in progress)
         *
         * @param locations    The locations to take control of
         */
        void takeControlOfTrainsAtLocations( const std::set< unsigned long >& locations );


        /**
         * This will send a change mode command to a train if it is not communicating with this agent.
         * (if one is not already in progress)
         *
         * @param trainId    The train to take control of
         */
        void takeControlOfTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This will schedule a change mode task for the given train if not already scheduled
         * It will also optionally add the observer to the scheduled or already running task,
         * which will be called back when the task is complete
         *
         * @param trainId    the train the task is for
         * @param observer    The observer to notify of task completion
         */
        void scheduleChangeModeTask( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                     bool changeMode,
                                     bool changeArea,
                                     ChangeModeCommandObserver* observer = NULL );


        /**
         * This will cancel any change mode or change area commands currently in progress.
         */
        void cancelTakeControlOfTrains();


        /**
         * This will cancel any ongoing take control commands for all trains within the given location
         *
         * @param location    The location to take control of
         */
        void cancelTakeControlOfTrainsAtLocation( unsigned long location );


        /**
         * This will cancel any ongoing take control commands for the given train.
         * It is used in the case a train has left the control area, or the agent has changed modes etc
         *
         * @param trainId    The train to take control of
         */
        void cancelTakeControlOfTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This will open a radio mode alarm for the given train if one is not already open
         *
         * @param trainId    the train to close the alarm for
         */
        void raiseRadioModeAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This will close any open radio mode alarms for the given train
         *
         * @param trainId    the train to close the alarm for
         */
        void closeRadioModeAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This will execute the Change Mode task for the given train.
         * (Send the messages and process the response).
         *
         * Note this is public in the interface ITimsModeManagerTaskCallback,
         * but private here. This is valid C++ and means that only the tasks
         * see this as a public method, where all other object having a reference
         * to TimsModeManager see these as private.
         *
         * @return the result of the operation. If there was a failure, the error string will hold details.
         *
         * @param trainId    The train ID the action will be performed on
         * @param errorDetails    Optional error details if errors were encountered
         */
        virtual EChangeModeCommandResult performChangeMode( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                            std::string& errorDetails );


        /**
         * This will execute the Change Area task for the given train. (Send the messages and process the response).
         *
         * Note this is public in the interface ITimsModeManagerTaskCallback, but private here.
         * This is valid C++ and means that only the tasks see this as a public method,
         * where all other object having a reference to TimsModeManager see these as private.
         *
         * @param trainId    The train ID the action will be performed on
         * @param errorDetails    Optional error details if errors were encountered
         */
        virtual EChangeModeCommandResult performChangeArea( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                            std::string& errorDetails );


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
         * Used to raise and close alarms
         */
        IAlarmUtility& m_alarms;


        /**
         * Used to check rights for operator actions
         */
        IRightsChecker& m_rightsChecker;


        /**
         * Used to send audit messages for operator actions
         */
        IAuditSender& m_auditSender;


        /**
         * Used to subscribe for ATS messages
         */
        AtsMessageProcessor& m_atsMessageProcessor;


        /**
         * Used to schedule work items
         */
        TA_Base_Core::ThreadPoolSingletonManager& m_threadPool;


        /**
         * The map of change mode tasks in progress
         */
        ChangeModeChangeAreaTaskMap m_changeModeTasks;


        /**
         * The lock for m_changeModeTasks
         */
        TA_Base_Core::NonReEntrantThreadLockable m_changeModeTaskLock;


        /**
         * The map of change area tasks scheduled or in progress
         */
        ChangeModeChangeAreaTaskMap m_changeAreaTasks;


        /**
         * The lock for m_changeAreaTasks
         */
        TA_Base_Core::NonReEntrantThreadLockable m_changeAreaTaskLock;

    };

} // TA_IRS_App

#endif // !defined(TimsModeManager_H)
