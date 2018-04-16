/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/radiomanager.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentStatusManager.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/RadioProfileEntityData.h"

#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"

#include "core/process_management/IDL/src/IControlStationCorbaDef.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/types/ControlStationComms_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "app/radio/radio_manager_gui/src/RadioErrorMsg.h"
#include "core/message/types/TrainAgentComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "bus/trains/TrainAgentCorba/IDL/src/ITrainDriverCallCorba.h"
#include "bus/trains/TrainCorbaProxy/src/CallTypesConversion.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentStatusManager.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/naming/src/INamedObject.h"

#include <sstream>
#include <iomanip>
#include <Windows.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const short RadioSystemStatus::MAX_NUMBER_GROUP = 10;
const short RadioSystemStatus::PROFILE_TALKGROUP_SIZE = 12;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TA_Base_Core::ReEntrantThreadLockable RadioSystemStatus::m_lock;

RadioSystemStatus::RadioSystemStatus() : m_MFTCommandCorbadef(NULL),m_subscribedToMessages(false)
{
	TA_Base_Core::IConsole* console = NULL;
	try {
		m_session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		std::string s = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);

		console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(m_session);
	}
	catch (TA_Base_Core::DatabaseException& e) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Database connection exception: %s", e.what());
		m_consoleName = "";
		m_consoleId = 0;
		m_entityLocation = 0;
		TA_THROW(e);
	}
	catch (TA_Base_Core::TransactiveException& e) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Transactive exception: %s", e.what());
		m_consoleName = "";
		m_consoleId = 0;
		m_entityLocation = 0;
		TA_THROW(e);
	}
	m_consoleName = console->getName();
	m_consoleId = console->getKey();
	m_entityLocation = console->getLocation();
	delete console;
	
	connectToAgents();

	//Subscribe message for TrainChangeover
	subscribeToMessages();
}

RadioSystemStatus::~RadioSystemStatus()
{
	unsubscribeToMessages();
}


void RadioSystemStatus::connectToAgents()
{
    FUNCTION_ENTRY("connectToAgents");

	//TD16870- get the reference to servant class instead of resolving thru Naming Service
	//m_MFTCommandCorbadef.setCorbaName("RadioManager",RadioMFTAgentHelper::getRadioMFTAgentEntityName());

	// RadioServerAgentIDL
	m_ServerAgentCorbaDef.setEntityName(RadioMFTAgentHelper::getRadioServerAgentEntityName(),true);

	m_mftCommandCorbadef.setEntityName(RadioMFTAgentHelper::getRadioMFTAgentEntityName(), true);

    /*try
    {
	    if( m_ServerAgentCorbaDef->_non_existent() )
	    {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
			    "m_ServerAgentCorbaDef is _non_existent.");
	    }
    }
    catch (...)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "m_ServerAgentCorbaDef is _non_existent.");
    }*/
	
	FUNCTION_EXIT;
}

void RadioSystemStatus::registerToMFT(RadioMFTAgentCallbackSubscriber* pMFTAgentCallbackSubscriber)
{
    try
    {
		//TD16870
        //if( !m_MFTCommandCorbadef->_non_existent() ) 
	    //{
		    m_MFTCommandCorbadef->registerRadioMFTAgentCallback(pMFTAgentCallbackSubscriber->_this());
			m_RadioMFTAgentCallbackSubscriber = pMFTAgentCallbackSubscriber;
	    //}
    }
    catch (...)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "Unable to register RadioMFTAgentCallback pointer");
    }
}

void RadioSystemStatus::beginEmergency(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("beginEmergency");
    
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->beginEmergency(corbaResource);
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in beginEmergency()");
	}

	FUNCTION_EXIT;
}

void RadioSystemStatus::acknowledgeEmergency(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("acknowledgeEmergency");
    
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->acknowledgeEmergency(corbaResource);
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in acknowledgeEmergency");
    }

	FUNCTION_EXIT;
}

void RadioSystemStatus::knockdownEmergency(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("knockdownEmergency");
    
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->knockdownEmergency(corbaResource);
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in knockdownEmergency()");
	}

	FUNCTION_EXIT;
}


bool RadioSystemStatus::updateEntity(RadioResource& resource, std::vector<RadioResource>& members)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("updateEntity");
    
	bool result = true;

	try
	{
		m_radioDAIImpl.updateEntity(resource,members);
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in updateEntity.");
		result = false;
	}

	FUNCTION_EXIT;
	return result;
}


bool RadioSystemStatus::deleteEntity(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("deleteEntity");
    
	bool result = true;
	
	try
	{
		m_radioDAIImpl.deleteEntity(resource);
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in deleteEntity.");
		result = false;
	}

	FUNCTION_EXIT;
	return result;
}


RadioResource RadioSystemStatus::locateSelectSpeaker()
{
	TA_Base_Core::ThreadGuard guard( m_lock );

	RadioResource selected =
		The_StatusManager::instance()->getSelectedSpeaker();


	return selected;
}


bool RadioSystemStatus::addResourceButtonConfig(RadioResource& resource, int nProfile, unsigned int buttonRef)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("addResourceButtonConfig");
    	
	bool result = true;
	
	try
	{
		if (nProfile == 0)
		{
			m_radioDAIImpl.associateButtonWithResource(m_session,resource,false,buttonRef);
		}
		else
		{
			m_radioDAIImpl.associateButtonWithResource(m_session,resource,true,buttonRef);
		}
		
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in deleteEntity.");
		result = false;
	}

	FUNCTION_EXIT;
	return result;
}

bool RadioSystemStatus::removeResourceButtonConfig(RadioResource& resource, int nProfile,unsigned int buttonRef)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("removeResourceButtonConfig");

	bool result = true;
	
	try
	{
		if (nProfile == 0)
		{
			m_radioDAIImpl.clearButton(m_session, resource, false,buttonRef);
		}
		else
		{
			m_radioDAIImpl.clearButton(m_session, resource, true,buttonRef);
		}
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in deleteEntity.");
		result = false;
	}

	FUNCTION_EXIT;
	return result;
}


bool RadioSystemStatus::assignResource(RadioResource& resource, unsigned int buttonRef)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("assignResource");
    
	bool result = true;
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->assignResource(CORBA::string_dup(m_session.c_str()),corbaResource,buttonRef);
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
		MessageBox(NULL, e.reason.in(), "Error connecting", MB_OK);
		result = false;
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
        MessageBox(NULL, TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Error connecting", MB_OK);

		result = false;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in assignResource()");
        AddErrorMessage("Agent cannot be contacted");
		result = false;
	}

	FUNCTION_EXIT;
	return result;
}

