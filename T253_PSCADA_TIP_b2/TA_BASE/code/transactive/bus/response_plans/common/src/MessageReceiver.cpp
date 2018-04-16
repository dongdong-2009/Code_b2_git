/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/bus/response_plans/common/src/MessageReceiver.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2015/02/06 18:35:32 $
  * Last modified by:  $Author: qi.huang $
  *
  **/

#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/response_plans/common/src/IAgency.h"
#include "bus/response_plans/common/src/MessageReceiver.h"

namespace TA_Base_Bus
{

/////////////////////////////////////////////////////////////////////////////
// MessageReceiver

MessageReceiver::MessageReceiver( IAgency& refCallback ) :
m_refAgency( refCallback )
{
    FUNCTION_ENTRY( "MessageReceiver" );

    FUNCTION_EXIT;
}

MessageReceiver::~MessageReceiver()
{
    FUNCTION_ENTRY( "~MessageReceiver" );

    FUNCTION_EXIT;
}

void MessageReceiver::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY( "receiveSpecialisedMessage" );

    // Try extracting to RpcRequest
    if ( 0 == TA_Base_Core::PlanAgentComms::RpcRequest.getTypeKey().compare( message.messageTypeKey.in() ))
    {
        TA_Base_Core::RpcRequest* detail = NULL;
        if ( message.messageState >>= detail )
        {
            std::vector<std::string> vecParameters;

            for ( size_t szLoop = 0u; szLoop < detail->parameters.length(); ++szLoop )
            {
                vecParameters.push_back( detail->parameters[szLoop].in() );
            }

            m_refAgency.handleReq( detail->location, detail->id, detail->type, detail->requestor, detail->plan, detail->instance, detail->step, detail->handler, vecParameters );
        }
    }
    else if ( 0 == TA_Base_Core::PlanAgentComms::RpcAcknowledge.getTypeKey().compare( message.messageTypeKey.in() ))
    {
        TA_Base_Core::RpcAck* detail = NULL;
        if ( message.messageState >>= detail )
        {
            m_refAgency.handleAck( detail->location, detail->id );
        }
    }

    FUNCTION_EXIT;
}

void MessageReceiver::subscribeReq()
{
    // Subscribe for active step updates
	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage( TA_Base_Core::PlanAgentComms::RpcRequest, this );
}

void MessageReceiver::subscribeAck()
{
    // Subscribe for active step updates
	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage( TA_Base_Core::PlanAgentComms::RpcAcknowledge, this );
}

void MessageReceiver::unsubscribe()
{
    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );
}

}
