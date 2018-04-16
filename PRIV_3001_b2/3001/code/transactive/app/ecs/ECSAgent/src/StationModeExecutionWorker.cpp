/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/StationModeExecutionWorker.cpp $
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

#include "app/ecs/ECSAgent/src/StationModeExecutionWorker.h"
#include "app/ecs/ECSAgent/src/ItaEcsAuditing.h"
#include "app/ecs/ECSAgent/src/ItaEcsAgentCompleteState.h"
#include "app/ecs/ECSAgent/src/ItaEcsDatapoints.h"
#include "app/ecs/ECSAgent/src/EcsHelpers.h"

#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/common_library/src/DpValue.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/TransactiveException.h"


static const unsigned long MAX_ABORT_TIME_IN_MSECS      = 5000;

// ExceptionChecked
StationModeExecutionWorker::StationModeExecutionWorker (
    ItaEcsCachedConfig& theConfig,
    ItaEcsAuditing& theAuditing,
    ItaEcsAgentCompleteState& theCompleteState,
    ItaEcsDatapoints& theDatapoints)
: m_isRunning(false),
  m_startTime(0),
  m_theConfig(theConfig),
  m_theAuditing(theAuditing),
  m_theCompleteState(theCompleteState),
  m_theDatapoints(theDatapoints),
  m_timerUtility(TA_Base_Core::SingletonTimerUtil::getInstance())

{
    FUNCTION_ENTRY("Constructor");

    // Setup proxies

	// get a handle to the ScadaProxyFactory
	m_proxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
    //TA_ASSERT ( ( 0 != m_proxyFactory ), "SCADA Proxy Factory not created");
	if (NULL == m_proxyFactory)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SCADA Proxy Factory not created");
		TA_THROW(TA_Base_Core::TransactiveException("SCADA Proxy Factory not created"));
	}
	m_proxyFactory->setProxiesToControlMode();

    // Use this method to reuse code
    processItaEcsCachedConfigUpdate(ItaEcsCachedConfig::CONFIG_MODE_CONTROL_CORBA_NAME);
    processItaEcsCachedConfigUpdate(ItaEcsCachedConfig::CONFIG_MODE_IN_PROGRESS_CORBA_NAME);

    m_theConfig.attachObserver(this, ItaEcsCachedConfig::CONFIG_MODE_CONTROL_CORBA_NAME);
    m_theConfig.attachObserver(this, ItaEcsCachedConfig::CONFIG_MODE_IN_PROGRESS_CORBA_NAME);

	FUNCTION_EXIT;
}



// ExceptionChecked
StationModeExecutionWorker::~StationModeExecutionWorker()
{
    FUNCTION_ENTRY("Destructor");

    //TA_ASSERT(!m_isRunning, "Deleting a running thread");
	if ( true == m_isRunning )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Deleting a running thread" );
	}

	// remove handle to ScadaProxyFactory
	m_proxyFactory->setProxiesToMonitorMode();
	TA_Base_Bus::ScadaProxyFactory::removeInstance();
	m_proxyFactory = NULL;

	FUNCTION_EXIT;
}


void StationModeExecutionWorker::timerExpired(long timerId, void* userData)
{
    m_enableSignal.signal();
}