bool RadioSystemStatus::deAssignResource(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("deAssignResource");
    
	bool result = true;
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->deassignResource(CORBA::string_dup(m_session.c_str()),corbaResource);
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
		result = false;
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );

		result = false;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in deassignResource()");
		result = false;
	}

	FUNCTION_EXIT;
	return result;
}

bool RadioSystemStatus::makePrivateCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("makePrivateCall");
    
	bool result = true;
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->makePrivateCall(CORBA::string_dup(m_session.c_str()),corbaResource,false);
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
		result = false;
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );

		result = false;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in makePrivateCall()");
		result = false;
	}

	FUNCTION_EXIT;
	return result;
}

int RadioSystemStatus::answerPrivateCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("answerPrivateCall");

	int result = 1;
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->answerPrivateCall(CORBA::string_dup(m_session.c_str()),corbaResource);
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
		result = 0;
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
		result = 0;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in answerPrivateCall()");
		result = 0;
	}

    FUNCTION_EXIT;
	return result;
}

bool RadioSystemStatus::endPrivateCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("endPrivateCall");
    
	bool result = true;
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->endCall(CORBA::string_dup(m_session.c_str()),corbaResource, false);
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
		result = false;
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
		result = false;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in endPrivateCall()");
		result = false;
	}

    FUNCTION_EXIT;
	return result;
}

bool RadioSystemStatus::makeAmbienceListeningCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("makeAmbienceListeningCall");
    
	bool result = true;
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->makeAmbienceListeningCall(CORBA::string_dup(m_session.c_str()),corbaResource);
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
		result = false;
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
		result = false;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in makeAmbienceListeningCall()");
		result = false;
	}

    FUNCTION_EXIT;
	return result;
}

bool RadioSystemStatus::selectSpeaker(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("selectSpeaker");
	bool result = true;
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->selectResource(CORBA::string_dup(m_session.c_str()),corbaResource);
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
		result = false;
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
		result = false;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in selectSepaker");
		result = false;
	}

    FUNCTION_EXIT;
	return result;
}

bool RadioSystemStatus::unselectSpeaker(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("unselectSpeaker");
	bool result = false;
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->deselectResource(CORBA::string_dup(m_session.c_str()),corbaResource);
		result = true;
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in unselectSpeaker()");
	}

    FUNCTION_EXIT;
	return result;
}

bool RadioSystemStatus::changeVolume(RadioResource& resource, unsigned int volume)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("changeVolume");

	bool result = false;
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		if (getResource_isSelected(resource))
		{
			m_MFTCommandCorbadef->changeVolume(m_session.c_str(),corbaResource,TA_IRS_Bus::Radio::SPEAKER_SELECT,volume);
		}
		else
		{
			m_MFTCommandCorbadef->changeVolume(m_session.c_str(),corbaResource,TA_IRS_Bus::Radio::SPEAKER_UNSELECTED,volume);
		}
        result = true;
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in changeVolume()");
	}

    FUNCTION_EXIT;
	return result;
}

bool RadioSystemStatus::connectGroup(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("connectGroup");

	bool result = false;
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->connectGroup(CORBA::string_dup(m_session.c_str()),corbaResource);
		result = true;
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
		MessageBox(NULL, e.reason.in(), "Error connecting", MB_OK);
		result = false;
	}
	catch(const CORBA::Exception& e)
	{
        MessageBox(NULL, TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Error connecting", MB_OK);
		result = false;
	}
	catch(...)
	{
        AddErrorMessage("Agent cannot be contacted");
		result = false;
	}

    FUNCTION_EXIT;
	return result;
}

bool RadioSystemStatus::releaseGroup(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("releaseGroup");
    
	bool result = false;
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->releaseGroup(CORBA::string_dup(m_session.c_str()),corbaResource);
		result = true;
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in releaseGroup()");
	}

    FUNCTION_EXIT;
	return result;
}

RadioResource RadioSystemStatus::getDGTalkgroup(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("releaseGroup");
    
	RadioResource talkgroup;
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		TA_IRS_Bus::Radio::RadioResource* cTalkgroup = m_MFTCommandCorbadef->getDGTalkgroup(CORBA::string_dup(m_session.c_str()),corbaResource);

		if (cTalkgroup != NULL)
		{ 
			RadioMFTAgentHelper::convertRadioResourceFromIDL(*cTalkgroup, talkgroup);
			delete cTalkgroup;
		}
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in releaseGroup()");
	}

    FUNCTION_EXIT;
	return talkgroup;
}

std::vector<RadioResource> RadioSystemStatus::getResourceCollectionAll()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResourceCollectionAll");
    
	std::vector<RadioResource> radiolist;
	try
	{
		TA_IRS_Bus::Radio::RadioResourceCollection* collection =  m_MFTCommandCorbadef->getResourceCollection(TA_IRS_Bus::Radio::RR_NIL);
		
		if (collection != NULL)
		{
			for ( int i = 0; i < collection->length(); i++ )
			{
				RadioResource resource;
                RadioMFTAgentHelper::convertRadioResourceFromIDL((*collection)[i], resource);

				radiolist.push_back(resource);
			}

			delete collection;
		}
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResourceCollectionAll()");
	}	

	FUNCTION_EXIT;
	return radiolist;
}

std::vector<RadioResource> RadioSystemStatus::getResourceCollectioForType(RadioResourceType type)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResourceCollectioForType");
    
	std::vector<RadioResource> resourceList;
	try
	{
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Trying to get resource list. %d",m_MFTCommandCorbadef);
		TA_IRS_Bus::Radio::RadioResourceCollection* collection =  m_MFTCommandCorbadef->getResourceCollection(convertResourceType(type));

		if (collection != NULL)
		{
			for ( int i = 0; i < collection->length(); i++ )
			{
				RadioResource resource;
                RadioMFTAgentHelper::convertRadioResourceFromIDL((*collection)[i], resource);

				resourceList.push_back(resource);
			}

			delete collection;
		}
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResourceCollectionForType()");
	}	

	FUNCTION_EXIT;
	return resourceList;
}


