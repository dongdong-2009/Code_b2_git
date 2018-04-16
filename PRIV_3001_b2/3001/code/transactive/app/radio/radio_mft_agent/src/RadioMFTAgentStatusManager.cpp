/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent/src/RadioMFTAgentStatusManager.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif


#include "app/radio/radio_mft_agent/src/RadioMFTAgentStatusManager.h"
#include "app/radio/radio_mft_agent/src/RadioResourceConfigFacade.h"
#include "app/radio/radio_mft_agent/src/IRadioMFTAgentCallback.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentHelper.h"

#include "core/data_access_interface/radio/src/IRadioDirectoryEntry.h"
#include "core/data_access_interface/radio/src/RadioDirectoryEntryFactory.h"
#include "core/data_access_interface/radio/src/IRadioDirectoryMember.h"
#include "core/data_access_interface/radio/src/RadioDirectoryMemberFactory.h"
#include "core/data_access_interface/radio/src/RadioSubscriberFactory.h"
#include "core/data_access_interface/radio/src/IRadioSubscriber.h"
#include "core/data_access_interface/radio/src/RadioBtnCfgFactory.h"
#include "core/data_access_interface/radio/src/IRadioBtnCfg.h"


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#include <algorithm>
#include <functional>

#include "app\radio\radio_mft_agent\src\TimerMngr.h"

TA_Base_Core::ReEntrantThreadLockable RadioMFTAgentStatusManager::m_lock;
TA_Base_Core::ReEntrantThreadLockable RadioMFTAgentStatusManager::m_flagConnectionLock;
TA_Base_Core::ReEntrantThreadLockable RadioMFTAgentStatusManager::m_flagTimerExpireLock;
TA_Base_Core::ReEntrantThreadLockable RadioMFTAgentStatusManager::m_radioMFtStateLock;

static bool IsGroupResource(RadioResourceType resType)
{
    return (resType == RR_PATCHGROUP || resType == RR_DYNGROUP ||
            resType == RR_MSELECT || resType == RR_MSITE);
}

static bool IsTransactiveResource(RadioResourceType resType)
{
    return (resType == RR_PATCHGROUP || resType == RR_DYNGROUP ||
            resType == RR_MSELECT || resType == RR_MSITE);
}

//++Fixed TD14320
struct ProfileMapData
{
	const char* name;
	int number;
};

static const ProfileMapData profileRack1[] = {
	{ "SD"	  , 1 },
	{ "ND"	  , 2 },
	{ "OLTC1" , 3 },
	{ "OLTC2" , 4 },
	{ "EC"    , 5 },
	{ "CC"    , 6 },
	{ "IA"    , 7 }
};

static const ProfileMapData profileRack2[] = {
	{ "MD"    , 1 },
	{ "TR"    , 2 },
	{ "PC"    , 3 },
	{ "RLTC1" , 4 },
	{ "RLTC2" , 5 },
	{ "ECS"   , 6 }
};

const unsigned long RadioMFTAgentStatusManager::MIN_DYNAMICGROUP_ID = 9000;
const unsigned long RadioMFTAgentStatusManager::MAX_DYNAMICGROUP_ID = 9999;
//--Fixed TD14320
RadioMFTAgentStatusManager::RadioMFTAgentStatusManager() : 
        m_bStart(true), m_RadioMFTAgentCallback(NULL), initialization(false), m_timeoutThread(NULL),m_radioStatus(false),m_iscsStatus(false),
		m_bIsMCCConnected(false),
		m_bIsTimerExpire(true),
		m_pvcSelectStatus(false),
		m_radioMFTState(UNKNOWN_STATE)
{
    FUNCTION_ENTRY("RadioMFTAgentStatusManager");
    
	m_profile = RadioMFTAgentHelper::getProfileName(); 

//TD18803
    bPermitProfile = true;
//TD18803
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Profile => %s",m_profile.c_str());

	m_timeoutHelper = new RemoteApiTimeoutHelper();
	//m_timeoutThread = new TA_Base_Core::AlarmClock(m_timeoutHelper);

	m_timerManager = new CTimerMngr (m_timeoutHelper);

    FUNCTION_EXIT;
}


RadioMFTAgentStatusManager::~RadioMFTAgentStatusManager()
{
    FUNCTION_ENTRY("~RadioMFTAgentStatusManager");
	
	if (m_timerManager)
	{
		delete m_timerManager;
		m_timerManager = NULL;
	}
	m_mapResourceIDFromChannelRec.clear();
    FUNCTION_EXIT;
}


void RadioMFTAgentStatusManager::setFeatureAttributeIsAssigned(const RadioResource& resource, bool isAssigned)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("setFeatureAttributeIsAssigned");

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		(*iter).second.isResourceAssigned = isAssigned;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}
	

    FUNCTION_EXIT;
}


bool RadioMFTAgentStatusManager::getFeatureAttributeIsAssigned(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("setFeatureAttributeIsAssigned");

	bool bIsAssigned = false;

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		RadioResourceMFTServerAttributes attr = (*iter).second;

		bIsAssigned = attr.isResourceAssigned;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}
	

    FUNCTION_EXIT;
	return bIsAssigned;
}

bool RadioMFTAgentStatusManager::getFeatureAttributeIsMonitored(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getFeatureAttributeIsMonitored");

	bool bIsMonitored = false;

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		RadioResourceMFTServerAttributes attr = (*iter).second;
		bIsMonitored = (attr.buttonOffset != 0);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

    FUNCTION_EXIT;

	return bIsMonitored;
}


bool RadioMFTAgentStatusManager::setFeatureAttributeButton(const RadioResource& resource, bool bProfile, unsigned int offset)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("setFeatureAttributeButton");

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );


	if (iter != m_MFTResourceCollection.end())
	{
		(*iter).second.buttonOffset = offset;
		(*iter).second.bIsProfile = bProfile;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
        return false;
	}
	
    FUNCTION_EXIT;
    return true;
}

bool RadioMFTAgentStatusManager::getFeatureAttributeButton(const RadioResource& resource, bool& bProfile, unsigned int& offset)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getFeatureAttributeButton");

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );


	if (iter != m_MFTResourceCollection.end())
	{
		RadioResourceMFTServerAttributes attr = (*iter).second;

		offset = attr.buttonOffset;
        bProfile = attr.bIsProfile;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
        bProfile = false;
        offset = 0;
        return false;
	}

    FUNCTION_EXIT;
	return true;
}


void RadioMFTAgentStatusManager::setFeatureAttributeIsSelected(const RadioResource& resource, AssociatedSpeaker state)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("setFeatureAttributeIsSelected");

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );


	if (iter != m_MFTResourceCollection.end())
	{
		(*iter).second.associatedSpeaker = state;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource[%s] not found in MFTResourceCollection.",resource.alias.c_str());
	}

    FUNCTION_EXIT;
	
}


AssociatedSpeaker RadioMFTAgentStatusManager::getFeatureAttributeIsSelected(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getFeatureAttributeIsSelected");

	AssociatedSpeaker speaker = NIL;

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		RadioResourceMFTServerAttributes attr = (*iter).second;

		speaker = attr.associatedSpeaker;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

    FUNCTION_EXIT;
	return speaker;
}

void RadioMFTAgentStatusManager::setFeatureAttributeSpeakerVolume(const RadioResource& resource, unsigned int volume)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("setFeatureAttributeSpeakerVolume");

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		(*iter).second.selectSpeakerVolume = volume;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}
	
    FUNCTION_EXIT;
	
}

unsigned int RadioMFTAgentStatusManager::getFeatureAttributeSpeakerVolume(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getFeatureAttributeSpeakerVolume");

	unsigned int volume = 0;

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		RadioResourceMFTServerAttributes attr = (*iter).second;

		volume = attr.selectSpeakerVolume;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

    FUNCTION_EXIT;
	
	return volume;
}


void RadioMFTAgentStatusManager::setFeatureAttributeUnSpeakerVolume(const RadioResource& resource, unsigned int volume)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	
    FUNCTION_ENTRY("setFeatureAttributeUnSpeakerVolume");

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		(*iter).second.unselectSpeakerVolume = volume;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

    FUNCTION_EXIT;
}


unsigned int RadioMFTAgentStatusManager::getFeatureAttributeUnSpeakerVolume(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getFeatureAttributeUnSpeakerVolume");

	unsigned int volume = 0;

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		RadioResourceMFTServerAttributes attr = (*iter).second;

		volume = attr.unselectSpeakerVolume;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

    FUNCTION_EXIT;
	return volume;
}


void RadioMFTAgentStatusManager::setFeatureAttributeIsGrpConnected(const RadioResource& resource, GroupConnectionState state)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("setFeatureAttributeIsGrpConnected");

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		(*iter).second.isGroupConnected = state;

		if (state != CONNECTED)
		{
			if (resource.type == RR_PATCHGROUP)
			{
				m_groupCollectionPG[resource.id] = 0;
			}
			else if (resource.type == RR_MSITE || resource.type == RR_MSELECT)
			{
				m_groupCollectionMSel[resource.id] = 0;
			}
		}

        // notify the manager of changes in connection state
        //if (m_RadioMFTAgentCallback != NULL)
        //{
        //    m_RadioMFTAgentCallback->callback_groupStateMessage(resource, state);
        //}
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

    FUNCTION_EXIT;
	
}


GroupConnectionState RadioMFTAgentStatusManager::getFeatureAttributeIsGrpConnected(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getFeatureAttributeIsGrpConnected");

	GroupConnectionState state = NAG;

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		RadioResourceMFTServerAttributes attr = (*iter).second;

		state = attr.isGroupConnected;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}
	
    FUNCTION_EXIT;

	return state;
}


