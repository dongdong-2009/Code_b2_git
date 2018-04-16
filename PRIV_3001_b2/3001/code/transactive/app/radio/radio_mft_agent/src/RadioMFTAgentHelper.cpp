/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent/src/RadioMFTAgentHelper.cpp $
 * @author:  Albertus Zakaria
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */ 
   
#include "app/radio/radio_mft_agent/src/RadioMFTAgentHelper.h"

#include "core/data_access_interface/entity_access/src/RadioMFTAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/entity_access/src/RadioProfileEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

const std::string RadioMFTAgentHelper::RADIO_MFT_AGENT_ENTITY_NAME = "RadioMFTAgent";
const std::string RadioMFTAgentHelper::RADIO_SERVER_AGENT_ENTITY_NAME = "RadioServerCommand";
int RadioMFTAgentHelper::DEFAULT_MCCRECONNECTION_WAITING_TIME = 10000; //Default value in milliseconds for waiting time before doing Registration
int RadioMFTAgentHelper::DEFAULT_MAX_RADIO_RETRIES         = 3; // Default value of 3 Retries
int RadioMFTAgentHelper::DEFAULT_MAX_TALKGROUP_QUERY_TIMEOUT = 500; // Default value Timeout in milliseconds
int RadioMFTAgentHelper::DEFAULT_MAX_RADIOUSER_QUERY_TIMEOUT = 500; // Default value Timeout in milliseconds
int RadioMFTAgentHelper::DEFAULT_MAX_DATABASE_MCC_QUERY_TIMEOUT = 10000; // Default value in milliseconds, This is to give a delay for the MCC Database to reloads its records
int RadioMFTAgentHelper::DEFAULT_GPIOM_AVAILABILITY               = 0; //By default GPIOM is not available



std::string RadioMFTAgentHelper::getRadioMFTAgentEntityName()
{
    TA_Base_Core::IEntityData* entity = getRadioMFTAgentEntity();
    std::string name = entity->getName();

    delete entity;
    return name;
}

unsigned long RadioMFTAgentHelper::getRadioMFTAgentEntityKey()
{
    TA_Base_Core::IEntityData* entity = getRadioMFTAgentEntity();
    unsigned long entityKey = entity->getKey();

    delete entity;
    return entityKey;
}



TA_Base_Core::IEntityData* RadioMFTAgentHelper::getRadioMFTAgentEntity()
{
    std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
    TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
    std::string consoleName = console->getName();
    delete console;
    
    std::vector<TA_Base_Core::IEntityData*> entityList;
    
    entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(
        RADIO_MFT_AGENT_ENTITY_NAME);

    // construct the name of the mft agent entity based on console
    std::string targetName = consoleName + RADIO_MFT_AGENT_ENTITY_NAME;

    TA_Base_Core::IEntityData* retEntity = NULL;

    for (int i = 0; i < entityList.size(); ++i)
    {
        if (entityList[i]->getName() == targetName)
        {
            retEntity = entityList[i];
        }
        else
        {
            delete entityList[i];
        }
    }
LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Searching for entity = %s",targetName.c_str());
    TA_ASSERT(retEntity != NULL, "No entity found for this console");
    
    return retEntity;
}


std::string RadioMFTAgentHelper::getRadioServerAgentEntityName()
{
    TA_Base_Core::IEntityData* entity = getRadioServerAgentEntity();
    std::string name = entity->getName();
    
    delete entity;
    return name;
}

unsigned long RadioMFTAgentHelper::getRadioServerAgentEntityKey()
{
    TA_Base_Core::IEntityData* entity = getRadioServerAgentEntity();
    unsigned long entityKey = entity->getKey();

    delete entity;
    return entityKey;
}


TA_Base_Core::IEntityData* RadioMFTAgentHelper::getRadioServerAgentEntity()
{
    std::vector<TA_Base_Core::IEntityData*> entityList;

	unsigned long locationKey = atol(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());

	entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( RadioMFTAgentHelper::RADIO_SERVER_AGENT_ENTITY_NAME, locationKey,true);

    // Should be just one entity of the agent
    //entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(
    //    RadioMFTAgentHelper::RADIO_SERVER_AGENT_ENTITY_NAME);
    TA_ASSERT(entityList.size()>0,"RadioServer entity not found for this location.");
    TA_ASSERT(entityList.size()<2,"More than one radio entity defined for this location.");

    return entityList[0];
}