GroupConnectionState RadioSystemStatus::getResource_isGroupConnected(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResource_isGroupConnected");
    
	GroupConnectionState bIsConnected = NAG;

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		bIsConnected = (GroupConnectionState)m_MFTCommandCorbadef->isResourceGroupConnected(corbaResource);
		
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResource_isGroupConnected()");
	}

	FUNCTION_EXIT;
	return bIsConnected;
}


bool RadioSystemStatus::getResource_isActive(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResource_isActive");

	bool bisActive = false;

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		bisActive = m_MFTCommandCorbadef->isResourceActive(corbaResource);
		
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResource_isActive()");
	}

	FUNCTION_EXIT;
	return bisActive;
}


PrivateCallState RadioSystemStatus::getResource_isInPrivateCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResource_isInPrivateCall");

	PrivateCallState state = PC_NOT_INVOLVED;

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		state = (PrivateCallState) m_MFTCommandCorbadef->isPrivateCallResourceInUse(corbaResource);
		
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResource_isInPrivateCall()");
	}

	FUNCTION_EXIT;
	return state;
}

void RadioSystemStatus::setResource_isInPrivateCall(RadioResource& resource, PrivateCallState statePrivateCall)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("setResource_isInPrivateCall");

	PrivateCallState state = PC_NOT_INVOLVED;

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
		RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->setPrivateCallState(corbaResource,(TA_IRS_Bus::Radio::EPrivateCallState)statePrivateCall);

	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResource_isInPrivateCall()");
	}

	FUNCTION_EXIT;
}


bool RadioSystemStatus::getResource_isInAmbienceListeningCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResource_isInAmbienceListeningCall");

	bool bIsInAmbCall = false;

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		bIsInAmbCall = m_MFTCommandCorbadef->isResourceInAmbienceListeningCall(corbaResource);
		
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResource_isInAmbienceListeningCall()");
	}

	FUNCTION_EXIT;
	return bIsInAmbCall;
}


bool RadioSystemStatus::getResource_isBeingMonitored(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResource_isBeingMonitored");
	
	bool bIsMonitored = false;

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		bIsMonitored = m_MFTCommandCorbadef->isResourceBeingMonitored(corbaResource);
		
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResource_isBeingMonitored()");
	}

	FUNCTION_EXIT;
	return bIsMonitored;
}


bool RadioSystemStatus::getResource_isSelected(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResource_isSelected");

	bool bIsSelected = false;

	if (resource.type ==RR_NIL)
	{
		return bIsSelected;
	}
	
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		bIsSelected = m_MFTCommandCorbadef->isResourceSelected(corbaResource);		
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResource_isSelected()");
	}

    FUNCTION_EXIT;
	return bIsSelected;
	
}

bool RadioSystemStatus::getResource_isAssigned(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResource_isAssigned");

	bool bIsAssigned = false;

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		bIsAssigned = m_MFTCommandCorbadef->isResourceAssigned(corbaResource);		
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResource_isAssigned()");
	}

    FUNCTION_EXIT;
	return bIsAssigned;
}



RadioEmergencyModeType RadioSystemStatus::getResource_isEmergencyMode(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResource_isEmergencyMode");

	RadioEmergencyModeType bEmergencyMode = RE_NO_EMERGENCY;

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		bEmergencyMode = (RadioEmergencyModeType) m_MFTCommandCorbadef->isResourceInEmergencyMode(corbaResource);
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResource_isAssigned()");
	}

    FUNCTION_EXIT;
	return bEmergencyMode;
}

int RadioSystemStatus::getResourceVolume(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResourceVolume");

	int volume = 0;

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		volume = m_MFTCommandCorbadef->getResourceVolume(corbaResource);		
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResource_isAssigned()");
	}

    FUNCTION_EXIT;
	return volume;
}

//std::vector<PredefinedMessage> RadioSystemStatus::getPredefinedMessageList()
//{
//	TA_Base_Core::ThreadGuard guard( m_lock );
//    FUNCTION_ENTRY("getPredefinedMessageList");
//    
//	std::vector<PredefinedMessage> msgList;
//
//	try
//	{
//		msgList = m_radioDAIImpl.getPredefinedMessageList(m_session);
//	}
//	catch(...)
//	{
//		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getPredefinedMessageList.");
//	}
//
//	FUNCTION_EXIT;
//	return msgList;
//}


//bool RadioSystemStatus::addPredefinedTextMessage(PredefinedMessage& predefinedMessageData)
//{
//	TA_Base_Core::ThreadGuard guard( m_lock );
//    FUNCTION_ENTRY("addPredefinedTextMessage");
//
//	bool result = true;
//
//	try
//	{
//		m_radioDAIImpl.changePredefinedMessage(m_session, predefinedMessageData);
//	}
//	catch(...)
//	{
//		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in addPredefinedTextMessage.");
//		result = false;
//	}
//	
//    FUNCTION_EXIT;
//	return result;
//}


//bool RadioSystemStatus::modifyPredefinedTextMessage(PredefinedMessage& predefinedMessageData)
//{
//	TA_Base_Core::ThreadGuard guard( m_lock );
//    FUNCTION_ENTRY("modifyPredefinedTextMessage");
//
//	bool result = true;
//
//	try
//	{
//		m_radioDAIImpl.changePredefinedMessage(m_session, predefinedMessageData);
//	}
//	catch(...)
//	{
//		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in modifyPredefinedTextMessage.");
//		result = false;
//	}
//	
//    FUNCTION_EXIT;
//	return result;
//}


//bool RadioSystemStatus::removePredefinedTextMessage(PredefinedMessage& predefinedMessageData)
//{
//	TA_Base_Core::ThreadGuard guard( m_lock );
//    FUNCTION_ENTRY("removePredefinedTextMessage");
//    
//	bool result = true;
//	try
//	{
//		m_radioDAIImpl.deletePredefinedMessage(m_session, predefinedMessageData.key);
//	}
//	catch(...)
//	{
//		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in removePredefinedTextMessage.");
//		result = false;
//	}
//	
//    FUNCTION_EXIT;
//	return result;
//}


