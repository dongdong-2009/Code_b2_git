/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentCommandServant.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 11:47:25 $
 * Last modified by:  $Author: builder $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentCommandServant.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioApplicationStateManager.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentStatusManager.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentHelper.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleDispatchException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "app/radio/radio_manager_gui_icdv4/src/DebugMsg.h"
#include<fstream>
using namespace std;
#include "RadioMFTAgentRemoteApiCommandImpl.h"
RadioMFTAgentCommandServant::RadioMFTAgentCommandServant(RadioMFTAgentSummary& summary, TA_Base_Core::IEntityData& entity) : 
    m_pMFTAgentCommandImpl(NULL), m_summary(summary)
{
    FUNCTION_ENTRY("RadioMFTAgentCommandServant");

    m_objectName = entity.getName();

	std::string username, password, ior;

	username = RadioMFTAgentHelper::getRadioSystemUsername();
	password = RadioMFTAgentHelper::getRadioSystemPassword();
	ior = RadioMFTAgentHelper::getRadioSystemIOR();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Read from database, Username: {%s}  Password: {%s}  IOR : {%s}",username.c_str(),password.c_str(),ior.c_str());

	m_pMFTAgentCommandImpl = new RadioMFTAgentRemoteAPICommandImpl(ior,username,password);

//TD18785
	m_SelectedSpeakerResources.clear();
	m_ActiveResources.clear();
//TD18785

    FUNCTION_EXIT;
}


RadioMFTAgentCommandServant::~RadioMFTAgentCommandServant()
{
    FUNCTION_ENTRY("~RadioMFTAgentCommandServant");

//TD18785
	m_SelectedSpeakerResources.clear();
	m_ActiveResources.clear();
//TD18785
    if (m_pMFTAgentCommandImpl)
    {
        delete m_pMFTAgentCommandImpl;
        m_pMFTAgentCommandImpl = NULL;
    }

    FUNCTION_EXIT;
}

void RadioMFTAgentCommandServant::start()
{
    TA_Assert(m_pMFTAgentCommandImpl != NULL);

    // TD16870
    // TD19154 set agent name using runparam UserAgentName
    TA_Base_Core::RunParams::getInstance().set("UseAgentName", m_objectName.c_str());
    activateServantWithName(m_objectName);
}

void RadioMFTAgentCommandServant::stop()
{
//TD18785
	terminateCurrentSessions();
//TD18785
	// TD16870
    deactivateAndDeleteServant();
}


void RadioMFTAgentCommandServant::registerRadioMFTAgentCallback(TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr pCallback)
{
    FUNCTION_ENTRY("registerRadioMFTAgentCallback");

	TA_Assert(m_pMFTAgentCommandImpl != NULL);
	
	if ( !m_pMFTAgentCommandImpl->registerRadioMFTAgentCallback(pCallback) )
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("RadioMFTAgentCallbackIDL_ptr is NULL");
		throw ex;
	}

	// TEMP
	m_pCallback = pCallback;

    FUNCTION_EXIT;
}


TA_IRS_Bus::Radio::RadioResourceCollection* RadioMFTAgentCommandServant::getResourceCollection(TA_IRS_Bus::Radio::ERadioResourceType type)
{
    FUNCTION_ENTRY("getResourceCollection");

	TA_IRS_Bus::Radio::RadioResourceCollection* rCollection = NULL;
	try
	{
        std::vector<RadioResource> resources = m_pMFTAgentCommandImpl->getResourceCollection(RadioMFTAgentHelper::convertResourceType(type));

		rCollection = new TA_IRS_Bus::Radio::RadioResourceCollection();

		int nSize = resources.size();
		rCollection->length(nSize);

		for ( int i= 0; i<nSize; i++ )
		{
			TA_IRS_Bus::Radio::RadioResource corbResource;
            RadioMFTAgentHelper::convertRadioResourceToIDL(resources[i], corbResource);

			(*rCollection)[i] = corbResource;	
		}
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failure to execute getResourceCollection.");

		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Exception occurred on getResourceCollection");

		throw ex;
	}
	
   
	return rCollection;

    FUNCTION_EXIT;    
}
void RadioMFTAgentCommandServant::assignAndSelectResource( 	const char* session,const TA_IRS_Bus::Radio::RadioResource& resource,CORBA::Short buttonRef)
{
		try{
		assignResource(session,resource,buttonRef);
		}
		catch(const TA_Base_Core::TransactiveException& e)
		{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
		}
		catch(...)
		{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in assignResource");

		throw ex;
		}
		

		try{
			selectResource(session,resource);
		}
		catch(const TA_Base_Core::TransactiveException& e)
		{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
		}
		catch(...)
		{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in selectResource");

		throw ex;
		}
		
}
		
