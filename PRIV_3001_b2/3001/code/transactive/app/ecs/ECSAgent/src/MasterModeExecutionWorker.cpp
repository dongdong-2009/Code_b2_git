/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/MasterModeExecutionWorker.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include <time.h>

#include "app/ecs/ECSAgent/src/EcsHelpers.h"
#include "app/ecs/ECSAgent/src/MasterModeExecutionWorker.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/ItaEcsAuditing.h"
#include "app/ecs/ECSAgent/src/ItaEcsAlarms.h"
#include "app/ecs/ECSAgent/src/ItaEcsAgentCompleteState.h"
#include "app/ecs/ECSAgent/src/ItaEcsDatapoints.h"
#include "app/ecs/ECSAgent/src/MasterModeManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/ECSAgentComms_MessageTypes.h"
#include "core/naming/src/NamedObject.h"
#include "core/exceptions/src/TransactiveException.h"

static const unsigned long MAX_ABORT_TIME_IN_MSECS      = 5000;


// ExceptionChecked
MasterModeExecutionWorker::MasterModeExecutionWorker(
    ItaEcsCachedMaps& theMaps,
    ItaEcsCachedConfig& theConfig,
    ItaEcsAuditing& theAuditing,
    ItaEcsAlarms& theAlarms,
    ItaEcsAgentCompleteState& theCompleteState,
	ItaEcsDatapoints& theDatapoints,
    ImmmArrowAccess& theArrows)
: m_isRunning(false),
  m_startTime(0),
  m_theMaps(theMaps),
  m_theConfig(theConfig),
  m_theAuditing(theAuditing),
  m_theAlarms(theAlarms),
  m_theCompleteState(theCompleteState),
  m_theDatapoints(theDatapoints),
  m_theArrows(theArrows),
  m_locksManager(theConfig,theMaps,theDatapoints),
  m_timerUtility(TA_Base_Core::SingletonTimerUtil::getInstance())
{
    FUNCTION_ENTRY("Constructor");

    m_stationAgents.clear();

    // Initialise Station ECS Agent named objects (code reused)
    processItaEcsCachedMapsUpdate(ItaEcsCachedMaps::MAP_STATION_ECS_AGENT_CORBA_NAMES);

    m_theMaps.attachObserver(this, ItaEcsCachedMaps::MAP_STATION_ECS_AGENT_CORBA_NAMES);

	FUNCTION_EXIT;
}



// ExceptionChecked
MasterModeExecutionWorker::~MasterModeExecutionWorker()
{
    FUNCTION_ENTRY("Destructor");
	if (m_isRunning)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Deleting a running thread" );
	}

	// Unsubscribe if not already.
	unsubscribeToStationEcsAgentComms();
    
    TA_Base_Core::ThreadGuard guard( m_lockForNamedObjects );

    m_theMaps.detachObserver(this);

    LocationToNamedObjectMap::iterator it=m_stationAgents.begin();
    for (; it!=m_stationAgents.end(); ++it)
    {
        if (it->second)
        {
            delete (it->second);
        }
    }
    m_stationAgents.clear();

	FUNCTION_EXIT;
}


void MasterModeExecutionWorker::setControl(bool isOnStartup)
{
    m_locksManager.setControl(isOnStartup);

    AbstractFailoverable::setControl(isOnStartup);

    subscribeToStationEcsAgentComms();
}

void MasterModeExecutionWorker::setMonitor(bool isOnStartup)
{
	// Unsubscribe if not already.
	unsubscribeToStationEcsAgentComms();

    AbstractFailoverable::setMonitor(isOnStartup);

    m_locksManager.setMonitor(isOnStartup);
}


void MasterModeExecutionWorker::timerExpired(long timerId, void* userData)
{
    m_enableSignal.signal();
}