//bool RadioSystemStatus::sendTextMessage(RadioTextMessageData& messageData,RadioResource& recepient)
//{
//	TA_Base_Core::ThreadGuard guard( m_lock );
//    FUNCTION_ENTRY("sendTextMessage");
//
//	bool result = true;
//	try
//	{
//		if ( !m_ServerAgentCorbaDef->_non_existent() )
//		{
//			// TD18354
//			TA_IRS_Bus::Radio::RadioResource corbaResource;
//			RadioMFTAgentHelper::convertRadioResourceToIDL(recepient, corbaResource);
//
//			corbaResource.hasError = false;
//			try
//			{
//				m_ServerAgentCorbaDef->sendText(m_consoleName.c_str(),corbaResource,TA_IRS_Bus::Radio::ET_UNENCODED,messageData.MessageText.c_str());
//			}//TD18706,jianghp, handle the operation mode
//			catch (TA_Base_Core::OperationModeException &) 
//			{
//				m_ServerAgentCorbaDef.clear();
//				m_ServerAgentCorbaDef->sendText(m_consoleName.c_str(),corbaResource,TA_IRS_Bus::Radio::ET_UNENCODED,messageData.MessageText.c_str());
//			}
//		}
//	}
//	catch(const CORBA::Exception& e)
//	{
//        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
//               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
//
//		result = false;
//	}
//	catch(...)
//	{
//		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in sendTextMessage.");
//		result = false;
//	}
//	
//    FUNCTION_EXIT;
//	return result;
//}


//std::vector<MessageInbox> RadioSystemStatus::getHistoricalTextMessages()
//{
//	TA_Base_Core::ThreadGuard guard( m_lock );
//    FUNCTION_ENTRY("getHistoricalTextMessages");
//    
////	std::vector<MessageInbox> data = m_radioDAIImpl.getMessageInbox(m_session);
//	std::vector<MessageInbox> data = m_radioDAIImpl.getMessageInbox(m_consoleName);
//	
//	FUNCTION_EXIT;
//	return data;
//}


std::vector<RadioResource> RadioSystemStatus::getResourceMembers(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResourceMembers");
    
	std::vector<RadioResource> members;
	
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		TA_IRS_Bus::Radio::RadioResourceCollection* memberList;

		m_MFTCommandCorbadef->getResourceMembers(corbaResource,memberList);	

		if (memberList != NULL)
		{
			for (int i=0; i< memberList->length();i++)
			{
				RadioResource rMem;
                RadioMFTAgentHelper::convertRadioResourceFromIDL((*memberList)[i], rMem);

				members.push_back(rMem);
			}

			delete memberList;
		}

	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResourceMembers()");
	}

	FUNCTION_EXIT;
	return members;
}


std::vector<ResourceButton> RadioSystemStatus::getMonitoredResource()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getMonitoredResource");
    
	std::vector<ResourceButton> resources;
	
	try
	{
		resources = m_radioDAIImpl.getResourceButtonCollection(m_session);
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getMonitoredResource.");
	}
	
    FUNCTION_EXIT;
	return resources;
}

RadioResource RadioSystemStatus::getResourceBySSI(unsigned long ssi)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResourceBySSI");

	RadioResource resource;
	try
	{
		TA_IRS_Bus::Radio::RadioResource* cResource = NULL;
		
		m_MFTCommandCorbadef->getResourceBySSI(ssi,cResource);

		if (cResource != NULL)
		{
            RadioMFTAgentHelper::convertRadioResourceFromIDL(*cResource, resource);

			delete cResource;
		}
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResourceBySSI()");
	}

	return resource;
}

RadioResource RadioSystemStatus::getResourceByRID(unsigned long rid,RadioResourceType type)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResourceByID");

	RadioResource resource;

	try
	{
		TA_IRS_Bus::Radio::RadioResource* cResource = NULL;

		m_MFTCommandCorbadef->getResourceByID(rid,convertResourceType(type),cResource);

		if (cResource != NULL)
		{
            RadioMFTAgentHelper::convertRadioResourceFromIDL(*cResource, resource);

			delete cResource;
		}
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResourceByRID");
	}

	return resource;
}

RadioResource RadioSystemStatus::getResourceByAlias(std::string alias,RadioResourceType type)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getResourceByID");

	RadioResource resource;

	try
	{
		TA_IRS_Bus::Radio::RadioResource* cResource = NULL;

		m_MFTCommandCorbadef->getResourceByAlias(CORBA::string_dup(alias.c_str()),convertResourceType(type),cResource);

		if (cResource != NULL)
		{
            RadioMFTAgentHelper::convertRadioResourceFromIDL(*cResource, resource);

			delete cResource;
		}
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in getResourceByRID");
	}

	return resource;
}


//void RadioSystemStatus::updateTextMessageReadState(unsigned long key, bool flag)
//{
//	TA_Base_Core::ThreadGuard guard( m_lock );
//    FUNCTION_ENTRY("updateTextMessage");
//	
//	try
//	{
//		m_radioDAIImpl.textMessageRead(key,flag);
//	}
//	catch(...)
//	{
//		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in textMessageRead[%d]",key);
//	}
//
//    FUNCTION_EXIT;
//}

//void RadioSystemStatus::createPatchGroup(const RadioResource& resource, bool isDLTSPatch/* =true */)
//{
//	TA_Base_Core::ThreadGuard guard( m_lock );
//    FUNCTION_ENTRY("createPatchGroup");
//	
//	try
//	{
//		TA_IRS_Bus::Radio::RadioResource corbaResource;
//        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);
//
//		m_MFTCommandCorbadef->createPatchGroup(corbaResource,isDLTSPatch);
//	}
//	catch(TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
//	{
//		TA_THROW(TA_Base_Core::TransactiveException(e.reason.in()));
//	}
//	catch(const CORBA::Exception& e)
//	{
//		TA_THROW(TA_Base_Core::TransactiveException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str()));
//	}
//	catch(...)
//	{
//		TA_THROW(TA_Base_Core::TransactiveException("Failed to create patch group"));
//	}
//
//    FUNCTION_EXIT;
//
//}


//void RadioSystemStatus::releasePatchGroup(bool isDLTSPatch)
//{
//	TA_Base_Core::ThreadGuard guard( m_lock );
//    FUNCTION_ENTRY("createPatchGroup");
//	
//	try
//	{
//		m_MFTCommandCorbadef->releasePatchGroup(isDLTSPatch);
//	}
//	catch(TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
//	{
//		TA_THROW(TA_Base_Core::TransactiveException(e.reason.in()));
//	}
//	catch(const CORBA::Exception& e)
//	{
//		TA_THROW(TA_Base_Core::TransactiveException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str()));
//	}
//	catch(...)
//	{
//		TA_THROW(TA_Base_Core::TransactiveException("Failed to release patch group"));
//	}
//
//    FUNCTION_EXIT;
//
//}