// ExceptionChecked
void StationModeExecutionWorker::run()
{
    FUNCTION_ENTRY("run");

    m_isRunning = true;

    m_timerUtility.startPeriodicTimeOutClock(this, m_theConfig.getWorkerThreadsSleepTimeInMSecs(), false);

    while (m_isRunning)
    {
        if (true == m_isEnabled)
        {
            try
            {
                // Retrieve global state
                TA_Base_Core::ThreadGuard guard( m_possibleModeStateModificationLock );
                TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData modeState;
                m_theCompleteState.getStationModeState(modeState);

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
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unexpected EEcsExecutionState used");
                }

                // Update global state - sends synchronization to Monitor agent
                m_theCompleteState.setStationModeState(modeState);
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
void StationModeExecutionWorker::terminate()
{
    FUNCTION_ENTRY("terminate");

    m_isRunning = false;

    FUNCTION_EXIT;
}

void StationModeExecutionWorker::execute(long fromStationMode, long toStationMode,
										 long forMasterMode, const std::string& sessionId)
{
    FUNCTION_ENTRY("execute");

    // Assumed that we are ready 
    TA_Base_Core::ThreadGuard guard( m_possibleModeStateModificationLock );

    TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData modeState;
    
    // Reset state variables
    modeState.fromStationMode   = fromStationMode;
    modeState.toStationMode     = toStationMode;
    modeState.forMasterMode     = forMasterMode;
    modeState.locationKey       = m_theConfig.getAgentLocationKey();
    modeState.sessionId         = sessionId.c_str();
    modeState.executionState    = TA_IRS_Bus::ECS_EXECUTION_CONTROL;
    modeState.executionError    = TA_IRS_Bus::ECS_ERROR_NONE;
    modeState.additionalInfo    = const_cast<const char *>("");

	// CL20458++ - will not send control when current mode is according to the requested mode
	//			   directly push to the complete state
    unsigned long ulModeInProgress = 0u;
    try
    {
        ulModeInProgress = m_modeInProgressProxy->getValue().getEnum();
    }
    catch(TA_Base_Core::ScadaProxyException& e)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", e.what() );
        throw;
    }

	if (( fromStationMode == toStationMode ) && ( ulModeInProgress == 0 ) )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CL20458: The requested mode is according to current mode, no need to send the control " );
		modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;	

		//zn++ CL-20830: in case of CL-20458, the last applied datapoint also required to be setted
		m_theDatapoints.setLastAppliedStationMode(modeState.toStationMode);
		m_theDatapoints.setLastAppliedMasterMode(modeState.forMasterMode);
		//++zn
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "will change station mode from %li to %li for master mode %li", fromStationMode, toStationMode, forMasterMode);

	// ++CL20458
    // Update global state - sends synchronisation to Monitor agent
    // Should trigger the worker thread to start processing too
    m_theCompleteState.setStationModeState(modeState);

    //to notify the thread to enter next loop
    m_enableSignal.signal();

    FUNCTION_EXIT;
}

void StationModeExecutionWorker::abortIfRequired()
{
    FUNCTION_ENTRY("abortIfRequired");

    TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData modeState;

    // Scope brackets must be used so we can interrupt the runtime poll loop
    // to force the execution state to completion.
    {
        // Retrieve global state
        TA_Base_Core::ThreadGuard guard( m_possibleModeStateModificationLock );
        m_theCompleteState.getStationModeState(modeState);

        if ( modeState.executionState == TA_IRS_Bus::ECS_EXECUTION_READY ||
             modeState.executionState == TA_IRS_Bus::ECS_EXECUTION_COMPLETED)
        {
            // No need to abort
            return;
        }

        modeState.executionState    = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;
        modeState.executionError    = TA_IRS_Bus::ECS_ERROR_ABORTED;
        modeState.additionalInfo    = const_cast<const char *>("");

        // This will trigger the worker thread to enter completion and then ready.
        m_theCompleteState.setStationModeState(modeState);
    }
    // End of scope will remove the lock to allow the runtime poll loop to resume 
    // processing the completion state.  

    unsigned long elapsedTime(0);
    while (modeState.executionState!=TA_IRS_Bus::ECS_EXECUTION_READY)
    {
        Thread::sleep ( m_theConfig.getWorkerThreadsSleepTimeInMSecs() );

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

        TA_Base_Core::ThreadGuard guard( m_possibleModeStateModificationLock );
        m_theCompleteState.getStationModeState(modeState);
    }

    FUNCTION_EXIT;
}

void StationModeExecutionWorker::processReadyState
    (TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& modeState)
{
    FUNCTION_ENTRY("processReadyState");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic, "Enter processReadyState");
    // Do nothing and wait for execute() to be called 

    FUNCTION_EXIT;
}

void StationModeExecutionWorker::processControlState(
    TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& modeState)
{
    FUNCTION_ENTRY("processControlState");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Enter processControlState");

    m_startTime = time(NULL);

    // Set station mode
    try
    {
        std::string strValue = m_modeControlProxy->getDerivedStateAsString( modeState.toStationMode );
        m_modeControlProxy->setValue( strValue, modeState.sessionId.in() );
        modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_IN_PROGRESS;
    }
    catch(TA_Base_Core::ScadaProxyException& e)
    {
        // abnormal termination - includes authentication error
        modeState.executionError = TA_IRS_Bus::ECS_ERROR_SM_WRITE_FAILURE_P;
        modeState.additionalInfo = const_cast<const char*>(e.what());
        modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;   
    }

	// Set last applied station mode and master mode
  	m_theDatapoints.setLastAppliedStationMode(modeState.toStationMode);
    m_theDatapoints.setLastAppliedMasterMode(modeState.forMasterMode);

	// processEntityUpdate will handle and set relevant execution state
    

    FUNCTION_EXIT;
}

