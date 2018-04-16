/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/CtaEcsAgentCompleteState.cpp $
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

#include <iostream>

#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/ItaEcsSynchronisation.h"
#include "app/ecs/ECSAgent/src/ItaEcsComms.h"
#include "app/ecs/ECSAgent/src/CtaEcsAgentCompleteState.h"
#include "app/ecs/ECSAgent/src/EcsHelpers.h"

#include "core/message/types/ECSAgentStateUpdate_MessageTypes.h"
#include "core/message/types/ECSAgentStateUpdateRequest_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/exceptions/src/ECSAgentExceptions.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"



CtaEcsAgentCompleteState::CtaEcsAgentCompleteState(
    ItaEcsComms& commsInstance, 
    ItaEcsSynchronisation& synchronisationInstance, 
    ItaEcsCachedConfig& cachedConfigInstance)
: m_isInControl(true),
  m_dataPoints(NULL),
  m_hasProposedModesChangedSinceCommit(false),
  m_commsInstance(commsInstance),
  m_synchronisationInstance(synchronisationInstance),
  m_cachedConfigInstance(cachedConfigInstance)
{
	FUNCTION_ENTRY("CtaEcsAgentCompleteState");

    m_stationModeState.fromStationMode  = TA_IRS_Bus::ECS_NONE_OR_INVALID;
    m_stationModeState.toStationMode    = TA_IRS_Bus::ECS_NONE_OR_INVALID;
    m_stationModeState.forMasterMode    = TA_IRS_Bus::ECS_NONE_OR_INVALID;
    m_stationModeState.locationKey      = m_cachedConfigInstance.getAgentLocationKey();
    m_stationModeState.sessionId        = const_cast<const char*>("");
    m_stationModeState.executionState   = TA_IRS_Bus::ECS_EXECUTION_READY;
    m_stationModeState.executionError   = TA_IRS_Bus::ECS_ERROR_NONE;
    m_stationModeState.additionalInfo   = const_cast<const char*>("");

	TA_IRS_Bus::IMasterEcsAgentCorbaDef::LocationIndex nullLocations;
    TA_IRS_Bus::IMasterEcsAgentCorbaDef::StationModeExecutionUpdateDatas nullStationStates;

    m_masterModeState.fromMasterMode        = TA_IRS_Bus::ECS_NONE_OR_INVALID;
    m_masterModeState.toMasterMode          = TA_IRS_Bus::ECS_NONE_OR_INVALID;
    m_masterModeState.locations             = nullLocations;
    m_masterModeState.sessionId             = const_cast<const char*>("");
    m_masterModeState.stationUpdateDatas    = nullStationStates;
    m_masterModeState.executionState        = TA_IRS_Bus::ECS_EXECUTION_READY;
    m_stationModeState.executionError       = TA_IRS_Bus::ECS_ERROR_NONE;
    m_masterModeState.additionalInfo        = const_cast<const char*>("");

	FUNCTION_EXIT;

}


CtaEcsAgentCompleteState::~CtaEcsAgentCompleteState()
{
	FUNCTION_ENTRY("~CtaEcsAgentCompleteState");

	TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

	FUNCTION_EXIT;

}


void CtaEcsAgentCompleteState::setMonitor(bool isOnStartup)
{
	FUNCTION_ENTRY("setMonitor");

    // Unsubscribe to Control messages
    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
			dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this));

    // Subscribe to Monitor messages
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
        TA_Base_Core::ECSAgentStateUpdate::EcsAgentParameterUpdate,
		dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this),
        m_cachedConfigInstance.getAgentName());

    if (isOnStartup) 
    {
        m_synchronisationInstance.sendEcsAgentStateUpdateRequest();
    }

    m_isInControl = false;

	FUNCTION_EXIT;

}

void CtaEcsAgentCompleteState::setControl(bool isOnStartup)
{
	FUNCTION_ENTRY("setControl");

    // Unsubscribe to Monitor messages
    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
			dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this));

    // Subscribe to Control messages
	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
        TA_Base_Core::ECSAgentStateUpdateRequest::EcsAgentStateUpdateRequest,
		dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this),
        m_cachedConfigInstance.getAgentName());

    m_isInControl = true;

	FUNCTION_EXIT;
}