/*
void RadioSystemStatus::makePhoneCall(const char* number,int length)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("makePhoneCall");
	
	try
	{
		m_MFTCommandCorbadef->makePhoneCall(CORBA::string_dup(number),length);
	}
	catch(TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
		TA_THROW(TA_Base_Core::TransactiveException(e.reason.in()));
	}
	catch(const CORBA::Exception& e)
	{
		TA_THROW(TA_Base_Core::TransactiveException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str()));
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Failed to make phone call "));
	}
}

void RadioSystemStatus::endPhoneCall(const char* number)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("makePhoneCall");
	
	try
	{
		m_MFTCommandCorbadef->endPhoneCall(CORBA::string_dup(number));
	}
	catch(TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
		TA_THROW(TA_Base_Core::TransactiveException(e.reason.in()));
	}
	catch(const CORBA::Exception& e)
	{
		TA_THROW(TA_Base_Core::TransactiveException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str()));
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Failed to end phone call "));
	}
}
*/
TA_IRS_Bus::Radio::ERadioResourceType RadioSystemStatus::convertResourceType(RadioResourceType type)
{
	TA_IRS_Bus::Radio::ERadioResourceType eType = TA_IRS_Bus::Radio::RR_NIL;

	switch(type)
	{
		case RR_RADIO:
			eType = TA_IRS_Bus::Radio::RR_RADIO;break;
		case RR_TALKGROUP:
			eType = TA_IRS_Bus::Radio::RR_TALKGROUP;break;
		case RR_DYNGROUP:
			eType = TA_IRS_Bus::Radio::RR_DYNGROUP;break;
		case RR_ANNGROUP:
			eType = TA_IRS_Bus::Radio::RR_ANNGROUP;break;
		case RR_PATCHGROUP:
			eType = TA_IRS_Bus::Radio::RR_PATCHGROUP; break;
		case RR_MSELECT:
			eType = TA_IRS_Bus::Radio::RR_MSELECT;	break;
		case RR_MSITE:
			eType = TA_IRS_Bus::Radio::RR_MSITE; break;
		case RR_SITE:
			eType = TA_IRS_Bus::Radio::RR_SITE; break;
		case RR_TRAIN:
			eType = TA_IRS_Bus::Radio::RR_TRAIN;	
	}

	return eType;

}

int RadioSystemStatus::getConfigValue(const std::string& name)
{
	int result = 0;

	std::vector<TA_Base_Core::IEntityData*> entityList =
					 TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType("RadioProfile");

    TA_ASSERT(entityList.size()>0,"Radio entity not found for this location.");
    TA_ASSERT(entityList.size()<2,"More than one radio entity defined for this location.");

	TA_Base_Core::RadioProfileEntityData* pData = dynamic_cast<TA_Base_Core::RadioProfileEntityData*>(entityList[0]);

	TA_ASSERT(pData != NULL,"RadioProfileEntityData is NULL");

	if (name.compare(TA_Base_Core::RadioProfileEntityData::CONSOLE_EVENT_TIMEOUT) == 0)
	{
		result = pData->getEventTimeout();
	}
	else if (name.compare(TA_Base_Core::RadioProfileEntityData::HISTORY_LIMIT) == 0)
	{
		result = pData->getHistoryLimit();
	}
	else if (name.compare(TA_Base_Core::RadioProfileEntityData::PROFILE_BUTTONS) == 0)
	{
		result = pData->getProfileButtons();
	}
	else if (name.compare(TA_Base_Core::RadioProfileEntityData::USER_BUTTONS) == 0)
	{
		result = pData->getUserDefinedButtons();
	}
	else if (name.compare(TA_Base_Core::RadioProfileEntityData::DIALPAD_MAXNUMBER_LENGTH) == 0)
	{
		result = pData->getDialpadMaxNumber();
	}
	else if (name.compare(TA_Base_Core::RadioProfileEntityData::DIALPAD_HISTORY_LENGTH) == 0)
	{
		result = pData->getDialpadMaxHistory();
	}
	else if (name.compare(TA_Base_Core::RadioProfileEntityData::FLASH_BUTTON_INTERVAL) == 0) // TD#14284
	{
		result = pData->getFlashButtonInterval();
	}

	delete pData;

	return result;
}

//TD18785
void RadioSystemStatus::addSelectSpeakerResource(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("addSelectSpeakerResource");
	
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->addSelectSpeakerResource(corbaResource);
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in acknowledgeEmergency");
    }

	FUNCTION_EXIT;
	
}

void RadioSystemStatus::deleteSelectSpeakerResource(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("deleteSelectSpeakerResource");

	try
	{

		m_MFTCommandCorbadef->deleteSelectSpeakerResource(resource.id);
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in acknowledgeEmergency");
    }

}

void RadioSystemStatus::addActiveResource(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("addActiveResource");

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		m_MFTCommandCorbadef->addActiveResource(corbaResource);
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in acknowledgeEmergency");
    }

}

void RadioSystemStatus::deleteActiveResource(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("deleteActiveResource");

	try
	{

		m_MFTCommandCorbadef->deleteActiveResource(resource.id);
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in acknowledgeEmergency");
    }

}
//TD18785


MonitorStatus RadioSystemStatus::requestRadioCheck(RadioResource& resource)
{
	FUNCTION_ENTRY("requestRadioCheck");

	MonitorStatus statusReturn;

	try {
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);
		 

		
		//CORBA_CALL_RETURN(stat, m_ServerAgentCorbaDef, requestRadioCheck, (corbaResource));
		::CORBA::String_member base, talkgroup;
		CORBA_CALL(m_ServerAgentCorbaDef, requestRadioCheck, (corbaResource,base,talkgroup));
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Called requestRadioCheck on radio agent");

		statusReturn.first = base;// "a";//base_corba;
		statusReturn.second = talkgroup;//"b";//talkgroup_corba;
	}
	catch(TA_Base_Core::BadParameterException& e)
	{
		TA_THROW(TA_IRS_App::TcpException(e.reason.in()));
	}
	catch(const TA_IRS_App::TcpMethodTimeoutException& e)
	{
		TA_THROW(TA_IRS_App::TcpMethodTimeoutException());
	}
	catch(const TA_IRS_App::TcpException& e)
	{
		TA_THROW(TA_IRS_App::TcpException(e.what()));
	}
	catch(const TA_Base_Core::ObjectResolutionException& e)
	{
		throw;
	}
	catch(const CORBA::Exception& e)
	{
		TA_THROW(TA_IRS_App::TcpException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str()));
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Exception occured while connecting"));
	}

	FUNCTION_EXIT;

	return statusReturn;
}