// ExceptionChecked
void MasterModeExecutionWorker::run()
{
    FUNCTION_ENTRY("run");

    m_isRunning = true;

    m_timerUtility.startPeriodicTimeOutClock(this, m_theConfig.getWorkerThreadsSleepTimeInMSecs(), false);

    while (m_isRunning)
    {
        if (m_isEnabled)
        {
            try
            {
                // Retrieve global state
                TA_Base_Core::ThreadGuard guard( m_possibleModeStateModificationLock );
                TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData modeState; 
                m_theCompleteState.getMasterModeState(modeState);
                
                switch (modeState.executionState)
                {
                case TA_IRS_Bus::ECS_EXECUTION_READY:
                    processReadyState(modeState);
                    break;
                case TA_IRS_Bus::ECS_EXECUTION_CONTROL: 
                    processControlState(modeState);
                    break;
                case TA_IRS_Bus::ECS_EXECUTION_IN_PROGRESS:
                    processInProgressState(modeState);
                    break;
                case TA_IRS_Bus::ECS_EXECUTION_COMPLETED:
                    processCompletedState(modeState);
                    break;              
                default:
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Invalid EEcsExecutionState enum used" );
                }
                
                // Update global state - sends synchronisation to Monitor agent
                m_theCompleteState.setMasterModeState(modeState);
            }
            catch (...)
            {
                // Not expecting any failures...
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
            }
        }

        m_enableSignal.wait();
        
    }

    m_timerUtility.stopPeriodicTimeOutClock(this);

    FUNCTION_EXIT;
}


// ExceptionChecked
void MasterModeExecutionWorker::terminate()
{
    FUNCTION_ENTRY("terminate");

    m_isRunning = false;
    m_enableSignal.signal();
    
    FUNCTION_EXIT;
}

void MasterModeExecutionWorker::execute(long fromMasterMode, long toMasterMode, const std::string& sessionId,
										const TA_IRS_Bus::EcStationModeRecords& stationModeRecords)
{
    FUNCTION_ENTRY("execute");

    // Assumed that we are ready
    // Interrupt the runtime thread between processing cycles.
    TA_Base_Core::ThreadGuard guard( m_possibleModeStateModificationLock );

    TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData modeState;
    
    // Reset state variables
    modeState.fromMasterMode    = fromMasterMode;
    modeState.toMasterMode      = toMasterMode;
    modeState.sessionId         = sessionId.c_str();
    modeState.executionState    = TA_IRS_Bus::ECS_EXECUTION_CONTROL;
    modeState.executionError    = TA_IRS_Bus::ECS_ERROR_NONE;
    modeState.additionalInfo    = const_cast<const char *>("");

    // Initialise for each station involved in this masterMode
    modeState.locations.length(stationModeRecords.length());
    modeState.stationUpdateDatas.length(stationModeRecords.length());

    for (int i=0; i<stationModeRecords.length(); ++i)
    {
        modeState.locations[i] = stationModeRecords[i].locationKey;

        // Just to provide some temporary values - soon to be overwritten when the 
        // StationECSAgents are called.  If the station ECS Agent cannot be contacted
        // we will use this data as a starting point for some sensible info
        modeState.stationUpdateDatas[i].fromStationMode   = TA_IRS_Bus::ECS_NONE_OR_INVALID;
        modeState.stationUpdateDatas[i].toStationMode     = stationModeRecords[i].stationMode;
        modeState.stationUpdateDatas[i].forMasterMode     = toMasterMode;
        modeState.stationUpdateDatas[i].locationKey       = stationModeRecords[i].locationKey;
        modeState.stationUpdateDatas[i].sessionId         = sessionId.c_str();
        modeState.stationUpdateDatas[i].executionState    = TA_IRS_Bus::ECS_EXECUTION_CONTROL;
        modeState.stationUpdateDatas[i].executionError    = TA_IRS_Bus::ECS_ERROR_NONE;
        modeState.stationUpdateDatas[i].additionalInfo    = const_cast<const char *>("");
    }

    // Update global state - sends synchronisation to Monitor agent
    // Should trigger the worker thread to start processing too
    m_theCompleteState.setMasterModeState(modeState);
	m_stationModeRecords = stationModeRecords;  //TD16709

    //to notity run() function to excute next loop immediately
    m_enableSignal.signal();

    FUNCTION_EXIT;
}

