/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentStatusManager.cpp $
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


#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentStatusManager.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioResourceConfigFacade.h"
#include "app/radio/radio_mft_agent_icdv4/src/IRadioMFTAgentCallback.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentHelper.h"

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


TA_Base_Core::ReEntrantThreadLockable RadioMFTAgentStatusManager::m_lock;

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
        m_bStart(true), m_RadioMFTAgentCallback(NULL)
{
    FUNCTION_ENTRY("RadioMFTAgentStatusManager");
    
	m_profile = RadioMFTAgentHelper::getProfileName(); 

//TD18803
    bPermitProfile = true;
//TD18803
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Profile => %s",m_profile.c_str());

    FUNCTION_EXIT;
}


RadioMFTAgentStatusManager::~RadioMFTAgentStatusManager()
{
    FUNCTION_ENTRY("~RadioMFTAgentStatusManager");
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
        if (m_RadioMFTAgentCallback != NULL)
        {
            m_RadioMFTAgentCallback->callback_groupStateMessage(resource, state);
        }
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
	FUNCTION_ENTRY("setFeatureAttributeEmergencyMode");
	
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
		for(iter = m_MFTResourceCollection.begin() ;iter!= m_MFTResourceCollection.end(); iter++) {
			if ((*iter).first.type == resource.type && (*iter).first.alias.compare(resource.alias)==0) {
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
	} else {
		RadioResource r = (*iter).first;
		RadioResourceMFTServerAttributes attr = (*iter).second;
		m_MFTResourceCollection.erase(iter);

		r.alias = resource.alias;
		m_MFTResourceCollection[r] = attr;
	}
	
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

		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "For testing purpose, dummy channel is used");

		//channelID = 1;

		// on the release, must throw the exception
		//TA_THROW(TA_Base_Core::TransactiveException(logErr.str()));
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
    FUNCTION_ENTRY("addPatchGroup");
	TA_Base_Core::ThreadGuard guard( m_lock );

	m_groupCollectionMSel[identifier] = mselGrp;
    FUNCTION_EXIT;
}