void CtaEcsAgentCompleteState::setDataPoints(std::vector<TA_Base_Bus::DataPoint*>& dataPoints)
{
	FUNCTION_ENTRY("setDataPoints");

    TA_Base_Core::ThreadGuard guard( m_dataPointsLock );

    m_dataPoints = &dataPoints;

	FUNCTION_EXIT;
}


// ExceptionChecked
void CtaEcsAgentCompleteState::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
{
 	FUNCTION_ENTRY("receiveSpecialisedMessage");

    std::string messageTypeKey(message.messageTypeKey);
    if (messageTypeKey == TA_Base_Core::ECSAgentStateUpdate::EcsAgentParameterUpdate.getTypeKey() )
    {
        processEcsAgentParameterUpdate(message);
    }
    else if (messageTypeKey == TA_Base_Core::ECSAgentStateUpdateRequest::EcsAgentStateUpdateRequest.getTypeKey() )
    {
        processEcsAgentStateUpdateRequest();
    }
    else
    {
    	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unexpected message received");
    }

    FUNCTION_EXIT;
}



// Called by another thread - need to ensure threadsafety of the datapoint container
// Note that we deliberately avoid sending the DataPointStateUpdateMessage because this
// would affect other clients such as the OPC Bridge.  Instead we send the ECS Agent specific
// message.

// ExceptionChecked
void CtaEcsAgentCompleteState::processEcsAgentStateUpdateRequest()
{
    FUNCTION_ENTRY("processECSAgentStateUpdateRequest");

    try
    {
        if(m_cachedConfigInstance.getIsMaster())
        {
            // Broadcast all master data
            sendProposedModes();
            sendMasterModeState();
            // We don't send datapoint internal states as this is only for ForcedDataPointSynchroniser
        }
        else
        {
            // Broadcast all station data
            sendStationModeState();
            // We don't send datapoint internal states as this is only for ForcedDataPointSynchroniser
        }
    }
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "Exception caught while processing event");
	}

    FUNCTION_EXIT;
}



/////////////////////////////////////////////////////////
// The following is taken from the VirtualDatapointAgent
//////////////////////////////////////////////////////////



// Called by another thread - need to ensure threadsafety of the datapoint container
//
// ExceptionChecked
void CtaEcsAgentCompleteState::processEcsAgentParameterUpdate( 
    const TA_Base_Core::StateUpdateMessageCorbaDef& message )
{
    FUNCTION_ENTRY("processEcsAgentParameterUpdate");

    try
    {
        const TA_IRS_Bus::EcsAgentParameter* ecsAgentParameter;
        
        // if successfull to extract the EcsAgentParameter
		if ( 0 == ( message.stateUpdateInfo >>= ecsAgentParameter ) )
		{
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
						"Failed to interpret state update");
 			FUNCTION_EXIT;
            return;
		}
        else if (ecsAgentParameter->parameter == TA_IRS_Bus::PARAM_PROPOSED_MODES)
        {
            receiveProposedModes(ecsAgentParameter->data);
        }
        else if (ecsAgentParameter->parameter == TA_IRS_Bus::PARAM_STATION_MODE_EXECUTION_UPDATE_DATA)
        {
            receiveStationModeState(ecsAgentParameter->data);
        }
        else if (ecsAgentParameter->parameter == TA_IRS_Bus::PARAM_MASTER_MODE_EXECUTION_UPDATE_DATA)
        {
            receiveMasterModeState(ecsAgentParameter->data);
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
						"Unrecognised EcsAgentParameter ignored");
			FUNCTION_EXIT;
            return;
        }
        
        ecsAgentParameter = NULL;
   	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Exception caught while processing state update");
	}

    FUNCTION_EXIT;
}



///////////////////////////////////////////////////////////////////////////////
//
// Set Methods
//
///////////////////////////////////////////////////////////////////////////////