void MasterModeExecutionWorker::abortIfRequired()
{
    FUNCTION_ENTRY("abortIfRequired");

    // Retrieve global state - and interrupt the runtime thread between processing cycles.
    TA_Base_Core::ThreadGuard guard( m_possibleModeStateModificationLock );
    TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData modeState;
    m_theCompleteState.getMasterModeState(modeState);

    if ( modeState.executionState == TA_IRS_Bus::ECS_EXECUTION_READY ||
         modeState.executionState == TA_IRS_Bus::ECS_EXECUTION_COMPLETED)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Abort was not required");
		//TD17328&TD17443++
		if ( ( modeState.toMasterMode >= TA_IRS_Bus::ECS_MIN_EMERGENCY_MASTER_MODE ) &&
			 ( modeState.toMasterMode <= TA_IRS_Bus::ECS_MAX_EMERGENCY_MASTER_MODE ) )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "abort the emergency lock" );
            m_locksManager.removeMasterModeLocks();

            unsigned long ecsZoneId = m_theMaps.getZoneIdFromMasterMode( m_theDatapoints.getCurrentMasterMode() );

            if ( TA_IRS_Bus::ECS_NONE_OR_INVALID != ecsZoneId )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "erase the last emergency one" );
                m_theDatapoints.setEmergencyApplied( ecsZoneId, false );
                m_theDatapoints.setEvacuationApplied( ecsZoneId, TA_IRS_Bus::ECS_ZONE_ARROWS_ENUM_NONE );
                m_theDatapoints.setAirflowApplied( ecsZoneId, TA_IRS_Bus::ECS_ZONE_ARROWS_ENUM_NONE );
            }
		}
		//++TD17328&TD17443
        return;
    }

    modeState.executionState    = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;
    modeState.executionError    = TA_IRS_Bus::ECS_ERROR_ABORTED;
    modeState.additionalInfo    = const_cast<const char *>("");

    // This will trigger the worker thread to enter completion and then ready.
    m_theCompleteState.setMasterModeState(modeState);

    unsigned long elapsedTime(0);
    while (modeState.executionState!=TA_IRS_Bus::ECS_EXECUTION_READY)
    {
        Thread::sleep ( m_theConfig.getWorkerThreadsSleepTimeInMSecs() );
        m_theCompleteState.getMasterModeState(modeState);

        // If for some reason, this takes a long time to complete, the CORBA thread calling
        // abortIfRequired() should timeout and fail the command.  
        // Just in case, we will cap the abort time.  

        elapsedTime += m_theConfig.getWorkerThreadsSleepTimeInMSecs();
        if (elapsedTime > MAX_ABORT_TIME_IN_MSECS)
        {
            // Tried our best - execute() should be able to proceed regardless
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                        "Failed to abort the execution in time.");
            break;
        }
    }

    FUNCTION_EXIT;
}

void MasterModeExecutionWorker::processReadyState
    (TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData& modeState)
{
    FUNCTION_ENTRY("processReadyState");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic, "enter ECS_EXECUTION_READY state");

    // Do nothing and wait for execute() to be called 

    FUNCTION_EXIT;
}

