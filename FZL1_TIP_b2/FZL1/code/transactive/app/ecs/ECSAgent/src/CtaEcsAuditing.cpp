/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ECSAgent/src/CtaEcsAuditing.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include <iostream>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/types/ECSAgentAudit_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/exceptions/src/ECSAgentExceptions.h"
#include "bus/ecs/ecs_agent/IDL/src/CommonEcsAgentCorbaDef.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/CtaEcsAuditing.h"
#include "app/ecs/ECSAgent/src/EcsHelpers.h"


CtaEcsAuditing::CtaEcsAuditing(ItaEcsCachedConfig& cachedConfigInstance)
: m_auditSender(NULL),
  m_cachedConfigInstance(cachedConfigInstance)
{
	FUNCTION_ENTRY("CtaEcsAuditing");
    if (!m_auditSender)
    {
        m_auditSender = 
            TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::ECSAgentAudit::Context );
    }
    FUNCTION_EXIT;

}

CtaEcsAuditing::~CtaEcsAuditing()
{
	FUNCTION_ENTRY("~CtaEcsAuditing");
    if (m_auditSender)
    {
        delete m_auditSender;
        m_auditSender = NULL;
    }
    FUNCTION_EXIT;
}


void CtaEcsAuditing::sendEcsAgentOnlineUpdate(const std::string& parameter)
{
 	FUNCTION_ENTRY("sendEcsAgentOnlineUpdate");
    TA_ASSERT(m_auditSender, "m_auditSender is NULL");

  	//TA_Base_Core::ThreadGuard guard( m_lock );


    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }

        // construct the DescriptionParameters list
	    TA_Base_Core::DescriptionParameters descriptionParameters;
	    TA_Base_Core::NameValuePair pair1 ( "Parameter", parameter );
	    descriptionParameters.push_back ( &pair1 );

        m_auditSender->sendAuditMessage(
            TA_Base_Core::ECSAgentAudit::EcsAgentOnlineUpdate,            // Message Type
            m_cachedConfigInstance.getAgentKey(),     // Entity key
            descriptionParameters,                          // Description
            "",                                             // Additional details
            "",                                             // SessionID if applicable
            "",                                             // AlarmID if an alarm associated with event
            "",                                             // IncidentKey if incident associated with event
            "");                                            // EventID of a parent event, used to link events

    } // try block
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to submit the EcsAgentOnlineUpdate audit message");
    }

    FUNCTION_EXIT;
}


void CtaEcsAuditing::sendEcsStationModeRequest(unsigned long fromMode,
                                                unsigned long toMode,
                                                const std::string& sessionId)
{
 	FUNCTION_ENTRY("sendEcsStationModeRequest");
    TA_ASSERT(m_auditSender, "m_auditSender is NULL");

  	//TA_Base_Core::ThreadGuard guard( m_lock );


    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }

        // construct the DescriptionParameters list
	    TA_Base_Core::DescriptionParameters descriptionParameters;
        TA_Base_Core::NameValuePair pair1 ( "FromMode", EcsHelpers::toString(fromMode) );
        TA_Base_Core::NameValuePair pair2 ( "ToMode", EcsHelpers::toString(toMode) );
	    descriptionParameters.push_back ( &pair1 );
        descriptionParameters.push_back ( &pair2 );

        m_auditSender->sendAuditMessage(
            TA_Base_Core::ECSAgentAudit::EcsStationModeRequest,    // Message Type
            m_cachedConfigInstance.getAgentKey(),     // Entity key
            descriptionParameters,                          // Description
            "",                                             // Additional details
            sessionId,                                      // SessionID if applicable
            "",                                             // AlarmID if an alarm associated with event
            "",                                             // IncidentKey if incident associated with event
            "");                                            // EventID of a parent event, used to link events

    } // try block
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to submit the EcsStationModeRequest audit message");
    }

    FUNCTION_EXIT;
}


void CtaEcsAuditing::sendEcsStationModeResult(unsigned long fromMode,
                                               unsigned long toMode,
                                               const std::string& failuresIfAny)
{
 	FUNCTION_ENTRY("sendEcsStationModeResult");
    TA_ASSERT(m_auditSender, "m_auditSender is NULL");

  	//TA_Base_Core::ThreadGuard guard( m_lock );


    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }

        // construct the DescriptionParameters list
	    TA_Base_Core::DescriptionParameters descriptionParameters;
        TA_Base_Core::NameValuePair pair1 ( "FromMode", EcsHelpers::toString(fromMode) );
        TA_Base_Core::NameValuePair pair2 ( "ToMode", EcsHelpers::toString(toMode) );
        TA_Base_Core::NameValuePair pair3 ( "FailuresIfAny", failuresIfAny);
	    descriptionParameters.push_back ( &pair1 );
        descriptionParameters.push_back ( &pair2 );
        descriptionParameters.push_back ( &pair3 );

        m_auditSender->sendAuditMessage(
            TA_Base_Core::ECSAgentAudit::EcsStationModeResult,    // Message Type
            m_cachedConfigInstance.getAgentKey(),     // Entity key
            descriptionParameters,                          // Description
            "",                                             // Additional details
            "",                                             // SessionID if applicable
            "",                                             // AlarmID if an alarm associated with event
            "",                                             // IncidentKey if incident associated with event
            "");                                            // EventID of a parent event, used to link events

    } // try block
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to submit the EcsStationModeResult audit message");
    }

    FUNCTION_EXIT;
}


