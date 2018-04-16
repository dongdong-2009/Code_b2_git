#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ECSAgent/src/MasterModeManager.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2012/06/12 13:35:44 $
  * Last modified by : $Author: builder $
  *
  *
  */

#include <sstream>
#include <iostream>
#include <time.h>

#include "app/ecs/ECSAgent/src/EcsHelpers.h"
#include "app/ecs/ECSAgent/src/MasterModeExecutionWorker.h"
#include "app/ecs/ECSAgent/src/EcsArrowManager.h"
#include "app/ecs/ECSAgent/src/MasterModeManager.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedMaps.h"
#include "app/ecs/ECSAgent/src/ItaEcsAuditing.h"
#include "app/ecs/ECSAgent/src/ItaEcsAlarms.h"
#include "app/ecs/ECSAgent/src/ItaEcsAgentCompleteState.h"
#include "app/ecs/ECSAgent/src/ItaEcsDatapoints.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_agent/src/GenericAgent.h"

#include "core/naming/src/Naming.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"


const unsigned short CORBA_TIMEOUT = 300; //TD16709

MasterModeManager::MasterModeManager(
    ItaEcsCachedMaps& theMaps,
    ItaEcsCachedConfig& theConfig,
    ItaEcsAuditing& theAuditing,
    ItaEcsAlarms& theAlarms,
    ItaEcsAgentCompleteState& theCompleteState,
	ItaEcsDatapoints& theDatapoints) //throw (TA_Core::GenericAgentException)
: m_masterModeExecutionWorker(
    theMaps,
    theConfig,
    theAuditing,
    theAlarms,
    theCompleteState,
    theDatapoints, 
    *this), // this is required for registering ourselves as the ImmmArrowAccess
  m_ecsArrowManager(theMaps, theDatapoints),
  m_theMaps(theMaps),
  m_theConfig(theConfig),
  m_theAuditing(theAuditing),
  m_theAlarms(theAlarms),
  m_theCompleteState(theCompleteState),
  m_theDatapoints(theDatapoints),
  m_timerUtility(TA_Base_Core::SingletonTimerUtil::getInstance())
{
	FUNCTION_ENTRY("Constructor");

    m_activeSessionIds.clear();
	m_emergencyStatus.clear();

    std::ostringstream msg;
    msg << "Registering Master ECS Agent CORBA object: " 
        << m_theConfig.getAgentName();

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, msg.str().c_str());
	
	activateServantWithName(m_theConfig.getAgentName());

    FUNCTION_EXIT;
}

MasterModeManager::~MasterModeManager()
{
    try
    {
        this->deactivateServant();

        if (m_masterModeExecutionWorker.getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING)
        {
            m_masterModeExecutionWorker.terminateAndWait();
        }

        m_activeSessionIds.clear();

		//TD16709
		if (NULL != m_workQueueManager)
		{
			delete m_workQueueManager;
			m_workQueueManager = NULL;
		}
    }
    catch ( ... )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Exception caught in destructor");
    }
}

void MasterModeManager::setControl(bool isOnStartup)
{
    m_masterModeExecutionWorker.setControl(isOnStartup);
    m_ecsArrowManager.setControl(isOnStartup);

    // Don't really care is theres a slight inconsistency with sessionIds
    // when failingover
    m_activeSessionIds.clear();

	m_workQueueManager = new MasterModeDataPointWorkQueueManager();  //TD16709

	m_theDatapoints.attachEmergencyObserver(this);

    AbstractFailoverable::setControl(isOnStartup);
}

void MasterModeManager::setMonitor(bool isOnStartup)
{
    AbstractFailoverable::setMonitor(isOnStartup);

    m_ecsArrowManager.setMonitor(isOnStartup);
    m_masterModeExecutionWorker.setMonitor(isOnStartup);
    m_timerUtility.stopPeriodicTimeOutClock(this);

	m_theDatapoints.detachEmergencyObserver(this);
}