void RadioMFTAgentStatusManager::setFeatureAttributeIsActive(const RadioResource& resource, bool isActive)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("setFeatureAttributeIsActive");

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		(*iter).second.isCurrentlyActive = isActive;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

    FUNCTION_EXIT;
	
}

bool RadioMFTAgentStatusManager::getFeatureAttributeIsActive(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getFeatureAttributeIsActive");

	bool isActive = false;

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		RadioResourceMFTServerAttributes attr = (*iter).second;

		isActive = attr.isCurrentlyActive;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}
	
    FUNCTION_EXIT;
	return isActive;
}


void RadioMFTAgentStatusManager::setFeatureAttributePtCallState(const RadioResource& resource, PrivateCallState state)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("setFeatureAttributePtCallState");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "setFeatureAttributePtCallState, state = %u", state);
	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		(*iter).second.privateCallState = state;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}
	
    FUNCTION_EXIT;
}

RadioResource RadioMFTAgentStatusManager::deactivateActivePrivateCall()
{
	TA_Base_Core::ThreadGuard guard( m_lock );

	FUNCTION_ENTRY("setFeatureAttributeAllResourcePtCallState");

	RadioResource r;
	r.initialise();

	MFTResourceCollection::iterator iter = m_MFTResourceCollection.begin();
	for(; iter != m_MFTResourceCollection.end(); iter++) {
		if (RR_RADIO == (*iter).first.type && PC_NOT_INVOLVED != (*iter).second.privateCallState) {
			(*iter).second.privateCallState = PC_NOT_INVOLVED;
			r = (*iter).first;
			break;
		}
	}
	if (0 != r.ssi) 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Found currently active private call resource. Id = %u", r.ssi);
	else
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Not found any currently active private call resource");

	FUNCTION_EXIT;

	return r;
}


PrivateCallState RadioMFTAgentStatusManager::getFeatureAttributePtCallState(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getFeatureAttributePtCallState");

	PrivateCallState state = PC_NOT_INVOLVED;

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		RadioResourceMFTServerAttributes attr = (*iter).second;

		state = attr.privateCallState;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

    FUNCTION_EXIT;
	return state;
}


void RadioMFTAgentStatusManager::setFeatureAttributeALCallState(const RadioResource& resource, AmbienceListeningCallState state)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("setFeatureAttributeALCallState");

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		(*iter).second.ambienceListeningCallSate = state;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

    FUNCTION_EXIT;
}


AmbienceListeningCallState RadioMFTAgentStatusManager::getFeatureAttributeALCallState(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getFeatureAttributeALCallState");

	AmbienceListeningCallState state = ALC_NOT_INVOLVED;

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		RadioResourceMFTServerAttributes attr = (*iter).second;

		state = attr.ambienceListeningCallSate;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

    FUNCTION_EXIT;
	return state;
}


void RadioMFTAgentStatusManager::setFeatureAttributeEmergencyMode(const RadioResource& resource, RadioEmergencyModeType type)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("setFeatureAttributeEmergencyMode");

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		(*iter).second.emergencyMode = type;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

    FUNCTION_EXIT;
}

RadioEmergencyModeType RadioMFTAgentStatusManager::getFeatureAttributeEmergencyMode(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getFeatureAttributeEmergencyMode");

	RadioEmergencyModeType type = RE_NO_EMERGENCY;

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
				CompareResource(resource) );
	
	if (iter != m_MFTResourceCollection.end())
	{
		RadioResourceMFTServerAttributes attr = (*iter).second;

		type = attr.emergencyMode;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

    FUNCTION_EXIT;
	return type;
}

void RadioMFTAgentStatusManager::setFeatureAttributeEmergencyRadioUnit(const RadioResource& resource, unsigned int radioSSI)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("setFeatureAttributeEmergencyRadioUnit");
	
	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
				CompareResource(resource) );
	
	if (iter != m_MFTResourceCollection.end())
	{
		(*iter).second.emergencyRadioSSI = radioSSI;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}
	
	FUNCTION_EXIT;
}

unsigned int RadioMFTAgentStatusManager::getFeatureAttributeEmergencyRadioUnit(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("getFeatureAttributeEmergencyRadioUnit");

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
				CompareResource(resource) );

	unsigned int radioSSI = 0;

	if (iter != m_MFTResourceCollection.end())
	{
		RadioResourceMFTServerAttributes attr = (*iter).second;

		radioSSI = attr.emergencyRadioSSI;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

	FUNCTION_EXIT;
	return radioSSI;
}

void RadioMFTAgentStatusManager::setFeatureAttributeIsPatchInvolved(const RadioResource& resource, unsigned int patchId)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("setFeatureAttributeIsPatchInvolved");

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
				CompareResource(resource) );

	unsigned int radioSSI = 0;

	if (iter != m_MFTResourceCollection.end())
	{
		iter->second.isPatchInvolved = patchId;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

	FUNCTION_EXIT;
}

unsigned int RadioMFTAgentStatusManager::getFeatureAttributeIsPatchInvolved(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("setFeatureAttributeIsPatchInvolved");

	unsigned int patchId = 0;
	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
				CompareResource(resource) );

	unsigned int radioSSI = 0;

	if (iter != m_MFTResourceCollection.end())
	{
		RadioResourceMFTServerAttributes attr = (*iter).second;
		patchId = attr.isPatchInvolved;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource [%s] not found in MFTResourceCollection.", resource.alias.c_str());
	}

	FUNCTION_EXIT;
	return patchId;
}

void RadioMFTAgentStatusManager::addResource(RadioResource& resource,std::vector<RadioResource>& members,unsigned long id)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("addResource");
	unsigned long rId = resource.id;

	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
				CompareResource(resource) );

	if (iter == m_MFTResourceCollection.end())
	{	
		bool checkAlias = false;
		for(iter = m_MFTResourceCollection.begin() ;iter!= m_MFTResourceCollection.end(); iter++) 
		{
			if ((*iter).first.type == resource.type && (*iter).first.alias.compare(resource.alias)==0) 
			{
				rId = (*iter).first.id;
				checkAlias = true;
				break;
			}
		}
		if (!checkAlias) {
		// initialise RadioResourceMFTServerAttributes;
		RadioResourceMFTServerAttributes attr;

		//add resource to MFTResourceCollection
		m_MFTResourceCollection[resource] = attr;
		}
	}
	else 
	{
		RadioResource r = (*iter).first;
		RadioResourceMFTServerAttributes attr = (*iter).second;
		m_MFTResourceCollection.erase(iter);

		r.alias = resource.alias;
		m_MFTResourceCollection[r] = attr;

	}

	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				 "Resource Collection Count : %d",m_MFTResourceCollection.size());
	
	switch(resource.type)
	{
		case RR_DYNGROUP:
		{
			m_dynGroupMembers[rId] = members;
		}
		break;
		case RR_PATCHGROUP: 
		{
			if ( m_groupCollectionPG.find(rId) == m_groupCollectionPG.end())
			{
				m_groupCollectionPG[rId] = id;
			}
			m_groupMembers[rId] = members;
		}
		break;

		case RR_MSELECT:
		case RR_MSITE:
		{
			if ( m_groupCollectionMSel.find(rId) == m_groupCollectionMSel.end())
			{
				m_groupCollectionMSel[rId] = id;
			}
			m_groupMembers[rId] = members;
		}
		break;
	}
	
	FUNCTION_EXIT;
}


void RadioMFTAgentStatusManager::delResource(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	
    FUNCTION_ENTRY("delResource");

	// delete from MFTResourceCollection
	MFTResourceCollection::iterator iter =  
			std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
						  CompareResource(resource) );

	if (iter != m_MFTResourceCollection.end())
	{
		m_MFTResourceCollection.erase(iter);
	}

	// delete from other Collections
	switch(resource.type)
	{
		case RR_RADIO:
		case RR_TALKGROUP:
		case RR_ANNGROUP:
		case RR_SITE:
		{
			ChannelCollection::iterator iter = m_channelCollection.find(resource.id);
			m_channelCollection.erase(iter);
		}
		break;
		case RR_DYNGROUP:
		{
			ResourceGroupMembersMap::iterator iter = m_dynGroupMembers.find(resource.id);

			if ( iter != m_dynGroupMembers.end() )
			{
				m_dynGroupMembers.erase(iter);
			}
		}
		break;

		case RR_PATCHGROUP: 
		{
			ResourceGroupCollection::iterator iter = m_groupCollectionPG.find(resource.id);

			if ( iter != m_groupCollectionPG.end() )
			{
				ResourceGroupMembersMap::iterator iMem = m_groupMembers.find(resource.id);
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "Deleting group, alias:%s", resource.alias.c_str());
				m_groupMembers.erase(iMem);	

				m_groupCollectionPG.erase(iter);
			}
		}
		break;

		case RR_MSELECT:
		case RR_MSITE:
		{
			ResourceGroupCollection::iterator iter = m_groupCollectionMSel.find(resource.id);

			if ( iter != m_groupCollectionMSel.end() )
			{
				ResourceGroupMembersMap::iterator iMem = m_groupMembers.find(resource.id);
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "Deleting group, alias:%s", resource.alias.c_str());
				m_groupMembers.erase(iMem);	

				m_groupCollectionMSel.erase(iter);
			}
		}
		break;

		default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unknown type in delResource");
	}


    FUNCTION_EXIT;
}


