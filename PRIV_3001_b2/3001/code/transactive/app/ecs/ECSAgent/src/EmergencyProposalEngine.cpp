/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/EmergencyProposalEngine.cpp $
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


#include "app/ecs/ECSAgent/src/EcsGlobals.h"
#include "app/ecs/ECSAgent/src/EmergencyProposalEngine.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedMaps.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/ItaEcsAgentCompleteState.h"

#include "core/utilities/src/DebugUtil.h"



EmergencyProposalEngine::EmergencyProposalEngine(
	ItaEcsCachedMaps& theMaps,
	ItaEcsCachedConfig& theConfig,
	ItaEcsAgentCompleteState& theCompleteState,
	ItaEcsDatapoints&	theDatapoints)
: m_theMaps(theMaps),
  m_theConfig(theConfig),
  m_theCompleteState(theCompleteState),
  m_theDatapoints(theDatapoints)
{
    FUNCTION_ENTRY("Constructor");

	m_theDatapoints.attachEmergencyObserver(this);

    FUNCTION_EXIT;
}


// ExceptionChecked
EmergencyProposalEngine::~EmergencyProposalEngine()
{
    FUNCTION_ENTRY("Destructor");

	m_theDatapoints.detachEmergencyObserver(this);

    FUNCTION_EXIT;
}


void EmergencyProposalEngine::processEmergencyUpdate(unsigned long zoneId, bool value)
{
	if (!m_isEnabled)
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode");
        return;
	}

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "Processing emergency notification at ECS Zone %ld with value %s", zoneId, value ? "True" : "False" );        

	// Update all emergency proposals
    m_theCompleteState.clearProposedModes(TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_MASTER);
	m_theCompleteState.clearProposedModes(TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_OVERRIDING_STATION);

	std::set<unsigned long> zones = m_theDatapoints.getZonesInEmergency();
	std::set<unsigned long>::iterator it = zones.begin();

	for (;it!=zones.end(); ++it)
	{
		try
		{
			// Determine mastermode(s)
			MasterModePair masterModes = m_theMaps.getMasterModesFromZoneId(*it);
			addMasterModeToProposals(masterModes.first);
			addMasterModeToProposals(masterModes.second);
        }
        catch ( const std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Unknown exception caught when getMasterModesFromZoneId" );
        }
	}

	addRemoveNormalModeToStationIfRequired();
	
	// broadcast update
	m_theCompleteState.commitProposedModes();
}

void EmergencyProposalEngine::addMasterModeToProposals(unsigned long masterMode)
{
	if (masterMode == TA_IRS_Bus::ECS_NONE_OR_INVALID)
	{
		return;
	}
	
	// MODE_MASTER proposal
	TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode proposedMasterMode;
	proposedMasterMode.mode = masterMode;
	proposedMasterMode.locationKey = TA_IRS_Bus::ECS_NONE_OR_INVALID; // Not required
	proposedMasterMode.modeType = TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_MASTER;

	m_theCompleteState.addProposedMode(proposedMasterMode);

	// For stations in the master mode, add/remove respective proposal
	TA_IRS_Bus::EcStationModeRecords stationModeRecords = 
			m_theMaps.getEcStationModeRecordsFromMasterMode(masterMode);

	TA_IRS_Bus::EcStationModeRecords normalStationModes = 
			m_theMaps.getEcStationModeRecordsFromMasterMode(m_theConfig.getNormalMasterMode());

    for (int i=0; i<stationModeRecords.length(); ++i)
    {
		// MODE_OVERRIDING_STATION proposal
		TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode proposedOverridingStationMode;
		proposedOverridingStationMode.mode = stationModeRecords[i].stationMode;
		proposedOverridingStationMode.locationKey = stationModeRecords[i].locationKey;
		proposedOverridingStationMode.modeType = TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_OVERRIDING_STATION;
		m_theCompleteState.addProposedMode(proposedOverridingStationMode);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"Add overriding Station Mode %lu for station %lu, emergency proposal succeed.",
					proposedOverridingStationMode.mode, proposedOverridingStationMode.locationKey); 
    }
}


void EmergencyProposalEngine::addRemoveNormalModeToStationIfRequired()
{
	TA_IRS_Bus::EcStationModeRecords normalStationModes = 
			m_theMaps.getEcStationModeRecordsFromMasterMode(m_theConfig.getNormalMasterMode());

	for (int i=0; i< normalStationModes.length(); ++i)
	{ 
		ProposedModeSet stationModes, overridingStationModes;
		m_theCompleteState.getProposedModeSet(normalStationModes[i].locationKey, 
			TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_STATION, stationModes);

		m_theCompleteState.getProposedModeSet(normalStationModes[i].locationKey, 
			TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_OVERRIDING_STATION, overridingStationModes);


		bool isNormalModeFound = false;
		bool isRequiredToSet = true;

		if (false == stationModes.empty())
		{
			ProposedModeSetIt it;
			for (it = stationModes.begin(); it != stationModes.end(); ++it)
			{
				if ((*it).mode == normalStationModes[i].stationMode )
				{ 
                    isNormalModeFound = true;
                    
					if (stationModes.size() > 1 || false == overridingStationModes.empty())
					{
						TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode proposedStationMode;
						proposedStationMode.mode = normalStationModes[i].stationMode;
						proposedStationMode.locationKey = normalStationModes[i].locationKey;
						proposedStationMode.locationKeyOfCongestionPair = TA_IRS_Bus::INVALID_LOCATION_KEY_OF_CONGESTION_PAIR;
						proposedStationMode.modeType = TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_STATION;
						m_theCompleteState.removeProposedMode(proposedStationMode);

						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
							"addRemoveNormalModeToStationIfRequired() - Remove Normal Station Mode %ld for station %s",
							proposedStationMode.mode, 
							m_theMaps.getLocationName(proposedStationMode.locationKey).c_str()); 
			
						isRequiredToSet = false;
						break;
					}
				}
									
				if	((*it).mode != normalStationModes[i].stationMode && stationModes.size() == 1)
				{
					isRequiredToSet = false;
					break;
				}
			}
				
			if (false == isRequiredToSet)
			{
				continue;
			}
		}

		if (false == isNormalModeFound)
		{
			if (true == overridingStationModes.empty() && true == stationModes.empty())
			{
				TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode proposedStationMode;
				proposedStationMode.mode = normalStationModes[i].stationMode;
				proposedStationMode.locationKey = normalStationModes[i].locationKey;
				proposedStationMode.locationKeyOfCongestionPair = TA_IRS_Bus::INVALID_LOCATION_KEY_OF_CONGESTION_PAIR;
				proposedStationMode.modeType = TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_STATION;
				m_theCompleteState.addProposedMode(proposedStationMode);

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
							"addNormalModeToStationIfRequired() - Add Normal Station Mode %ld for station %s",
							proposedStationMode.mode, 
							m_theMaps.getLocationName(proposedStationMode.locationKey).c_str()); 
			}
		}
	}
}


ItaEcsAgentCompleteState& EmergencyProposalEngine::getCompleteState()
{
	return m_theCompleteState;
}