std::string RadioMFTAgentHelper::getProfileName()
{
	TA_Base_Core::ISession* session  = NULL;

	std::string profileName;
    try
    {
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        session = TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId);

        int profileKey = session->getProfileKey();
		
		TA_Base_Core::IProfile* iProfile = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(profileKey);
		profileName = iProfile->getName();
		
		delete iProfile;
        delete session;
    }
    catch(TA_Base_Core::TransactiveException&)
    {
        if(session)
        {
            delete session;
            session = NULL;
        }
    }

	return profileName;
}

void RadioMFTAgentHelper::convertRadioResourceFromIDL(const TA_IRS_Bus::Radio::RadioResource& source, RadioResource& target)
{
	target.id = source.id;
	target.alias = source.alias;
	target.type = convertResourceType(source.type);
	target.ssi = source.ssi;
    target.hasError = source.hasError;
}

void RadioMFTAgentHelper::convertRadioResourceToIDL(const RadioResource& source, TA_IRS_Bus::Radio::RadioResource& target)
{
	target.id = source.id;
	target.alias = CORBA::string_dup(source.alias.c_str());
	target.type = convertResourceType(source.type);
	target.ssi = source.ssi;
    target.hasError = source.hasError;
}

RadioResourceType RadioMFTAgentHelper::convertResourceType(TA_IRS_Bus::Radio::ERadioResourceType etype)
{
	RadioResourceType type = RR_NIL;

	switch(etype)
	{
		case TA_IRS_Bus::Radio::RR_RADIO: 
			type = RR_RADIO; break;
		case TA_IRS_Bus::Radio::RR_TALKGROUP:
			type = RR_TALKGROUP; break;
		case TA_IRS_Bus::Radio::RR_DYNGROUP:
			type = RR_DYNGROUP; break;
		case TA_IRS_Bus::Radio::RR_ANNGROUP:
			type = RR_ANNGROUP; break;
		case TA_IRS_Bus::Radio::RR_PATCHGROUP:
			type = RR_PATCHGROUP; break;
		case TA_IRS_Bus::Radio::RR_SITE:
			type = RR_SITE; break;
		case TA_IRS_Bus::Radio::RR_MSITE:
			type = RR_MSITE; break;
		case TA_IRS_Bus::Radio::RR_MSELECT:
			type = RR_MSELECT; break;
		case TA_IRS_Bus::Radio::RR_TRAIN:
			type = RR_TRAIN;
	}

	return type;
}

TA_IRS_Bus::Radio::ERadioResourceType RadioMFTAgentHelper::convertResourceType(RadioResourceType etype)
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

TA_IRS_Bus::Radio::EPrivateCallState RadioMFTAgentHelper::convertPCState(PrivateCallState state)
{
    TA_IRS_Bus::Radio::EPrivateCallState eState = TA_IRS_Bus::Radio::PC_NOT_INVOLVED;

	switch(state)
	{
	case PC_INCOMING:
        eState = TA_IRS_Bus::Radio::PC_INCOMING;
		break;
	case PC_OUTGOING:
        eState = TA_IRS_Bus::Radio::PC_OUTGOING;
		break;
	case PC_ESTABLISHED:
        eState = TA_IRS_Bus::Radio::PC_ESTABLISHED;
		break;
	}

	return eState;
}

ApiState RadioMFTAgentHelper::convertApiStateFromIDL(TA_IRS_Bus::Radio::EApiState state)
{
	ApiState aState;

	switch(state) {
		case TA_IRS_Bus::Radio::EApiState::AP_ASSIGN_STATE:
			aState = RA_ASSIGN_STATE;
			break;
		case TA_IRS_Bus::Radio::EApiState::AP_DEASSIGN_STATE:
			aState = RA_DEASSIGN_STATE;
			break;
		case TA_IRS_Bus::Radio::EApiState::AP_SELECT_STATE:
			aState = RA_SELECT_STATE;
			break;
		case TA_IRS_Bus::Radio::EApiState::AP_DESELECT_STATE:
			aState = RA_DESELECT_STATE;
			break;
	}

	return aState;
}


