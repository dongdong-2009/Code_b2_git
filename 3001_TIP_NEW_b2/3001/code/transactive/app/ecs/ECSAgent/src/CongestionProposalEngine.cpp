/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/CongestionProposalEngine.cpp $
  * @author:  Ripple
  * @version: $Revision: #9 $
  *
  * Last modification: $DateTime: 2017/12/08 17:42:23 $
  * Last modified by:  $Author: Ouyang $
  *
  */


#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include "app/ecs/ECSAgent/src/EcsHelpers.h"
#include "app/ecs/ECSAgent/src/CongestionProposalEngine.h"
#include "app/ecs/ECSAgent/src/ItaEcsAgentCompleteState.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedMaps.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"


#include <algorithm>


CongestionProposalEngine::CongestionProposalEngine(
    ItaEcsCachedConfig& theConfig, 
    ItaEcsCachedMaps& theMaps, 
    ItaEcsDatapoints& theDatapoints,
    ItaEcsAgentCompleteState& theCompleteState)
: m_theMaps(theMaps),
  m_theConfig(theConfig),
  m_theDatapoints(theDatapoints),
  m_theCompleteState(theCompleteState),
  m_booleanContext(theDatapoints)
{
    FUNCTION_ENTRY("Constructor");

    refreshMaps();

    m_theDatapoints.attachCongestionObserver(this);

    FUNCTION_EXIT;
}


// ExceptionChecked
CongestionProposalEngine::~CongestionProposalEngine()
{
    FUNCTION_ENTRY("Destructor");

    m_theDatapoints.detachCongestionObserver(this);

	cleanUpMaps();

    FUNCTION_EXIT;
}

void CongestionProposalEngine::cleanUpMaps()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdates );

	LocationToEvaluationDetailsMapIt it; 
	for (it=m_locationEvaluationDetails.begin(); it!=m_locationEvaluationDetails.end(); ++it)
	{
		delete (it->second).m_expression;
	}

    m_locationEvaluationDetails.clear();
	m_zoneLocations.clear();
}


void CongestionProposalEngine::setControl(bool isOnStartup)
{
    AbstractFailoverable::setControl(isOnStartup);

	// Reprocess MODE_STATION proposal for all locations
    m_theCompleteState.clearProposedModes(TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_STATION);
    std::vector<unsigned long> locationKeys = m_theMaps.getAllStationEcsAgentLocationKeys();

    std::vector<unsigned long>::iterator it; 
    for (it= locationKeys.begin(); it!=locationKeys.end(); ++it)
    {
        processProposalsForLocation(*it, TA_IRS_Bus::INVALID_LOCATION_KEY_OF_CONGESTION_PAIR);
    }

    m_theCompleteState.commitProposedModes();
}

void CongestionProposalEngine::setMonitor(bool isOnStartup)
{
    AbstractFailoverable::setMonitor(isOnStartup);
}

void CongestionProposalEngine::processCongestionUpdate(unsigned long zoneId, unsigned long value)
{
    if (!m_isEnabled)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode");
        return;
    }

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "Processing congestion notification at ECS Zone %ld with value %ld", zoneId, value);

	// Perform congestion initialization
	std::pair<ZoneToLocationsMapIt, ZoneToLocationsMapIt> range = m_zoneLocations.equal_range(zoneId);

    for (ZoneToLocationsMapIt iter = range.first; iter != range.second; ++iter)
    {
		m_theCompleteState.clearProposedStationModes(iter->second);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "Clear station proposals for station %lu before process new congestion update.", iter->second);    
	}

	// Find locations of congestion pair
	range = m_zoneLocations.equal_range(zoneId);

    if ( m_zoneLocations.end() == range.first )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Zone id %d is not found in location map", zoneId );
        return;
    }

	unsigned long firstLocationKey = (range.first)->second;
	unsigned long secondLocationKey = TA_IRS_Bus::INVALID_LOCATION_KEY_OF_CONGESTION_PAIR;

    for (ZoneToLocationsMapIt iter=range.first; iter!=range.second; ++iter)
    {  
		if(firstLocationKey!=iter->second)
		{
			secondLocationKey = iter->second;
			break;
		}
    }

	processProposalsForLocation(firstLocationKey, secondLocationKey);

    m_theCompleteState.commitProposedModes();
}