void StationModeExecutionWorker::processInProgressState
    (TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& modeState)
{
    FUNCTION_ENTRY("processInProgressState");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Enter processInProgressState");

    if ( (time(NULL) - m_startTime) > m_theConfig.getMaxModeExecutionTimeInSecs() )
    {
        // abnormal termination
        modeState.executionError = TA_IRS_Bus::ECS_ERROR_TIME_LIMIT_EXCEEDED;
        modeState.additionalInfo = const_cast<const char *>("");
        modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;   
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Max Mode Execution timeout, MaxModeExecutionTimeInSecs=%d, timediff=%d",
				m_theConfig.getMaxModeExecutionTimeInSecs(), (time(NULL) - m_startTime));  
        
        return;
    }

	// processEntityUpdate will handle and set relevant execution state

    FUNCTION_EXIT;
}

void StationModeExecutionWorker::processCompletedState
    (TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& modeState)
{
    FUNCTION_ENTRY("processCompletedState");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Enter processCompletedState");

    // update and summarize results

	// If station error found
    if (modeState.executionError!=TA_IRS_Bus::ECS_ERROR_NONE)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "station mode execution complete meet error!");
    }
    
    m_theAuditing.sendEcsStationModeResult(modeState.fromStationMode, modeState.toStationMode, 
										   static_cast<const char*>(modeState.additionalInfo));

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "station mode execution complete!");

    modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_READY;

    FUNCTION_EXIT;
}

void StationModeExecutionWorker::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
{
    TA_Base_Core::ThreadGuard guardConfig( m_lockForConfigUpdates );

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[DP] %lu state changed, type: %d", entityKey, updateType);

    unsigned long ulModeInProgress = 0u;
    unsigned long ulModeControl = 0u;

    try
    {
        ulModeInProgress = m_modeInProgressProxy->getEntityKey();
    }
    catch(std::exception& e)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
    }

    try
    {
        ulModeControl = m_modeControlProxy->getEntityKey();
    }
    catch(std::exception& e)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
    }

    if ( entityKey == ulModeInProgress )
    {
        // C955 Mode in progress is treated as RCC. Do not check the value in ECS Agent own logic.
        //processModeInProgressUpdate(updateType);
    }
    else if ( entityKey == ulModeControl )
    {
        processModeControlUpdate(updateType);
    }
}

void StationModeExecutionWorker::processModeControlUpdate(TA_Base_Bus::ScadaEntityUpdateType updateType)
{
    // Retrieve global state
    TA_Base_Core::ThreadGuard guard( m_possibleModeStateModificationLock );

    TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData modeState;
    m_theCompleteState.getStationModeState(modeState);

    if (modeState.executionState!=TA_IRS_Bus::ECS_EXECUTION_IN_PROGRESS)
    {
        return;
    }

    try
    {
       // Check for any write request failures
        if (updateType == TA_Base_Bus::ControlStateUpdate)
        {
            TA_Base_Core::ThreadGuard guardConfig( m_lockForConfigUpdates );

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Control State Change to %d", m_modeControlProxy->getControlState());

            switch(m_modeControlProxy->getControlState())
            {
            case TA_Base_Bus::noControl:
            case TA_Base_Bus::written:
            case TA_Base_Bus::pending:
                break;

            case TA_Base_Bus::conditionalSuccess:
            case TA_Base_Bus::success:
                modeState.executionError = TA_IRS_Bus::ECS_ERROR_NONE;
                modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;    
                break;
            case TA_Base_Bus::failed:
                // Abnormal termination
                modeState.executionError = TA_IRS_Bus::ECS_ERROR_SM_WRITE_FAILURE_P;
                modeState.additionalInfo = const_cast<const char *>("");
                modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;    
                break;
            case TA_Base_Bus::LCC_failed:
                // Abnormal termination
                modeState.executionError = TA_IRS_Bus::ECS_ERROR_SM_WRITE_FAILURE_P;
                modeState.additionalInfo = const_cast<const char *>("LCC");
                modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;    
                break;
            case TA_Base_Bus::RCC_failed:
                // Abnormal termination
                modeState.executionError = TA_IRS_Bus::ECS_ERROR_SM_WRITE_FAILURE_P;
                modeState.additionalInfo = const_cast<const char *>("RCC");
                modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;    
                break;
            case TA_Base_Bus::comms_failed:
                // Abnormal termination
                modeState.executionError = TA_IRS_Bus::ECS_ERROR_SM_WRITE_FAILURE_P;
                modeState.additionalInfo = const_cast<const char *>("Communications");
                modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;    
                break;
            default:
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
							"Unexpected ControlState received");
            }
        }
    }
    catch(TA_Base_Core::ScadaProxyException& e)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", e.what() );
    }
    catch (...)
    {
        // Not expecting any failures...
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
    }

    // Update global state - sends synchronization to Monitor agent
    m_theCompleteState.setStationModeState(modeState);

    //to notify the thread to enter next loop
    m_enableSignal.signal();
}


