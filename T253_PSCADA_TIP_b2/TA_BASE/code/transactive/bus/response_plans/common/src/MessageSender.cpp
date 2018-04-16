/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/bus/response_plans/common/src/MessageSender.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/02/06 18:35:32 $
  * Last modified by:  $Author: qi.huang $
  *
  **/

#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/response_plans/common/src/MessageSender.h"

namespace TA_Base_Bus
{
    
/////////////////////////////////////////////////////////////////////////////
// MessageSender

MessageSender::MessageSender() : 
m_commsSender( NULL )
{
    FUNCTION_ENTRY( "MessageSender" );

    m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::PlanAgentComms::Context );

    FUNCTION_EXIT;
}

MessageSender::~MessageSender()
{
    FUNCTION_ENTRY( "~MessageSender" );

    if ( NULL != m_commsSender )
    {
        delete m_commsSender;
        m_commsSender = NULL;
    }

    FUNCTION_EXIT;
}

bool MessageSender::sendReq( const unsigned int unId, const unsigned int ulToLocation, const int eType, 
    const unsigned int ulPlanLocation, const unsigned int ulPlanId, const unsigned int ulInstanceNum, 
    const unsigned int ulStepNum, const unsigned int ulHandler, const std::vector<std::string>& vecParameters )
{
    if ( NULL == m_commsSender )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "CommsSender is NULL, fail to send message" );

        return false;
    }

    TA_Base_Core::RpcRequest detail;

    detail.id = unId;
    detail.handler = ulHandler;
    detail.requestor = ulToLocation;
    detail.location = ulPlanLocation;
    detail.plan = ulPlanId;
    detail.instance = ulInstanceNum;
    detail.step = ulStepNum;
    detail.type = eType;
    detail.parameters.length( vecParameters.size() );

    int nParamLoop = 0;
    for ( std::vector<std::string>::const_iterator itLoop = vecParameters.begin(); vecParameters.end() != itLoop; ++itLoop )
    {
        detail.parameters[nParamLoop++] = itLoop->c_str();
    }

    CORBA::Any commsData;
    commsData <<= detail;

    m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::PlanAgentComms::RpcRequest, TA_Base_Core::ANY_ENTITY, commsData );

    return true;
}

bool MessageSender::sendAck( const unsigned int unId, const unsigned int ulToLocation )
{
    if ( NULL == m_commsSender )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "CommsSender is NULL, fail to send message" );

        return false;
    }

    TA_Base_Core::RpcAck detail;
    detail.id = unId;
    detail.location = ulToLocation;

    CORBA::Any commsData;
    commsData <<= detail;

    m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::PlanAgentComms::RpcAcknowledge, TA_Base_Core::ANY_ENTITY, commsData );

    return true;
}

}