bool CongestionProposalEngine::processProposalForLocation( unsigned long location, unsigned long ulPairKey )
{
    if( TA_IRS_Bus::INVALID_LOCATION_KEY_OF_CONGESTION_PAIR == location )
    {
        return false;
    }

    std::pair<LocationToEvaluationDetailsMapIt, LocationToEvaluationDetailsMapIt> range = m_locationEvaluationDetails.equal_range(location);

    bool bMatched = false;

    for (LocationToEvaluationDetailsMapIt iter=range.first; iter!=range.second; ++iter)
    {
        EvaluationDetail& detail = iter->second;

        if ( NULL == detail.m_expression )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "ExpressionEvaluator is NULL for station %lu failed", location);
            continue;
        }

        try
        {
            bMatched = (detail.m_expression)->evaluateBooleanExpression();
        }
        catch (TA_Base_Core::MathematicalEvaluationException& )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Failed to evaluate congestion expression - process congestion proposals for station %lu failed", location);
        }

        if ( bMatched )
        {
            // Lazy evaluation - we quit after the first success

            // Add proposed mode for congestion location
            TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode proposedStationMode;
            proposedStationMode.mode = detail.m_stationMode;
            proposedStationMode.locationKey = location;
            proposedStationMode.locationKeyOfCongestionPair = ulPairKey;
            proposedStationMode.modeType = TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_STATION;

            m_theCompleteState.addProposedMode(proposedStationMode); 

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "Evaluated Station Mode %ld for station %lu, congestion proposal succeed.",
                proposedStationMode.mode, proposedStationMode.locationKey); 

            break;
        } 
    }

    return bMatched;
}

void CongestionProposalEngine::processProposalsForLocation(unsigned long location, unsigned long locationKeyOfCongestionPair)
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdates );
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"Need to evaluate proposals for congested pair of stations: %lu and %lu.", location, locationKeyOfCongestionPair);

    // Expect three expressions to calculate usually.
    bool congestionProposed = processProposalForLocation(location, locationKeyOfCongestionPair);
    bool pairCongestionProposed = processProposalForLocation(locationKeyOfCongestionPair, location);

	//
	// See if we need to propose the Normal Station Mode
	//

    bool bProposedNormal = false;
	// No need to propose normal mode if congestion proposed
    if (!congestionProposed)
    {
        ProposedModeSet overridingStationModes;
        // No need to propose normal mode if overriding station mode proposed 
        m_theCompleteState.getProposedModeSet(location, TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_OVERRIDING_STATION, overridingStationModes);
        bProposedNormal = overridingStationModes.empty();
    }

    bool bProposedPairNormal = false;
    if ( !pairCongestionProposed )
    {
        ProposedModeSet overridingStationModes;
        // No need to propose normal mode if overriding station mode proposed 
        m_theCompleteState.getProposedModeSet(locationKeyOfCongestionPair, TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_OVERRIDING_STATION, overridingStationModes);
        bProposedPairNormal = overridingStationModes.empty();
    }

    if ( !bProposedNormal && !bProposedPairNormal )
    {
        return;
    }

	//
    // We are clear to propose the Normal station mode as there are no congestions nor master modes proposed.
	//

    // I slight problem - The Normal station modes are not known by the 
    // Master ECS Agent directly.  We have to find this station's normal
    // Station Mode by looking at the Normal Master Mode.
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"No congestion or master mode proposals found, now find normal modes for the current pair of stations: %lu and %lu...", location, locationKeyOfCongestionPair);

	TA_IRS_Bus::EcStationModeRecords normalStationModes = m_theMaps.getEcStationModeRecordsFromMasterMode(m_theConfig.getNormalMasterMode());
    // In case the Normal station mode cannot be found
    bool normalStationModeFound = !bProposedNormal;
	bool pairStationModeFound = !bProposedPairNormal && (locationKeyOfCongestionPair == TA_IRS_Bus::INVALID_LOCATION_KEY_OF_CONGESTION_PAIR);

    for (int i=0; i< normalStationModes.length(); ++i)
    {
        // Find normal station mode for the location of congestion pair

        if (!normalStationModeFound && (normalStationModes[i].locationKey == location))
		{
            normalStationModeFound = true;

            // Mode proposal

            TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode proposedStationMode;

            proposedStationMode.mode = normalStationModes[i].stationMode;
            proposedStationMode.locationKey = location;
            proposedStationMode.locationKeyOfCongestionPair = TA_IRS_Bus::INVALID_LOCATION_KEY_OF_CONGESTION_PAIR;
            proposedStationMode.modeType = TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_STATION;

            m_theCompleteState.addProposedMode(proposedStationMode);

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "Evaluated Normal Station Mode %ld for station %lu, congestion over.", 
                proposedStationMode.mode, proposedStationMode.locationKey); 
		}
        else if (!pairStationModeFound && (normalStationModes[i].locationKey == locationKeyOfCongestionPair))
        {
            pairStationModeFound = true;

            // Mode proposal

            TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode proposedStationMode;

            proposedStationMode.mode = normalStationModes[i].stationMode;
            proposedStationMode.locationKey = locationKeyOfCongestionPair;
            proposedStationMode.locationKeyOfCongestionPair = TA_IRS_Bus::INVALID_LOCATION_KEY_OF_CONGESTION_PAIR;
            proposedStationMode.modeType = TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_STATION;

            m_theCompleteState.addProposedMode(proposedStationMode);

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "Evaluated Normal Station Mode %ld for paired station %lu, congestion over.", 
                proposedStationMode.mode, proposedStationMode.locationKey); 
		}

		if (pairStationModeFound && normalStationModeFound)
		{
			break;
		}  
    } 

    if (!normalStationModeFound)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Cannot propose the Normal Station Mode for %lu, StationMode is not found.", location);        
    }

	if (!pairStationModeFound)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Cannot propose the Normal Station Mode for %lu, pairStationMode is not found.", locationKeyOfCongestionPair);        
    }
}

