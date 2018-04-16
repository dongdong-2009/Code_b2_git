#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/StationModeManager.cpp $
  * @author  A. Parker
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2009/10/28 18:12:47 $
  * Last modified by : $Author: builder $
  *
  *
  */

#include <sstream>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include "core/naming/src/Naming.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/RunParams.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "app/ecs/ECSAgent/src/ItaEcsAuditing.h"
#include "app/ecs/ECSAgent/src/ItaEcsAgentCompleteState.h"
#include "app/ecs/ECSAgent/src/ItaEcsDatapoints.h"
#include "app/ecs/ECSAgent/src/EcsHelpers.h"
#include "app/ecs/ECSAgent/src/StationModeExecutionWorker.h"
#include "app/ecs/ECSAgent/src/StationModeManager.h"
#include "bus/ecs/ecs_agent/IDL/src/CommonEcsAgentCorbaDef.h"		// TD17328&TD17443

const unsigned short CORBA_TIMEOUT = 300; //TD16709


StationModeManager::StationModeManager(
    ItaEcsCachedConfig& theConfig,
    ItaEcsAuditing& theAuditing,
    ItaEcsAgentCompleteState& theCompleteState,
	ItaEcsDatapoints& theDatapoints)//throw (TA_Core::GenericAgentException)
: m_stationModeExecutionWorker(
    theConfig, 
    theAuditing, 
    theCompleteState, 
    theDatapoints),
  m_theConfig(theConfig),
  m_theAuditing(theAuditing),
  m_theCompleteState(theCompleteState),
  m_theDatapoints(theDatapoints),
  m_rightsMgr(NULL)		// CL17274
{
	FUNCTION_ENTRY("Constructor");

	// CL17274++
	TA_Base_Bus::RightsLibraryFactory rlf;
	m_rightsMgr = rlf.buildRightsLibrary();
	TA_ASSERT( m_rightsMgr != NULL, "Rights library not created." );
	// ++CL17274

    std::ostringstream msg;
    msg << "Registering Station ECS Agent CORBA object: " 
        << m_theConfig.getAgentName();
    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, msg.str().c_str());

	activateServantWithName(m_theConfig.getAgentName());

    FUNCTION_EXIT;
}

StationModeManager::~StationModeManager()
{
	// CL17274++
	if (m_rightsMgr != NULL)
	{
		delete m_rightsMgr;
		m_rightsMgr = NULL;
	}
	// ++CL17274
    try
    {
        this->deactivateServant();


        // To be sure
        setMonitor();

        if (m_stationModeExecutionWorker.getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING)
        {
            m_stationModeExecutionWorker.terminateAndWait();
        }
    }
    catch ( ... )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Exception caught in destructor");
    }
}

void StationModeManager::setControl(bool isOnStartup)
{
    m_stationModeExecutionWorker.setControl(isOnStartup);    
    AbstractFailoverable::setControl(isOnStartup);
}

void StationModeManager::setMonitor(bool isOnStartup)
{
    AbstractFailoverable::setMonitor(isOnStartup);
    m_stationModeExecutionWorker.setMonitor(isOnStartup);
    
}

// IDL Interface functions