LogicalChannel RadioMFTAgentStatusManager::getChannel(ResourceIdentifier resourceID)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getChannel");

	LogicalChannel channelID = 0;

	// Precondition - resourceID type must be Radio,TG,AG,Site

	ChannelCollection::iterator value = m_channelCollection.find(resourceID);

	if ( value != m_channelCollection.end() )
	{
		channelID = (*value).second;
	}
	else
	{
		std::ostringstream logErr;
		logErr << "No CHANNEL assigned for RID[" << resourceID <<"]" <<std::endl;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "No channel assigned for RID %u", resourceID);
	}
	

    FUNCTION_EXIT;
    return channelID;
}


unsigned int RadioMFTAgentStatusManager::getGroupNumber(TransactiveResourceIdentifier rid)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getGroupNumber");

	unsigned int grpNum = 0;

	ResourceGroupCollection::iterator iter = m_groupCollectionPG.find(rid);

	if (iter != m_groupCollectionPG.end())
	{
        grpNum = (*iter).second;
	}	
	else
	{
		iter = m_groupCollectionMSel.find(rid);
		if (iter != m_groupCollectionMSel.end())
		{
			grpNum = (*iter).second;
		}	
	}

    FUNCTION_EXIT;
    return grpNum;
}


std::vector<RadioResource> RadioMFTAgentStatusManager::getGroupMembers(TransactiveResourceIdentifier rid)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getGroupMembers");
	
	ResourceGroupMembersMap::iterator iter = m_groupMembers.find(rid);
	
	unsigned int grpNum = 0;
	if (iter != m_groupMembers.end())
	{
		FUNCTION_EXIT
        return (*iter).second;
	}	

	std::vector<RadioResource> members;

    FUNCTION_EXIT;
    return members;
}


RadioResource RadioMFTAgentStatusManager::getResourcebyChannel(LogicalChannel identifier)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getResourcebyChannel");

    RadioResource returnValue;

	ResourceIdentifier rid = 0;

	ChannelCollection::iterator iter = m_channelCollection.begin();

	for(; iter!=m_channelCollection.end(); ++iter)
	{
		if ( (*iter).second == identifier)
		{
			rid = (*iter).first;
			break;
		}
	}

	if (rid == 0)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Channel [%d] is not found.",identifier);
		return returnValue;	
	}
	else if (rid == m_privateCallRes.id)
	{
		returnValue = m_privateCallRes;
	}
	else if (rid == m_BIMRes.id)
	{
		returnValue = m_BIMRes;
	}
	else if (rid == m_phoneResource.id)
	{
		returnValue = m_phoneResource;
	}
	else
	{
		returnValue = getConsoleResourceByID(rid);
        if (returnValue.type == RR_NIL)
        {
    		returnValue = getTransactiveResourceByID(rid);
            if (returnValue.type == RR_NIL)
            {
    			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "NO resource for channel [%d] is found.",identifier);
		    }
        }
	}

    FUNCTION_EXIT;    

    return returnValue;
}


RadioResource RadioMFTAgentStatusManager::getResourceBySSI(unsigned long unitID)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getResourceBySSI");
    
	RadioResource resource;

	MFTResourceCollection::iterator iter = m_MFTResourceCollection.begin();

	for (; iter!=m_MFTResourceCollection.end();++iter)
	{
		if ( unitID == ((*iter).first).ssi )
		{
			resource = (*iter).first;
			break;
		}
	}

	FUNCTION_EXIT;
	return resource;
}

RadioResource RadioMFTAgentStatusManager::getRadioResourceBySSI(unsigned long unitID)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getRadioResourceBySSI");
    
	RadioResource resource;

	MFTResourceCollection::iterator iter = m_MFTResourceCollection.begin();

	for (; iter!=m_MFTResourceCollection.end();++iter)
	{
		if ( ((*iter).first).type == RR_RADIO && unitID == ((*iter).first).ssi )
		{
			resource = (*iter).first;
			break;
		}
	}

	FUNCTION_EXIT;
	return resource;
}

RadioResource RadioMFTAgentStatusManager::getConsoleResourceByID(ResourceIdentifier rid)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getConsoleResourceByID");
    
	RadioResource resource;

	bool bResult = false;

	{
		MFTResourceCollection::iterator iter = m_MFTResourceCollection.begin();

		if (rid == m_privateCallRes.id)
		{
			resource = m_privateCallRes;
			bResult = true;
		}
		else if (rid == m_BIMRes.id)
		{
			resource = m_BIMRes;
			bResult = true;
		}
		else if (rid == m_phoneResource.id)
		{
			resource = m_phoneResource;
			bResult = true;
		}
        else
        {
		    for (; iter!=m_MFTResourceCollection.end();++iter)
		    {
			    if ( rid == ((*iter).first).id && !IsTransactiveResource(((*iter).first).type))
			    {
				    resource = (*iter).first;
				    bResult = true;
				    break;
			    }
		    }
        }
	}

	if (!bResult)
	{
		std::ostringstream logErr;
		logErr << "Resource with [" <<rid <<"] is not found.";
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logErr.str().c_str());
	}

	FUNCTION_EXIT;
	return resource;
}

RadioResource RadioMFTAgentStatusManager::getTransactiveResourceByID(ResourceIdentifier rid)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getTransactiveResourceByID");
    
	RadioResource resource;

	bool bResult = false;

	{
		MFTResourceCollection::iterator iter = m_MFTResourceCollection.begin();

		for (; iter!=m_MFTResourceCollection.end();++iter)
		{
			if ( rid == ((*iter).first).id && IsTransactiveResource(((*iter).first).type))
			{
				resource = (*iter).first;
				bResult = true;
				break;
			}
		}
    }

	if (!bResult)
	{
		std::ostringstream logErr;
		logErr << "Resource with [" <<rid <<"] is not found.";
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logErr.str().c_str());
	}

	FUNCTION_EXIT;
	return resource;
}

RadioResource RadioMFTAgentStatusManager::getResourceByID(ResourceIdentifier rid,RadioResourceType type /* = RR_NIL */)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("getResourceByID");
    
	RadioResource resource;

	bool bResult = false;

	{
		RadioResource temp;
		temp.id = rid;
		temp.type = type;

		MFTResourceCollection::iterator iter =  
				std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
							  CompareResource(temp) );
		
		if (iter != m_MFTResourceCollection.end())
		{
			resource = (*iter).first;
			bResult = true;
		}	
	}

	if ((!bResult) && (rid != m_privateCallRes.id) && (rid != m_BIMRes.id))
	{
		std::ostringstream logErr;
		logErr << "Resource with [" <<rid <<"] is not found.";
		TA_THROW(TA_Base_Core::TransactiveException(logErr.str()));
	}

	FUNCTION_EXIT;
	return resource;
}


void RadioMFTAgentStatusManager::addChannel(ResourceIdentifier rid, LogicalChannel lcid, bool bInitMode /* = false */)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("addChannelCollection");
    
	m_channelCollection[rid] = lcid;

	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"rid : %d lcid : %d",rid, lcid);

    // add to MFT collection if not found (initialization)
	if ((rid != m_privateCallRes.id) && (rid != m_BIMRes.id) && (bInitMode))
    {
        MFTResourceCollection::iterator mftIter = m_MFTResourceCollection.begin();
        for (; mftIter != m_MFTResourceCollection.end(); ++mftIter)
        {
            if (mftIter->first.id == rid)
            {
                // already in collection
                return;
            }
        }

        // assume the type will be RR_RADIO
	    {
		    std::vector<RadioResource> tempList;
		    RadioResource temp;
		    temp.type = RR_RADIO;
		    temp.id = rid;
    
		    addResource(temp, tempList, 0);
	    }
    }

	FUNCTION_EXIT;
} 

void RadioMFTAgentStatusManager::deleteChannel(ResourceIdentifier rid)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("deleteChannel");
    
	ChannelCollection::iterator iter = m_channelCollection.find(rid);

	if (iter != m_channelCollection.end())
	{
		m_channelCollection.erase(iter);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
			"Resource not found in ChannelCollection.");
	}

	FUNCTION_EXIT;
}

std::vector<RadioResource> RadioMFTAgentStatusManager::getMFTResourceCollection(RadioResourceType type)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	
    FUNCTION_ENTRY("getMFTResourceCollection");

	std::vector<RadioResource> rCollection;

	MFTResourceCollection::iterator iter = m_MFTResourceCollection.begin();
	
	

	for (; iter!=m_MFTResourceCollection.end();++iter)
	{

		if ((iter->first.type == RR_PATCHGROUP) && (iter->first.id == m_tempPatchGroup.id))
		{
			continue; // do not return Temp PG created
		}

		if ( type == RR_NIL ) // retrieve all types
		{
			rCollection.push_back((*iter).first);
		}
		else if ( type == (*iter).first.type )
		{
			rCollection.push_back((*iter).first);
		}
	}

	FUNCTION_EXIT;    

	return rCollection;
}



void RadioMFTAgentStatusManager::addPatchGroup(unsigned int patchGrp,ResourceIdentifier identifier)
{
    FUNCTION_ENTRY("addPatchGroup");
	TA_Base_Core::ThreadGuard guard( m_lock );

	m_groupCollectionPG[identifier] = patchGrp;
    FUNCTION_EXIT;
}


