/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision: #7 $
  *
  * Last modification: $Date: 2010/06/29 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/trains/train_agent/src/OperationModeManager.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/IDutyAgentInterface.h"
#include "app/trains/train_agent/src/IStateUpdateMessageSender.h"
#include "app/trains/train_agent/src/IMessageSender.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"
#include "app/trains/train_agent/src/IOperationModeObserver.h"
#include "app/trains/train_agent/src/TrainCommsState.h"

#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"
//#include "bus/managed_agent/src/AgentManager.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/timers/src/SingletonTimerUtil.h"


namespace TA_IRS_App
{
	bool OperationModeManager::m_runningControl = false;

    OperationModeManager::OperationModeManager( ITrainAgentConfiguration& configuration,
                                                IStateUpdateMessageSender& stateUpdateSender,
                                                IMessageSender& messageSender,
                                                ITrainStatusManager& statusManager,
                                                IDutyAgentInterface& dutyAgent )
            : m_configuration( configuration ),
              m_stateUpdateSender( stateUpdateSender ),
              m_messageSender( messageSender ),
              m_dutyAgent( dutyAgent ),
              m_lastUpdateTime( 0 ),
              m_statusManager( statusManager ),
              m_terminate( false ),
              m_queueSemaphore( 0 )
    {
        FUNCTION_ENTRY( "OperationModeManager" );

        // start the thread
        start();

        FUNCTION_EXIT;
    }


    OperationModeManager::~OperationModeManager()
    {
        FUNCTION_ENTRY( "~OperationModeManager" );

        // stop the thread
        terminateAndWait();

        FUNCTION_EXIT;
    }


    void OperationModeManager::registerForOperationModeChanges( IOperationModeObserver* observer )
    {
        FUNCTION_ENTRY( "registerForOperationModeChanges" );

        addObserver( observer );

        FUNCTION_EXIT;
    }


    void OperationModeManager::deregisterForOperationModeChanges( IOperationModeObserver* observer )
    {
        FUNCTION_ENTRY( "deregisterForOperationModeChanges" );

        removeObserver( observer );

        FUNCTION_EXIT;
    }


	//***************************************************************************************
	// Query Methods                                                                        *
	//***************************************************************************************
	void OperationModeManager::ensureControlState()
	{
		FUNCTION_ENTRY("ensureControlState");
		
		if( m_runningControl == false )
		{
			throw TA_Base_Core::OperationModeException();
		}
		FUNCTION_EXIT;
	}	
	
	bool OperationModeManager::isRunningControl(void)
	{
		FUNCTION_ENTRY("isRunningControl");
		
		FUNCTION_EXIT;
		return m_runningControl;
	}

    bool OperationModeManager::isInControlOfLocation( unsigned long locationKey ) const
    {
        FUNCTION_ENTRY( "isInControlOfLocation" );

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        // check m_locationControlMap to see the current state
        LocationControlMap::const_iterator findIter = m_locationControlMap.find( locationKey );

        if ( m_locationControlMap.end() != findIter )
        {
            // return the state associated with the location in the map

            FUNCTION_EXIT;
            return findIter->second;
        }

        // if the location is not found, return false

        FUNCTION_EXIT;
        return false;
    }


