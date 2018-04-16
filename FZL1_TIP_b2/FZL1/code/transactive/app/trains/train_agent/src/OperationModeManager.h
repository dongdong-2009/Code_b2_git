/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source$
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $Date: 2012/06/12 $
 * Last modified by:  $Author: builder $
 *
 */
#if !defined(OperationModeManager_H)
#define OperationModeManager_H

#include "app/trains/train_agent/src/IOperationModeManager.h"
#include "app/trains/train_agent/src/IOperationModeManagerServantInterface.h"
#include "app/trains/train_agent/src/ISubsystemDutyStateObserver.h"

#include "bus/trains/TrainCommonLibrary/src/GenericObserverManager.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"

#include <queue>

// forward declarations
namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
    class IDutyAgentInterface;
    class IStateUpdateMessageSender;
    class IMessageSender;
    class ITrainStatusManager;
}

namespace TA_IRS_App
{

    /**
     * This handles the train operation modes and the way the train agent operates.
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:47:55 PM
     */
    class OperationModeManager :  public IOperationModeManager,
                                  public IOperationModeManagerServantInterface,
                                  public ISubsystemDutyStateObserver,
                                  public TA_IRS_Bus::GenericObserverManager<IOperationModeObserver>,
                                  public TA_Base_Core::Thread
    {

    public:

		/**
         * Constructor with required information
         *
         * @param configuration    The agent configuration object
         * @param stateUpdateSender    Used to send updates to peer agents
         * @param messageSender    Used to send updates to clients
         * @param statusManager    Used to query train state
         * @param dutyAgent    Used to access the local duty agent
         */
        OperationModeManager( ITrainAgentConfiguration& configuration,
                              IStateUpdateMessageSender& stateUpdateSender,
                              IMessageSender& messageSender,
                              ITrainStatusManager& statusManager,
                              IDutyAgentInterface& dutyAgent );


        /**
         * Destructor
         */
        virtual ~OperationModeManager();


        /**
         * This will register for operation mode updates.
         *
         * @param observer    The observer to add
         */
        virtual void registerForOperationModeChanges( IOperationModeObserver* observer );


        /**
         * This will deregister a previously registered observer for operation mode updates
         *
         * @param observer    The previously registered observer to remove
         */
        virtual void deregisterForOperationModeChanges( IOperationModeObserver* observer );

		/**
		* This method checks the current operation State and throws an exception if
		* the current State is NOT Control.
		* @exception TA_Base_Core::OperationStateException thrown if the current operating
		* State is NOT Control
		* 
		*/
		static void ensureControlState();

		/************************************************************************/
		/* just return if running in control, no exception                                                                     */
		/************************************************************************/
		static bool isRunningControl(void);

        /**
         * This will check the current agent state to determine if this agent is responsible for trains at the given location.
         *
         * @return true if trains at this locaiton are controlled by this agent, false otherwise.
         *
         * @param locationKey    The location to check
         */
        bool isInControlOfLocation( unsigned long locationKey ) const;


        /**
         * Given a train ID, this will check if this agent should be in control of it.
         * This will be true if the train is at one of this agent's controlled locations,
         * optionally if the train's location is unknown or its ats data is delocalised.
         *
         * @return whether this train can be controlled
         *
         * @param trainId    The train ID to check
         * @param allowDelocalised    Whether trains with invalid localisation are to be considered controllable, defaults to true
         * @param allowInvalid    Whether trains with invalid ATS are to be considered controllable, defaults to true
         */
        bool isInControlOfTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                 bool allowDelocalised = true,
                                 bool allowInvalid = true ) const;


        /**
         * Get the list of locations this agent is in control of.
         *
         * @return the list of locations this agent is responsible for
         */
        virtual std::set< unsigned long > getControlledLocations() const;


        /**
         * Checks if this agent is in degraded mode. This means the OCC is not contactable.
         *
         * @return true if in degraded mode, false otherwise
         */
        virtual bool isIscsDegradedMode() const;


        /**
         * Checks if the local radio base station is in fallback. This is only applicable at a station.
         *
         * @return true if in degraded mode, false otherwise
         */
        virtual bool isLocalRadioInFallback() const;


        /**
         * This gets the current agent operation mode.
         *
         * @return the current operation mode of the agent
         */
        virtual TA_IRS_Bus::TrainInformationTypes::AgentOperationMode getCurrentOperationMode() const;


        /**
         * Called by the radio agent to notify the local train agent that it has entered
         * or left radio fallback mode. If not already in local duty mode,
         * entering fallback will trigger local duty mode.
         *
         * @param fallback    Whether the radio system is in fallback or not (true is fallback, false is normal)
         */
        virtual void inFallback( bool fallback );


        /**
         * Called by the ATS agent to notify the local train agent that it has entered
         * or left double OCC ATS failure mode. If not already in local duty mode,
         * entering fallback will trigger local duty mode.
         *
         * @param doubleAtsFailure
         */
        virtual void inDoubleAtsFailure( bool doubleAtsFailure );