//TD16709
void MasterModeManager::timerExpired(long timerId, void* userData)
{

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "User(s) logged out due to inactivity");
   
	TA_Base_Core::ThreadGuard guard( m_lock );

	// Set selection ModeSymbols to ECS_NONE_OR_INVALID 
    // Don't touch last applied ModeSymbols 


	bool isEmergency = false;

	std::map<unsigned long, bool>::iterator iter;
	for(iter=m_emergencyStatus.begin(); iter!=m_emergencyStatus.end(); ++iter)
	{
		if (iter->second == true)
		{
			isEmergency = true;
			break;
		}
	}

	std::vector<unsigned long> locationKeys = m_theMaps.getAllStationEcsAgentLocationKeys();	
	for (std::vector<unsigned long>::iterator it=locationKeys.begin(); it!=locationKeys.end(); ++it)
	{   
		if (isEmergency = false)
		{
			m_externalMasterMode[*it] = TA_IRS_Bus::ECS_NONE_OR_INVALID;
			m_externalStationMode[*it] = TA_IRS_Bus::ECS_NONE_OR_INVALID;
		}
		else
		{
			m_externalMasterMode[*it] = TA_IRS_Bus::ECS_NONE_AND_EXPECTING;
			m_externalStationMode[*it] = TA_IRS_Bus::ECS_NONE_AND_EXPECTING;
		}
	}

	// Set barrier to set datapoints
	//ACE_Barrier barrier(m_externalStationMode.size() + 1);
	ACE_Barrier barrier(0);

	processExternalStationDp(m_externalStationMode, m_externalMasterMode, barrier);

	//barrier.wait();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"MasterModeManager::timerExpired - barrier end");

    // Update selected arrows
    m_ecsArrowManager.setSelectedMasterMode(TA_IRS_Bus::ECS_NONE_OR_INVALID);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Total sessions = %ld", m_activeSessionIds.size());

}

void MasterModeManager::processEmergencyUpdate(unsigned long zoneId, bool value)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

	if (zoneId == TA_IRS_Bus::ECS_NONE_OR_INVALID)
	{
		return;
	}

	std::map<unsigned long, bool>::iterator emergencyStatusIt;

	// Add the status if emergency arises
	emergencyStatusIt=m_emergencyStatus.find(zoneId);
	if (true == value)
	{	
		if ( emergencyStatusIt == m_emergencyStatus.end() )
		{
			m_emergencyStatus[zoneId] = value;
		}
	}
	else
	{
		if ( emergencyStatusIt != m_emergencyStatus.end() )
		{
			m_emergencyStatus.erase(emergencyStatusIt);
		}		
	}

	MasterModePair masterModePair = m_theMaps.getMasterModesFromZoneId(zoneId);
	std::set<unsigned long> affectedStnLoc = getStationsLocationByZone(zoneId);
	std::set<unsigned long>::const_iterator it;

	//std::vector<unsigned long> locationKeys = m_theMaps.getAllStationEcsAgentLocationKeys();	
	//for (std::vector<unsigned long>::iterator it=locationKeys.begin(); it!=locationKeys.end(); ++it)
	for (it=affectedStnLoc.begin(); it!=affectedStnLoc.end(); ++it)
	{  
		if (false == value)
		{
			m_externalMasterMode[*it] = TA_IRS_Bus::ECS_NONE_OR_INVALID;
			m_externalStationMode[*it] = TA_IRS_Bus::ECS_NONE_OR_INVALID;
		}
		else
		{
			m_externalMasterMode[*it] = TA_IRS_Bus::ECS_NONE_AND_EXPECTING;
			m_externalStationMode[*it] = TA_IRS_Bus::ECS_NONE_AND_EXPECTING;
		}
	}
	
	// CL-20830: remove CL17274
	// CL17274: only set the two selected datapoint when selecting them
	// Set barrier to set datapoints
	//ACE_Barrier barrier(m_externalStationMode.size() + 1);
	ACE_Barrier barrier(0);

	processExternalStationDp(m_externalStationMode, m_externalMasterMode, barrier);

	//barrier.wait();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"MasterModeManager::timerExpired - barrier end");


}


//TD16709
// IDL Interface functions