void StationModeExecutionWorker::processModeInProgressUpdate(TA_Base_Bus::ScadaEntityUpdateType updateType)
{
    // Retrieve global state
    TA_Base_Core::ThreadGuard guard( m_possibleModeStateModificationLock );
    TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData modeState;
    m_theCompleteState.getStationModeState(modeState);

    try
    {
        // Check for transition to 0 only
        //TD13744, also need to process event ConfigAvailable
        if ( ( updateType == TA_Base_Bus::ValueStateUpdate ||
               updateType == TA_Base_Bus::ConfigAvailable ) && 
             m_modeInProgressProxy->isConfigValid() )
        {
            TA_Base_Core::ThreadGuard guardConfig( m_lockForConfigUpdates );

			unsigned long curValue = m_modeInProgressProxy->getValue().getEnum();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Mode in progress update, value: %lu, Current execution state: %d", curValue, modeState.executionState);

            if (curValue == 0u && modeState.executionState == TA_IRS_Bus::ECS_EXECUTION_IN_PROGRESS)
            {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Station Mode Enter Completion state");
                // Enter Completion state.
                modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_COMPLETED;
            }
            else if ( curValue != 0u )
            {
                // Mode in progress is set to active state manually (not by ISCS), need to reset timeout
                if ( modeState.executionState != TA_IRS_Bus::ECS_EXECUTION_IN_PROGRESS )
                {
                    m_startTime = time(NULL);
                }

                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Station Mode Still in Progress state");
                modeState.executionState = TA_IRS_Bus::ECS_EXECUTION_IN_PROGRESS;
            }
            else
            {
                // Not in progress, leave it as previous execution state
            }
        }
    }
    catch(TA_Base_Core::ScadaProxyException& e)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", e.what() );
    }
    catch (...)
    {
        // Not expecting any failures...
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
    }

    // Update global state - sends synchronisation to Monitor agent
    m_theCompleteState.setStationModeState(modeState);

    //to notify the thread to enter next loop
    m_enableSignal.signal();
    
}

void StationModeExecutionWorker::processItaEcsCachedConfigUpdate(ItaEcsCachedConfig::EItaEcsCachedConfigItemKey key)
{
    switch (key)
    {
    case ItaEcsCachedConfig::CONFIG_MODE_CONTROL_CORBA_NAME:
        {
			//zn++
            //TA_Base_Core::ThreadGuard guard(m_lockForConfigUpdates);
			//++zn

			// This bit of silly code seems to be necessary otherwise it breaks the solaris build
            const unsigned long ulKey = m_theConfig.getModeControlDpEntityKey();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Station ECS Agent will monitor [DP]:%d", ulKey );

            if ( 0u != ulKey )
            {
                m_proxyFactory->createDataPointProxy(ulKey, *this, m_modeControlProxy); 
            }
        }
        break;
    case ItaEcsCachedConfig::CONFIG_MODE_IN_PROGRESS_CORBA_NAME:
        {
			//zn++
            //TA_Base_Core::ThreadGuard guard(m_lockForConfigUpdates);
			//++zn

            const unsigned long ulKey = m_theConfig.getModeInProgressDpEntityKey();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Station ECS Agent will monitor [DP]:%d", ulKey );

            if ( 0u != ulKey )
            {
                m_proxyFactory->createDataPointProxy(ulKey, *this, m_modeInProgressProxy);
            }
        }
        break;

    default:
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unexpected ECachedConfigItemKey received");
    }
}