TA_IRS_Bus::Radio::EApiState RadioMFTAgentHelper::convertApiStateToIDL(ApiState aState)
{
	TA_IRS_Bus::Radio::EApiState state;

	switch(aState) {
		case RA_ASSIGN_STATE:
			state = TA_IRS_Bus::Radio::EApiState::AP_ASSIGN_STATE;
			break;
		case RA_DEASSIGN_STATE:
			state = TA_IRS_Bus::Radio::EApiState::AP_DEASSIGN_STATE;
			break;
		case RA_SELECT_STATE:
			state = TA_IRS_Bus::Radio::EApiState::AP_SELECT_STATE;
			break;
		case RA_DESELECT_STATE:
			state = TA_IRS_Bus::Radio::EApiState::AP_DESELECT_STATE;
			break;
	}

	return state;
}

/*void RadioMFTAgentHelper::convertTelephoneFromIDL(const TA_IRS_Bus::Telephony::TelephoneResource& source, TA_IRS_Bus::TelephoneResource& target)
{
	target.id = source.id;
	target.alias = source.alias.in();
	target.number= source.number;

	switch(source.type)
	{
		case TA_IRS_Bus::Telephony::TR_DLTS:
			target.type = TA_IRS_Bus::TR_DLTS; break;
		case TA_IRS_Bus::Telephony::TR_DLTS_CONFERENCE:
			target.type = TA_IRS_Bus::TR_DLTS_CONFERENCE; break;
		case TA_IRS_Bus::Telephony::TR_EPAX:
			target.type = TA_IRS_Bus::TR_EPAX; break;

		default:
			target.type = TA_IRS_Bus::TR_NIL; break;
	}
	target.groupType = TA_IRS_Bus::TT_UNKNOWN; // not needed

}


void RadioMFTAgentHelper::convertTelephoneToIDL(const TA_IRS_Bus::TelephoneResource& source, TA_IRS_Bus::Telephony::TelephoneResource& target)
{
	target.id = source.id;
	target.alias = CORBA::string_dup(source.alias.c_str());
	target.number= source.number;

	switch(source.type)
	{
		case TA_IRS_Bus::TR_DLTS:
			target.type = TA_IRS_Bus::Telephony::TR_DLTS; break;
		case TA_IRS_Bus::TR_DLTS_CONFERENCE:
			target.type = TA_IRS_Bus::Telephony::TR_DLTS_CONFERENCE; break;
		case TA_IRS_Bus::TR_EPAX:
			target.type = TA_IRS_Bus::Telephony::TR_EPAX; break;

		default:
			target.type = TA_IRS_Bus::Telephony::TR_NIL; break;
	}
	target.groupType = TA_IRS_Bus::Telephony::TT_UNKNOWN; // not needed
}
*/
int RadioMFTAgentHelper::getBIMResourceID()
{
	int result = 0;
	// get BIM resource
    std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
    TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
    std::string consoleName = console->getName();
    delete console;

	std::string entityName = consoleName + "RadioMFTAgent";
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Get RadioMFTAgent entity = %s",entityName.c_str());

	TA_Base_Core::IEntityData* pEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);		
	TA_ASSERT(pEntity != NULL,"RadioMFTAgent entity not found for this location.");
	
	TA_Base_Core::RadioMFTAgentEntityData * pData = new TA_Base_Core::RadioMFTAgentEntityData(pEntity->getKey());
	if (pData != NULL)
	{
		result = pData->getBIMResource();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Get BIMResource ID = %d",result);
	}
	delete pData;

	return result;
}

// TD16770
int RadioMFTAgentHelper::getSPIResourceID()
{
	int result = 0;
	// get SPI resource
    std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
    TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
    std::string consoleName = console->getName();
    delete console;

	std::string entityName = consoleName + "RadioMFTAgent";
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Get RadioMFTAgent entity = %s",entityName.c_str());

	TA_Base_Core::IEntityData* pEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);		
	TA_ASSERT(pEntity != NULL,"RadioMFTAgent entity not found for this location.");
	
	TA_Base_Core::RadioMFTAgentEntityData * pData = new TA_Base_Core::RadioMFTAgentEntityData(pEntity->getKey());
	if (pData != NULL)
	{
		result = pData->getSPIResource();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Get SPIResource ID = %d",result);

	}
	delete pData;
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "GetSPIResource ID = %d",result);
	return result;
}