void CongestionProposalEngine::refreshMaps()
{
	cleanUpMaps();

	TA_IRS_Bus::EcStationModeRecords records;
	m_theMaps.getEcStationModeRecords(records);

	for (unsigned long i=0;i< records.length(); ++i)
	{
		// Check for a non-null CongestionLogic field
		if (strlen(records[i].congestionLogic.in()) == 0)
		{
			continue;
		}
		
		// Found congestion expression.
		TA_Base_Bus::ExpressionEvaluator* expression = new TA_Base_Bus::ExpressionEvaluator(records[i].congestionLogic.in());

        expression->addBooleanContext(&m_booleanContext);
		
		EvaluationDetail detail;
		detail.m_expression	 = expression;
		detail.m_stationMode = records[i].stationMode;

		m_locationEvaluationDetails.insert(Location2EvaluationDetail(records[i].locationKey, detail));
	
		// Parse congestionLogic for zone id
		std::string congestionLogic(static_cast<const char*>(records[i].congestionLogic));
		
        unsigned long lastFoundPos = 0;
		unsigned long ecsZoneId = getNextZoneId(congestionLogic, lastFoundPos);
		while (ecsZoneId != TA_IRS_Bus::ECS_NONE_OR_INVALID)
		{
            unsigned long loc = records[i].locationKey;

			ZoneToLocationsMapIt value = std::find(m_zoneLocations.begin(), m_zoneLocations.end(), Zone2Location(ecsZoneId, records[i].locationKey));
			if(value ==m_zoneLocations.end())
			{
				m_zoneLocations.insert(Zone2Location(ecsZoneId, records[i].locationKey));
			}

			ecsZoneId = getNextZoneId(congestionLogic, lastFoundPos);
		}
	}
}

unsigned long CongestionProposalEngine::getNextZoneId(std::string& congestionLogic, unsigned long& lastFoundPos)
{
	if (congestionLogic.empty())
	{
		return TA_IRS_Bus::ECS_NONE_OR_INVALID;
    }

    std::string::size_type startOfZoneId = congestionLogic.find_first_of(NUMERALS, lastFoundPos);

    if (startOfZoneId == std::string::npos)
    {
        congestionLogic = "";
        return TA_IRS_Bus::ECS_NONE_OR_INVALID;
    }

    std::string::size_type endOfZoneId = congestionLogic.find_first_not_of(NUMERALS, startOfZoneId);

    std::string zoneId;

    if (endOfZoneId == std::string::npos)
    {
        zoneId = congestionLogic.substr(startOfZoneId);
    }
    else
    {
        zoneId = congestionLogic.substr(startOfZoneId, endOfZoneId-startOfZoneId);
        lastFoundPos = endOfZoneId;
    }

	return atol(zoneId.c_str());
}