void MasterModeExecutionWorker::processControlState
    (TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData& modeState)
{
    FUNCTION_ENTRY("processControlState");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "enter ECS_EXECUTION_CONTROL state");

    m_startTime = time(NULL);

    // Update ModeInProgress datapoint
	m_theDatapoints.setMasterModeInProgress(1); // 1==true

    // reset all Master Mode Locks to false only if the master mode was executed for a fire
    unsigned long ulZoneId = m_theMaps.getZoneIdFromMasterMode(modeState.toMasterMode);

    if ( TA_IRS_Bus::ECS_NONE_OR_INVALID != ulZoneId )
    {
        if (m_theDatapoints.getEmergency( ulZoneId ))
        {
            m_locksManager.removeMasterModeLocks();
        }
    }

    // Check for basic data integrity
    if (m_stationModeRecords.length() != modeState.locations.length())
    {
        // Abnormal termination
        modeState.executionError = TA_IRS_Bus::ECS_ERROR_CORRUPTED_STATE;
        modeState.additionalInfo = const_cast<const char *>("");
        modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;
        return;
    }

    for (int i=0; i<m_stationModeRecords.length(); ++i)
    {
        // Check for finer data integrity
        if (modeState.locations[i] != m_stationModeRecords[i].locationKey)
        {
            // Abnormal termination
            modeState.executionError = TA_IRS_Bus::ECS_ERROR_CORRUPTED_STATE;
            modeState.additionalInfo = const_cast<const char *>("");
            modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;
            return;
        }

        // Call execute on this StationECSAgent
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling execute() on %s Station ECS Agent",
			m_theMaps.getLocationName(m_stationModeRecords[i].locationKey).c_str());        

        try
        {
            StationEcsAgentNamedObject* stationAgent = m_stationAgents[modeState.locations[i]];

            if (NULL == stationAgent)
            {
                // StationECSAgent not configured.
                modeState.stationUpdateDatas[i].executionError = TA_IRS_Bus::ECS_ERROR_SM_STN_AGENT_FAILURE_P;
                modeState.stationUpdateDatas[i].additionalInfo = const_cast<const char *>("Not configured");
                modeState.stationUpdateDatas[i].executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"%s Station ECS Agent is null",
					m_theMaps.getLocationName(m_stationModeRecords[i].locationKey).c_str());  

                continue;
            }
            CORBA_CALL( (*stationAgent), executeStationModeForMasterMode, ( m_stationModeRecords[i].stationMode,
                                                                            modeState.toMasterMode,
                                                                            static_cast<const char*>(modeState.sessionId) ) );//limin
        }
    	//TD16709
		catch( const TA_IRS_Bus::IStationEcsAgentCorbaDef::StationInLocalMode& e)
		{
			modeState.stationUpdateDatas[i].executionError = TA_IRS_Bus::ECS_ERROR_SM_STN_AGENT_FAILURE_P;
            modeState.stationUpdateDatas[i].additionalInfo = TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str();
            modeState.stationUpdateDatas[i].executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Caught IStationEcsAgentCorbaDef::StationInLocalMode on %s Station ECS Agent when calling executeStationModeForMasterMode",
				m_theMaps.getLocationName(m_stationModeRecords[i].locationKey).c_str());  

            continue;
		}
		catch( const TA_IRS_Bus::IStationEcsAgentCorbaDef::StationLocked& e)
		{
			modeState.stationUpdateDatas[i].executionError = TA_IRS_Bus::ECS_ERROR_SM_STN_AGENT_FAILURE_P;
            modeState.stationUpdateDatas[i].additionalInfo = TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str();
            modeState.stationUpdateDatas[i].executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Caught IStationEcsAgentCorbaDef::StationLocked on %s Station ECS Agent when calling executeStationModeForMasterMode",
				m_theMaps.getLocationName(m_stationModeRecords[i].locationKey).c_str());  

            continue;
		}			
		// CL-17274++
		catch(const TA_Base_Core::AccessDeniedException& e)
		{
			modeState.stationUpdateDatas[i].executionError = TA_IRS_Bus::ECS_ERROR_SM_WRITE_FAILURE_P;
			modeState.stationUpdateDatas[i].additionalInfo = e.reason;
			modeState.stationUpdateDatas[i].executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"CL-17274 Caught TA_Base_Core::AccessDeniedException on %s Station ECS Agent when calling executeStationModeForMasterMode",
				m_theMaps.getLocationName(m_stationModeRecords[i].locationKey).c_str());  

			continue;

		}
		// ++CL-17274
		catch(const CORBA::Exception& e)
        {
            // Should cover:
            // AccessDeniedException
            // BadParameterException
            // ObjectResolutionException
            modeState.stationUpdateDatas[i].executionError = TA_IRS_Bus::ECS_ERROR_SM_STN_AGENT_FAILURE_P;	
            modeState.stationUpdateDatas[i].additionalInfo = TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str();
            modeState.stationUpdateDatas[i].executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Caught CORBA::Exception on %s Station ECS Agent when calling executeStationModeForMasterMode",
				m_theMaps.getLocationName(m_stationModeRecords[i].locationKey).c_str());  
			
            continue;
        }
        catch(const TA_Base_Core::TransactiveException& e)
        {
            modeState.stationUpdateDatas[i].executionError = TA_IRS_Bus::ECS_ERROR_SM_STN_AGENT_FAILURE_P;
            modeState.stationUpdateDatas[i].additionalInfo = const_cast<const char*>(e.what());
            modeState.stationUpdateDatas[i].executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Caught TA_Base_Core::TransactiveException on %s Station ECS Agent when calling executeStationModeForMasterMode",
				m_theMaps.getLocationName(m_stationModeRecords[i].locationKey).c_str());  

            continue;
        }
        catch(...)
        {
            modeState.stationUpdateDatas[i].executionError = TA_IRS_Bus::ECS_ERROR_SM_STN_AGENT_FAILURE_P;
            modeState.stationUpdateDatas[i].additionalInfo = const_cast<const char*>("");
            modeState.stationUpdateDatas[i].executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Caught unknown exception on %s Station ECS Agent when calling executeStationModeForMasterMode",
				m_theMaps.getLocationName(m_stationModeRecords[i].locationKey).c_str());  

            continue;
        }        
    } // for loop

    modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_IN_PROGRESS;

    FUNCTION_EXIT;
}

