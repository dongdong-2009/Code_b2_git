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
#if !defined(TimsStatusManager_H)
#define TimsStatusManager_H

#include "app/trains/train_agent/src/IAtsChangeProcessor.h"
#include "app/trains/train_agent/src/IActivityTimerHandler.h"
#include "app/trains/train_agent/src/IOperationModeObserver.h"
#include "app/trains/train_agent/src/TrainCommsState.h"

#include "bus/trains/TrainTransactionModel/src/ITrainEventProcessor.h"
#include "bus/trains/TrainProtocolLibrary/src/TestCallEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/AtcEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/Oa1Event.h"
#include "bus/trains/TrainProtocolLibrary/src/ChangeoverStatusEvent.h"

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

//#include "bus/managed_agent/src/IOperationStateCallback.h"


// forward declarations
namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
    class ITrainStatusManager;
    class IOperationModeManager;
    class ITimsModeManagerInternal;
    class IAlarmUtility;
    class IMessageSender;
    class IStateUpdateMessageSender;
    class AtsMessageProcessor;
    class IAtsAgentInterface;
    class TrainActivityMonitor;
}

namespace TA_IRS_Bus
{
    class ITrainTransactionManager;
}

namespace TA_IRS_App
{

    /**
     * This manages changing trains betwwen Depot and Mainline mode and between normal and degraded modes.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:14 PM
     */
    class TimsStatusManager : public IAtsChangeProcessor,
                              public IActivityTimerHandler,
                              public IOperationModeObserver,
                              public TA_IRS_Bus::ITrainEventProcessor
    {

    public:

        /**
         * Constructor
         *
         * @param configuration    The agent configuration object
         * @param statusManager    The object holding all train state
         * @param transactionManager    The object used to communicate with trains
         * @param operationModeManager    The object used to check agent operational control mode
         * @param modeManager    Used to trigger the sending of normal/degraded mode to a train
         * @param alarms    The object used to raise and close alarms
         * @param messageSender    The object used to send client updates
         * @param stateUpdateSender    The object used to send messages to other train agents
         * @param atsMessageProcessor    The object used to receive ATS updates
         * @param atsAgent    The object used to send information to ATS
         */
        TimsStatusManager( ITrainAgentConfiguration& configuration,
                           ITrainStatusManager& statusManager,
                           TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                           IOperationModeManager& operationModeManager,
                           ITimsModeManagerInternal& modeManager,
                           IAlarmUtility& alarms,
                           IMessageSender& messageSender,
                           IStateUpdateMessageSender& stateUpdateSender,
                           AtsMessageProcessor& atsMessageProcessor,
                           IAtsAgentInterface& atsAgent );


        /**
         * Destructor
         */
        virtual ~TimsStatusManager();


        /**
         * The operation mode of the agent has changed.
         *
         * When entering control mode the timers for trains in this agents control areas need to be scheduled.
         *
         * @param operationState    The new operation mode
         */
        virtual void notifyOperationStateChange( TA_Base_Core::EOperationMode operationState );


        /**
         * This will process the changes between the new and old ats information
         *
         * @param oldInformation    The previous information
         * @param newInformation    The latest updated information
         */
        virtual void processAtsUpdate( const AtsTrainInformation& oldInformation,
                                       const AtsTrainInformation& newInformation );


        /**
         *
         * @param trainId
         */
        virtual void processOa1Timeout( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         *
         * @param trainId
         */
        virtual void processAtcTimeout( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


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
         * This will process the events from trains that this object is subscribed for.
         *
         * @param event
         */
        virtual void processEvent( TA_IRS_Bus::TrainEventPtr event );


    private:


        /**
         * Private default constructor
         */
        TimsStatusManager();


        /**
         * This will process a test call from a train, sent on train wakeup.
         *
         * @param testCallEvent    The test call event
         */
        void processTestCallEvent( TA_IRS_Bus::TestCallEventPtr testCallEvent,
                                   TrainCommsStatePtr commsState );


        /**
         * This will process an OA1 table from a train.
         *
         * @param oa1Event    The OA1 table
         */
        void processOa1Event( TA_IRS_Bus::Oa1EventPtr oa1Event,
                              TrainCommsStatePtr commsState );


        /**
         * This will process an ATC table from a train.
         *
         * @param atcEvent    The ATC table event
         */
        void processAtcEvent( TA_IRS_Bus::AtcEventPtr atcEvent,
                              TrainCommsStatePtr commsState );


        /**
         * This will process an MPU changeover from a train, sent when the primary MPU fails and the secondary takes over.
         *
         * @param changeoverEvent    The MPU changover event
         *
         * @return true if the MPU that sent this is the new primary MPU
         */
        bool processChangeoverStatusEvent( TA_IRS_Bus::ChangeoverStatusEventPtr changeoverEvent,
                                           TrainCommsStatePtr commsState );


        /**
         * This will close all communication alarms for a train when this agent is no longer in control of the train for some reason.
         *
         * @param trainId    The train to close alarms for
         */
        void closeAlarmsForTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This is called when a train in the control area of this agent has had its validity updated.
         * If appropriate, this will reset its state - it is called in the case a train has been shut down.
         *
         * @param trainId    The train that may have been shut down
         */
        void resetTrainState( TrainCommsStatePtr commsState );


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
         * Used solely for train wakeup at station. In this case a change mode must be triggered.
         */
        ITimsModeManagerInternal& m_modeManager;


        /**
         * Used to raise and close alarms
         */
        IAlarmUtility& m_alarms;


        /**
         * Used to send client updates
         */
        IMessageSender& m_messageSender;


        /**
         * Used to send non duty train tsi messages to other train agents
         */
        IStateUpdateMessageSender& m_stateUpdateSender;


        /**
         * Used to subscribe for ATS messages
         */
        AtsMessageProcessor& m_atsMessageProcessor;


        /**
         * Used to forward OA1/ATC tables to ATS
         */
        IAtsAgentInterface& m_atsAgent;


        /**
         * Used to keep track of train activity/inactivity
         */
        TrainActivityMonitor* m_activityMonitor;


        /**
         * Whether the ATS agent comms are ok, used to tell if an alarm needs to be raised or closed
         */
        bool m_atsAgentCommsOk;

    };

} // TA_IRS_App

#endif // !defined(TimsStatusManager_H)
