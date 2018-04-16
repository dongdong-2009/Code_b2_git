#ifndef RADIO_RESOURCE_SYNC_H_INCLUDED
#define RADIO_RESOURCE_SYNC_H_INCLUDED

#include "bus/radio/radio_bus/src/radiotypes.h"
#include "bus/radio/radio_agent/IDL/src/RadioServerAgentIDL.h"
#include "bus/radio/radio_agent/IDL/src/CommonRadioAgentIDL.h"

#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/RadioEntityData.h"

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"

class RadioResourceSync
{
public:
	RadioResourceSync();
	~RadioResourceSync();

	void onResourceChange(const RadioResource& oldResource, const RadioResource& newResource);
	void onProfileDatabaseChange(unsigned long profileKey, std::vector<std::string> talkgroups);

private:
	void initialize();
	TA_IRS_Bus::Radio::ERadioResourceType convertResourceType(RadioResourceType etype);
	
	TA_Base_Core::CommsMessageSender* m_commsMessageSender;

	bool m_bUpdateUi;
};

#endif