    bool OperationModeManager::isInControlOfTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                   bool allowDelocalised,
                                                   bool allowInvalid ) const
    {
        FUNCTION_ENTRY( "isInControlOfTrain" );

        try
        {
            // get the train comms state from m_statusManager
            TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

            bool atsValid = false;
            bool localisationValid = false;
            unsigned long trainLocation = 0;

            {
                // acquire the lock for the train state
                TA_IRS_Bus::TrainStateLock commsStateLock( commsState );

                // get the train's ATS validity and localisation validity
                atsValid = commsState->isAtsValid();
                localisationValid = commsState->isLocalisationValid();

                // get the train's location
                trainLocation = commsState->getLocation();

                logControlledLocations();

                LOG_GENERIC( SourceInfo,
					TA_Base_Core::DebugUtil::DebugInfo,
					"[atsValid=%d][localisationValid=%d][trainLocation=%d][trainId=%d]",
					atsValid,
					localisationValid,
					trainLocation,
					trainId );

                // release the lock for the train state
            }

            if ( true == atsValid )
            {
                // if the trains ATS validity is true, and its localisation is true
                if ( true == localisationValid )
                {
                    FUNCTION_EXIT;
                    return isInControlOfLocation( trainLocation );
                }

                // if the trains ATS validity is true, and its localisation is false,
                // and allowDelocalised is true, and its location is not 0
                else if ( ( true == allowDelocalised ) &&
                          ( 0 != trainLocation ) )
                {
                    FUNCTION_EXIT;
                    return isInControlOfLocation( trainLocation );
                }

                // if the trains ATS validity is true, and its localisation is false
                else
                {
                    FUNCTION_EXIT;
                    return allowDelocalised;
                }
            }

            // if the trains ATS validity is false
            else
            {
                FUNCTION_EXIT;
                return allowInvalid;
            }
        }
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
        }

        // if the train is not found, return false

        FUNCTION_EXIT;
        return false;
    }


    std::set< unsigned long > OperationModeManager::getControlledLocations() const
    {
        FUNCTION_ENTRY( "getControlledLocations" );

        TA_THREADGUARD( m_stateLock );

        // from m_locationControlMap create a set of locations where agent control is 'true'
        std::set< unsigned long > controlledLocations;

        for ( LocationControlMap::const_iterator iter = m_locationControlMap.begin();
              m_locationControlMap.end() != iter; ++iter )
        {
            if ( true == iter->second )
            {
                controlledLocations.insert( iter->first );
            }
        }

        FUNCTION_EXIT;
        return controlledLocations;
    }


    bool OperationModeManager::isIscsDegradedMode() const
    {
        FUNCTION_ENTRY( "isIscsDegradedMode" );
        FUNCTION_EXIT;
        return false;
    }


    bool OperationModeManager::isLocalRadioInFallback() const
    {
        FUNCTION_ENTRY( "isLocalRadioInFallback" );
        FUNCTION_EXIT;
        return false;
    }


    TA_IRS_Bus::TrainInformationTypes::AgentOperationMode OperationModeManager::getCurrentOperationMode() const
    {
        FUNCTION_ENTRY( "getCurrentOperationMode" );

        // create an AgentOperationMode structure
        TA_IRS_Bus::TrainInformationTypes::AgentOperationMode operationMode;

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        // populate it with m_locationControlMap
        operationMode.controlledLocations = getControlledLocations();

        // populate the last update time with m_lastUpdateTime
        operationMode.lastUpdateTime = m_lastUpdateTime;

        // set each boolean member to false, and return it
        operationMode.fallbackMode = false;
        operationMode.doubleAtsFailure = false;
        operationMode.groupOffline = false;
        operationMode.localDuty = false;

        FUNCTION_EXIT;
        return operationMode;
    }


    void OperationModeManager::inFallback( bool fallback )
    {
        FUNCTION_ENTRY( "inFallback" );

        // Do nothing in this base implementation

        FUNCTION_EXIT;
    }


    void OperationModeManager::inDoubleAtsFailure( bool doubleAtsFailure )
    {
        FUNCTION_ENTRY( "inDoubleAtsFailure" );

        // Do nothing in this base implementation

        FUNCTION_EXIT;
    }


    bool OperationModeManager::radioInFallback() const
    {
        FUNCTION_ENTRY( "radioInFallback" );

        // return false in this base implementation

        FUNCTION_EXIT;
        return false;
    }


    void OperationModeManager::processOccStateChange( bool occOnline )
    {
        FUNCTION_ENTRY( "processOccStateChange" );

        // Do nothing in this base implementation

        FUNCTION_EXIT;
    }


    void OperationModeManager::processSubsystemStateChange( const SubsystemDutyState& dutyState )
    {
        FUNCTION_ENTRY( "processSubsystemStateChange" );

        // Do nothing in this base implementation

        FUNCTION_EXIT;
    }


    bool OperationModeManager::syncCurrentOperationMode( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& operationMode )
    {
        FUNCTION_ENTRY( "syncCurrentOperationMode" );

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        // if the update time is newer than the time stored,

        if ( operationMode.lastUpdateTime > m_lastUpdateTime )
        {
            // set m_locationControlMap to controlledLocations
            LocationControlMap newlocationControlMap;

            for ( std::set< unsigned long >::const_iterator iter = operationMode.controlledLocations.begin();
                  operationMode.controlledLocations.end() != iter; ++iter )
            {
                newlocationControlMap.insert( LocationControlMap::value_type( *iter, true ) );
            }

            m_locationControlMap.swap( newlocationControlMap );

            m_lastUpdateTime = operationMode.lastUpdateTime;

            FUNCTION_EXIT;
            return true;
        }

        FUNCTION_EXIT;
        return false;
    }


	//*****************************************************
	// TrainAgent IGenericAgentUser interface             *
	//*****************************************************
	
	/**
	* setControlMode  This pure virtual method is called by GenericAgent whenever it
	* receives a request from the Process Monitor to change its operating state to
	* control. The derived agent should respond by subscribing/unsubscribing to/from
	* the  appropriate message channels and performing any other internal house
	* keeping.
	*
	* The derived agent, Train agent, calls this method, effectively passing the
	* subscription management to the the OperationModeManager.
	*/
	void OperationModeManager::agentSetControl()
	{
		FUNCTION_ENTRY("agentSetControl");
		
		//std::cout << "Control" << std::endl;
		
		m_runningControl = true;
		
//		m_trainEventSubscriber->resubscribe();
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"Agent is running in CONTROL mode");
		m_semaphore.post();

        TA_Base_Core::SingletonTimerUtil::getInstance().startPeriodicTimeOutClock( this, 30000, false );//limin++, CL-21120

		FUNCTION_EXIT;
	}
	
	/**
	* setMonitorMode  This pure virtual method is called by GenericAgent whenever it
	* receives a request from the Process Monitor to change its operating state to
	* control. The derived agent should respond by subscribing/unsubscribing to/from
	* the  appropriate message channels and performing any other internal house
	* keeping.
	*
	* The derived agent, Train agent, calls this method, effectively passing the
	* subscription management to the the OperationModeManager.
	*/
	void OperationModeManager::agentSetMonitor()
	{
		FUNCTION_ENTRY("agentSetMonitor");
		
		//std::cout << "Monitor" << std::endl;
		
		m_runningControl = false;
//		m_trainEventSubscriber->resubscribe();
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"Agent is running in MONITOR mode");
		m_semaphore.post();

        TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );//limin++, CL-21120

		FUNCTION_EXIT;
	}

    void OperationModeManager::run()
    {
        FUNCTION_ENTRY( "run" );

        TA_Base_Core::EOperationMode state = TA_Base_Core::NotApplicable;

        bool exit = false;
        //TA_Base_Bus::OperationStateObserver observer;

        while ( ( false == m_terminate ) &&
                ( false == exit ) )
        {
            // wait for the agent operation state to change
            //state = observer.waitForOperationStateChange();
			m_semaphore.wait();

            // when the state changes, switch on state
			if ( false == isRunningControl() )
			{
				// exit the while loop
                exit = true;
			}
			else
            {
                // set m_lastUpdateTime to the current time
                m_lastUpdateTime = time( NULL );

                // call controlModeInitialisation then exit the while loop
                controlModeInitialisation();
                exit = true;
            }
        }

        // clear the queue:
        {
            // acquire m_queueLock
            TA_THREADGUARD( m_queueLock );

            // while the queue is not empty

            while ( false == m_updateQueue.empty() )
            {
                delete m_updateQueue.front();
                m_updateQueue.pop();
            }
        }

        while ( false == m_terminate )
        {
            m_queueSemaphore.wait();

            OperationModeUpdate* newItem = NULL;

            {
                // acquire m_queueLock
                TA_THREADGUARD( m_queueLock );

                // if m_updateQueue is not empty
                if ( false == m_updateQueue.empty() )
                {
                    newItem = m_updateQueue.front();

                    if ( NULL != newItem )
                    {
                        m_updateQueue.pop();
                    }
                }
            }

            if ( NULL != newItem )
            {
                // process the new item
                OperationModeManager::ObserverList observers = getObservers();

                // for each observer in observers

                for ( OperationModeManager::ObserverList::iterator iter = observers.begin();
                      observers.end() != iter; ++iter )
                {
                    switch ( newItem->updateType )
                    {

                        case ControlAcquired:
                            ( *iter )->locationControlAcquired( newItem->locationKey );
                            break;

                        case ControlReleased:
                            ( *iter )->locationControlReleased( newItem->locationKey );
                            break;

                        default:
                            TA_ASSERT( false, "Unknown EOperationModeUpdateType" );
                            break;
                    }
                }

                delete newItem;
                newItem = NULL;
            }
        }

        FUNCTION_EXIT;
    }


    void OperationModeManager::terminate()
    {
        FUNCTION_ENTRY( "terminate" );

        // sets m_terminate to true.
        m_terminate = true;

        // signals m_queueSemaphore
        m_queueSemaphore.post();

        FUNCTION_EXIT;
    }


    void OperationModeManager::controlModeInitialisation()
    {
        FUNCTION_ENTRY( "controlModeInitialisation" );

        // Do nothing in this base implementation
        // just send out the state

        notifyStateChange();

        FUNCTION_EXIT;
    }


    bool OperationModeManager::takeControlOfLocation( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "takeControlOfLocation" );

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        // check m_locationControlMap to see the current state
        LocationControlMap::iterator iter = m_locationControlMap.find( locationKey );

        if ( m_locationControlMap.end() != iter )
        {
            if ( true == iter->second )
            {
                // if the state is already true (control already acquired) return false

                FUNCTION_EXIT;
                return false;
            }
            else
            {
                iter->second = true;
            }
        }
        else
        {
            m_locationControlMap.insert( LocationControlMap::value_type( locationKey, true ) );
        }


        // set m_lastUpdateTime to the current time
        m_lastUpdateTime = time( NULL );

        notifyStateChange();

        logLocationState();

        // create and queue a client update
        OperationModeUpdate* update = new OperationModeUpdate();
        update->updateType = ControlAcquired;
        update->locationKey = locationKey;

        // acquire m_queueLock
        TA_THREADGUARD( m_queueLock );
        m_updateQueue.push( update );

        //post the m_queueSemaphore
        m_queueSemaphore.post();

        FUNCTION_EXIT;
        return true;
    }


    bool OperationModeManager::releaseControlOfLocation( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "releaseControlOfLocation" );

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        // check m_locationControlMap to see the current state
        LocationControlMap::iterator iter = m_locationControlMap.find( locationKey );

        if ( m_locationControlMap.end() != iter )
        {
            if ( false == iter->second )
            {
                // if the state is already false (control already released) return false

                FUNCTION_EXIT;
                return false;
            }
            else
            {
                iter->second = false;
            }
        }
        else
        {
            m_locationControlMap.insert( LocationControlMap::value_type( locationKey, false ) );
        }

        // set m_lastUpdateTime to the current time
        m_lastUpdateTime = time( NULL );

        notifyStateChange();

        logLocationState();

        // create and queue a client update
        OperationModeUpdate* update = new OperationModeUpdate();
        update->updateType = ControlReleased;
        update->locationKey = locationKey;

        // acquire m_queueLock
        TA_THREADGUARD( m_queueLock );
        m_updateQueue.push( update );

        //post the m_queueSemaphore
        m_queueSemaphore.post();

        FUNCTION_EXIT;
        return true;
    }


    void OperationModeManager::notifyStateChange() const
    {
        FUNCTION_ENTRY( "notifyStateChange" );
        
        TA_IRS_Bus::TrainInformationTypes::AgentOperationMode operationMode = getCurrentOperationMode();//limin++, CL-20691
        
        // call m_stateUpdateSender and m_messageSender with the operation mode structure
        m_stateUpdateSender.sendStateUpdate( operationMode );
        m_messageSender.sendClientUpdate( operationMode );
        
        FUNCTION_EXIT;
    }


    void OperationModeManager::logLocationState() const
    {
        FUNCTION_ENTRY( "logLocationState" );

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        // for each location in m_locationControlMap

        for ( LocationControlMap::const_iterator iter = m_locationControlMap.begin();
              m_locationControlMap.end() != iter; ++iter )
        {
            // log in info message containing the location key, and whether control is present or not
            // eg Location : X  Control : True/False
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Location Control [location: %d][control: %s]",
                         iter->first,
                         ( iter->second == true ) ? "True" : "False" );
        }

        FUNCTION_EXIT;
    }


    void OperationModeManager::logControlledLocations() const
    {
        FUNCTION_ENTRY( "logControlledLocations" );

        std::ostringstream controlledLocations;

        for ( LocationControlMap::const_iterator iter = m_locationControlMap.begin();
              m_locationControlMap.end() != iter; ++iter )
        {
            if ( true == iter->second )
            {
                controlledLocations << "," << ( iter->first );
            }
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Controlled Locations=%s", controlledLocations.str().c_str() );

        FUNCTION_EXIT;
    }


    ////limin++, CL-21120
    void OperationModeManager::timerExpired(long timerId, void* userData)
    {
        FUNCTION_ENTRY( "timerExpired" );

        syncWithDutyAgent();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "synchronized TCS with duty agent." );
        
        FUNCTION_EXIT;
    }
    ////limin++, CL-21120

} // TA_IRS_App