void RadioMFTAgentCommandServant::deassignAndDeselectResource( const char* session,const TA_IRS_Bus::Radio::RadioResource& resource)
{
		try{	
		deassignResource(session,resource);
		}
		catch(const TA_Base_Core::TransactiveException& e)
		{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
		}
		catch(...)
		{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in deassignResource");

		throw ex;
		}
		
		
		try{
			deselectResource(session,resource);
		}
		catch(const TA_Base_Core::TransactiveException& e)
		{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
		}
		catch(...)
		{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in deselectResource");

		throw ex;
		}
		
}
void RadioMFTAgentCommandServant::assignResource(
	const char* session,
	const TA_IRS_Bus::Radio::RadioResource& resource,
	CORBA::Short buttonRef)
{
    FUNCTION_ENTRY("assignResource");

	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

		m_pMFTAgentCommandImpl->assignResource(nResource);
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(...)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in assignResource");

		throw ex;
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentCommandServant::deassignResource(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("deassignResource");

	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

		m_pMFTAgentCommandImpl->deassignResource(nResource);
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failure to execute deAssignResource.");

		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in deAssignResource");

		throw ex;
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentCommandServant::selectResource(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("selectResource");

	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

		m_pMFTAgentCommandImpl->selectResource(nResource);

	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failure to execute selectResource.");

		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in selectResource");

		throw ex;
	}
	

    FUNCTION_EXIT;
}


void RadioMFTAgentCommandServant::deselectResource(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("deselectResource");

	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

		m_pMFTAgentCommandImpl->deselectResource(nResource);
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failure to execute deselectResource.");

		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in deselectResource");

		throw ex;
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentCommandServant::changeVolume (
			const char* session, 
			const TA_IRS_Bus::Radio::RadioResource& resource, 
            TA_IRS_Bus::Radio::ESpeakerState speaker,
			CORBA::UShort volume 
)
{
    FUNCTION_ENTRY("changeVolume");

	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

		m_pMFTAgentCommandImpl->changeVolume(nResource,volume);
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failure to execute changeVolume.");

		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in changeVolume");

		throw ex;
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentCommandServant::connectGroup(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("connectGroup");

	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

		switch(nResource.type)
		{
			case RR_PATCHGROUP:
				m_pMFTAgentCommandImpl->connectPatchGroup(nResource);
				break;
			case RR_MSELECT:
				m_pMFTAgentCommandImpl->connectMselGroup(nResource);
				break;
			case RR_MSITE:
				m_pMFTAgentCommandImpl->connectMselSite(nResource);
				break;
			case RR_SITE:
				m_pMFTAgentCommandImpl->connectSite(nResource);
				break;
			case RR_DYNGROUP:
				m_pMFTAgentCommandImpl->connectDynRegroup(nResource);
		}

	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, e.what());
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failure to execute connectGroup.");

		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in connectGroup");

		throw ex;
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentCommandServant::releaseGroup(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("releaseGroup");

	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

		switch(nResource.type)
		{
			case RR_PATCHGROUP:
				m_pMFTAgentCommandImpl->releasePatchGroup(nResource);
				break;
			case RR_MSELECT:
				m_pMFTAgentCommandImpl->releaseMselGroup(nResource,true);
				break;
			case RR_MSITE:
				m_pMFTAgentCommandImpl->releaseMselSite(nResource);
				break;
			case RR_SITE:
				m_pMFTAgentCommandImpl->releaseSite(nResource);
				break;
			case RR_DYNGROUP:
				m_pMFTAgentCommandImpl->releaseDynRegroup(nResource);
		}
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, e.what());

		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failure to execute releaseGroup.");

		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in releaseGroup");

		throw ex;
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentCommandServant::makeAmbienceListeningCall(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("makeAmbienceListeningCall");

	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

		m_pMFTAgentCommandImpl->makeAmbienceListeningCall(nResource);
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failure to execute makeAmbienceListeningCall.");

		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in makeAmbienceListeningCall");

		throw ex;
	}
	
    FUNCTION_EXIT;
}


void RadioMFTAgentCommandServant::makePrivateCall(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource,CORBA::Boolean bEmergencyMode)
{
    FUNCTION_ENTRY("makePrivateCall");

	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

/* AZ: TODO: determine the emergency state ?
        if (The_StatusManager::instance()->getFeatureAttributeEmergencyState(nResource) != CE_NONE)
		{
			m_pMFTAgentCommandImpl->makeEmergencyCall(nResource);
		}
		else
        */
		{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finding channel for private call");		
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		LogicalChannel channel = The_StatusManager::instance()->getChannel(pc.id);
		if (0 == channel) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "No channel found");
			m_pMFTAgentCommandImpl->assignResource(pc);
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Channel = %u", channel);
			m_pMFTAgentCommandImpl->makePrivateCall(nResource);
		}
		
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(...)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in makePrivateCall");

		throw ex;
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentCommandServant::answerPrivateCall(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("answerPrivateCall");

	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Finding channel for private call");		
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		LogicalChannel channel = The_StatusManager::instance()->getChannel(pc.id);
		if (0 == channel) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No channel found");
			m_pMFTAgentCommandImpl->assignResource(pc);
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Channel = %u", channel);

		m_pMFTAgentCommandImpl->answerPrivateCall(nResource);
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(...)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in answerPrivateCall");

		throw ex;
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentCommandServant::endCall(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource,CORBA::Boolean bEmergencyMode)
{
    FUNCTION_ENTRY("endCall");

	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

/* AZ: TODO: determine the emergency state ?
		if (bEmergencyMode)
		{
			m_pMFTAgentCommandImpl->endEmergencyCall(nResource);
		}
		else
        */
		{

// ++ Huangxin TD14319 
			PrivateCallState ptCallState = The_StatusManager::instance()->getFeatureAttributePtCallState(nResource);

            if (ptCallState != PC_NOT_INVOLVED)
            {
				switch(ptCallState)
				{
				case PC_INCOMING:
					m_pMFTAgentCommandImpl->cancelPrivateCall(nResource,PC_INCOMING);
					break;
				case PC_OUTGOING:
					m_pMFTAgentCommandImpl->cancelPrivateCall(nResource,PC_OUTGOING);
					break;
				case PC_ESTABLISHED:
					m_pMFTAgentCommandImpl->endPrivateCall(nResource);
					break;
				}
			}
			else if (The_StatusManager::instance()->getFeatureAttributeALCallState(nResource) == ALC_ESTABLISHED)
			{
				m_pMFTAgentCommandImpl->endAmbienceListeningCall(nResource);
			}
// TD14319 ++
		}
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());
		throw ex;
	}
	catch(...)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in endCall");
		throw ex;
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentCommandServant::beginEmergency(const TA_IRS_Bus::Radio::RadioResource& resource)
{
	RadioResource nResource;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

	try
	{
    	m_pMFTAgentCommandImpl->makeEmergencyCall(nResource);
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());
		throw ex;
	}
	catch(...)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in beginEmergency");
		throw ex;
	}
}