void RadioMFTAgentStatusManager::addPatchMember(unsigned int patchGrp, RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("addPatchMember");
	
	ResourceGroupCollection::iterator iter = m_groupCollectionPG.begin();

	for (;iter!=m_groupCollectionPG.end(); ++iter)
	{
		if ((*iter).second == patchGrp)
		{
			ResourceGroupMembersMap::iterator iMem = m_groupMembers.find((*iter).first);

            if (iMem != m_groupMembers.end())
            {
			    std::vector<RadioResource> memlist = (*iMem).second;

			    // check if it already exists in list of members 
			    std::vector<RadioResource>::iterator mIter = memlist.begin();

			    bool bFlag = false;
			    for(;mIter != memlist.end(); ++mIter)
			    {
				    if ( (*mIter).id == resource.id)
				    {
					    bFlag = true;
					    break;
				    }
			    }

			    if (!bFlag)
			    {
				    (*iMem).second.push_back(resource);
			    }
            }

			break;
		}
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentStatusManager::delPatchMember(unsigned int patchGrp,RadioResource& member)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("delPatchMember");
	
	ResourceGroupCollection::iterator iter = m_groupCollectionPG.begin();

	for (;iter!=m_groupCollectionPG.end(); ++iter)
	{
		// traverse PatchGroupCollection == patchGrp
		if ((*iter).second == patchGrp)
		{
			ResourceGroupMembersMap::iterator iMem = m_groupMembers.find((*iter).first);

			std::vector<RadioResource> memlist = (*iMem).second;

			// find in list of members
			std::vector<RadioResource>::iterator mIter = memlist.begin();

			for(;mIter != memlist.end(); ++mIter)
			{
				if ( (*mIter).id == member.id)
				{
					memlist.erase(mIter);
					break;
				}
			}
			break;
		}
	}	

    FUNCTION_EXIT;
}


void RadioMFTAgentStatusManager::delPatchGroup(ResourceIdentifier identifier)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("delPatchGroup");

	ResourceGroupCollection::iterator iter = m_groupCollectionPG.find(identifier);
	
	if ( iter != m_groupCollectionPG.end() )
	{
		(*iter).second = 0;

		ResourceGroupMembersMap::iterator iMem = m_groupMembers.find((*iter).first);
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "Deleting group");
		m_groupMembers.erase(iMem);
	}
	else
	{
		std::ostringstream logStr;
		logStr << "Resource [" << identifier << "] is not found in GroupCollection." <<std::endl;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, logStr.str().c_str());
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentStatusManager::addMselMember(unsigned int mselGrp, RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("addMselMember");
	
	ResourceGroupCollection::iterator iter = m_groupCollectionMSel.begin();

	for (;iter!=m_groupCollectionMSel.end(); ++iter)
	{
		if ((*iter).second == mselGrp)
		{
			ResourceGroupMembersMap::iterator iMem = m_groupMembers.find((*iter).first);

            if (iMem != m_groupMembers.end())
            {
			    std::vector<RadioResource> memlist = (*iMem).second;

			    // check if it already exists in list of members 
			    std::vector<RadioResource>::iterator mIter = memlist.begin();

			    bool bFlag = false;
			    for(;mIter != memlist.end(); ++mIter)
			    {
				    if ( (*mIter).id == resource.id)
				    {
					    bFlag = true;
					    break;
				    }
			    }

			    if (!bFlag)
			    {
				    (*iMem).second.push_back(resource);
			    }
            }

			break;
		}
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentStatusManager::addMselGroup(unsigned int mselGrp,ResourceIdentifier identifier)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("addMselGroup");

	m_groupCollectionMSel[identifier] = mselGrp;
    FUNCTION_EXIT;
}


void RadioMFTAgentStatusManager::delMselMember(unsigned int mselGrp, RadioResource& member)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    FUNCTION_ENTRY("delMselMember");
	
	ResourceGroupCollection::iterator iter = m_groupCollectionMSel.begin();

	for (;iter!=m_groupCollectionMSel.end(); ++iter)
	{
		// traverse GroupCollection == mselGrp
		if ((*iter).second == mselGrp)
		{
			ResourceGroupMembersMap::iterator iMem = m_groupMembers.find((*iter).first);

			std::vector<RadioResource> memlist = (*iMem).second;

			// find in list of members
			std::vector<RadioResource>::iterator mIter = memlist.begin();

			for(;mIter != memlist.end(); ++mIter)
			{
				if ( (*mIter).id == member.id)
				{
					memlist.erase(mIter);
					break;
				}
			}
			break;
		}
	}	

    FUNCTION_EXIT;
}


RadioResource RadioMFTAgentStatusManager::getResourceByGroupNo(unsigned int grpNo,RadioResourceType type)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResourceByGroupNo");

	RadioResource resource;

	if (type == RR_PATCHGROUP )
	{
		ResourceGroupCollection::iterator iter = m_groupCollectionPG.begin();
		for (;iter!=m_groupCollectionPG.end(); ++iter)
		{
			if ((*iter).second == grpNo)
			{
		   		resource = getTransactiveResourceByID((*iter).first);	
				break;
			}
		}
	}
	else if ((type==RR_MSELECT) || (type==RR_MSITE))
	{
		ResourceGroupCollection::iterator iter = m_groupCollectionMSel.begin();
		for (;iter!=m_groupCollectionMSel.end(); ++iter)
		{
			if ((*iter).second == grpNo)
			{
		   		resource = getTransactiveResourceByID((*iter).first);	
				break;
			}
		}
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Resource is not a Transactive group type.");
	}

	FUNCTION_EXIT;
	return resource;
}



std::vector<RadioResource> RadioMFTAgentStatusManager::getResourceMembers(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResourceMembers");

	std::vector<RadioResource> members;

	switch(resource.type)
	{
		case RR_DYNGROUP:
		{
			ResourceGroupMembersMap::iterator dGroup = m_dynGroupMembers.find(resource.id);
			if (dGroup != m_dynGroupMembers.end() )
			{
				members = (*dGroup).second;
			}
		}
		break;
		case RR_PATCHGROUP:
		case RR_MSITE:
		case RR_MSELECT:
			members = getGroupMembers(resource.id);
			break;
		default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				"No members associated with [ %s ],",resource.alias.c_str());
	}

    FUNCTION_EXIT;
	return members;	
}

void RadioMFTAgentStatusManager::initDataFromDb()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("initDataFromDb");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start retrieving resource from DB");

    // get the list from TransactiveDB
    std::vector<RadioResource> transactiveList;

	//fixed TD13419, added the 1st params
    std::vector<TA_IRS_Core::IRadioSubscriber*> subscribers = 
        TA_IRS_Core::RadioSubscriberFactory::getInstance().getAllRadioSubscribers(false);

	for (std::vector<TA_IRS_Core::IRadioSubscriber*>::iterator it = subscribers.begin();
	it != subscribers.end(); ++it)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Adding RadioResource name: %s, RID: %u", (*it)->getAlias().c_str(), (*it)->getResourceId());
		RadioResource resource;
		resource.id= (*it)->getResourceId();
		resource.type = (RadioResourceType)(*it)->getResourceType();
		resource.alias = (*it)->getAlias();
		resource.ssi = (*it)->getSSI();

        transactiveList.push_back(resource);
		delete (*it);
	}		

    // rebuild the collection with the actual resource type from MCDB, and other attributes
	// No need to synchronize here
    //std::vector<RadioResource> mcdbList;
    //The_ResourceConfig::instance()->getRadioResource(mcdbList);

    // merge the list from transactive with MCDB, get the alias from transactive
    //std::vector<RadioResource>::iterator mcdbIt;
    std::vector<RadioResource>::iterator transactiveIt;

    // store the MFT resource collection temporarily, so we can rebuild
    // with the correct resource type & id from MCDB
    std::vector<RadioResource> tempKeyList;
    std::vector<RadioResourceMFTServerAttributes> tempAttrList;
    for (MFTResourceCollection::iterator oldIt = m_MFTResourceCollection.begin(); 
        oldIt != m_MFTResourceCollection.end(); ++oldIt)
    {
        tempKeyList.push_back(oldIt->first);
        tempAttrList.push_back(oldIt->second);
    }

    m_MFTResourceCollection.clear();

	// initialise RadioResourceMFTServerAttributes;
    RadioResourceMFTServerAttributes tempAttr;

    bool keyFound = false;
	for(transactiveIt = transactiveList.begin(); transactiveIt!=transactiveList.end(); ++transactiveIt)
    //for (mcdbIt = mcdbList.begin(); mcdbIt != mcdbList.end(); ++mcdbIt)
    {
        keyFound = false;

		if ((*transactiveIt).type == RR_PRIVATECALL) {
			m_privateCallRes = (*transactiveIt);
		}

        // check if we can find it in the temporary list,
        // and assign attribute if that's the case
        // do it from last to first, so we can erase safely
        for (int i = tempKeyList.size() - 1; i >= 0; --i)
        {
			if (tempKeyList[i].id == transactiveIt->id)
            //if (tempKeyList[i].id == mcdbIt->id)
            {
				m_MFTResourceCollection[*transactiveIt] = tempAttrList[i];
                //m_MFTResourceCollection[*mcdbIt] = tempAttrList[i];
                keyFound = true;
                tempKeyList.erase(tempKeyList.begin() + i);
                tempAttrList.erase(tempAttrList.begin() + i);
            }
        }

		// use default attribute && DG Talkgroups & BIM resource types are not visible to users
        if (!keyFound)
        {
            m_MFTResourceCollection[*transactiveIt] = tempAttr;
        }
    }

	//remove Private Call resource in the collection
	MFTResourceCollection::iterator pcIt = 		
		std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
					  CompareResource(m_privateCallRes) );
	if (pcIt!=m_MFTResourceCollection.end())
	{
		m_MFTResourceCollection.erase(pcIt);
	}

    loadTransactiveResources();

	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				 "@ISCSC DB -- Resource Collection Count : %d", m_MFTResourceCollection.size());
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finish retrieving data from DB");
}