bool RadioSystemStatus::connectDynamicRegroup(RadioResource& resource,std::string talkgroupAlias)
{
	FUNCTION_ENTRY("connectDynamicRegroup");

	CORBA::Boolean status = false;
	try {
		if (resource.ssi == 0) {
			FUNCTION_EXIT;
			return false;
		}
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);
		 
		const char* talkgroup = talkgroupAlias.c_str();
		CORBA_CALL_RETURN(status, m_ServerAgentCorbaDef, connectDynamicRegroup, (corbaResource,talkgroup));
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Called connectDynamicRegroup on radio agent");
	}
	catch(TA_Base_Core::BadParameterException& e)
	{
		TA_THROW(TA_IRS_App::TcpException(e.reason.in()));
	}
	catch(const TA_IRS_App::TcpMethodTimeoutException& e)
	{
		TA_THROW(TA_IRS_App::TcpMethodTimeoutException());
	}
	catch(const TA_IRS_App::TcpException& e)
	{
		TA_THROW(TA_IRS_App::TcpException(e.what()));
	}
	catch(const TA_Base_Core::ObjectResolutionException& e)
	{
		throw;
	}
	catch(const CORBA::Exception& e)
	{
		TA_THROW(TA_IRS_App::TcpException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str()));
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Exception occured while connecting"));
	}

	if (!status) {
		AddErrorMessage("Unable to perform requested operation");
	}

	FUNCTION_EXIT;
	return status;
}

bool RadioSystemStatus::releaseDynamicRegroup(RadioResource& resource, std::string talkgroupAlias)
{
	FUNCTION_ENTRY("releaseDynamicRegroup");

	CORBA::Boolean status = false;
	try {
		if (resource.ssi == 0) {
			FUNCTION_EXIT;
			return false;
		}
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);
		
		const char* talkgroup = talkgroupAlias.c_str();
		CORBA_CALL_RETURN(status, m_ServerAgentCorbaDef, releaseDynamicRegroup, (corbaResource,talkgroup));
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Called releaseDynamicRegroup on radio agent");
	}
	catch(TA_Base_Core::BadParameterException& e)
	{
		TA_THROW(TA_IRS_App::TcpException(e.reason.in()));
	}
	catch(const TA_IRS_App::TcpMethodTimeoutException& e)
	{
		TA_THROW(TA_IRS_App::TcpMethodTimeoutException());
	}
	catch(const TA_IRS_App::TcpException& e)
	{
		TA_THROW(TA_IRS_App::TcpException(e.what()));
	}
	catch(const TA_Base_Core::ObjectResolutionException& e)
	{
		throw;
	}
	catch(const CORBA::Exception& e)
	{
		TA_THROW(TA_IRS_App::TcpException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str()));
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Exception occured while connecting"));
	}

	if (!status) {
		AddErrorMessage("Unable to perform requested operation");
	}

	FUNCTION_EXIT;
	return status;
}

std::vector<std::string> RadioSystemStatus::updateOperatorProfile(std::string opAlias)
{
	FUNCTION_ENTRY("updateOperatorProfile");
	std::vector<std::string> talkgroups;
	TA_IRS_Bus::Radio::OpTalkgroups_var talkgroupsCorba;

	try {		
		CORBA_CALL(m_ServerAgentCorbaDef, updateOperatorProfile, (CORBA::string_dup(opAlias.c_str()),talkgroupsCorba));
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Called updateOpProfile on radio agent");

		for (int i = 0; i < 12; i++) {
			talkgroups.push_back((std::string)talkgroupsCorba[i]);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "updateOpProfile, talkgroup i alias = %s", talkgroups[i].c_str());
		}
	}
	catch(TA_Base_Core::BadParameterException& e)
	{
		TA_THROW(TA_IRS_App::TcpException(e.reason.in()));
	}
	catch(const TA_IRS_App::TcpMethodTimeoutException& e)
	{
		TA_THROW(TA_IRS_App::TcpMethodTimeoutException());
	}
	catch(const TA_IRS_App::TcpException& e)
	{
		TA_THROW(TA_IRS_App::TcpException(e.what()));
	}
	catch(const TA_Base_Core::ObjectResolutionException& e)
	{
		throw;
	}
	catch(const CORBA::Exception& e)
	{
		TA_THROW(TA_IRS_App::TcpException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str()));
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Exception occured while connecting"));
	}
	
	FUNCTION_EXIT;
	return talkgroups;

}

bool RadioSystemStatus::callForwarding(bool cond, unsigned long forwardingISSI, unsigned long forwardedISSI) 
{
	FUNCTION_ENTRY("callForwarding");

	CORBA::Boolean ret = false;
	try {		
		CORBA_CALL_RETURN(ret, m_ServerAgentCorbaDef, callForwarding, (cond,forwardingISSI,forwardedISSI));
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Called callForwarding on radio agent");
	}
	catch(TA_Base_Core::BadParameterException& e)
	{
		TA_THROW(TA_IRS_App::TcpException(e.reason.in()));
	}
	catch(const TA_IRS_App::TcpMethodTimeoutException& e)
	{
		TA_THROW(TA_IRS_App::TcpMethodTimeoutException());
	}
	catch(const TA_IRS_App::TcpException& e)
	{
		TA_THROW(TA_IRS_App::TcpException(e.what()));
	}
	catch(const TA_Base_Core::ObjectResolutionException& e)
	{
		throw;
	}
	catch(const CORBA::Exception& e)
	{
		TA_THROW(TA_IRS_App::TcpException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str()));
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Exception occured while connecting"));
	}

	FUNCTION_EXIT;
	return ret;
}