void RadioMFTAgentStatusManager::delMselMember(unsigned int mselGrp, RadioResource& member)
{
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




void RadioMFTAgentStatusManager::initDataFromDB()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("initDataFromDB");

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
    std::vector<RadioResource> mcdbList;
    The_ResourceConfig::instance()->getRadioResource(mcdbList);

    // merge the list from transactive with MCDB, get the alias from transactive
    std::vector<RadioResource>::iterator mcdbIt;
    std::vector<RadioResource>::iterator transactiveIt;

	int bimRID = RadioMFTAgentHelper::getBIMResourceID();
    for (mcdbIt = mcdbList.begin(); mcdbIt != mcdbList.end();)
    {
        // new motorola sdk do not have type for private call, use label instead
		//if (mcdbIt->type ==RR_PRIVATECALL)
		if (mcdbIt->type == RR_PRIVATECALL)
		{
			m_privateCallRes = *mcdbIt;
			//mcdbList.erase(mcdbIt);
		}
        else if (mcdbIt->type ==RR_BIM)
		{
			if (bimRID == mcdbIt->id)
			m_BIMRes = *mcdbIt;
			//mcdbList.erase(mcdbIt);
		}
		// all talkgroup resources are DGNA-able
		else if (mcdbIt->type == RR_TALKGROUP)//(mcdbIt->type == RR_DYNGROUP)
        {
            // note that the DYNAMIC GROUP resources are separated to their own collection
            // they are not supposed to be visible to the users, and we only need to get the resource ID
			// ++Fixed TD14320
			//for (transactiveIt = transactiveList.begin(); transactiveIt != transactiveList.end(); ++transactiveIt)
			//{
			//	if (transactiveIt->id == mcdbIt->id)
			//	{
			//		DynGroup dyn;
			//		dyn.isFree = true;
			//		dyn.dgID   = 0; // TD14133
			//		dyn.dynGroup.type  = RR_DYNGROUP;
			//		dyn.dynGroup.id    = mcdbIt->id;
			//		dyn.dynGroup.ssi   = transactiveIt->ssi;
			//		dyn.dynGroup.alias = transactiveIt->alias;

			//		m_dynamicRegroup.push_back(dyn);

			//		transactiveList.erase(transactiveIt);
			//		break;
			//	}
			//}
			// --Fixed TD14320

			//DynGroup dyn;
			//dyn.isFree = true;
			//dyn.dgID = 0;
			//dyn.dynGroup.type = RR_DYNGROUP;
			//dyn.dynGroup.ssi = mcdbIt->ssi;
			//dyn.dynGroup.id = mcdbIt->id;
			//dyn.dynGroup.alias = mcdbIt->alias;

			//m_dynamicRegroup.push_back(dyn);
			//transactiveList.push_back(dyn.dynGroup);
        }
        else
		{
			for (transactiveIt = transactiveList.begin(); transactiveIt != transactiveList.end(); ++transactiveIt)
			{
				if (transactiveIt->id == mcdbIt->id)
				{
					mcdbIt->ssi = transactiveIt->ssi;
					transactiveList.erase(transactiveIt);
					break;
				}
			}

		}
		++mcdbIt;
    }

    // add the remainder??
    for (transactiveIt = transactiveList.begin(); transactiveIt != transactiveList.end(); ++transactiveIt)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RadioResourceAlias:%s, RID:%u", (*transactiveIt).alias.c_str(), (*transactiveIt).id);
        mcdbList.push_back(*transactiveIt);
    }
    transactiveList.clear();

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
    for (mcdbIt = mcdbList.begin(); mcdbIt != mcdbList.end(); ++mcdbIt)
    {
        keyFound = false;

        // check if we can find it in the temporary list,
        // and assign attribute if that's the case
        // do it from last to first, so we can erase safely
        for (int i = tempKeyList.size() - 1; i >= 0; --i)
        {
            if (tempKeyList[i].id == mcdbIt->id)
            {
                m_MFTResourceCollection[*mcdbIt] = tempAttrList[i];
                keyFound = true;
                tempKeyList.erase(tempKeyList.begin() + i);
                tempAttrList.erase(tempAttrList.begin() + i);
            }
        }

		// use default attribute && DG Talkgroups & BIM resource types are not visible to users
        if ((!keyFound)  && (mcdbIt->type != RR_BIM)) //(mcdbIt->type != RR_DYNGROUP)
        {
            m_MFTResourceCollection[*mcdbIt] = tempAttr;
        }
    }


    // we do not add the resources in the temporary list, because we do not
    // have enough information for it

	//remove Private Call resource in the collection
	MFTResourceCollection::iterator pcIt = 		
		std::find_if( m_MFTResourceCollection.begin(), m_MFTResourceCollection.end(),
					  CompareResource(m_privateCallRes) );
	if (pcIt!=m_MFTResourceCollection.end())
	{
		m_MFTResourceCollection.erase(pcIt);
	}

    loadTransactiveResources();
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

	int count = 0;
    for (std::vector<DynGroup>::iterator it = m_dynamicRegroup.begin();
        it != m_dynamicRegroup.end(); ++it)
        {
            if (!it->isFree)
				count++;
        }

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

	if (m_phoneResource.id == 0)
	{
		m_phoneResource.id    = RadioMFTAgentHelper::getSPIResourceID(); // TD16770
		m_phoneResource.type  = RR_SPI;		
		m_phoneResource.alias = "CEB1-SPI-01";

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[getPhoneResource] SPI resource=%s RID=[%d]",m_phoneResource.alias.c_str(),m_phoneResource.id);
	}

	return m_phoneResource;
}

//TD14133
void RadioMFTAgentStatusManager::setSelectedDG(const RadioResource& dynGrp,TransactiveResourceIdentifier id)
{
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
}

bool RadioMFTAgentStatusManager::getSelectedDGState(const RadioResource& resource)
{
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

	return false;
}
//TD14133

bool RadioMFTAgentStatusManager::isAudibleAlarmOn()
{
	return m_audibleAlarmOn;
}

void RadioMFTAgentStatusManager::setAudibleAlarm(bool alarmMode)
{
	m_audibleAlarmOn = alarmMode;
}

//TD18526 marc_bugfix
//Check if profile is allowed to connect
bool RadioMFTAgentStatusManager::bCheckConnectionPermission()
{
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

	return false;
}
//TD18803
bool RadioMFTAgentStatusManager::bPermitProfileAcquireDG()
{
	return bPermitProfile;
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
	if (m_RadioMFTAgentCallback != NULL)
	{
		m_RadioMFTAgentCallback->callback_applicationStateMessage(RA_UNKNOWN, 0);
	}
	FUNCTION_EXIT;
}