void CtaEcsAuditing::sendEcsMasterModeRequest(unsigned long fromMode,
                                               unsigned long toMode,
                                               const std::string& sessionId)
{
 	FUNCTION_ENTRY("sendEcsMasterModeRequest");
    TA_ASSERT(m_auditSender, "m_auditSender is NULL");

  	//TA_Base_Core::ThreadGuard guard( m_lock );


    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }

        // construct the DescriptionParameters list
	    TA_Base_Core::DescriptionParameters descriptionParameters;
        TA_Base_Core::NameValuePair pair1 ( "FromMode", EcsHelpers::toString(fromMode) );
        TA_Base_Core::NameValuePair pair2 ( "ToMode", EcsHelpers::toString(toMode) );
	    descriptionParameters.push_back ( &pair1 );
        descriptionParameters.push_back ( &pair2 );

        m_auditSender->sendAuditMessage(
            TA_Base_Core::ECSAgentAudit::EcsMasterModeRequest,    // Message Type
            m_cachedConfigInstance.getAgentKey(),     // Entity key
            descriptionParameters,                          // Description
            "",                                             // Additional details
            sessionId,                                      // SessionID if applicable
            "",                                             // AlarmID if an alarm associated with event
            "",                                             // IncidentKey if incident associated with event
            "");                                            // EventID of a parent event, used to link events

    } // try block
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to submit the EcsMasterModeRequest audit message");
    }

    FUNCTION_EXIT;
}


void CtaEcsAuditing::sendEcsMasterModeResult(unsigned long fromMode,
                                              unsigned long toMode,
                                              const std::string& failuresIfAny,
											  const std::string& sessionId)	// CL17278++ - Sheet 3
{
 	FUNCTION_ENTRY("sendEcsMasterModeResult");
    TA_ASSERT(m_auditSender, "m_auditSender is NULL");

  	//TA_Base_Core::ThreadGuard guard( m_lock );


    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }

        // construct the DescriptionParameters list
	    TA_Base_Core::DescriptionParameters descriptionParameters;
        TA_Base_Core::NameValuePair pair1 ( "FromMode", EcsHelpers::toString(fromMode) );
        TA_Base_Core::NameValuePair pair2 ( "ToMode", EcsHelpers::toString(toMode) );
        TA_Base_Core::NameValuePair pair3 ( "FailuresIfAny", failuresIfAny);
	    descriptionParameters.push_back ( &pair1 );
        descriptionParameters.push_back ( &pair2 );
        descriptionParameters.push_back ( &pair3 );

        m_auditSender->sendAuditMessage(
            TA_Base_Core::ECSAgentAudit::EcsMasterModeResult,    // Message Type
            m_cachedConfigInstance.getAgentKey(),     // Entity key
            descriptionParameters,                          // Description
            "",                                             // Additional details
            sessionId,										// SessionID if applicable - CL17278++, Sheet3: indicate the operator
            "",                                             // AlarmID if an alarm associated with event
            "",                                             // IncidentKey if incident associated with event
            "");                                            // EventID of a parent event, used to link events

    } // try block
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to submit the EcsMasterModeResult audit message");
    }

    FUNCTION_EXIT;
}


void CtaEcsAuditing::sendEcsAgentExecutionState(TA_IRS_Bus::EEcsExecutionState state,
                                                 unsigned long fromMode,
                                                 unsigned long toMode)
                                                
{
 	FUNCTION_ENTRY("sendEcsAgentExecutionState");
    TA_ASSERT(m_auditSender, "m_auditSender is NULL");

  	//TA_Base_Core::ThreadGuard guard( m_lock );


    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }

        // construct the DescriptionParameters list
	    TA_Base_Core::DescriptionParameters descriptionParameters;
	    TA_Base_Core::NameValuePair pair1 ( "State", EcsHelpers::toString(state) );
        TA_Base_Core::NameValuePair pair2 ( "FromMode", EcsHelpers::toString(fromMode) );
        TA_Base_Core::NameValuePair pair3 ( "ToMode", EcsHelpers::toString(toMode) );
	    descriptionParameters.push_back ( &pair1 );
        descriptionParameters.push_back ( &pair2 );
        descriptionParameters.push_back ( &pair3 );

        m_auditSender->sendAuditMessage(
            TA_Base_Core::ECSAgentAudit::EcsAgentExecutionState,    // Message Type
            m_cachedConfigInstance.getAgentKey(),     // Entity key
            descriptionParameters,                          // Description
            "",                                             // Additional details
            "",                                             // SessionID if applicable
            "",                                             // AlarmID if an alarm associated with event
            "",                                             // IncidentKey if incident associated with event
            "");                                            // EventID of a parent event, used to link events

    } // try block
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to submit the EcsAgentExecutionState audit message");
    }

    FUNCTION_EXIT;
}