void MasterModeExecutionWorker::processInProgressState
    (TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData& modeState)
{
    FUNCTION_ENTRY("processInProgressState");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "enter ECS_EXECUTION_IN_PROGRESS state");

    if ( (time(NULL) - m_startTime) > m_theConfig.getMaxModeExecutionTimeInSecs() )
    {
        // abnormal Termination
        modeState.executionError = TA_IRS_Bus::ECS_ERROR_TIME_LIMIT_EXCEEDED;
        modeState.additionalInfo = const_cast<const char *>("");
        modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;  
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Max Mode Execution timeout, MaxModeExecutionTimeInSecs=%d, timediff=%d",
				m_theConfig.getMaxModeExecutionTimeInSecs());  

		FUNCTION_EXIT;
        return;
    }

    
    // Check to see when all stations are completed.
    // Overall completion => all stations READY and not with the ABORT error.
    // 
    // Consider the scenarios:
    // 
    // 1. Station Mode executed by operator && Master Mode is applied
    //    The Master Mode will abort the Station Mode.  The Station ECS Agent
    //    will update the StationModeExecutionUpdateData to ABORT and READY but
    //    using the previous operator's sessionId.  This update message will not
    //    be processed by the Master ECS Agent because processStationEcsPacket()
    //    ignores all messages not related to the Master Mode.
    // 
    // 2. Master Mode is applied once and then applied again before completion.
    //    The Master Mode will abort the previoud Station Modes.  The Station ECS 
    //    Agent will update the StationModeExecutionUpdateData to ABORT and READY 
    //    but this time, the session ID happens to be the same as the Master Mode's 
    //    This update message will be processed by the Master ECS Agent.  Therefore
    //    we must be careful that the master mode does not consider the station ECS 
    //    Agent to be complete.  It is important that the terminating condition for 
    //    a given station ECS Agent is that the station state is READY and NOT aborted.
    // 

    bool allStationsReady(true);


    for (int i=0; i<m_stationModeRecords.length(); ++i)
    {

        bool thisStationReady = false; 

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Statino Mode State: %li, %li, %li, %li, %d, %d", 
			modeState.stationUpdateDatas[i].fromStationMode, 
			modeState.stationUpdateDatas[i].toStationMode, 
			modeState.stationUpdateDatas[i].forMasterMode,  
			modeState.stationUpdateDatas[i].locationKey, 
			modeState.stationUpdateDatas[i].executionState,
			modeState.stationUpdateDatas[i].executionError);

        // if station not contactable
        if( (modeState.stationUpdateDatas[i].executionError == TA_IRS_Bus::ECS_ERROR_SM_STN_AGENT_FAILURE_P) &&
            (modeState.stationUpdateDatas[i].executionState == TA_IRS_Bus::ECS_EXECUTION_COMPLETED) )
        {
            thisStationReady = true; 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "station %d not contactable", i);
        }
        else // if station is contactable
        {
            thisStationReady = 
                ( (modeState.stationUpdateDatas[i].executionState == TA_IRS_Bus::ECS_EXECUTION_READY) && 
                  (modeState.stationUpdateDatas[i].executionError != TA_IRS_Bus::ECS_ERROR_ABORTED) );
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "station %d contactable", i);
        }

        allStationsReady &= thisStationReady;
    }

    if (true == allStationsReady)
    {
		TA_Base_Core::ThreadGuard guard( m_possibleModeStateModificationLock );	// zhangnan++
        // Skip to completion
        modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED; 

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"All stations ready, set execution state to complete");  
		processCompletedState(modeState);	// zhangnan++, unless, can not enter the completed state function.
    }

    // wait until no longer in progress as dictated by incoming updates from Station ECS Agents

    FUNCTION_EXIT;
}