void CtaEcsAgentCompleteState::setStationModeState(const TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& stationModeState)
{
	FUNCTION_ENTRY("setStationModeState");

	TA_Base_Core::ThreadGuard guard( m_stationModeStateLock );

    if (true == m_isInControl)
    {
        // Only broadcast if contents have changed
        if (EcsHelpers::isNotEqual(m_stationModeState, stationModeState))
        {
            m_stationModeState = stationModeState;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "current station mode state change: %li, %li, %li, %d, %d", 
				stationModeState.fromStationMode, 
				stationModeState.toStationMode, 
				stationModeState.forMasterMode, 
				stationModeState.executionState, 
				stationModeState.executionError);

            // Synchronise Monitor instance
            sendStationModeState();

            // Update ECS Managers
            TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData* disposableUpdateData= 
                new TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData();
               
            *disposableUpdateData = m_stationModeState;

            m_commsInstance.sendEcsStationModeExecutionUpdate
                (disposableUpdateData,
                (const char *)m_stationModeState.sessionId);
        }       
    }
    else 
    {
        // This assignment is not made if in Control and contents are the same
        m_stationModeState = stationModeState;
    }

	FUNCTION_EXIT;
}


void CtaEcsAgentCompleteState::setMasterModeState(const TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData& masterModeState)
{
	FUNCTION_ENTRY("setMasterModeState");

	TA_Base_Core::ThreadGuard guard( m_masterModeStateLock );

    if (true==m_isInControl)
    {
        // Only broadcast if contents have changed
        if (EcsHelpers::isNotEqual(m_masterModeState, masterModeState))
        {
            m_masterModeState = masterModeState;

            // Synchronise Monitor instance
            sendMasterModeState();

            // Update ECS Managers
            TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData* disposableUpdateData= 
                new TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData();
               
            *disposableUpdateData = m_masterModeState;

            m_commsInstance.sendEcsMasterModeExecutionUpdate
                (disposableUpdateData,
                (const char *)m_masterModeState.sessionId);
        }
    }
    else 
    {
        // This assignment is not made if in Control and contents are the same
        m_masterModeState = masterModeState;
    }

	FUNCTION_EXIT;

}

void CtaEcsAgentCompleteState::commitProposedModes()
{
	FUNCTION_ENTRY("commitProposedModes");

	TA_Base_Core::ThreadGuard guard( m_proposedModeSetLock );

    if (false==m_isInControl || false==m_hasProposedModesChangedSinceCommit)
    {
		m_hasProposedModesChangedSinceCommit = false;
		FUNCTION_EXIT;
		return;
	}

	// Only synchronise if contents have changed

    // BroadcastUpdate
    sendProposedModes();

    // Update ECS Managers
    TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData* disposableUpdateData= 
        new TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData();

    EcsHelpers::proposedModeSetToCorba(m_proposedModeSet, *disposableUpdateData);

    m_commsInstance.sendEcsProposedModesUpdate(disposableUpdateData);
          
    m_hasProposedModesChangedSinceCommit = false;

	FUNCTION_EXIT;
}

// Modifiers for ProposedModes

void CtaEcsAgentCompleteState::addProposedMode(const TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode& mode)
{
	FUNCTION_ENTRY("addProposedMode");

	switch (mode.modeType)
	{
    case TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_STATION:
		{
		    // Unique for a station.
			TA_Base_Core::ThreadGuard guard( m_proposedModeSetLock );

			m_proposedModeSet.insert(mode);
    		m_hasProposedModesChangedSinceCommit = true;
		}
		break;
	case TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_MASTER:
		{
		    // Cumulative across circle line

			// Create a copy so we force the locationKey to a common invalid
			// value (ECS_NONE_OR_INVALID).  This will ensure that the std::set
			// equality matching ignores the locationKey for MasterModes

			TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode copyOfMode(mode);
			copyOfMode.locationKey = TA_IRS_Bus::ECS_NONE_OR_INVALID;

			TA_Base_Core::ThreadGuard guard( m_proposedModeSetLock );
    
			ProposedModeSetIt it = m_proposedModeSet.find(copyOfMode);
			if (it == m_proposedModeSet.end())
			{    
				m_proposedModeSet.insert(copyOfMode);
				m_hasProposedModesChangedSinceCommit = true;
			}
		}
		break;
	case TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_OVERRIDING_STATION:
		{
		    // Cumulative across circle line
			TA_Base_Core::ThreadGuard guard( m_proposedModeSetLock );

			ProposedModeSetIt it = m_proposedModeSet.find(mode);
			if (it == m_proposedModeSet.end())
			{    
				m_proposedModeSet.insert(mode);
				m_hasProposedModesChangedSinceCommit = true;
			}
		}
		break;
	default:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown proposed mode received");
		break;
	}

	FUNCTION_EXIT;
}