std::string RadioMFTAgentHelper::getRadioSystemIOR()
{
	std::string result;
	// get SPI resource
    std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
    TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
    std::string consoleName = console->getName();
    delete console;

	std::string entityName = consoleName + "RadioMFTAgent";

	TA_Base_Core::IEntityData* pEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);		
	TA_ASSERT(pEntity != NULL,"RadioMFTAgent entity not found for this location.");
	
	unsigned long key = pEntity->getKey();

	TA_Base_Core::EntityHelper help( key,"RadioMFTAgent");
	result = help.getParameter("RadioSystemIOR");

	return result;
}

std::string RadioMFTAgentHelper::getRadioSystemUsername()
{
	std::string result;
	// get SPI resource
    std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
    TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
    std::string consoleName = console->getName();
    delete console;

	std::string entityName = consoleName + "RadioMFTAgent";

	TA_Base_Core::IEntityData* pEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);		
	TA_ASSERT(pEntity != NULL,"RadioMFTAgent entity not found for this location.");
	
	unsigned long key = pEntity->getKey();

	TA_Base_Core::EntityHelper help( key,"RadioMFTAgent");
	result = help.getParameter("RadioSystemUsername");

	return result;
}

std::string RadioMFTAgentHelper::getRadioSystemPassword()
{
	std::string result;
	// get SPI resource
    std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
    TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
    std::string consoleName = console->getName();
    delete console;

	std::string entityName = consoleName + "RadioMFTAgent";

	TA_Base_Core::IEntityData* pEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);		
	TA_ASSERT(pEntity != NULL,"RadioMFTAgent entity not found for this location.");
	
	unsigned long key = pEntity->getKey();

	TA_Base_Core::EntityHelper help( key,"RadioMFTAgent");
	result = help.getParameter("RadioSystemPassword");

	return result;
}

std::string RadioMFTAgentHelper::getRadioSystemISSI()
{
	std::string result;
	// get SPI resource
    std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
    TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
    std::string consoleName = console->getName();
    delete console;

	std::string entityName = consoleName + "RadioMFTAgent";

	TA_Base_Core::IEntityData* pEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);		
	TA_ASSERT(pEntity != NULL,"RadioMFTAgent entity not found for this location.");
	
	unsigned long key = pEntity->getKey();

	TA_Base_Core::EntityHelper help( key,"RadioMFTAgent");
	result = help.getParameter("RadioSystemISSI");

	return result;
}

int RadioMFTAgentHelper::getMCCReconnectionWaitingTime()
{
	std::string result;
	int waitingTimeMCCReconnection;

	std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
	std::string consoleName = console->getName();
	delete console;

	std::string entityName = consoleName + "RadioMFTAgent";

	TA_Base_Core::IEntityData* pEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);		
	TA_ASSERT(pEntity != NULL,"RadioMFTAgent entity not found for this location.");

	unsigned long key = pEntity->getKey();

	TA_Base_Core::EntityHelper help( key,"RadioMFTAgent");
	result = help.getParameter("MCCReconnectionWaitingTime");
	waitingTimeMCCReconnection = atoi(result.c_str());

	//If waitingTimeMCCReconnection = 0, set to default value.
	if ( 0 == waitingTimeMCCReconnection )
	{
		waitingTimeMCCReconnection = DEFAULT_MCCRECONNECTION_WAITING_TIME;
	}

	return waitingTimeMCCReconnection;
}

int RadioMFTAgentHelper::getMaxRadioRetries()
{
	std::string result;
	int maxRadioRetries;

	std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
	std::string consoleName = console->getName();
	delete console;

	std::string entityName = consoleName + "RadioMFTAgent";

	TA_Base_Core::IEntityData* pEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);		
	TA_ASSERT(pEntity != NULL,"RadioMFTAgent entity not found for this location.");

	unsigned long key = pEntity->getKey();

	TA_Base_Core::EntityHelper help( key,"RadioMFTAgent");
	result = help.getParameter("MaxRadioRetries");
	maxRadioRetries = atoi(result.c_str());

	//If maxRadioRetries = 0, set to default value.
	if ( 0 == maxRadioRetries )
	{
		maxRadioRetries = DEFAULT_MAX_RADIO_RETRIES;
	}

	return maxRadioRetries;
}