        /**
         * Checks whether the agent is in local control due to radio fallback or local duty.
         * This is only applicable at station locations.
         *
         * This is called by the ATS agent to determine if it should be exchanging ATS data locally,
         * or using OCC ATS to do the data exchange.
         *
         * @return true if ATS should be in local mode
         */
        virtual bool radioInFallback() const;


        /**
         * This will notify of the current state of the OCC servers.
         * It will tell if the OCC is available or not.
         *
         * @param occOnline    This is the status of the OCC, up (true) or down (false)
         */
        virtual void processOccStateChange( bool occOnline );


        /**
         * This processes a change of state for the train agent subsystem at a location.
         *
         * @param dutyState    The duty state that changed
         */
        virtual void processSubsystemStateChange( const SubsystemDutyState& dutyState );


        /**
         * This sets the current agent operation mode (from a sync update)
         *
         * @param operationMode    The agent operation mode received from a state update.
         *
         * @return true if the information was applied, false if it was old
         */
        virtual bool syncCurrentOperationMode( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& operationMode );


        /**
         * This run method will initially be used to get initial state, then it will
         * be used to send updates out to observers once the OperationModeManager is up and running.
         */
        virtual void run();

		//*****************************************************
		// TrainAgent IGenericAgentUser interface             *
		//*****************************************************
		void agentSetControl();
		void agentSetMonitor();

        /**
         * This will stop the thread if it is running.
         */
        virtual void terminate();


    protected:


        /**
         * This maps a list of controllable locations, to the current control state
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:47:55 PM
         */
        typedef std::map< unsigned long, bool > LocationControlMap;



        /**
         * This will do any initialisation required when the agent is in control mode.
         */
        virtual void controlModeInitialisation();


        /**
         * This will assume control of the given location if not already in control.
         * It will ensure that the given location is in the controllable locations before it assumes control
         * It will also ensure a state update is sent and observers notified if there are any changes.
         *
         * @return true if there was a change in state (means observers were notified and an update sent)
         *
         * @param locationKey    the location to assume control of
         */
        bool takeControlOfLocation( unsigned long locationKey );


        /**
         * This will release control of the given location if in control.
         * It will ensure a state update is sent and observers notified if there are any changes.
         *
         * @return true if there was a change in state (means observers were notified and an update sent)
         *
         * @param locationKey    the location to release control of
         */
        bool releaseControlOfLocation( unsigned long locationKey );


        /**
         * This will send messages out to peer agents and clients that the operation mode has changed.
         * It does not notify internal observers, only external.
         */
        void notifyStateChange() const;


        /**
         * This must be acquired for any state changes
         */
        mutable TA_Base_Core::ReEntrantThreadLockable m_stateLock;


        /**
         * Used to access agent configuration parameters
         */
        ITrainAgentConfiguration& m_configuration;


        /**
         * Used to send state updates to peer agents
         */
        IStateUpdateMessageSender& m_stateUpdateSender;


        /**
         * Used to send updates to clients
         */
        IMessageSender& m_messageSender;


        /**
         * Access to functions on the local duty agent
         */
        IDutyAgentInterface& m_dutyAgent;


        /**
         * The map of locations and whether this agent is in control
         * of them or not.
         */
        LocationControlMap m_locationControlMap;


        /**
         * The time the agent state last changed
         */
        time_t m_lastUpdateTime;


    private:

        /**
         * The type of update, control acquired or released.
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:47:55 PM
         */
        enum EOperationModeUpdateType
        {
            ControlAcquired = 0,
            ControlReleased = 1
        };


        /**
         * This is an update to queue and notify observers of an operation mode change at a location
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:47:55 PM
         */
        struct OperationModeUpdate
        {

            /**
             * The type of change
             */
            EOperationModeUpdateType updateType;


            /**
             * The location that was updated
             */
            unsigned long locationKey;

        };


        /**
         * A queue of updates to notify observers of
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:47:56 PM
         */
        typedef std::queue< OperationModeUpdate* > OperationModeUpdateQueue;


        /**
         * Private default constructor
         */
        OperationModeManager();


        /**
         * This will log the current control state of each location this agent can be responsible for.
         */
        void logLocationState() const;

		//*****************************************************
		// Operation modes                                    *
		//*****************************************************
		static bool m_runningControl;

        /**
         * Used to retrieve train state
         */
        ITrainStatusManager& m_statusManager;


        /**
         * Thread termination flag
         */
        volatile bool m_terminate;


        /**
         * Lock for the observer notification queue
         */
        TA_Base_Core::NonReEntrantThreadLockable m_queueLock;


        /**
         * A semaphore to control the queue processing thread
         */
        TA_Base_Core::Semaphore m_queueSemaphore;


        /**
         * A queue of state changes to notify observers of
         */
        OperationModeUpdateQueue m_updateQueue;

		/************************************************************************/
		/* add by hongzhi to monitor state change
		*/
		/************************************************************************/
		TA_Base_Core::Semaphore m_semaphore;
    };

}  // TA_IRS_App

#endif // !defined(OperationModeManager_H)
