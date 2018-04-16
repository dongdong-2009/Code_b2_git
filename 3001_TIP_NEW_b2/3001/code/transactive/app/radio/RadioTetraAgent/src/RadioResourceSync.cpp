#pragma once
#include "app/radio/RadioTetraAgent/src/RadioResourceSync.h"

RadioResourceSync::RadioResourceSync() : m_bUpdateUi (false)
{
	initialize();
}

RadioResourceSync::~RadioResourceSync()
{
}

void RadioResourceSync::initialize()
{
	FUNCTION_ENTRY("initialize");
	m_commsMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::RadioComms::RadioDirectoryNotification);

	FUNCTION_EXIT;
}

void RadioResourceSync::onResourceChange(const RadioResource& oldResource, const RadioResource& newResource)
{
	TA_IRS_Bus::RadioServerAgentIDL::DbResSync data;
	CORBA::Any sendData;

	data.oldResource.id = oldResource.id;
	data.oldResource.alias = CORBA::string_dup(oldResource.alias.c_str());
	data.oldResource.type = convertResourceType(oldResource.type);
	data.oldResource.ssi = oldResource.ssi;
	data.oldResource.hasError = oldResource.hasError;

	data.newResource.id = newResource.id;
	data.newResource.alias = CORBA::string_dup(newResource.alias.c_str());
	data.newResource.type = convertResourceType(newResource.type);
	data.newResource.ssi = newResource.ssi;
	data.newResource.hasError = newResource.hasError;

	sendData <<= data;

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Broadcast update notification resource change");//, oldResource.id, newResource.id);
	
	// this flag is use just for the purpose not to update
	// the ui on run time
	if (m_bUpdateUi)
	{
		m_commsMessageSender->sendBroadcastCommsMessage(TA_Base_Core::RadioComms::RadioDirectoryNotification, sendData);
	}
	
}

void RadioResourceSync::onProfileDatabaseChange(unsigned long profileKey, std::vector<std::string> talkgroups)
{
	TA_IRS_Bus::RadioServerAgentIDL::ProfileTgSync data;
	CORBA::Any sendData;

	data.profileKey = profileKey;
	data.talkgroups.length(talkgroups.size());
	for (unsigned short i = 0; i < talkgroups.size(); i++) {
		data.talkgroups[i] = CORBA::string_dup(talkgroups[i].c_str());
	}

	sendData <<= data;

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Broadcast update notification for predefined talkgroup: profileKey = %u", profileKey);
	
	// this flag is use just for the purpose not to update
	// the ui on run time
	if (m_bUpdateUi)
	{
		m_commsMessageSender->sendBroadcastCommsMessage(TA_Base_Core::RadioComms::RadioDirectoryNotification, sendData);
	}
	
}

TA_IRS_Bus::Radio::ERadioResourceType RadioResourceSync::convertResourceType(RadioResourceType etype)
{
    TA_IRS_Bus::Radio::ERadioResourceType type = TA_IRS_Bus::Radio::RR_NIL;

	switch(etype)
	{
		case RR_RADIO: 
			type = TA_IRS_Bus::Radio::RR_RADIO; break;
		case RR_TALKGROUP:
			type = TA_IRS_Bus::Radio::RR_TALKGROUP; break;
		case RR_DYNGROUP:
			type = TA_IRS_Bus::Radio::RR_DYNGROUP; break;
		case RR_ANNGROUP:
			type = TA_IRS_Bus::Radio::RR_ANNGROUP; break;
		case RR_PATCHGROUP:
			type = TA_IRS_Bus::Radio::RR_PATCHGROUP; break;
		case RR_SITE:
			type = TA_IRS_Bus::Radio::RR_SITE; break;
		case RR_MSITE:
			type = TA_IRS_Bus::Radio::RR_MSITE; break;
		case RR_MSELECT:
			type = TA_IRS_Bus::Radio::RR_MSELECT;
	}

	return type;
}