bool RadioSystemStatus::cancelCallForwarding(unsigned long ISSI) 
{
	FUNCTION_ENTRY("cancelCallForwarding");

	CORBA::Boolean ret = false;
	try {		
		CORBA_CALL_RETURN(ret, m_ServerAgentCorbaDef, cancelCallForwarding, (ISSI));
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Called cancelCallForwarding on radio agent");
	}
	catch(TA_Base_Core::BadParameterException& e)
	{
		TA_THROW(TA_IRS_App::TcpException(e.reason.in()));
	}
	catch(const TA_IRS_App::TcpMethodTimeoutException& e)
	{
		TA_THROW(TA_IRS_App::TcpMethodTimeoutException());
	}
	catch(const TA_IRS_App::TcpException& e)
	{
		TA_THROW(TA_IRS_App::TcpException(e.what()));
	}
	catch(const TA_Base_Core::ObjectResolutionException& e)
	{
		throw;
	}
	catch(const CORBA::Exception& e)
	{
		TA_THROW(TA_IRS_App::TcpException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str()));
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Exception occured while connecting"));
	}

	FUNCTION_EXIT;
	return ret;
}

CallForwardingStatus RadioSystemStatus::queryCallForwarding(unsigned long ISSI) 
{
	FUNCTION_ENTRY("queryCallForwarding");

	CallForwardingStatus stat;
	stat.condition = false;
	stat.forwardedISSI = 0;
	stat.forwardingISSI = 0;
	CORBA::UShort cond;
	CORBA::ULong forwISSI;
	CORBA::ULong forwdedISSI;

	try {		
		CORBA_CALL(m_ServerAgentCorbaDef, queryCallForwarding, (ISSI, cond, forwISSI, forwdedISSI));
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Called queryCallForwarding on radio agent");
		
		stat.condition = cond;
		stat.forwardingISSI = forwISSI;
		stat.forwardedISSI = forwdedISSI;
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CallForwarding Status");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Condition:%u", cond);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ForwardingISSI:%u", forwISSI);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ForwardedISSI:%u", forwdedISSI);
	}
	catch(TA_Base_Core::BadParameterException& e)
	{
		TA_THROW(TA_IRS_App::TcpException(e.reason.in()));
	}
	catch(const TA_IRS_App::TcpMethodTimeoutException& e)
	{
		TA_THROW(TA_IRS_App::TcpMethodTimeoutException());
	}
	catch(const TA_IRS_App::TcpException& e)
	{
		TA_THROW(TA_IRS_App::TcpException(e.what()));
	}
	catch(const TA_Base_Core::ObjectResolutionException& e)
	{
		throw;
	}
	catch(const CORBA::Exception& e)
	{
		TA_THROW(TA_IRS_App::TcpException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str()));
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Exception occured while connecting"));
	}

	FUNCTION_EXIT;
	return stat;
}

void RadioSystemStatus::addRadioResource(RadioResource& resource, std::vector<RadioResource>& members, unsigned long id)
{
	FUNCTION_ENTRY("addRadioResource");

	TA_Base_Core::ThreadGuard guard( m_lock );


	try
	{


		TA_IRS_Bus::Radio::RadioResource corbaResource;
		TA_IRS_Bus::Radio::RadioResourceCollection* corbaMembers = NULL;
		corbaMembers = new TA_IRS_Bus::Radio::RadioResourceCollection();
		corbaMembers->length(members.size());
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);
		for (int i = 0; i < members.size(); i++) {
			TA_IRS_Bus::Radio::RadioResource temp;
			RadioMFTAgentHelper::convertRadioResourceToIDL(members[i], temp);
			(*corbaMembers)[i] = temp;
		}

		//CORBA_CALL(m_mftCommandCorbadef, addRadioResource, (corbaResource, *corbaMembers, size, corbaId));

		m_MFTCommandCorbadef->addRadioResource(corbaResource, 
			*corbaMembers, 
			members.size(), id
			);

	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
		TA_THROW(TA_Base_Core::TransactiveException(e.reason.in()));
	}
	catch(const CORBA::Exception& e)
	{
		TA_THROW(TA_Base_Core::TransactiveException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str()));
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Exception occured while connecting"));
	}
	FUNCTION_EXIT;
}

void RadioSystemStatus::showErrorMessageFromRadio()
{
	AddErrorMessage("Unable to perform requested operation");
}

void RadioSystemStatus::restartConnection()
{
	FUNCTION_ENTRY("restartConnection");
	TA_Base_Core::ThreadGuard guard( m_lock );

	try {
		m_MFTCommandCorbadef->restartConnection();
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
		TA_THROW(TA_Base_Core::TransactiveException(e.reason.in()));
	}
	catch(const CORBA::Exception& e)
	{
		TA_THROW(TA_Base_Core::TransactiveException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str()));
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Exception occured while connecting"));
	}
	FUNCTION_EXIT;
}

int RadioSystemStatus::getResourceCounter(RadioResource& resource)
{
	FUNCTION_ENTRY("getResourceCounter");
	TA_Base_Core::ThreadGuard guard( m_lock );

	TA_IRS_Bus::Radio::RadioResource corbaResource;
	RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);
	int count;
	::CORBA::Short c;

	try {
		//m_mftCommandCorbadef->addSelectSpeakerResource(corbaResource);
		m_MFTCommandCorbadef->getRadioResourceCount(corbaResource,c);
		count = c;
	}
	catch(const TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException& e)
	{
	    TA_THROW(TA_Base_Core::TransactiveException(e.reason.in()));
	}
	catch(const CORBA::Exception& e)
	{
		TA_THROW(TA_Base_Core::TransactiveException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str()));
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Exception occured while connecting"));
	}

	FUNCTION_EXIT;
	return count;
}

void RadioSystemStatus::updateGX(std::string message)
{
	FUNCTION_ENTRY("updateGX");
	TA_Base_Core::ThreadGuard guard( m_lock );

	TA_Base_Core::IControlStationCorbaDef::TAToGXMessage   event;
	CORBA::Any                                data;
	event.targetSessionID = CORBA::string_dup(m_session.c_str());
	event.targetSchematicID = CORBA::string_dup("");
	event.sourceEntityName = CORBA::string_dup((TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME)).c_str());
	event.message = CORBA::string_dup(message.c_str());

	data <<= event;

	TA_Base_Core::CommsMessageSender* commsMessageSender =
		TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::ControlStationComms::TAToGXMessageNotification );


	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "updateGX: m_consoleId=%u, m_entityLocation=%u, message=%s", m_consoleId,m_entityLocation,message.c_str());

	commsMessageSender->sendCommsMessage(TA_Base_Core::ControlStationComms::TAToGXMessageNotification,
		m_consoleId,
		data,
		0,
		m_entityLocation,
		NULL);

	FUNCTION_EXIT;
}