void RadioMFTAgentCommandServant::acknowledgeEmergency(const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("acknowledgeEmergency");

	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);
		m_pMFTAgentCommandImpl->ackEmergency(nResource);
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());
		throw ex;
	}
	catch(...)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in acknowledgeEmergency");
		throw ex;
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentCommandServant::knockdownEmergency(const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("knockdownEmergency");

	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);
		m_pMFTAgentCommandImpl->endEmergencyCall(nResource);
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());
		throw ex;
	}
	catch(...)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in acknowledgeEmergency");
		throw ex;
	}

    FUNCTION_EXIT;
}

TA_IRS_Bus::Radio::EPrivateCallState RadioMFTAgentCommandServant::isPrivateCallResourceInUse(const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("isPrivateCallResourceInUse");

	RadioResource nResource;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);


    FUNCTION_EXIT;
	return (TA_IRS_Bus::Radio::EPrivateCallState) The_StatusManager::instance()->getFeatureAttributePtCallState(nResource);
}

CORBA::Boolean RadioMFTAgentCommandServant::isResourceActive(const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("isResourceActive");

	RadioResource nResource;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

    FUNCTION_EXIT;
	return The_StatusManager::instance()->getFeatureAttributeIsActive(nResource);
}

TA_IRS_Bus::Radio::EGroupConnectionState RadioMFTAgentCommandServant::isResourceGroupConnected(const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("isResourceGroupConnected");

	RadioResource nResource;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);
	
    FUNCTION_EXIT;
	return (TA_IRS_Bus::Radio::EGroupConnectionState) The_StatusManager::instance()->getFeatureAttributeIsGrpConnected(nResource);
}