int RadioMFTAgentHelper::getMaxTalkgroupQueryTimeout()
{
	std::string result;
	int maxTalkgroupQuery;

	std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
	std::string consoleName = console->getName();
	delete console;

	std::string entityName = consoleName + "RadioMFTAgent";

	TA_Base_Core::IEntityData* pEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);		
	TA_ASSERT(pEntity != NULL,"RadioMFTAgent entity not found for this location.");

	unsigned long key = pEntity->getKey();

	TA_Base_Core::EntityHelper help( key,"RadioMFTAgent");
	result = help.getParameter("MaxTalkgroupQueryTimeout");
	maxTalkgroupQuery = atoi(result.c_str());

	//If maxRadioRetries = 0, set to default value.
	if ( 0 == maxTalkgroupQuery )
	{
		maxTalkgroupQuery = DEFAULT_MAX_TALKGROUP_QUERY_TIMEOUT;
	}

	return maxTalkgroupQuery;
}

int RadioMFTAgentHelper::getMaxRadioUserQueryTimeout()
{
	std::string result;
	int maxRadioUserQuery;

	std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
	std::string consoleName = console->getName();
	delete console;

	std::string entityName = consoleName + "RadioMFTAgent";

	TA_Base_Core::IEntityData* pEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);		
	TA_ASSERT(pEntity != NULL,"RadioMFTAgent entity not found for this location.");

	unsigned long key = pEntity->getKey();

	TA_Base_Core::EntityHelper help( key,"RadioMFTAgent");
	result = help.getParameter("MaxRadioUserQueryTimeout");
	maxRadioUserQuery = atoi(result.c_str());

	//If maxRadioRetries = 0, set to default value.
	if ( 0 == maxRadioUserQuery )
	{
		maxRadioUserQuery = DEFAULT_MAX_RADIOUSER_QUERY_TIMEOUT;
	}

	return maxRadioUserQuery;
}

int RadioMFTAgentHelper::getDatabaseMCCQueryTimeout()
{
	std::string result;
	int mccDatabaseQueryDelay;

	std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
	std::string consoleName = console->getName();
	delete console;

	std::string entityName = consoleName + "RadioMFTAgent";

	TA_Base_Core::IEntityData* pEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);		
	TA_ASSERT(pEntity != NULL,"RadioMFTAgent entity not found for this location.");

	unsigned long key = pEntity->getKey();

	TA_Base_Core::EntityHelper help( key,"RadioMFTAgent");
	result = help.getParameter("DatabaseMCCQueryDelay");
	mccDatabaseQueryDelay = atoi(result.c_str());

	//If mccDatabaseQueryDelay = 0, set to default value.
	if ( 0 == mccDatabaseQueryDelay )
	{
		mccDatabaseQueryDelay = DEFAULT_MAX_DATABASE_MCC_QUERY_TIMEOUT;
	}

	return mccDatabaseQueryDelay;
}

int RadioMFTAgentHelper::getGPIOMAvailability()
{
	std::string result;
	int GPIOM_Availability;

	std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
	std::string consoleName = console->getName();
	delete console;

	std::string entityName = consoleName + "RadioMFTAgent";

	TA_Base_Core::IEntityData* pEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);		
	TA_ASSERT(pEntity != NULL,"RadioMFTAgent entity not found for this location.");

	unsigned long key = pEntity->getKey();

	TA_Base_Core::EntityHelper help( key,"RadioMFTAgent");
	result = help.getParameter("GPIOM_Available");
	GPIOM_Availability = atoi(result.c_str());

	//If GPIOM_Availability parameter is not exist in the Database, set to default value.
	if ( 0 == GPIOM_Availability )
	{
		GPIOM_Availability = DEFAULT_GPIOM_AVAILABILITY;
	}

	return GPIOM_Availability;
}

std::string RadioMFTAgentHelper::convertGPIOMToString(int gpiom)
{
	std::string _ret;
	switch (gpiom)
	{
	case 1026:
		_ret = "GPIOM_AVAILABLE";
		break;

	case 1024:
		_ret = "GPIOM_NOT_AVAILABLE";
		break;;
	default:
		_ret = "Wrong value";
		break;
	}
	return _ret;
}