void RadioSystemStatus::updateResource(const RadioResource& oldResource, const RadioResource& newResource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("updateResource");
	TA_IRS_Bus::Radio::RadioResource corbaOldResource, corbaNewResource;
	RadioMFTAgentHelper::convertRadioResourceToIDL(oldResource, corbaOldResource);
	RadioMFTAgentHelper::convertRadioResourceToIDL(newResource, corbaNewResource);

	try {
		m_MFTCommandCorbadef->updateResource(corbaOldResource,corbaNewResource);
	}
	catch(const CORBA::Exception& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s",TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str());
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Exception occured while connecting"));
	}
	FUNCTION_EXIT;
}

void RadioSystemStatus::addNewResource(const RadioResource& newResource)
{	
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("addNewResource");
	TA_IRS_Bus::Radio::RadioResource corbaResource;
	RadioMFTAgentHelper::convertRadioResourceToIDL(newResource, corbaResource);

	try {
		m_MFTCommandCorbadef->addNewResource(corbaResource);
	}
	catch(const CORBA::Exception& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s",TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str());
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Exception occured while connecting"));
	}
	FUNCTION_EXIT;
}

void RadioSystemStatus::deleteResource(const RadioResource& oldResource)
{	
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("deleteResource");
	TA_IRS_Bus::Radio::RadioResource corbaResource;
	RadioMFTAgentHelper::convertRadioResourceToIDL(oldResource, corbaResource);

	try {
		m_MFTCommandCorbadef->deleteResource(corbaResource);
	}
	catch(const CORBA::Exception& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s",TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str());
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Exception occured while connecting"));
	}
	FUNCTION_EXIT;
}

void RadioSystemStatus::notifyEndDriverCall(const RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("notifyEndDriverCall");

	TA_IRS_Bus::Radio::RadioResource corbaResource;
	RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);
	TA_IRS_Bus::Radio::EPrivateCallState status = TA_IRS_Bus::Radio::PC_NOT_INVOLVED;

	m_RadioMFTAgentCallbackSubscriber->privateCallMessage(corbaResource, status);
	
	FUNCTION_EXIT;
}

void RadioSystemStatus::removeResource(RadioResource& resource)
{	
	TA_Base_Core::ThreadGuard guard(m_lock);

	FUNCTION_ENTRY("removeResource");

	if ( RR_RADIO == resource.type) {
		m_RadioMFTAgentCallbackSubscriber->privateCallRemoved(resource);
	}

	FUNCTION_EXIT;
}

void RadioSystemStatus::unsubscribeToMessages()
{
	FUNCTION_ENTRY( "unsubscribeToMessages" );

	// unsubscribe to messages from the train agent if subscribe
	if ( true == m_subscribedToMessages )
	{
		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );
		m_subscribedToMessages = false;
	}

	FUNCTION_EXIT;
}

void RadioSystemStatus::subscribeToMessages()
{
	FUNCTION_ENTRY( "subscribeToMessages" );

	// subscribe to messages from the train agent if not already subscribed
	// (as per the Train Agent Message Types package)
	try
	{

		unsigned long locationKey = atol( TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str() );
		TA_Base_Core::CorbaNameList agentNameList =
			TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(TA_Base_Core::TrainAgentEntityData::getStaticType(), locationKey, true);

		if (agentNameList.size() == 0)
		{
			TA_THROW(TA_Base_Core::DataException("At least 1 Train agent must be configured in the database", TA_Base_Core::DataException::NO_VALUE, "Train Agent Entity"));
		}

		if ( false == m_subscribedToMessages )
		{
			//Comms Messages TrainChangeover
			TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
				TA_Base_Core::TrainAgentComms::TrainChangeover, this,
				agentNameList[0].getEntityKey(), 0, agentNameList[0].getLocationKey() );

			m_subscribedToMessages = true;
		}
	}
	catch (TA_Base_Core::DataException& ex)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Unknown exception occur.");
	}

	FUNCTION_EXIT;
}

void RadioSystemStatus::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload )
{
	FUNCTION_ENTRY("receiveSpecialisedMessage");

	// extract the message into its corba type, use the corba
	// conversion functions to convert to the native C++ type
	TA_Base_Bus::ITrainDriverCallCorba::TrainChangeover* trainChangeover = NULL;

	if ( payload.messageState >>= trainChangeover )
	{
		TA_IRS_Bus::CallTypes::TrainChangeover cppType = TA_IRS_Bus::CallTypesConversion::convertToCpp( *trainChangeover );

		// Logs the full details of the received message
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"Received %s", cppType.getLogString().c_str() );

		processTrainChangeOver(cppType);

	}

	FUNCTION_EXIT;
}

void RadioSystemStatus::processTrainChangeOver(TA_IRS_Bus::CallTypes::TrainChangeover trainChangeover)
{
	FUNCTION_ENTRY("processTrainChangeOver");

	//Set the Radio resource id based on the TrainChangeover 
	RadioResource radioResource;
	radioResource.id = trainChangeover.resourceId;
	radioResource.type = RR_RADIO;

	//Get the private call state
	PrivateCallState radioCallState = The_StatusManager::instance()->getFeatureAttributePtCallState(radioResource);
	//Check current state cache if Radio Resource ID is equal to PC_NOT_INVOLVED state, if so, then ignore the message 
	if( PrivateCallState::PC_NOT_INVOLVED == radioCallState )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"TrainChangeover ignored, Private Call State: %d",
			static_cast<int>(radioCallState) );
		return;
	}

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Call will be terminated due to changeover, Resource ID:%d",
				radioResource.id);

	//1. End the Private call
	endPrivateCall(radioResource);

	//2. Pop up a message to inform the operator
	//that the call has been terminated due to TrainChangeover
	if( PrivateCallState::PC_OUTGOING == radioCallState  || PrivateCallState::PC_ESTABLISHED == radioCallState )
	{
		TA_Base_Bus::TransActiveMessage userMsg;
		userMsg << trainChangeover.resourceId;
		userMsg << static_cast<unsigned long>(trainChangeover.trainId);
		userMsg.showMsgBox(IDS_UI_630001);
	}

	FUNCTION_EXIT;
}