CORBA::Boolean RadioMFTAgentCommandServant::isResourceBeingMonitored(const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("isResourceBeingMonitored");

	RadioResource nResource;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

    FUNCTION_EXIT;
	return  The_StatusManager::instance()->getFeatureAttributeIsMonitored(nResource);
}

CORBA::Boolean RadioMFTAgentCommandServant::isResourceInAmbienceListeningCall(const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("isResourceInAmbienceListeningCall");

	RadioResource nResource;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

	bool bResult = false;

	try
	{
		if (The_StatusManager::instance()->getFeatureAttributeALCallState(nResource)==ALC_ESTABLISHED)
		{
			bResult = true;
		}
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failure to execute isResourceInAmbienceListeningCall.");
	}	
    FUNCTION_EXIT;
    return bResult;
}

CORBA::Boolean RadioMFTAgentCommandServant::isResourceSelected(const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("isResourceSelected");

	RadioResource nResource;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

	bool bResult = false;

	try
	{
		switch (nResource.type)
		{
		case RR_PATCHGROUP:
			{
				if (The_StatusManager::instance()->getFeatureAttributeIsGrpConnected(nResource)!=CONNECTED)
					break;

				std::vector<RadioResource> rList = The_StatusManager::instance()->getResourceMembers(nResource);
				int nSize = rList.size();

				for ( int i= 0; i<nSize; i++ )
				{
					// if any one of the member is selected, the patchgroup is selected
					if (The_StatusManager::instance()->getFeatureAttributeIsSelected(rList[i])==SELECTED_SPEAKER)
					{
						bResult = true;
						break;
					}
				}
			}
			break;

		case RR_MSELECT:
		case RR_MSITE:
			{
				if (The_StatusManager::instance()->getFeatureAttributeIsGrpConnected(nResource)!=CONNECTED)
					break;

				std::vector<RadioResource> rList = The_StatusManager::instance()->getResourceMembers(nResource);
				int nSize = rList.size();

				for ( int i= 0; i<nSize; i++ )
				{
					// if any one of the member is not selected, the msel is not selected
					if (The_StatusManager::instance()->getFeatureAttributeIsSelected(rList[i])==SELECTED_SPEAKER)
					{
						bResult = true;
					}
					else
					{
						bResult = false;
						break;
					}
				}
			}
			break;

		case RR_DYNGROUP:
			{
				// TD14133
				bResult = The_StatusManager::instance()->getSelectedDGState(nResource);
			}
		default:
			if (The_StatusManager::instance()->getFeatureAttributeIsSelected(nResource)==SELECTED_SPEAKER)
			{
				bResult = true;
			}
		}
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failure to execute getFeatureAttributeIsSelected.");
	}	

    FUNCTION_EXIT;
    return bResult;
}

CORBA::Boolean RadioMFTAgentCommandServant::isResourceAssigned(const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("isResourceSelected");

	RadioResource nResource;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

	bool bResult = false;

	try
	{
		if (The_StatusManager::instance()->getFeatureAttributeIsAssigned(nResource))
		{
			bResult = true;
		}
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failure to execute getFeatureAttributeIsAssigned.");
	}	
    FUNCTION_EXIT;
    return bResult;
}



TA_IRS_Bus::Radio::EmergencyModeState RadioMFTAgentCommandServant::isResourceInEmergencyMode(const TA_IRS_Bus::Radio::RadioResource& resource)
{
	FUNCTION_ENTRY("isResourceSelected");

    TA_IRS_Bus::Radio::EmergencyModeState ret = TA_IRS_Bus::Radio::RE_NO_EMERGENCY;
	RadioResource nResource;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

	try
	{
        ret = (TA_IRS_Bus::Radio::EmergencyModeState) The_StatusManager::instance()->
            getFeatureAttributeEmergencyMode(nResource);
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failure to execute getFeatureAttributeEmergencyMode.");
	}	

    FUNCTION_EXIT;
    return ret;
}

CORBA::UShort RadioMFTAgentCommandServant::getResourceVolume(const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("getResourceVolume");
	
	CORBA::UShort result = 0;

	RadioResource nResource;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

	result = The_StatusManager::instance()->getFeatureAttributeSpeakerVolume(nResource);

	FUNCTION_EXIT;
	return result;
}