void MasterModeExecutionWorker::processCompletedState
    (TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData& modeState)
{
    FUNCTION_ENTRY("processCompletedState");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "enter ECS_EXECUTION_COMPLETED state");

    // Final check
    if (modeState.executionError==TA_IRS_Bus::ECS_ERROR_NONE) //TD7858
    {
        // Have not noticed any errors yet - check thoroughly


        bool stationErrorEncountered(false);
        for (int i=0; i<m_stationModeRecords.length(); ++i)
        {
            // Assume that if the station is READY and executionError=false =>
            // the desired station mode was set.  
            if (modeState.stationUpdateDatas[i].executionError)
            {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"MasterModeExecutionWorker::processCompletedState: Station mode at location %d encountered execution error", i);

                stationErrorEncountered = true;
                break;
            }
        }

        if (true == stationErrorEncountered)
        {
            modeState.executionError = TA_IRS_Bus::ECS_ERROR_WRONG_MODE_RESULT;
            modeState.additionalInfo = const_cast<const char *>("");

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Station error encountered, set execution error to ECS_ERROR_WRONG_MODE_RESULT");  
        }
    }

    // Everything went okay - tidy up
	// CL17438++ when one or more station is locked, their station mode failed, stop set current master mode
	// do nothing
	if (TA_IRS_Bus::ECS_ERROR_WRONG_MODE_RESULT == modeState.executionError)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "master mode contain station mode failed! Do nothing");
		modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_READY;
		FUNCTION_EXIT;
		return;
	}
	// ++CL17438

     //7858
    if ( modeState.executionError != TA_IRS_Bus::ECS_ERROR_NONE &&
		 modeState.executionError != TA_IRS_Bus::ECS_ERROR_WRONG_MODE_RESULT)
    {
		 // Update MasterModeStatus to the negative value to indicate a failure
        m_theDatapoints.setCurrentMasterMode(-(modeState.toMasterMode));

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"MasterModeExecutionWorker::processCompletedState: Setting master mode to -%d, mode error=%d.", 
			modeState.toMasterMode, modeState.executionError);
	}
	// CL17438 caused only for ECS_ERROR_NONE
    else
    {
		// Update MasterModeStatus
        m_theDatapoints.setCurrentMasterMode(modeState.toMasterMode);

		// Was this execution in response to a fire condition?
		unsigned long ecsZoneId = m_theMaps.getZoneIdFromMasterMode(modeState.toMasterMode);

        if ( TA_IRS_Bus::ECS_NONE_OR_INVALID != ecsZoneId )
        {
		    if ( m_theDatapoints.getEmergency(ecsZoneId) )
		    {
                ProposedModeSet proposedModes;
                m_theCompleteState.getProposedModeSet(proposedModes);

                // If a Master Mode is applied that does not correspond to a currently 
                // Proposed Master Mode, then the stations are not locked.  
                // Subsequently station operators may continue to apply Station Modes 
                // for the affected stations.
                bool fProposedMode = false;
                for(ProposedModeSetIt iter=proposedModes.begin(); iter != proposedModes.end(); ++iter)
                {
                    TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode propMode = *iter;
                    if(propMode.mode == modeState.toMasterMode)
                    {
                        fProposedMode = true;
                        break;
                    }
                }

                if(fProposedMode)
                {
				    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "applyMasterModeLocks");
			        // Set master mode locks for the affected stations only if the
			        // master mode was executed for a fire.
			        m_locksManager.applyMasterModeLocks(modeState.toMasterMode);
                }
    			
			    // We only set the emergency applied datapoint to true in response to an
			    // actual emergency condition - not for any old Master Mode execution.        
			    m_theDatapoints.setEmergencyApplied(ecsZoneId, true);
            }
		}

        // Set arrows to applied
        m_theArrows.setAppliedMasterMode(modeState.toMasterMode);
		m_theArrows.setSelectedMasterMode(TA_IRS_Bus::ECS_NONE_OR_INVALID);
    }

    // Update ModeInProgress datapoint
	m_theDatapoints.setMasterModeInProgress(0); // 0==false
    
    m_theAuditing.sendEcsMasterModeResult(
        modeState.fromMasterMode,
        modeState.toMasterMode, 
        static_cast<const char*>(modeState.additionalInfo),
		modeState.sessionId.in());		//CL17278++ Sheet3

    modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_READY;

    FUNCTION_EXIT;
}