void MasterModeManager::login(long zoneId, const char* sessionId)
{
	FUNCTION_ENTRY("login");
    LOGOUT("login");

    TA_Base_Core::ThreadGuard guard( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

	if (zoneId == TA_IRS_Bus::ECS_NONE_OR_INVALID)
    {
		m_activeSessionIds.insert(sessionId);
		return;
	}

    if (true == m_activeSessionIds.empty())
    {   
		if(zoneId == TA_IRS_Bus::ECS_NONE_OR_INVALID)
        {
			m_externalStationMode.clear();
			m_externalMasterMode.clear();
			std::vector<unsigned long> locationKeys = m_theMaps.getAllStationEcsAgentLocationKeys();	
			for (std::vector<unsigned long>::iterator it=locationKeys.begin(); it!=locationKeys.end(); ++it)
			{      
				m_externalMasterMode[*it] = TA_IRS_Bus::ECS_NONE_OR_INVALID;
				m_externalStationMode[*it] = TA_IRS_Bus::ECS_NONE_OR_INVALID;
			}
		}
		else
		{
			// At most there will be only two master modes for a zone, if not
			// configuration error and at least there should be 1
			MasterModePair masterModePair = m_theMaps.getMasterModesFromZoneId(zoneId);
			std::set<unsigned long> affectedStnLocList = getStationsLocationByZone(zoneId);
			std::set<unsigned long>::const_iterator affectedStnLocIter;

			/*
			if(masterModePair.first != TA_IRS_Bus::ECS_NONE_OR_INVALID)
			{
				TA_IRS_Bus::EcStationModeRecords stationModeRecords = 
					m_theMaps.getEcStationModeRecordsFromMasterMode(masterModePair.first);
				for(int i=0; i<stationModeRecords.length(); ++i)
				{
					affectedStnLocList.insert(stationModeRecords[i].locationKey);
				}                
			}

			if (masterModePair.second != TA_IRS_Bus::ECS_NONE_OR_INVALID)
			{
				TA_IRS_Bus::EcStationModeRecords stationModeRecords = 
					m_theMaps.getEcStationModeRecordsFromMasterMode(masterModePair.second);
				for(int i=0; i<stationModeRecords.length(); ++i)
				{
					unsigned long locationKey = stationModeRecords[i].locationKey;

					affectedStnLocIter = affectedStnLocList.find(locationKey);
					if (affectedStnLocIter == affectedStnLocList.end())
					{
						affectedStnLocList.insert(stationModeRecords[i].locationKey);
					}
				}               
			}
			*/

			m_externalStationMode.clear();
			m_externalMasterMode.clear();
			std::vector<unsigned long> locationKeys = m_theMaps.getAllStationEcsAgentLocationKeys();	
			for (std::vector<unsigned long>::iterator it=locationKeys.begin(); it!=locationKeys.end(); ++it)
			{
				affectedStnLocIter = affectedStnLocList.find(*it);
				if (affectedStnLocIter != affectedStnLocList.end())
				{
					m_externalMasterMode[*it] = TA_IRS_Bus::ECS_NONE_AND_EXPECTING;
					m_externalStationMode[*it] = TA_IRS_Bus::ECS_NONE_AND_EXPECTING;
				}
				else
				{
					m_externalMasterMode[*it] = TA_IRS_Bus::ECS_NONE_OR_INVALID;
					m_externalStationMode[*it] = TA_IRS_Bus::ECS_NONE_OR_INVALID;
				}

			}
		}

		// CL-20830: remove CL17274
		// CL17274: only set the two selected datapoint when selecting them
		// Set barrier to set datapoints
		//ACE_Barrier barrier(m_externalStationMode.size() + 1);
		ACE_Barrier barrier(0);

		processExternalStationDp(m_externalStationMode, m_externalMasterMode, barrier);

		//barrier.wait();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"MasterModeManager::login - barrier end");

		if (zoneId == TA_IRS_Bus::ECS_NONE_OR_INVALID)
		{
			m_ecsArrowManager.setSelectedMasterMode(TA_IRS_Bus::ECS_NONE_OR_INVALID);	
		}
		else
		{
			m_ecsArrowManager.setSelectedMasterMode(TA_IRS_Bus::ECS_NONE_AND_EXPECTING);
		}
    }

    m_activeSessionIds.insert(sessionId);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Total sessions = %ld", m_activeSessionIds.size());
    
    FUNCTION_EXIT;
}

//TD16709
void MasterModeManager::logout(const char* sessionId)
{
	FUNCTION_ENTRY("logout");
    LOGOUT("logout");

    TA_Base_Core::ThreadGuard guard( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();
    SessionIdsIt it = m_activeSessionIds.find(sessionId);
    if (it == m_activeSessionIds.end())
    {
		FUNCTION_EXIT;
		return;
	}

    m_activeSessionIds.erase(it);
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Total sessions = %ld", m_activeSessionIds.size());

    if (true == m_activeSessionIds.empty())
    {
        // Last user - 
        // Set ModeSymbols to ECS_NONE_OR_INVALID 
        // Don't touch last applied ModeSymbols 

        m_externalStationMode.clear();
		m_externalMasterMode.clear();
		std::vector<unsigned long> locationKeys = m_theMaps.getAllStationEcsAgentLocationKeys();	
		for (std::vector<unsigned long>::iterator it=locationKeys.begin(); it!=locationKeys.end(); ++it)
		{      
			m_externalMasterMode[*it] = TA_IRS_Bus::ECS_NONE_OR_INVALID;
			m_externalStationMode[*it] = TA_IRS_Bus::ECS_NONE_OR_INVALID;
		}

		// CL-20830: remove CL17274
		// CL17274: only set the two selected datapoint when selecting them
		// Set barrier to set datapoints
		//ACE_Barrier barrier(m_externalStationMode.size() + 1);
		ACE_Barrier barrier(0);

		processExternalStationDp(m_externalStationMode, m_externalMasterMode, barrier);

		//barrier.wait();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"MasterModeManager::logout - barrier end");

		// Update selected arrows
		m_ecsArrowManager.setSelectedMasterMode(TA_IRS_Bus::ECS_NONE_OR_INVALID);
    }

    FUNCTION_EXIT;
}

void MasterModeManager::executeMasterMode(CORBA::Long masterMode, CORBA::Boolean canOverrideMasterModeInProgress, 
										  const char* sessionId)
{
	FUNCTION_ENTRY("executeMasterMode");
    LOGOUT("executeMasterMode");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "executeMasterMode");

	TA_Base_Bus::GenericAgent::ensureControlMode();
    m_theAuditing.sendEcsMasterModeRequest(m_theDatapoints.getCurrentMasterMode(), masterMode, sessionId);
    m_timerUtility.stopPeriodicTimeOutClock(this);

    TA_Base_Core::ThreadGuard guard( m_lock );

    ///////////////////////////
    // Check Local Modes
    ///////////////////////////

    std::list<unsigned long> stationsInLocalMode = getStationsInLocalMode(masterMode);

    if (false == stationsInLocalMode.empty())
    {
		std::ostringstream msg;
		msg <<  TA_IRS_Bus::ECS_ERROR_MSG_MM_LOCAL_MODES_P << toCommaSeparatedLocations(stationsInLocalMode);

        m_theAuditing.sendEcsMasterModeResult(
            m_theDatapoints.getCurrentMasterMode(),
            masterMode, 
            msg.str().c_str(),
			sessionId);		// CL17278++ Sheet3

        TA_IRS_Bus::IMasterEcsAgentCorbaDef::StationsInLocalMode exception;
        exception.locations.length(stationsInLocalMode.size());
		exception.what = msg.str().c_str();

        std::list<unsigned long>::iterator it = stationsInLocalMode.begin();
        int j=0;
        for (; it!=stationsInLocalMode.end(); ++it, ++j)
        {
            exception.locations[j] = *it;
        
        }

        throw(exception);
    }

    ///////////////////////////////////////////
    // Checks for Normal Master Mode execution
    ///////////////////////////////////////////

    if (masterMode == m_theConfig.getNormalMasterMode())
    {
        std::set<unsigned long> zonesInCongestion = m_theDatapoints.getZonesInCongestion();
        std::set<unsigned long> zonesInEmergency = m_theDatapoints.getZonesInEmergency();

		bool isInCongestionOrEmergency = false;  //TD16709

        std::ostringstream details;
            
        if (!zonesInCongestion.empty() )
        {
            details << "Ecs zone " 
                    << EcsHelpers::toCommaSeparatedList(zonesInCongestion)
                    << "in congestion. ";

			isInCongestionOrEmergency = true;
        }

        if (!zonesInEmergency.empty() )
        {
            details << "Ecs zone " 
                    << EcsHelpers::toCommaSeparatedList(zonesInEmergency)
                    << "in emergency. ";
			isInCongestionOrEmergency = true;
        }

		if (true == isInCongestionOrEmergency)
		{
			std::ostringstream msg;
			msg << TA_IRS_Bus::ECS_ERROR_MSG_MM_NORMAL_MM_FAILURE_P << details.str().c_str();

			m_theAuditing.sendEcsMasterModeResult(
				m_theDatapoints.getCurrentMasterMode(),
				masterMode, 
				msg.str().c_str(),
				sessionId);		//CL17278++ Sheet3

			TA_IRS_Bus::IMasterEcsAgentCorbaDef::NormalMasterModeFailure exception;
			exception.what = msg.str().c_str();
			exception.zonesInEmergency.length(zonesInEmergency.size());
			exception.zonesInCongestion.length(zonesInCongestion.size());          

			std::set<unsigned long>::iterator it;
			int j=0;

			for (j=0,it=zonesInEmergency.begin(); it!=zonesInEmergency.end(); ++it, ++j)
			{
				exception.zonesInEmergency[j] = *it;
			}

			for (j=0,it=zonesInCongestion.begin(); it!=zonesInCongestion.end(); ++it, ++j)
			{
				exception.zonesInCongestion[j] = *it;
			}

			throw(exception);	
		}   
    }

    ///////////////////////////////////////////
    // Check if a master mode is already active
    ////////////////////////////////////////////
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getCurrentMasterMode = %lu!!!", m_theDatapoints.getCurrentMasterMode() );

    if ( false == canOverrideMasterModeInProgress && 
		( TA_IRS_Bus::ECS_NONE_OR_INVALID != m_theDatapoints.getCurrentMasterMode() || 0 != m_theDatapoints.getCurrentMasterMode() )
		  && 32767 != m_theDatapoints.getCurrentMasterMode() )	// CL20389 - 32767 is the actually value get from the getCurrentMasterMode()
    {
		std::ostringstream msg;
        msg << TA_IRS_Bus::ECS_ERROR_MSG_MM_ACTIVE_P << m_theDatapoints.getCurrentMasterMode();
 
        m_theAuditing.sendEcsMasterModeResult(
            m_theDatapoints.getCurrentMasterMode(),
            masterMode, 
            msg.str().c_str(),
			sessionId);		//CL17278++ Sheet3
    
        throw(TA_IRS_Bus::IMasterEcsAgentCorbaDef::MasterModeActive(m_theDatapoints.getCurrentMasterMode(), msg.str().c_str()));
    }



    /////////////////////////////////
    // check for current executions
    /////////////////////////////////

    TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData masterModeState;
    m_theCompleteState.getMasterModeState(masterModeState);

    if (false==canOverrideMasterModeInProgress && 1==m_theDatapoints.getMasterModeInProgress())
    {
		std::ostringstream msg;
        msg << TA_IRS_Bus::ECS_ERROR_MSG_MM_IN_PROGRESS_P << masterModeState.toMasterMode;

        m_theAuditing.sendEcsMasterModeResult(
            m_theDatapoints.getCurrentMasterMode(),
            masterMode, 
            msg.str().c_str(),
			sessionId);		// CL17278++ Sheet3
    
        throw(TA_IRS_Bus::IMasterEcsAgentCorbaDef::MasterModeInProgress (masterModeState.toMasterMode, msg.str().c_str()));
    }



    /////////////////////////////////
    // All checks clear
    /////////////////////////////////

    // Update ModeSymbol datapoints (regardless of success)

    // Set selection ModeSymbols to ECS_NONE_OR_INVALID 
    // Set last applied ModeSymbols according to modes corresponding to the masterMode

	// CL-20830: remove CL17274
	// CL17274: only set the two selected datapoint when selecting them
	//TD16709
    m_externalStationMode.clear();
	m_externalMasterMode.clear();
	std::vector<unsigned long> locationKeys = m_theMaps.getAllStationEcsAgentLocationKeys();	
	for (std::vector<unsigned long>::iterator it=locationKeys.begin(); it!=locationKeys.end(); ++it)
	{      
		m_externalMasterMode[*it] = TA_IRS_Bus::ECS_NONE_OR_INVALID;
		m_externalStationMode[*it] = TA_IRS_Bus::ECS_NONE_OR_INVALID;
	}

	TA_IRS_Bus::EcStationModeRecords stationModeRecords = 
        m_theMaps.getEcStationModeRecordsFromMasterMode( masterMode );


	// CL-20830: remove CL17274
	// CL17274: only set the two selected datapoint when selecting them
	// Set barrier to set datapoints
	//ACE_Barrier barrier(m_externalStationMode.size() + 1);
	ACE_Barrier barrier(0);

	processExternalStationDp(m_externalStationMode, m_externalMasterMode, barrier);

	//barrier.wait();

	// TD17328&TD17443++
	if ( true == canOverrideMasterModeInProgress && 
		( masterMode >= TA_IRS_Bus::ECS_MIN_EMERGENCY_MASTER_MODE ) &&
		( masterMode <= TA_IRS_Bus::ECS_MAX_EMERGENCY_MASTER_MODE ) &&
		( m_theDatapoints.getCurrentMasterMode() >= TA_IRS_Bus::ECS_MIN_EMERGENCY_MASTER_MODE ) &&
		( m_theDatapoints.getCurrentMasterMode() <= TA_IRS_Bus::ECS_MAX_EMERGENCY_MASTER_MODE ) )	//avoid 32767
	{		
		TA_IRS_Bus::EcStationModeRecords currentStationModeRecords = 
		m_theMaps.getEcStationModeRecordsFromMasterMode( m_theDatapoints.getCurrentMasterMode() );
		
		if ( EcsHelpers::isStationOverlapped( currentStationModeRecords, stationModeRecords ) )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TD17328&TD17443 an emergency master mode overlapped another emergency master mode" );
			m_masterModeExecutionWorker.abortIfRequired();
		}
	}
	// ++TD17328&TD17443


	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"MasterModeManager::executeMasterMode - barrier end");

    m_masterModeExecutionWorker.execute(m_theDatapoints.getCurrentMasterMode(),masterMode,
				sessionId, stationModeRecords);

    FUNCTION_EXIT;
}