void RadioMFTAgentCommandServant::getResourceMembers(const TA_IRS_Bus::Radio::RadioResource& resource, TA_IRS_Bus::Radio::RadioResourceCollection_out members)
{
    FUNCTION_ENTRY("getResourceMembers");
    
	members = new TA_IRS_Bus::Radio::RadioResourceCollection();

	RadioResource nResource;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

	std::vector<RadioResource> rList = The_StatusManager::instance()->getResourceMembers(nResource);

	int nSize = rList.size();
	members->length(nSize);

	for ( int i= 0; i<nSize; i++ )
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(rList[i], corbaResource);

		(*members)[i] = corbaResource;	
	}

	FUNCTION_EXIT;
}


void RadioMFTAgentCommandServant::getResourceBySSI(CORBA::ULong ssi, TA_IRS_Bus::Radio::RadioResource_out resource)
{
    FUNCTION_ENTRY("getResourceBySSI");
    
	RadioResource res = The_StatusManager::instance()->getResourceBySSI(ssi);

	resource = new TA_IRS_Bus::Radio::RadioResource();
    RadioMFTAgentHelper::convertRadioResourceToIDL(res, *resource);

	FUNCTION_EXIT;
}

void RadioMFTAgentCommandServant::getResourceByID(CORBA::ULong rid, TA_IRS_Bus::Radio::ERadioResourceType type, TA_IRS_Bus::Radio::RadioResource_out resource)
{
    FUNCTION_ENTRY("getResourceByID");
    
	try
	{
        RadioResource res = The_StatusManager::instance()->getResourceByID(rid, RadioMFTAgentHelper::convertResourceType(type));

		resource = new TA_IRS_Bus::Radio::RadioResource();
        RadioMFTAgentHelper::convertRadioResourceToIDL(res, *resource);
	}
	catch(...)
	{
	}
	FUNCTION_EXIT;
}


void RadioMFTAgentCommandServant::assignSelectSpeaker(const TA_IRS_Bus::Radio::RadioResource& resource)
{
    FUNCTION_ENTRY("assignSelectSpeaker");

	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

		m_pMFTAgentCommandImpl->assignSelectSpeaker(nResource);
	}
	catch(const TA_IRS_App::ConsoleDispatchException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(...)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unknown Exception occured in assignSelectSpeaker");

		throw ex;
	}
	
    FUNCTION_EXIT;
}


void RadioMFTAgentCommandServant::createPatchGroup(const TA_IRS_Bus::Radio::RadioResource& resource,CORBA::Boolean bIsDLTSPatch)
{
	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

		m_pMFTAgentCommandImpl->createPatchGroup(nResource,bIsDLTSPatch);
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(...)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unable to create a temporary patch group");

		throw ex;
	}
}

void RadioMFTAgentCommandServant::releasePatchGroup(CORBA::Boolean bIsDLTSPatch)
{
	try
	{
		RadioResource pg = The_StatusManager::instance()->getTempPatchGroup();

		if (pg.id != 0)
		{
			m_pMFTAgentCommandImpl->releasePatchGroup(pg);

			The_StatusManager::instance()->delResource(pg);

			RadioResource dummy;
			The_StatusManager::instance()->setTempPatchGroup(dummy);
		}
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(e.what());

		throw ex;
	}
	catch(...)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unable to release Patch Group");

		throw ex;
	}
}
TA_IRS_Bus::Radio::RadioResource* RadioMFTAgentCommandServant::getDGTalkgroup(const char* session, const TA_IRS_Bus::Radio::RadioResource& resource)
{
	TA_IRS_Bus::Radio::RadioResource* cDGTalkgroup = NULL;;
	try
	{
		RadioResource nResource;
        RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

		RadioResource dgTalkgroup = The_StatusManager::instance()->getDynamicGroupForResource(nResource);

		cDGTalkgroup = new TA_IRS_Bus::Radio::RadioResource;
		RadioMFTAgentHelper::convertRadioResourceToIDL(dgTalkgroup, *cDGTalkgroup);
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(ex.reason);

		throw ex;
	}
	catch(...)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unable to connect Dynamic ReGroup");

		throw ex;
	}

	return cDGTalkgroup;
}
/*
void RadioMFTAgentCommandServant::makePhoneCall(const char* number, CORBA::Short length)
{
	try
	{
		m_pMFTAgentCommandImpl->makePhoneCall(number);
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(ex.reason);

		throw ex;
	}
	catch(...)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unable to connect Dynamic ReGroup");

		throw ex;
	}
}


void RadioMFTAgentCommandServant::endPhoneCall(const char* epaxNumber)
{
	try
	{
		std::string num = epaxNumber;
		m_pMFTAgentCommandImpl->endPhoneCall(num);
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup(ex.reason);

		throw ex;
	}
	catch(...)
	{
		TA_IRS_Bus::RadioCommonExceptionsIDL::CommandFailureException ex;
		ex.reason = CORBA::string_dup("Unable to connect Dynamic ReGroup");

		throw ex;
	}
}
*/
//TD18785
void RadioMFTAgentCommandServant::addActiveResource(const TA_IRS_Bus::Radio::RadioResource& resource)
{
	RadioResource nResource;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

	m_ActiveResources[nResource.id] = nResource;
	
}
		