void StationModeManager::executeStationMode(CORBA::Long stationMode, CORBA::Boolean canOverrideStationModeInProgress, const char* sessionId)
{
	FUNCTION_ENTRY("executeStationMode");
    LOGOUT("executeStationMode");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "execute StationMode: %li ", stationMode);

	TA_Base_Bus::GenericAgent::ensureControlMode();
    m_theAuditing.sendEcsStationModeRequest(m_theDatapoints.getCurrentStationMode(), stationMode, sessionId);

    TA_Base_Core::ThreadGuard guard( m_lock );

    ///////////////////////////
    // Check Rights CL-17274++ : already checked at ECS Station Mode Dialog
    ///////////////////////////
	if (!checkPermission(sessionId))
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "No rights to perform station mode %li to station %lu", stationMode, m_theConfig.getAgentLocationKey());
		throw TA_Base_Core::AccessDeniedException("No rights to perform station mode.");
	}


    ///////////////////////////
    // Check Master Mode Locks
    ///////////////////////////

    if (m_theDatapoints.getMasterModeLocked())
    {

        m_theAuditing.sendEcsStationModeResult(
            m_theDatapoints.getCurrentStationMode(),
            stationMode, 
            TA_IRS_Bus::ECS_ERROR_MSG_SM_MASTER_MODE_LOCKED);

        throw(TA_IRS_Bus::IStationEcsAgentCorbaDef::StationLocked(static_cast<const char *>(TA_IRS_Bus::ECS_ERROR_MSG_SM_MASTER_MODE_LOCKED)));
    }


    ///////////////////////////
    // Check Local Mode
    ///////////////////////////

    //TD13744, local mode define is: true = ocr, false = lcp-vcp
    if (m_theDatapoints.getLocalMode() == false)
    {
        m_theAuditing.sendEcsStationModeResult(
            m_theDatapoints.getCurrentStationMode(),
            stationMode, 
            TA_IRS_Bus::ECS_ERROR_MSG_SM_LOCAL_MODE);
        
        throw(TA_IRS_Bus::IStationEcsAgentCorbaDef::StationInLocalMode(static_cast<const char *>(TA_IRS_Bus::ECS_ERROR_MSG_SM_LOCAL_MODE)));
    }

    /////////////////////////////////
    // Check for current executions
    /////////////////////////////////

    TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData stationModeState;
    m_theCompleteState.getStationModeState(stationModeState);

    //TD13744, only consider ECS_EXECUTION_IN_PROGRESS as in progress status
    if ( (stationModeState.executionState == TA_IRS_Bus::ECS_EXECUTION_IN_PROGRESS) &&
            (!canOverrideStationModeInProgress) )
    {
		std::ostringstream msg;
		//Hongzhi, CL20341, TD- problem in ECS station mode change event
		char  buffer[200] = {0};
		sprintf(buffer,TA_IRS_Bus::ECS_ERROR_MSG_SM_IN_PROGRESS_P,stationModeState.toStationMode);
		msg << buffer;
		
        m_theAuditing.sendEcsStationModeResult(
            m_theDatapoints.getCurrentStationMode(),
            stationMode, 
            msg.str().c_str());

        throw(TA_IRS_Bus::IStationEcsAgentCorbaDef::StationModeInProgress
            (stationModeState.toStationMode, 
             msg.str().c_str()));
    }



    /////////////////////////////////
    // All checks clear
    /////////////////////////////////

    m_stationModeExecutionWorker.abortIfRequired();

    m_stationModeExecutionWorker.execute
        (m_theDatapoints.getCurrentStationMode(),
         stationMode,
         TA_IRS_Bus::ECS_NONE_OR_INVALID,
         sessionId);

    FUNCTION_EXIT;
}

void StationModeManager::executeStationModeForMasterMode(CORBA::Long stationMode, CORBA::Long forMasterMode, const char* sessionId)
{
	FUNCTION_ENTRY("executeStationModeForMasterMode");
    LOGOUT("executeStationModeForMasterMode");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "execute StationMode: %li for MasterMode: %li ", stationMode, forMasterMode);

	TA_Base_Bus::GenericAgent::ensureControlMode();
    TA_Base_Core::ThreadGuard guard( m_lock );

	///////////////////////////
    // Check Rights CL-17274++
    ///////////////////////////
	if ( !checkPermission( sessionId ) )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CL-17274 No rights to perform station mode %li fro master mode %li to station %lu", stationMode, forMasterMode, m_theConfig.getAgentLocationKey());
		throw TA_Base_Core::AccessDeniedException("No rights to perform station mode");
	}
    
    ///////////////////////////
    // Check Master Mode Locks
    ///////////////////////////

    if ( m_theDatapoints.getMasterModeLocked() == true )
    {
		// TD17328&TD17443
		// should release the lock, because user operation has priority
		if ( ( forMasterMode >= TA_IRS_Bus::ECS_MIN_EMERGENCY_MASTER_MODE ) &&
			 ( forMasterMode <= TA_IRS_Bus::ECS_MAX_EMERGENCY_MASTER_MODE ) )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "release the lock" );
			setMasterModeLockDataPoint( true );
		}
		else // Now the current execution master mode is not an emergency mode, so can not break the lock
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "if no emergency master mode, can not break the station lock" );
			throw( TA_IRS_Bus::IStationEcsAgentCorbaDef::StationLocked( static_cast<const char *>( TA_IRS_Bus::ECS_ERROR_MSG_SM_MASTER_MODE_LOCKED ) ) );
		}
    }


    ///////////////////////////
    // Check Local Mode
    ///////////////////////////

    //TD13744, local mode define is: true = ocr, false = lcp-vcp
    if (m_theDatapoints.getLocalMode() == false)
    {
        throw(TA_IRS_Bus::IStationEcsAgentCorbaDef::StationInLocalMode(static_cast<const char *>(TA_IRS_Bus::ECS_ERROR_MSG_SM_LOCAL_MODE)));
    }

    /////////////////////////////////
    // All checks clear
    /////////////////////////////////

    m_stationModeExecutionWorker.abortIfRequired();

    m_stationModeExecutionWorker.execute(m_theDatapoints.getCurrentStationMode(),
         stationMode, forMasterMode, sessionId);

    FUNCTION_EXIT;
}