void RadioMFTAgentStatusManager::initDataFromMcc()
{	
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("initDataFromMcc");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start retrieving resource from MCC");

    // get the list from MCC (done already)
    std::vector<RadioResource> mccList;
    The_ResourceConfig::instance()->getRadioResource(mccList);
	std::vector<RadioResource>::iterator mccIt;

    // store the MFT resource collection temporarily, so we can rebuild
    // with the correct resource type & id from MCDB
    std::vector<RadioResource> tempKeyList;
    std::vector<RadioResourceMFTServerAttributes> tempAttrList;
    for (MFTResourceCollection::iterator oldIt = m_MFTResourceCollection.begin(); 
        oldIt != m_MFTResourceCollection.end(); ++oldIt)
    {
        tempKeyList.push_back(oldIt->first);
        tempAttrList.push_back(oldIt->second);
    }

    m_MFTResourceCollection.clear();

	// initialise RadioResourceMFTServerAttributes;
    RadioResourceMFTServerAttributes tempAttr;

    bool keyFound = false;
	for(mccIt = mccList.begin(); mccIt!=mccList.end(); ++mccIt)
    {
        keyFound = false;

		if ((*mccIt).type == RR_PRIVATECALL) {
			m_privateCallRes = (*mccIt);
		}

        // check if we can find it in the temporary list,
        // and assign attribute if that's the case
        // do it from last to first, so we can erase safely
        for (int i = tempKeyList.size() - 1; i >= 0; --i)
        {
			if (tempKeyList[i].id == mccIt->id)
            //if (tempKeyList[i].id == mcdbIt->id)
            {
				m_MFTResourceCollection[*mccIt] = tempAttrList[i];
                //m_MFTResourceCollection[*mcdbIt] = tempAttrList[i];
                keyFound = true;
                tempKeyList.erase(tempKeyList.begin() + i);
                tempAttrList.erase(tempAttrList.begin() + i);
            }
        }

		// use default attribute && DG Talkgroups & BIM resource types are not visible to users
        if (!keyFound)
        {
            m_MFTResourceCollection[*mccIt] = tempAttr;
        }
    }

	//remove Private Call resource in the collection
	MFTResourceCollection::iterator pcIt = 		
		std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
					  CompareResource(m_privateCallRes) );
	if (pcIt!=m_MFTResourceCollection.end())
	{
		m_MFTResourceCollection.erase(pcIt);
	}

    loadTransactiveResources();

	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
		"@MCC DB -- Resource Collection Count : %d", m_MFTResourceCollection.size());

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finish retrieving resource from MCC");
	
	FUNCTION_EXIT;
}

void RadioMFTAgentStatusManager::syncFromDb(RadioResourceType type, IRadioMFTAgentCallback* callback)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("syncFromDb");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start synchronizing resource from DB");

	// get resources from DB
    std::vector<RadioResource> transactiveList;
	std::vector<TA_IRS_Core::IRadioSubscriber*> subscribers;
	bool updateChange = true;
	if (RR_NIL == type) {
		subscribers = TA_IRS_Core::RadioSubscriberFactory::getInstance().getAllRadioSubscribers(false);
	} else {
		updateChange = false;
		subscribers = TA_IRS_Core::RadioSubscriberFactory::getInstance().getRadioSubscribersByResourseType(type, false);
	}

	for (std::vector<TA_IRS_Core::IRadioSubscriber*>::iterator it = subscribers.begin();
	it != subscribers.end(); ++it)
	{
		RadioResource resource;
		resource.id= (*it)->getResourceId();
		resource.type = (RadioResourceType)(*it)->getResourceType();
		resource.alias = (*it)->getAlias();
		resource.ssi = (*it)->getSSI();

		transactiveList.push_back(resource);
		delete (*it);
	}	
	
	std::vector<RadioResource> mftList;
	if (RR_NIL != type) {
		MFTResourceCollection::iterator iterC = m_MFTResourceCollection.begin();
		for (; iterC!=m_MFTResourceCollection.end();++iterC)
		{
			if (type == (*iterC).first.type) {
				mftList.push_back((*iterC).first);
			}
		}
	} else {
		MFTResourceCollection::iterator iterC = m_MFTResourceCollection.begin();
		for (; iterC!=m_MFTResourceCollection.end();++iterC)
		{
			if (RR_TALKGROUP == (*iterC).first.type || RR_RADIO == (*iterC).first.type) {
				mftList.push_back((*iterC).first);
			}
		}
	}

	// sync
	bool change = false;
	std::vector<RadioResource>::iterator transactiveIt;
	int mftNumRec = mftList.size();
	int index = 0;
	bool found = false;
	RadioResource r, emptyR;
	for (; index < mftNumRec; index++) {
		transactiveIt = transactiveList.begin();
		r = mftList[index];
		found = false;
		for (; transactiveIt != transactiveList.end(); transactiveIt++) {
			if (r.id == (*transactiveIt).id) {
				if (r.alias == (*transactiveIt).alias) {
					// no change
				} else {
					
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ResourceSync: Updating resource with id = %u on MFT, from alias = %s to alias = %s. No update done", r.id, r.alias.c_str(), (*transactiveIt).alias.c_str());

					// send call back to radio manager
					//callback->callback_resourceChangeMessage(r, (*transactiveIt));

					// update its own collection
					//updateResource(r, (*transactiveIt));
				}
				// mark it, as we dont want to use it anymore
				(*transactiveIt).initialise();
				found = true;
				break;
			} 
			if (r.alias == (*transactiveIt).alias) {
				if (r.id == (*transactiveIt).id) {
					// no change
				} else {
					// TODO: update the database
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ResourceSync: Updating resource with alias = %s on MFT, from id = %u to id = %u. No update done", r.alias.c_str(), r.id, (*transactiveIt).id);					

					if (updateChange) {
						// send call back to radio manager
						callback->callback_resourceChangeMessage(r, (*transactiveIt));

						// update its own collection
						updateResource(r, (*transactiveIt));
					}
				}
				// mark it, as we dont want to use it anymore
				(*transactiveIt).initialise();
				found = true;
				change = true;
				break;
			}
		}
		if (!found) {
			// delete from the collection
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ResourceSync: Deleting resource with alias = %s from the MFT", r.alias.c_str());
			// send call back to radio manager
			callback->callback_resourceChangeMessage(r, emptyR);

			// update its own collection
			deleteResource(r);
			change = true;
		}		
	}
	transactiveIt = transactiveList.begin();
	for (; transactiveIt != transactiveList.end(); transactiveIt++) {
		// add to the database
		if (RR_NIL != (*transactiveIt).type) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ResourceSync: Adding resource with alias = %s to the MFT", (*transactiveIt).alias.c_str());

			// send call back to radio manager
			callback->callback_resourceChangeMessage(emptyR, (*transactiveIt));

			// update its own collection
			addNewResource(*transactiveIt);
			change = true;
		}
	}
	if (change) {
		callback->callback_resourceChangeMessage(emptyR, emptyR);
	}
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finish synchronizing resource from DB");

	FUNCTION_EXIT;
}

void RadioMFTAgentStatusManager::syncFromMcc(RadioResourceType type, IRadioMFTAgentCallback* callback)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("syncFromMcc");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start synchronizing resource from MCC");

	// get the list from MCC (done already)
    std::vector<RadioResource> rawList;
	std::vector<RadioResource> mccList;
    The_ResourceConfig::instance()->getRadioResource(rawList);
	std::vector<RadioResource>::iterator mccIt;
	bool updateChange = true;
	
	std::vector<RadioResource> mftList;
	if (RR_NIL != type) {
		updateChange = false;
		mccIt = rawList.begin();
		for (; mccIt != rawList.end(); mccIt++) {
			if (type == (*mccIt).type) 
				mccList.push_back(*mccIt);
		}

		mftList = getMFTResourceCollection(type);
	} else {
		mccList = rawList;
		MFTResourceCollection::iterator iterC = m_MFTResourceCollection.begin();
		for (; iterC!=m_MFTResourceCollection.end();++iterC)
		{
			if (RR_TALKGROUP == (*iterC).first.type || RR_RADIO == (*iterC).first.type) {
				mftList.push_back((*iterC).first);
			}
		}
	}

	// sync
	bool change = false;
	int mftNumRec = mftList.size();
	int index = 0;
	bool found = false;
	RadioResource r, emptyR;
	for (; index < mftNumRec; index++) {
		mccIt = mccList.begin();
		r = mftList[index];
		found = false;
		for (; mccIt != mccList.end(); mccIt++) {
			if (r.id == (*mccIt).id) {
				if (r.alias == (*mccIt).alias) {
					// no change
				} else {
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ResourceSync: Update resource with id = %u on MFT, from alias = %s to alias = %s. No update done", r.id, r.alias.c_str(), (*mccIt).alias.c_str());

					//if (updateChange){
					//	// send call back to radio manager
					//	callback->callback_resourceChangeMessage(r, (*mccIt));

					//	// update its own collection
					//	updateResource(r, (*mccIt));
					//}
				}
				// mark it, as we dont want to use it anymore
				(*mccIt).initialise();
				found = true;
				break;
			} 
			if (r.alias == (*mccIt).alias) {
				if (r.id == (*mccIt).id) {
					// no change
				} else {
					// TODO: update the database
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ResourceSync: Update resource with alias = %s on MFT, from id = %u to id = %u. No update done", r.alias.c_str(), r.id, (*mccIt).id);						
					
					if (updateChange) {
						// send call back to radio manager
						callback->callback_resourceChangeMessage(r, (*mccIt));

						// update its own collection
						updateResource(r, (*mccIt));
					}
				}
				// mark it, as we dont want to use it anymore
				(*mccIt).initialise();
				found = true;
				change = true;
				break;
			}
		}
		if (!found) {
			// delete from the collection
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ResourceSync: Deleting resource with alias = %s from the MFT", r.alias.c_str());
			// send call back to radio manager
			callback->callback_resourceChangeMessage(r, emptyR);

			// update its own collection
			deleteResource(r);
			change = true;
		}		
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start adding new resource. New resource size=%u",mccList.size());
	mccIt = mccList.begin();
	for (; mccIt != mccList.end(); mccIt++) {
		// add to the database
		if (RR_NIL != (*mccIt).type) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ResourceSync: Adding resource with alias = %s to the MFT", (*mccIt).alias.c_str());

			// send call back to radio manager
			callback->callback_resourceChangeMessage(emptyR, (*mccIt));

			// update its own collection
			addNewResource(*mccIt);

			//temp
			//	TA_IRS_Core::RadioSubscriberFactory::getInstance().createNew(*mccIt);
			change = true;
		}
	}
	if (change) {
		callback->callback_resourceChangeMessage(emptyR, emptyR);
	}
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finish synchronizing resource from MCC");
	FUNCTION_EXIT;
}