void MasterModeManager::selectMasterMode(CORBA::Long masterMode, const char* sessionId)
{
	FUNCTION_ENTRY("selectMasterMode");
    LOGOUT("selectMasterMode");


	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"MasterModeManager::selectMasterMode - Master mode = %ld", masterMode);

	m_timerUtility.startPeriodicTimeOutClock(this, m_theConfig.getSelectionTimeoutInMSecs());
    
    TA_Base_Core::ThreadGuard guard( m_lock );

    SessionIdsIt sit = m_activeSessionIds.find(sessionId);

    if (sit == m_activeSessionIds.end())
    {
        // Not registered (failover inconsistency)
        m_activeSessionIds.insert(sessionId);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"Total sessions = %ld", m_activeSessionIds.size());

    }

    // Set selection ModeSymbols to either TA_IRS_Bus::ECS_NONE_AND_GUI_OPEN or the corresponding to the masterMode
    // Don't touch last applied ModeSymbols 
    //m_ecsArrowManager.setSelectedMasterMode(masterMode);

    TA_IRS_Bus::EcStationModeRecords stationModeRecords = 
        m_theMaps.getEcStationModeRecordsFromMasterMode(masterMode);

	int stationModeCounter = 0;
	m_externalStationMode.clear();
	m_externalMasterMode.clear();
    std::vector<unsigned long> locationKeys = m_theMaps.getAllStationEcsAgentLocationKeys();
	
    for (std::vector<unsigned long>::iterator it=locationKeys.begin(); it!=locationKeys.end(); ++it)
    {      
        if (masterMode != TA_IRS_Bus::ECS_NONE_OR_INVALID &&
			stationModeCounter != stationModeRecords.length())
        {
			// Search in stationModeRecords for this locationKey
			bool found(false);
			for(int i=0; i<stationModeRecords.length(); ++i)
			{
				if (stationModeRecords[i].locationKey == *it)
				{
					// Found locationKey - use respective stationMode
					m_externalMasterMode[*it] = masterMode;
					m_externalStationMode[*it] = stationModeRecords[i].stationMode;
					stationModeCounter++;
					found = true;
					break; // for i;
				}
			}

			if (true == found)
			{
				continue; 
			}	
        }

		m_externalMasterMode[*it] = TA_IRS_Bus::ECS_NONE_OR_INVALID;
		m_externalStationMode[*it] = TA_IRS_Bus::ECS_NONE_OR_INVALID;

    }

	// Set barrier to set datapoints
	//ACE_Barrier barrier(m_externalStationMode.size() + 1);
	ACE_Barrier barrier(0);

	processExternalStationDp(m_externalStationMode, m_externalMasterMode, barrier);

	//barrier.wait();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"MasterModeManager::selectMasterMode - barrier end");

    // Update selected arrows
    m_ecsArrowManager.setSelectedMasterMode(masterMode);
    
    FUNCTION_EXIT;
}

TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* MasterModeManager::getAllProposedModes()
{
	FUNCTION_ENTRY("getAllProposedModes");
    LOGOUT("getAllProposedModes");

    TA_Base_Core::ThreadGuard guard( m_lock );
    
	TA_Base_Bus::GenericAgent::ensureControlMode();
    TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* disposableData = 
        new TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes();

    m_theCompleteState.getProposedModes(*disposableData);

    FUNCTION_EXIT;

    return disposableData;
}

std::list<unsigned long> MasterModeManager::getStationsInLocalMode(unsigned long masterMode)
{
    std::list<unsigned long> stationsInLocalMode;

    TA_IRS_Bus::EcStationModeRecords stationModeRecords = 
        m_theMaps.getEcStationModeRecordsFromMasterMode(masterMode);

    for (int i=0; i<stationModeRecords.length(); ++i)
    {
        if (true == m_theDatapoints.getExternalStationInLocalMode(stationModeRecords[i].locationKey))
        {
            stationsInLocalMode.push_back(stationModeRecords[i].locationKey);
        }
    }

    return stationsInLocalMode;
}


std::string MasterModeManager::toCommaSeparatedLocations(const std::list<unsigned long>& locationKeys)
{
    if (locationKeys.size() == 0)
    {
        return "-";
    }

    std::ostringstream oss;
    std::list<unsigned long>::const_iterator it = locationKeys.begin();
    
    for (; it!=locationKeys.end(); ++it)
    {
        oss << m_theMaps.getLocationName(*it) << ",";
    }

    // return above minus the last comma
    return oss.str().substr(0, oss.str().size()-1);
}