void StationModeManager::setSelectedMasterModeDataPoint(CORBA::Long value)
{
	FUNCTION_ENTRY("setSelectedMasterModeDataPoint");
    LOGOUT("setSelectedMasterModeDataPoint");

	//no used
	//TA_Base_Bus::GenericAgent::ensureControlMode();
    //m_theDatapoints.setSelectedMasterMode(value);

    FUNCTION_EXIT;
}

void StationModeManager::setSelectedOverridingStationModeDataPoint(CORBA::Long value)
{
	FUNCTION_ENTRY("setSelectedOverridingStationModeDataPoint");
    LOGOUT("setSelectedOverridingStationModeDataPoint");

	//no used
    //m_theDatapoints.setSelectedOverridingStationMode(value);

    FUNCTION_EXIT;
}

void StationModeManager::setLastAppliedMasterModeDataPoint(CORBA::Long value)
{
	FUNCTION_ENTRY("setLastAppliedMasterModeDataPoint");
    LOGOUT("setLastAppliedMasterModeDataPoint");

	TA_Base_Bus::GenericAgent::ensureControlMode();
    m_theDatapoints.setLastAppliedMasterMode(value);

    FUNCTION_EXIT;
}

void StationModeManager::setLastAppliedStationModeDataPoint(CORBA::Long value)
{
	FUNCTION_ENTRY("setLastAppliedStationModeDataPoint");
    LOGOUT("setLastAppliedStationModeDataPoint");

	TA_Base_Bus::GenericAgent::ensureControlMode();
    m_theDatapoints.setLastAppliedStationMode(value);

    FUNCTION_EXIT;
}

void StationModeManager::setMasterModeLockDataPoint(CORBA::Boolean value)
{
	FUNCTION_ENTRY("setMasterModeLockDataPoint");
    LOGOUT("setMasterModeLockDataPoint");

	TA_Base_Bus::GenericAgent::ensureControlMode();
    m_theDatapoints.setMasterModeLock(value);
    m_theDatapoints.setMasterModeLockControl(value);

    FUNCTION_EXIT;
}

//TD16709
CORBA::UShort StationModeManager::getFlexibleTimeOutValue()
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	return CORBA_TIMEOUT;
}

// CL-17274++
bool StationModeManager::checkPermission(const char* sessionID)
{
	bool actionAllowed = false;
	std::string reason;
	std::string session = sessionID;
	TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "agentkey = %lu", m_theConfig.getAgentLocationKey());
	
	try
	{
		actionAllowed = m_rightsMgr->isActionPermittedOnResource( 
			session, m_theConfig.getAgentKey(), 
			TA_Base_Bus::aca_DATA_POINT_WRITE, 
			reason, decisionModule );		
	}
	catch (...)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception caught when checking the action permission for masterEcsAgent" );
		
		actionAllowed = false;
	}  
	
	return actionAllowed;
}
//++CL-17274