void RadioMFTAgentCommandServant::deleteActiveResource(unsigned long rid)
{
	std::map<unsigned long, RadioResource>::iterator iT = m_ActiveResources.find(rid);

	if (iT != m_ActiveResources.end())
	{
		m_ActiveResources.erase(iT);
	}

}

void RadioMFTAgentCommandServant::addSelectSpeakerResource(const TA_IRS_Bus::Radio::RadioResource& resource)
{
	RadioResource nResource;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nResource);

	m_SelectedSpeakerResources[nResource.id] = nResource;
}

void RadioMFTAgentCommandServant::deleteSelectSpeakerResource(unsigned long rid)
{
	std::map<unsigned long, RadioResource>::iterator iT = m_SelectedSpeakerResources.find(rid);

	if (iT != m_SelectedSpeakerResources.end())
	{
		m_SelectedSpeakerResources.erase(iT);
	}
}

void RadioMFTAgentCommandServant::terminateCurrentSessions()
{
	std::map<unsigned long, RadioResource>::iterator iTS = m_SelectedSpeakerResources.begin();

	while(iTS != m_SelectedSpeakerResources.end())
	{
		try
		{
			m_pMFTAgentCommandImpl->deselectResource(iTS->second);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				"Error releasing speaker of resource %i", iTS->second.id);
		}
		iTS++;
	}

	std::map<unsigned long, RadioResource>::iterator iTA = m_ActiveResources.begin();
	
	while(iTA != m_ActiveResources.end())
	{
		try
		{
			switch(iTA->second.type)
			{
			case RR_RADIO:
				{
					PrivateCallState ptCallState = The_StatusManager::instance()->getFeatureAttributePtCallState(iTA->second);

					if (ptCallState != PC_NOT_INVOLVED)
					{
						switch(ptCallState)
						{
						case PC_INCOMING:
							m_pMFTAgentCommandImpl->cancelPrivateCall(iTA->second,PC_INCOMING);
							break;
						case PC_OUTGOING:
							m_pMFTAgentCommandImpl->cancelPrivateCall(iTA->second,PC_OUTGOING);
							break;
						case PC_ESTABLISHED:
							m_pMFTAgentCommandImpl->endPrivateCall(iTA->second);
							break;
						}
					}
					else if (The_StatusManager::instance()->getFeatureAttributeALCallState(iTA->second) == ALC_ESTABLISHED)
					{
						m_pMFTAgentCommandImpl->endAmbienceListeningCall(iTA->second);
					}
				}
				break;
			case RR_PATCHGROUP:
				m_pMFTAgentCommandImpl->releasePatchGroup(iTA->second);
				break;
			case RR_MSELECT:
				m_pMFTAgentCommandImpl->releaseMselGroup(iTA->second,true);
				break;
			case RR_MSITE:
				m_pMFTAgentCommandImpl->releaseMselSite(iTA->second);
				break;
			case RR_SITE:
				m_pMFTAgentCommandImpl->releaseSite(iTA->second);
				break;
			case RR_DYNGROUP:
				m_pMFTAgentCommandImpl->releaseDynRegroup(iTA->second);
				break;
			default:
				break;
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				"Error releasing =resource %i", iTA->second.id);
		}
		iTA++;
	}	

}


//TD18785

void RadioMFTAgentCommandServant::addRadioResource(const ::TA_IRS_Bus::Radio::RadioResource& resource, 
												   const ::TA_IRS_Bus::Radio::RadioResourceCollection& members, 
												   ::CORBA::ULong memberSize, ::CORBA::ULong id
												   )
{
	FUNCTION_ENTRY("addRadioResource");
	RadioResource nresource;
	std::vector<RadioResource> nmembers;
	RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, nresource);
	for (int i = 0; i < memberSize; i++) {
		RadioResource temp;
		RadioMFTAgentHelper::convertRadioResourceFromIDL(members[i], temp);
		nmembers.push_back(temp);
	}
	The_StatusManager::instance()->addResource(nresource, nmembers, id);
	FUNCTION_EXIT;
}