void MasterModeExecutionWorker::processItaEcsCachedMapsUpdate(ItaEcsCachedMaps::EItaEcsCachedMapsItemKey key)
{

    if (key != ItaEcsCachedMaps::MAP_STATION_ECS_AGENT_CORBA_NAMES)
    {
		return;
	}

    TA_Base_Core::ThreadGuard guard( m_lockForNamedObjects );

    // Clear existing collection
    for (LocationToNamedObjectMap::iterator it=m_stationAgents.begin(); it!=m_stationAgents.end(); ++it)
    {
        if (it->second)
        {
            delete (it->second);
        }
    }
    m_stationAgents.clear();

    // Add new collection
    std::vector<unsigned long> locations = m_theMaps.getAllStationEcsAgentLocationKeys();

    for (std::vector<unsigned long>::iterator locationIt=locations.begin(); locationIt!=locations.end(); ++locationIt)
    {
		try
		{
			TA_Base_Core::CorbaName name = m_theMaps.getStationEcsAgentCorbaName(*locationIt);

            StationEcsAgentNamedObject* newAgent = new StationEcsAgentNamedObject();
			newAgent->setCorbaName(name);

			// Safe to use operator[] instead of insert as the target is a pointer which 
			// is default initialised to NULL;
			m_stationAgents[*locationIt] = newAgent;
		}
		catch(TA_Base_Core::CachedMappingNotFoundException& cmnfe)
		{
            // Note - Should not have got to the StationEcsAgentNamedObject construction state.  
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Cannot locate Station [%d] ECS Agent CORBA object - Master ECS Agent access to this station will not be possible",
				*locationIt );

			m_theAlarms.raiseEcsInvalidConfiguration(cmnfe.what());

            continue;
        }
        catch ( const std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
            continue;
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Unknown exception caught when getStationEcsAgentCorbaName" );
            continue;
        }
    }
   
}



