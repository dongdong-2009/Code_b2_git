/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/CtaEcsSynchronisation.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/09/25 14:02:39 $
  * Last modified by:  $Author: huangjian $
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include <iostream>
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/types/ECSAgentStateUpdate_MessageTypes.h"
#include "core/message/types/ECSAgentStateUpdateRequest_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/exceptions/src/ECSAgentExceptions.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/EcsHelpers.h"
#include "app/ecs/ECSAgent/src/CtaEcsSynchronisation.h"


CtaEcsSynchronisation::CtaEcsSynchronisation(ItaEcsCachedConfig& ecsCachedConfigInstance)
: m_stateUpdateSender(NULL),
  m_stateUpdateRequestSender(NULL),
  m_ecsCachedConfigInstance(ecsCachedConfigInstance)
{
	FUNCTION_ENTRY("CtaEcsSynchronisation");
    m_stateUpdateSender = 
        TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender( TA_Base_Core::ECSAgentStateUpdate::Context );
    m_stateUpdateRequestSender = 
        TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender( TA_Base_Core::ECSAgentStateUpdateRequest::Context );
	FUNCTION_EXIT;
}

CtaEcsSynchronisation::~CtaEcsSynchronisation()
{
	FUNCTION_ENTRY("~CtaEcsSynchronisation");
	//TA_Base_Core::ThreadGuard guard( m_lock );

    if (m_stateUpdateSender)
    {
        delete m_stateUpdateSender;
        m_stateUpdateSender = NULL;
    }
    if (m_stateUpdateRequestSender)
    {
        delete m_stateUpdateRequestSender;
        m_stateUpdateRequestSender = NULL;
    }
	FUNCTION_EXIT;
}

void CtaEcsSynchronisation::sendEcsAgentStateUpdateRequest()
{
	FUNCTION_ENTRY("sendEcsAgentStateUpdateRequest");
	//TA_Base_Core::ThreadGuard guard( m_lock );

    CORBA::Any data;

    m_stateUpdateRequestSender->sendStateUpdateMessage(
        TA_Base_Core::ECSAgentStateUpdateRequest::EcsAgentStateUpdateRequest,
		m_ecsCachedConfigInstance.getAgentKey(),  // Not used
        m_ecsCachedConfigInstance.getAgentName(), 
        data );       
	FUNCTION_EXIT;
}

void CtaEcsSynchronisation::sendEcsAgentParameterUpdate( 
    TA_IRS_Bus::EEcsAgentParameter parameter, 
    const CORBA::Any& parameterData,
    unsigned long entityKey)
{
	FUNCTION_ENTRY("sendEcsAgentParameterUpdate");

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
        "Sending sendEcsAgentParameterUpdate message to subscribers");

	//TA_Base_Core::ThreadGuard guard( m_lock );

    TA_IRS_Bus::EcsAgentParameter* disposableUpdateData = 
        new TA_IRS_Bus::EcsAgentParameter();

    disposableUpdateData->parameter     = parameter;
    disposableUpdateData->data          = parameterData;

    CORBA::Any packetData;

    packetData <<= disposableUpdateData;

    m_stateUpdateSender->sendStateUpdateMessage(
		TA_Base_Core::ECSAgentStateUpdate::EcsAgentParameterUpdate,
		entityKey,                                      // Used only for datapoint updates
        m_ecsCachedConfigInstance.getAgentName(),
		packetData );

    disposableUpdateData = NULL;
	FUNCTION_EXIT;
}
