/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/CtaEcsComms.cpp $
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
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/types/ECSAgentComms_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/exceptions/src/ECSAgentExceptions.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/CtaEcsComms.h"


CtaEcsComms::CtaEcsComms(ItaEcsCachedConfig& cachedConfigInstance)
: m_commsSender(NULL),
  m_cachedConfigInstance(cachedConfigInstance)
{
	FUNCTION_ENTRY("CtaEcsComms");
    if (m_commsSender == NULL)
    {
        m_commsSender = 
            TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::ECSAgentComms::Context );
    }
    FUNCTION_EXIT;

}

CtaEcsComms::~CtaEcsComms()
{
	FUNCTION_ENTRY("~CtaEcsComms");
    if (m_commsSender != NULL)
    {
        delete m_commsSender;
        m_commsSender = NULL;
    }
    FUNCTION_EXIT;

}

void CtaEcsComms::sendEcsStationModeExecutionUpdate(
    TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData* disposableUpdateData,
    const std::string& sessionIdFilter)
{
 	FUNCTION_ENTRY("sendEcsStationModeExecutionUpdate");

  	//TA_Base_Core::ThreadGuard guard( m_lock );

    CORBA::Any data;
    TA_Base_Core::NameValuePair nvp( TA_IRS_Bus::ECS_SESSION_ID_FILTER_STRING,
                       sessionIdFilter );

    std::vector<TA_Base_Core::NameValuePair*> extraFilterableData;
    extraFilterableData.push_back(&nvp);

    data <<= disposableUpdateData;

    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }

        // Send Comms message for PA Managers
        if (m_commsSender != NULL)
        {
            m_commsSender->sendCommsMessage(
                TA_Base_Core::ECSAgentComms::EcsStationModeExecutionUpdate,           // Context::Type
                m_cachedConfigInstance.getAgentKey(),                  // EntityKey
                data,                                                   // Data
                m_cachedConfigInstance.getAgentSubsystemKey(),         // Subsystem
		        m_cachedConfigInstance.getAgentLocationKey(),          // LocationKey
                &extraFilterableData);                                  // SessionId
        }
    }
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to send the EcsStationModeExecutionUpdate data");
    }
    
    FUNCTION_EXIT;
}


void CtaEcsComms::sendEcsMasterModeExecutionUpdate(
    TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData* disposableUpdateData,
    const std::string& sessionIdFilter)
{
 	FUNCTION_ENTRY("sendEcsMasterModeExecutionUpdate");

  	//TA_Base_Core::ThreadGuard guard( m_lock );

    CORBA::Any data;
    TA_Base_Core::NameValuePair nvp( TA_IRS_Bus::ECS_SESSION_ID_FILTER_STRING,
                       sessionIdFilter );

    std::vector<TA_Base_Core::NameValuePair*> extraFilterableData;
    extraFilterableData.push_back(&nvp);

    data <<= disposableUpdateData;

    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }

        // Send Comms message for PA Managers
        if (m_commsSender != NULL)
        {
            m_commsSender->sendCommsMessage(
                TA_Base_Core::ECSAgentComms::EcsMasterModeExecutionUpdate,           // Context::Type
                m_cachedConfigInstance.getAgentKey(),                  // EntityKey
                data,                                                   // Data
                m_cachedConfigInstance.getAgentSubsystemKey(),         // Subsystem
		        m_cachedConfigInstance.getAgentLocationKey(),          // LocationKey
                &extraFilterableData);                                  // SessionId
        }
    }
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to send the EcsMasterModeExecutionUpdate data");
    }
    
    FUNCTION_EXIT;
}

void CtaEcsComms::sendEcsProposedModesUpdate(
    TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData* disposableUpdateData)
{
 	FUNCTION_ENTRY("sendEcsProposedModesUpdate");

  	//TA_Base_Core::ThreadGuard guard( m_lock );

    CORBA::Any data;
    data <<= disposableUpdateData;

    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }

        // Send Comms message for PA Managers
        if (m_commsSender != NULL)
        {
            m_commsSender->sendCommsMessage(
                TA_Base_Core::ECSAgentComms::EcsProposedModesUpdate,                  // Context::Type
                m_cachedConfigInstance.getAgentKey(),                  // EntityKey
                data,                                                   // Data
                m_cachedConfigInstance.getAgentSubsystemKey(),         // Subsystem
		        m_cachedConfigInstance.getAgentLocationKey(),          // LocationKey
                0);                                                     // No extra filters
        }
    }
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to send the EcsProposedModesUpdate data");
    }
    
    FUNCTION_EXIT;
}