void RadioMFTAgentStatusManager::loadTransactiveResources()
{
    // add the transactive managed group resources
    std::vector<TA_IRS_Core::IRadioDirectoryEntry*> groupList;
    unsigned long categories[] = {RR_PATCHGROUP, RR_DYNGROUP, RR_MSELECT, RR_MSITE};

    for (int j = 0; j < 4; ++j)
    {
        try
        {
            groupList = TA_IRS_Core::RadioDirectoryEntryFactory::getInstance()
                                            .getRadioDirectoryEntries(categories[j]);

            for (std::vector<TA_IRS_Core::IRadioDirectoryEntry*>::iterator ite = groupList.begin(); 
                 ite != groupList.end(); ++ite)
            {
		        RadioResource group;
		        group.id= (*ite)->getKey();
		        group.type = (RadioResourceType) categories[j];
                group.alias = (*ite)->getName();

                // get the members
                std::vector<TA_IRS_Core::IRadioDirectoryMember*> memberList = 
                    TA_IRS_Core::RadioDirectoryMemberFactory::getInstance()
                                        .getRadioDirectoryMembers(group.id);

                std::vector<RadioResource> members;
                RadioResource res;
                for (std::vector<TA_IRS_Core::IRadioDirectoryMember*>::iterator itm = memberList.begin();
                     itm != memberList.end(); ++itm)
                {
                    RadioResourceType resourceType = (RadioResourceType) (*itm)->getResourceType();
                    ResourceIdentifier resourceId = (*itm)->getResourceId();
                    res = getConsoleResourceByID(resourceId);
                    if (res.type != RR_NIL)
                    {
                        members.push_back(res);
                    }
                    delete *itm;
                }
			    addResource(group, members, group.id);
                delete *ite;
            }
        }
        catch (TA_Base_Core::TransactiveException& e)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, e.what());
        }
        catch (...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Unable to retrieve resource for category %d", categories[j]);
        }
    }
}

RadioResource RadioMFTAgentStatusManager::getSelectedSpeaker()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	
    FUNCTION_ENTRY("getSelectedSpeaker");

	RadioResource selected;

	std::vector<RadioResource> rCollection;

	MFTResourceCollection::iterator iter = m_MFTResourceCollection.begin();

	for (; iter!=m_MFTResourceCollection.end();++iter)
	{
		RadioResourceMFTServerAttributes attr = (*iter).second;
		// TD14133
		if ( attr.associatedSpeaker == SELECTED_SPEAKER)
		{
			if (attr.isPatchInvolved > 0) 
			{
				selected = getResourceByID(attr.isPatchInvolved,RR_PATCHGROUP);
			}
			else
			{
				selected = (*iter).first;
			}
			break;
		}
		// TD14133
	}

	// TD14133 - search from DG collection
	if (selected.id == 0)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[SMEE] Search selected resource from DG collection" );
		for (std::vector<DynGroup>::iterator it = m_dynamicRegroup.begin();
			it != m_dynamicRegroup.end(); ++it)
		{
			if (it->dgID != 0)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[SMEE] DG selected=%d",it->dgID );
				selected = getResourceByID(it->dgID,RR_DYNGROUP);
				break;
			}
		}
	}
	// TD14133 

    FUNCTION_EXIT;
	return selected;
}

unsigned long RadioMFTAgentStatusManager::getNextTransResourceId()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	static unsigned long id = rand() % 100;

	return id++;
}

RadioResource RadioMFTAgentStatusManager::getFreeDynamicGroup(const RadioResource& res)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getFreeDynamicGroup");
	RadioResource ret;

//TD18526 Check if current profile is allowed to connect
//TD18803	
	if (!bCheckConnectionPermission())
	{
//		ret.ssi = 0xFFFFFFFF;
		bPermitProfile = false;
		return ret;
	}
	else
	{
		bPermitProfile = true;
	}


    for (std::vector<DynGroup>::iterator it = m_dynamicRegroup.begin();
        it != m_dynamicRegroup.end(); ++it)
        {
			// All talkgroups can be used as DGNA
            if ((it->isFree))// && (isDGAvailable(it->dynGroup)))
            {
				it->dgID   = 0; // TD14133
                it->isFree = false;
                m_dynGroupMap[res.id] = *it;
                return it->dynGroup;
            }
        }

	
	FUNCTION_EXIT;

    
	return ret;
}

void RadioMFTAgentStatusManager::releaseDynamicGroup(const RadioResource& res, unsigned long groupId)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

	FUNCTION_ENTRY("releaseDynamicGroup");

    for (std::vector<DynGroup>::iterator it = m_dynamicRegroup.begin();
        it != m_dynamicRegroup.end(); ++it)
        {
            if (it->dynGroup.ssi == groupId)
            {
                it->isFree = true;
				it->dgID   = 0; // TD14133
                return;
            }
        }

	FUNCTION_EXIT;
}

RadioResource RadioMFTAgentStatusManager::getDynamicGroupForResource(const RadioResource& resource)
{
	FUNCTION_ENTRY("getDynamicGroupForResource");

	TA_Base_Core::ThreadGuard guard( m_lock );
    DynGroup ret = m_dynGroupMap[resource.id];
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DGNA, resource.id = %u", resource.id);

	FUNCTION_EXIT;
    return ret.dynGroup;
}
int RadioMFTAgentStatusManager::getNumberOfActiveDG()
{
	TA_Base_Core::ThreadGuard guard( m_lock );

	FUNCTION_ENTRY("getDynamicGroupForResource");

	int count = 0;
    for (std::vector<DynGroup>::iterator it = m_dynamicRegroup.begin();
        it != m_dynamicRegroup.end(); ++it)
        {
            if (!it->isFree)
				count++;
        }

	FUNCTION_EXIT;
	return count;
}

bool RadioMFTAgentStatusManager::canDeassignResource(const RadioResource& resource, const RadioResource* parent)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	
    FUNCTION_ENTRY("canDeassignResource");

	MFTResourceCollection::iterator iter = m_MFTResourceCollection.begin();

	for (; iter!=m_MFTResourceCollection.end();++iter)
	{
        const RadioResource& res = (*iter).first;
        if (IsGroupResource(res.type))
        {
            if (parent != NULL && parent->type == res.type && parent->id == res.id) continue;

        	GroupConnectionState state = NAG;

		    RadioResourceMFTServerAttributes attr = (*iter).second;
		    state = attr.isGroupConnected;
            if (state == CONNECTED)
            {
                // check if one of the members is the resource we are looking at
                std::vector<RadioResource> members = getGroupMembers(res.id);
                for (std::vector<RadioResource>::iterator it = members.begin();
                        it != members.end(); ++it)
                {
                    if (it->id == resource.id)
                    {
                        return false;
                    }
                }
            }
        }
        else if (parent != NULL && res.id == resource.id)
        {
		    RadioResourceMFTServerAttributes attr = (*iter).second;
            if (attr.isResourceAssigned && attr.buttonOffset > 0)
            {
                return false;
            }
        }
    }

    FUNCTION_EXIT;
	return true;
}

RadioResource RadioMFTAgentStatusManager::getPendingConnectResource(RadioResourceType pType)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    switch (pType)
    {
    case RR_PATCHGROUP:
        return m_pendingPatchGroup;

    case RR_MSELECT:
    case RR_MSITE:
        return m_pendingMSel;

    case RR_SITE:
        return m_pendingSite;
    }

    RadioResource nil;
    return nil;
}


void RadioMFTAgentStatusManager::setPendingConnectResource(const RadioResource& pResource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    switch (pResource.type)
    {
    case RR_PATCHGROUP:
        m_pendingPatchGroup = pResource;
        break;

    case RR_MSELECT:
    case RR_MSITE:
        m_pendingMSel = pResource;
        break;

    case RR_SITE:
        m_pendingSite = pResource;
        break;
    }
}


void RadioMFTAgentStatusManager::clearPendingConnectResource(RadioResourceType pType)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    switch (pType)
    {
    case RR_PATCHGROUP:
        m_pendingPatchGroup.type = RR_NIL;
        break;

    case RR_MSELECT:
    case RR_MSITE:
        m_pendingMSel.type = RR_NIL;
        break;

    case RR_SITE:
        m_pendingSite.type = RR_NIL;
        break;
    }
}

bool RadioMFTAgentStatusManager::hasPendingConnectResource(RadioResourceType pType)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    switch (pType)
    {
    case RR_PATCHGROUP:
        return (m_pendingPatchGroup.type != RR_NIL);

    case RR_MSELECT:
    case RR_MSITE:
        return (m_pendingMSel.type != RR_NIL);

    case RR_SITE:
        return (m_pendingSite.type != RR_NIL);
    }

    return false;
}