//TD16709
CORBA::UShort MasterModeManager::getFlexibleTimeOutValue()
{
	return CORBA_TIMEOUT;
}


void MasterModeManager::processExternalStationDp(std::map<unsigned long, long>& externalStationMode,
												 std::map<unsigned long, long>& externalMasterMode,
												 ACE_Barrier& barrier)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	for (std::map<unsigned long, long>::iterator iter=externalStationMode.begin();
		 iter != externalStationMode.end();
		 ++iter)
	{
		ExternalStationWorkQueueItem *item = new ExternalStationWorkQueueItem(
			m_theDatapoints, iter->first, iter->second, externalMasterMode[iter->first], barrier);

		m_workQueueManager->execute(item);
	}
}


std::set<unsigned long> MasterModeManager::getStationsLocationByZone(unsigned long zoneId)
{
	MasterModePair masterModePair = m_theMaps.getMasterModesFromZoneId(zoneId);
	std::set<unsigned long> affectedStnLocList;
	std::set<unsigned long>::const_iterator affectedStnLocIter;

	if(masterModePair.first != TA_IRS_Bus::ECS_NONE_OR_INVALID)
	{
		TA_IRS_Bus::EcStationModeRecords stationModeRecords = 
			m_theMaps.getEcStationModeRecordsFromMasterMode(masterModePair.first);
		for(int i=0; i<stationModeRecords.length(); ++i)
		{
			affectedStnLocList.insert(stationModeRecords[i].locationKey);
		}                
	}

	if (masterModePair.second != TA_IRS_Bus::ECS_NONE_OR_INVALID)
	{
		TA_IRS_Bus::EcStationModeRecords stationModeRecords = 
			m_theMaps.getEcStationModeRecordsFromMasterMode(masterModePair.second);
		for(int i=0; i<stationModeRecords.length(); ++i)
		{
			unsigned long locationKey = stationModeRecords[i].locationKey;

			affectedStnLocIter = affectedStnLocList.find(locationKey);
			if (affectedStnLocIter == affectedStnLocList.end())
			{
				affectedStnLocList.insert(stationModeRecords[i].locationKey);
			}
		}               
	}

	return affectedStnLocList;

}