void CtaEcsAgentCompleteState::removeProposedMode(const TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedMode& mode)
{
	FUNCTION_ENTRY("removeProposedMode");

	TA_Base_Core::ThreadGuard guard( m_proposedModeSetLock );

    ProposedModeSetIt it = m_proposedModeSet.find(mode);
    if (it != m_proposedModeSet.end())
    {
        m_proposedModeSet.erase(it);
        m_hasProposedModesChangedSinceCommit = true;
    }

	FUNCTION_EXIT;
}


void CtaEcsAgentCompleteState::clearProposedModes(TA_IRS_Bus::IMasterEcsAgentCorbaDef::EProposedModeType modeType)
{
	FUNCTION_ENTRY("clearProposedModes");

	TA_Base_Core::ThreadGuard guard( m_proposedModeSetLock );

    ProposedModeSetIt it = m_proposedModeSet.begin();
    while (it != m_proposedModeSet.end())
    {   
		if ((*it).modeType == modeType)
        {
			//zn++ - CL20717
            //m_proposedModeSet.erase(it);
			m_proposedModeSet.erase(it++);
			//++zn
            m_hasProposedModesChangedSinceCommit = true;
        }
		else
		{
			++it;
		}
    }

	FUNCTION_EXIT;
}

void CtaEcsAgentCompleteState::clearProposedOverridingStationModes(unsigned long locationKey)
{
	FUNCTION_ENTRY("clearProposedOverridingStationModes");

	TA_Base_Core::ThreadGuard guard( m_proposedModeSetLock );

    ProposedModeSetIt it=m_proposedModeSet.begin();
    while (it != m_proposedModeSet.end())
    {
        if ((*it).modeType == TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_OVERRIDING_STATION && 
            (*it).locationKey == locationKey)
        {
            m_proposedModeSet.erase(it++);
            m_hasProposedModesChangedSinceCommit = true;
        }
		else
		{   
			++it;
		}
    }

	FUNCTION_EXIT;
}