bool RadioMFTAgentStatusManager::isTempPatchInvolved(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

	bool result = false;

	ResourceGroupMembersMap::iterator iter = m_groupMembers.find(m_tempPatchGroup.id);

	if (iter != m_groupMembers.end())
	{
		ResourceCollection members = iter->second;

		for (ResourceCollection::iterator it=members.begin(); it!= members.end(); it++)
		{
			if (it->ssi = resource.ssi)
			{
				result = true;
				break;
			}
		}
	}

	return result;
}
bool RadioMFTAgentStatusManager::isDGAvailable(const RadioResource& resource)
{
    FUNCTION_ENTRY("isDGAvailable");

	bool result = false;

	if (resource.ssi >= MIN_DYNAMICGROUP_ID && resource.ssi <= MAX_DYNAMICGROUP_ID)
	{
		result = true;
	}	
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SSI out of range for DG Talkgroup [%s]",resource.alias.c_str());
	}
#if 0
	int p_rack = resource.ssi / 1000000;
	int p_card = (resource.ssi % 1000000) / 10000;

	int p_idNumber = (resource.ssi % 1000000) % 10000;

//	TD18526, prevent "Super" profile from getting a dynamic group
	if (m_profile.find("Super") != std::string::npos)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Profile is SUPER.");
		if (p_idNumber >= MIN_DYNAMICGROUP_ID && p_idNumber <= MAX_DYNAMICGROUP_ID)
		{
			result = true;
		}	
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SSI out of range for DG Talkgroup [%s]",resource.alias.c_str());
		}		
	}


	// check card number this profile is associated with
	int cardNum = 0;
	if (p_rack == 1)
	{
		for (int i=0; i<7; i++)
		{
			if (profileRack1[i].name == m_profile)
			{
				cardNum	= profileRack1[i].number;
				break;
			}
		}
	}
	else if (p_rack == 2)
	{
		for (int i=0; i<6; i++)
		{
			if (profileRack2[i].name == m_profile)
			{
				cardNum	= profileRack2[i].number;
				break;
			}
		}
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid rack number for DG Talkgroup [%s]",resource.alias.c_str());
	}

	if ((cardNum > 0) && (cardNum == p_card))
	{
		if (p_idNumber >= MIN_DYNAMICGROUP_ID && p_idNumber <= MAX_DYNAMICGROUP_ID)
		{
			result = true;
		}	
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SSI out of range for DG Talkgroup [%s]",resource.alias.c_str());
		}
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid COIM card number for DG Talkgroup [%s]",resource.alias.c_str());
	}
#endif

    FUNCTION_EXIT;
    return result;
}


RadioResource RadioMFTAgentStatusManager::getPhoneResource()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("getPhoneResource");

	if (m_phoneResource.id == 0)
	{
		m_phoneResource.id    = RadioMFTAgentHelper::getSPIResourceID(); // TD16770
		m_phoneResource.type  = RR_SPI;		
		m_phoneResource.alias = "CEB1-SPI-01";

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[getPhoneResource] SPI resource=%s RID=[%d]",m_phoneResource.alias.c_str(),m_phoneResource.id);
	}

	FUNCTION_EXIT;
	return m_phoneResource;
}

//TD14133
void RadioMFTAgentStatusManager::setSelectedDG(const RadioResource& dynGrp,TransactiveResourceIdentifier id)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("setSelectedDG");

    for (std::vector<DynGroup>::iterator it = m_dynamicRegroup.begin();
        it != m_dynamicRegroup.end(); ++it)
    {
        if (dynGrp.id == it->dynGroup.id)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[setSelectedDG] DG=%d TransactiveId=%d",dynGrp.id,id);
			it->dgID   = id;
			break;
        }
    }
	FUNCTION_EXIT;
}

bool RadioMFTAgentStatusManager::getSelectedDGState(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("getSelectedDGState");
	// if the dgID of one of the DG resource is assigned to the resource(parameter), 
	// then this Transactive DG entity is in SELECTED
    for (std::vector<DynGroup>::iterator it = m_dynamicRegroup.begin();
        it != m_dynamicRegroup.end(); ++it)
    {
        if (resource.id == it->dgID)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[getSelectedDGState] DG is selected %d",resource.id);
			return true;
        }
    }

	FUNCTION_EXIT;
	return false;
}
//TD14133

bool RadioMFTAgentStatusManager::isAudibleAlarmOn()
{
	FUNCTION_ENTRY("isAudibleAlarmOn");
	return m_audibleAlarmOn;
	FUNCTION_EXIT;
}

void RadioMFTAgentStatusManager::setAudibleAlarm(bool alarmMode)
{
	FUNCTION_ENTRY("setAudibleAlarm");
	m_audibleAlarmOn = alarmMode;
	FUNCTION_EXIT;
}

//TD18526 marc_bugfix
//Check if profile is allowed to connect
bool RadioMFTAgentStatusManager::bCheckConnectionPermission()
{
	FUNCTION_ENTRY("setAudibleAlarm");
	register int i;

	for (i=0; i<7; i++)
	{
		if (profileRack1[i].name == m_profile)
		{
			return true;		
		}

	}

	for (i=0; i<6; i++)
	{
		if (profileRack2[i].name == m_profile)
		{
			return true;		
		}

	}

	FUNCTION_EXIT;
	return false;
}
//TD18803
bool RadioMFTAgentStatusManager::bPermitProfileAcquireDG()
{
	FUNCTION_ENTRY("bPermitProfileAcquireDG");
	return bPermitProfile;
	FUNCTION_EXIT;
}
//TD18803


void RadioMFTAgentStatusManager::sendErrorMessage(char* errorMsg)
{
	FUNCTION_ENTRY("sendErrorMessage");
	if (m_RadioMFTAgentCallback != NULL)
	{
		m_RadioMFTAgentCallback->callback_applicationErrorMessage(errorMsg);
	}
	FUNCTION_EXIT;
}

void RadioMFTAgentStatusManager::failConnectToRadio()
{
	FUNCTION_ENTRY("failConnectToRadio");
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Radio MFT Agent (MCC) is now disconnected");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Removing all resources");
	clearAllResources();

	initialization = false;
	
	setMccConnectionStatus (false);

	m_timerManager->timerStop();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Calling Reconnection");

	if (m_RadioMFTAgentCallback != NULL)
	{
		m_RadioMFTAgentCallback->callback_applicationStateMessage(RA_CONNECTION_FAILURE_MODE, 0);
	}
	
	FUNCTION_EXIT;
}

void RadioMFTAgentStatusManager::initialized()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Radio MFT Agent (MCC) is now initialized");
	initialization = true;
}

void RadioMFTAgentStatusManager::startTimeoutTimer()
{
	{
		TA_Base_Core::ThreadGuard guard( m_flagConnectionLock );

		if (m_bIsMCCConnected)
		{
	
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Radio MFT Agent restart the timer");

			// set flag for timer to false, saying its started and not yet expired
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Starting Timer");
			startTimer();

		}
	}
	
}

int RadioMFTAgentStatusManager::updateResourceCounter(RadioResource& resource, bool add) 
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("updateResourceCounter");
	int counter = 0;
	bool deleteFirstOne = false;
	std::vector<RadioResource>::iterator deletedItem = m_resourceCounter.end();

	if (add) 
	{
		m_resourceCounter.push_back(resource);
		FUNCTION_EXIT;
		return 0;
	} 
	else 
	{
		if (m_resourceCounter.size() == 0) 
		{
			FUNCTION_EXIT;
			return 0;
		}

		std::vector<RadioResource>::iterator iter = m_resourceCounter.begin();

		for (; iter!=m_resourceCounter.end(); iter++) 
		{
			if ((*iter).id == resource.id && (*iter).type == resource.type) 
			{
				if (!deleteFirstOne) 
				{
					deleteFirstOne = true;
					deletedItem = iter;
				}
				counter++;
			}
		}
	}

	if (deletedItem != m_resourceCounter.end())
	{
		m_resourceCounter.erase(deletedItem);
		counter--;
	}

	FUNCTION_EXIT;
	return counter;
}

int RadioMFTAgentStatusManager::getResourceCounter(RadioResource& resource) 
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("getResourceCounter");
	int counter = 0;

	std::vector<RadioResource>::iterator iter = m_resourceCounter.begin();
	for (; iter!=m_resourceCounter.end(); iter++) {
		if ((*iter).id == resource.id && (*iter).type == resource.type) {
			counter++;
		}
	}

	FUNCTION_EXIT;
	return counter;
}

RadioResource RadioMFTAgentStatusManager::getActiveResource()
{
	TA_Base_Core::ThreadGuard guard( m_lock );	
	return m_curActResource;
}
void RadioMFTAgentStatusManager::setActiveResource(RadioResource& r)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting current activity to %s", r.alias.c_str());
	m_curActResource = r;
}

void RadioMFTAgentStatusManager::setPrivateCallResourceSelected(bool select) 
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("setPrivateCallResourceSelected");

	m_pvcSelectStatus = select;

	FUNCTION_EXIT;
}

bool RadioMFTAgentStatusManager::isPrivateCallResourceSelected()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("isPrivateCallResourceSelected");

	return m_pvcSelectStatus;

	FUNCTION_EXIT;
}