void MasterModeExecutionWorker::subscribeToStationEcsAgentComms()
{
    TA_Base_Core::ThreadGuard guard( m_lockForNamedObjects );

    // Clear existing collection
    for (LocationToNamedObjectMap::iterator it=m_stationAgents.begin(); it!=m_stationAgents.end(); ++it)
    {
        //TA_Base_Core::CorbaName corbaName = (it->second)->getCorbaName();
        try
        {
            TA_Base_Core::CorbaName corbaName = m_theMaps.getStationEcsAgentCorbaName(it->first);

            // Subscribe for updates from the local ECS Agents
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
                TA_Base_Core::ECSAgentComms::EcsStationModeExecutionUpdate,     //context
                this,				                                            //SpecialisedMessageSubscriber
                corbaName.getEntityKey(),                                       //unsigned long entityKey
                0,		    						                            //unsigned long subsystemKey
                corbaName.getLocationKey(),                                     //unsigned long regionKey
                NULL);                                                          //no filterable data
        }
        catch ( const std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
            continue;
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Unknown exception caught when getStationEcsAgentCorbaName" );
            continue;
        }
    }
}

void MasterModeExecutionWorker::unsubscribeToStationEcsAgentComms()
{
    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
}


// ExceptionChecked
void MasterModeExecutionWorker::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef&  message)
{
    FUNCTION_ENTRY("receiveSpecialisedMessage");

	if (false == m_isEnabled)
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode");        
		FUNCTION_EXIT;
        return;
	}

    try
    {          
        const TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData* stationEcsPacket = 0;
	    if ( (message.messageState >>= stationEcsPacket) == 0 )
        {
            LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugError, "Failed to interpret message");
			FUNCTION_EXIT;
            return;
        }
        
        processStationEcsPacket(*stationEcsPacket);
	}
    catch (...)
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed to process data from Station ECS Agent");
	}

    FUNCTION_EXIT;
}

void MasterModeExecutionWorker::processStationEcsPacket(const TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& packet)
{
	if (false == m_isEnabled)
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode");        
	}

    // Retrieve global state
    TA_Base_Core::ThreadGuard guard( m_possibleModeStateModificationLock );
    TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData modeState; 
    m_theCompleteState.getMasterModeState(modeState);

    // determine location index.
    bool found(false);
    int i = 0;
    for (; i<modeState.locations.length(); ++i)
    {
        if (modeState.locations[i] == packet.locationKey)
        {
            found = true;
            break;
        }
    }

    if (false == found)
    {
        // Ignore packet from a station outside our scope
	    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
	        "Ignoring station ecs packet update from a station not part of the Master Mode");        
        return;
    }

    if (strcmp(packet.sessionId,modeState.sessionId)!=0)
    {
        // Ignore packet generated by another operator.
	    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
	        "Ignoring station ecs packet update for an execution from another operator");        
        return;    
    }

    // Check for validity
  if (/*modeState.stationUpdateDatas[i].fromStationMode != packet.fromStationMode ||*/
      modeState.stationUpdateDatas[i].toStationMode   != packet.toStationMode   ||
      modeState.stationUpdateDatas[i].forMasterMode   != packet.forMasterMode)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "receive the station mode upate from station agent, but abort it. ");
        // Abort this station early
        modeState.stationUpdateDatas[i].executionError = TA_IRS_Bus::ECS_ERROR_CORRUPTED_STATE;
        modeState.stationUpdateDatas[i].additionalInfo = const_cast<const char *>("");
        modeState.stationUpdateDatas[i].executionState = TA_IRS_Bus::ECS_EXECUTION_READY;

        m_theCompleteState.setMasterModeState(modeState);
        return;
    }

    // All OK to process and update the master state with.
    modeState.stationUpdateDatas[i].executionState = packet.executionState;
    modeState.stationUpdateDatas[i].executionError = packet.executionError;
    modeState.stationUpdateDatas[i].additionalInfo = static_cast<const char*>(packet.additionalInfo);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "receive the station mode upate from station agent, next set master mode state. ");

    m_theCompleteState.setMasterModeState(modeState);

	
    //to notity run() function to excute next loop immediately, zhangnan++
    m_enableSignal.signal();
}