void CtaEcsAgentCompleteState::clearProposedStationModes(unsigned long locationKey)
{
	FUNCTION_ENTRY("clearProposedStationModes");

	TA_Base_Core::ThreadGuard guard( m_proposedModeSetLock );

    ProposedModeSetIt it=m_proposedModeSet.begin();
    while (it != m_proposedModeSet.end())
    {
        if ((*it).modeType == TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_STATION && 
            (*it).locationKey == locationKey)
        {
            m_proposedModeSet.erase(it++);
            m_hasProposedModesChangedSinceCommit = true;
        }
        else
        {
            ++it;  
        }
    }

	FUNCTION_EXIT;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get Methods
//
///////////////////////////////////////////////////////////////////////////////

void CtaEcsAgentCompleteState::getStationModeState(TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData& returnValue)
{
	FUNCTION_ENTRY("getStationModeState");

	TA_Base_Core::ThreadGuard guard( m_stationModeStateLock );

    returnValue = m_stationModeState;

	FUNCTION_EXIT;
}

void CtaEcsAgentCompleteState::getStationModeState(TA_IRS_Bus::EEcsExecutionState& returnValue)
{
	FUNCTION_ENTRY("getStationModeState");

	TA_Base_Core::ThreadGuard guard( m_stationModeStateLock );

    returnValue = m_stationModeState.executionState;

	FUNCTION_EXIT;
}

void CtaEcsAgentCompleteState::getMasterModeState(TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData& returnValue)
{
	FUNCTION_ENTRY("getMasterModeState");

	TA_Base_Core::ThreadGuard guard( m_masterModeStateLock );

    returnValue = m_masterModeState;

	FUNCTION_EXIT;
}

void CtaEcsAgentCompleteState::getProposedModeSet(
    unsigned long locationKey, 
    TA_IRS_Bus::IMasterEcsAgentCorbaDef::EProposedModeType modeType,
    ProposedModeSet& returnValue)
{
	FUNCTION_ENTRY("getProposedModeSet");

	TA_Base_Core::ThreadGuard guard( m_proposedModeSetLock );

    returnValue.clear();

    for (ProposedModeSetIt it = m_proposedModeSet.begin(); it != m_proposedModeSet.end(); ++it)
    {
        if ( ((*it).modeType == modeType) &&
             ((*it).locationKey == locationKey))
        {
            returnValue.insert(*it);
        }
    }

	FUNCTION_EXIT;
}

void CtaEcsAgentCompleteState::getProposedModes(TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes& returnValue)
{
	FUNCTION_ENTRY("getProposedModes");

	TA_Base_Core::ThreadGuard guard( m_proposedModeSetLock );

    EcsHelpers::proposedModeSetToCorba(m_proposedModeSet, returnValue);

	FUNCTION_EXIT;
}

void CtaEcsAgentCompleteState::getProposedModeSet(ProposedModeSet& returnValue)
{
	FUNCTION_ENTRY("getProposedModeSet");

	TA_Base_Core::ThreadGuard guard( m_proposedModeSetLock );

    returnValue = m_proposedModeSet;

	FUNCTION_EXIT;
}

///////////////////////////////////////////////////////////////////////////////
//
// Send To Monitor Methods
//
///////////////////////////////////////////////////////////////////////////////

void CtaEcsAgentCompleteState::sendStationModeState()
{
	FUNCTION_ENTRY("sendStationModeState");

	TA_Base_Core::ThreadGuard guard( m_stationModeStateLock );

    CORBA::Any data;

    data <<= m_stationModeState;

    m_synchronisationInstance.sendEcsAgentParameterUpdate(TA_IRS_Bus::PARAM_STATION_MODE_EXECUTION_UPDATE_DATA, data);
	FUNCTION_EXIT;
}


void CtaEcsAgentCompleteState::sendMasterModeState()
{
	FUNCTION_ENTRY("sendMasterModeState");

	TA_Base_Core::ThreadGuard guard( m_masterModeStateLock );

    CORBA::Any data;

    data <<= m_masterModeState;

    m_synchronisationInstance.sendEcsAgentParameterUpdate(TA_IRS_Bus::PARAM_MASTER_MODE_EXECUTION_UPDATE_DATA, data);
	FUNCTION_EXIT;

}

void CtaEcsAgentCompleteState::sendProposedModes()
{
	FUNCTION_ENTRY("sendProposedModes");

	TA_Base_Core::ThreadGuard guard( m_proposedModeSetLock );

    // Convert to CORBA format
    TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes proposedModes;
    EcsHelpers::proposedModeSetToCorba(m_proposedModeSet, proposedModes);

    CORBA::Any data;

    data <<= proposedModes;

    m_synchronisationInstance.sendEcsAgentParameterUpdate(TA_IRS_Bus::PARAM_PROPOSED_MODES, data);
	FUNCTION_EXIT;
}

void CtaEcsAgentCompleteState::receiveStationModeState(const CORBA::Any& data)
{
	FUNCTION_ENTRY("receiveStationModeState");

	TA_Base_Core::ThreadGuard guard( m_stationModeStateLock );

    const TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData* stationModeState = NULL;

    if(0 == (data >>= stationModeState))
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"Failed to interpret state update for StationModeState");
		FUNCTION_EXIT;
        return;
    }

    m_stationModeState = *stationModeState;

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Received state update for StationModeState");

	FUNCTION_EXIT;
}


void CtaEcsAgentCompleteState::receiveMasterModeState(const CORBA::Any& data)
{
	FUNCTION_ENTRY("receiveMasterModeState");

	TA_Base_Core::ThreadGuard guard( m_masterModeStateLock );

    const TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData* masterModeState = NULL;

    if(0 == (data >>= masterModeState))
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"Failed to interpret state update for MasterModeState");
		FUNCTION_EXIT;
        return;
    }

    m_masterModeState = *masterModeState;

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Received state update for MasterModeState");

	FUNCTION_EXIT;
}

void CtaEcsAgentCompleteState::receiveProposedModes(const CORBA::Any& data)
{
	FUNCTION_ENTRY("receiveProposedModes");

    const TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* proposedModes;

    if(0 == (data >>= proposedModes))
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"Failed to interpret state update for ProposedModes");
		FUNCTION_EXIT;
        return;
    }
    
    TA_Base_Core::ThreadGuard guard( m_proposedModeSetLock );

    // Convert from CORBA format
    EcsHelpers::corbaToProposedModeSet(*proposedModes, m_proposedModeSet);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"Received state update for ProposedModes");

	FUNCTION_EXIT;
}