void RadioMFTAgentStatusManager::updateResource(RadioResource& oldResource, RadioResource& newResource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("updateResource");

	MFTResourceCollection::iterator iter =  
		std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
		CompareResource(oldResource) );

	if (iter == m_MFTResourceCollection.end()) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Resource %u not found on the MFT Agent resource list. Not being updated", oldResource.id);
		return;
	} else {
		RadioResourceMFTServerAttributes attr = (*iter).second;
		m_MFTResourceCollection.erase(iter);
		m_MFTResourceCollection[newResource] = attr;
	}

	// for patch and multi-select
	ResourceGroupMembersMap::iterator groupIter = m_groupMembers.begin();
	for (; groupIter != m_groupMembers.end(); groupIter++) {
		ResourceCollection::iterator memberIter = (*groupIter).second.begin();
		for (; memberIter != (*groupIter).second.end(); memberIter++) {
			if ((*memberIter).id == oldResource.id) {
				(*memberIter) = newResource;
				break;
			}
		}
	}
	
	// for dgna
	ResourceGroupMembersMap::iterator dynIter = m_dynGroupMembers.begin();
	for (; dynIter != m_dynGroupMembers.end(); dynIter++) {
		ResourceCollection::iterator memberIter = (*dynIter).second.begin();
		for (; memberIter != (*dynIter).second.end(); memberIter++) {
			if ((*memberIter).id == oldResource.id) {
				(*memberIter) = newResource;
				break;
			}
		}
	}

	// for one operation at a time
	std::vector<RadioResource>::iterator cIter = m_resourceCounter.begin();
	for(; cIter != m_resourceCounter.end(); cIter++) {
		if ((*cIter).id == oldResource.id) {
			(*cIter) = newResource;
		}
	}

	// for channel 
	ChannelCollection::iterator chIter = m_channelCollection.find(oldResource.id);
	if (chIter != m_channelCollection.end()) {
		m_channelCollection[newResource.id] = (*chIter).second;
		m_channelCollection.erase(chIter);
	}

	// current resource for general transmit
	if (m_curActResource.id == oldResource.id) {
		m_curActResource = newResource;
	}

	FUNCTION_EXIT;
}

void RadioMFTAgentStatusManager::addNewResource(RadioResource& newResource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("addNewResource");
    RadioResourceMFTServerAttributes tempAttr;
	m_MFTResourceCollection[newResource] = tempAttr;
	FUNCTION_EXIT;
}

void RadioMFTAgentStatusManager::deleteResource(RadioResource& oldResource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("deleteResource");	
	MFTResourceCollection::iterator iter =  
		std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
		CompareResource(oldResource) );

	if (iter == m_MFTResourceCollection.end()) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Resource %u not found on the MFT Agent resource list. Not being deleted", oldResource.id);
		return;
	} else {
		m_MFTResourceCollection.erase(iter);
	}

	ResourceGroupMembersMap::iterator groupIter = m_groupMembers.begin();
	for (; groupIter != m_groupMembers.end(); groupIter++) {
		ResourceCollection::iterator memberIter = (*groupIter).second.begin();
		for (; memberIter != (*groupIter).second.end(); memberIter++) {
			if ((*memberIter).id == oldResource.id) {
				(*groupIter).second.erase(memberIter);
				break;
			}
		}
	}

	ResourceGroupMembersMap::iterator dynIter = m_dynGroupMembers.begin();
	for (; dynIter != m_dynGroupMembers.end(); dynIter++) {
		ResourceCollection::iterator memberIter = (*dynIter).second.begin();
		for (; memberIter != (*dynIter).second.end(); memberIter++) {
			if ((*memberIter).id == oldResource.id) {
				(*dynIter).second.erase(memberIter);
				break;
			}
		}
	}

	int count = getResourceCounter(oldResource);
	for(int i = 0; i < count; i++) {
		updateResourceCounter(oldResource,false);
	}

	FUNCTION_EXIT;
}

bool RadioMFTAgentStatusManager::getRadioStatus()
{
	FUNCTION_ENTRY("getRadioStatus");
	FUNCTION_EXIT;
	// local testing
	//return true;
	return m_radioStatus;
}

void RadioMFTAgentStatusManager::setRadioStatus(bool value)
{
	FUNCTION_ENTRY("setRadioStatus");
	m_radioStatus = value;
	// local testing
	//if (m_radioStatus) {
	//	The_ResourceConfig::instance()->startRetrieveDB(RadioResourceConfigFacade::DB_SYNC_RADIO_MODE);
	//} else {
	//	syncFromDb(RR_NIL, m_RadioMFTAgentCallback);
	//}
	FUNCTION_EXIT;
}

bool RadioMFTAgentStatusManager::getIscsStatus()
{
	FUNCTION_ENTRY("getIscsStatus");
	FUNCTION_EXIT;
	return m_iscsStatus;
}

void RadioMFTAgentStatusManager::setIscsStatus(bool value)
{
	FUNCTION_ENTRY("setIscsStatus");
	m_iscsStatus = value;
	// local testing
	//if (m_iscsStatus) {
	//	The_ResourceConfig::instance()->startRetrieveDB(RadioResourceConfigFacade::DB_SYNC_RADIO_MODE);
	//} else {
	//	syncFromDb(RR_NIL, m_RadioMFTAgentCallback);
	//}
	FUNCTION_EXIT;
}

void RadioMFTAgentStatusManager::clearAllResources()
{
	TA_Base_Core::ThreadGuard guard( m_lock );

	FUNCTION_ENTRY("clearAllResources");

	m_channelCollection.clear();

	m_groupCollectionPG.clear();
	m_groupCollectionMSel.clear();
	m_groupMembers.clear();

	m_dynGroupMembers.clear();
	m_dynGroupMap.clear();
	m_dynamicRegroup.clear();

	m_MFTResourceCollection.clear();

	m_privateCallRes.initialise();

	m_tempPatchGroup.initialise();

	m_pendingPatchGroup.initialise();
	m_pendingMSel.initialise();
	m_pendingSite.initialise();

	m_resourceCounter.clear();
	m_curActResource.initialise();

	FUNCTION_EXIT;
}

void RadioMFTAgentStatusManager::setMccConnectionStatus (bool isConnected)
{
	if (isConnected)
	{
		// let the callback know that the MCC already created an object
		// note, the connection failure is no need to inform since there is a separate
		// operation for that.
		m_RadioMFTAgentCallback->callback_applicationStateMessage(RA_MCC_CONNECTION_ESTABLISH,0);
	}
	
	{
		TA_Base_Core::ThreadGuard guard( m_flagConnectionLock );
		m_bIsMCCConnected = isConnected;

	}

	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, 
				"MCC connection status :%d :: Connected : 1 , Not Connected : 0", m_bIsMCCConnected);

}

bool RadioMFTAgentStatusManager::getMccConnectionStatus ()
{
	FUNCTION_ENTRY ("getMccConnectionStatus");

	bool bRet;
	{
		TA_Base_Core::ThreadGuard guard( m_flagConnectionLock );
		bRet = m_bIsMCCConnected;
	}
	
	FUNCTION_EXIT;
	return bRet;
	 

}

void RadioMFTAgentStatusManager::startTimer()
{
	FUNCTION_ENTRY ("startTimer");

	m_timerManager->timerStart(65000);

	FUNCTION_EXIT;
}

long RadioMFTAgentStatusManager::insertResourceChannelOnMap (const long & lUnitID,const long & lResourceId)
{
	
	long lCurrentRecord = 0;

	std::map <long,long>::iterator iterMap;

	TA_Base_Core::ThreadGuard guard( m_lock );


	iterMap = m_mapResourceIDFromChannelRec.find(lUnitID);

	if (iterMap == m_mapResourceIDFromChannelRec.end())
	{
		m_mapResourceIDFromChannelRec.insert(std::pair <long,long>(lUnitID,lResourceId));
	}
	
	lCurrentRecord = m_mapResourceIDFromChannelRec.size();

	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Inserted To Map Channel : %d with Resource ID %d Map Size : %d",
				 lUnitID,lResourceId,lCurrentRecord);

	return lCurrentRecord;	
}

long RadioMFTAgentStatusManager::getResourceIdFromChannelId (const long & lUnitID)
{
	long lResourceId = 0;

	std::map <long,long>::iterator iterMap;
    TA_Base_Core::ThreadGuard guard( m_lock );
	iterMap = m_mapResourceIDFromChannelRec.find(lUnitID);

	if (iterMap != m_mapResourceIDFromChannelRec.end())
	{
		lResourceId = iterMap->second;
	}

	return lResourceId;
}

long RadioMFTAgentStatusManager::deleteResourceChangeRecOnMap (const long & lUnitID)
{
	long lCurrentRecord = 0;
	long lResourceID = 0;

	std::map <long,long>::iterator iterMap;
	TA_Base_Core::ThreadGuard guard( m_lock );
	iterMap = m_mapResourceIDFromChannelRec.find(lUnitID);

	if (iterMap != m_mapResourceIDFromChannelRec.end())
	{
		lResourceID = iterMap->second;
		m_mapResourceIDFromChannelRec.erase(iterMap);
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Deleting Resource ID %d @ UnitID : %d Current Size : %d",
					lResourceID,lUnitID,m_mapResourceIDFromChannelRec.size());
	}
	else
	{
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Cannot delete Resource ID %d @ UnitID : %d Current Size : %d",
					lResourceID,lUnitID,m_mapResourceIDFromChannelRec.size());
	}

	lCurrentRecord = m_mapResourceIDFromChannelRec.size();
	
	return lCurrentRecord;
}

void RadioMFTAgentStatusManager::setRadioMFTState(RadioMFTState radioState)
{
	TA_Base_Core::ThreadGuard guard(m_radioMFtStateLock);
	m_radioMFTState = radioState;
}

RadioMFTAgentStatusManager::RadioMFTState RadioMFTAgentStatusManager::getRadioMFTState()
{
	TA_Base_Core::ThreadGuard guard(m_radioMFtStateLock);
	return m_radioMFTState;
}