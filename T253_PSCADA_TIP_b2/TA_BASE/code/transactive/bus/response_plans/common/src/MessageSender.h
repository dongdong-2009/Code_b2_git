/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/bus/response_plans/common/src/MessageSender.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/10/30 19:47:26 $
  * Last modified by:  $Author: qi.huang $
  *
  **/

#ifndef MESSAGESENDER_H__INCLUDED_
#define MESSAGESENDER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

namespace TA_Base_Core
{
    class CommsMessageSender;
}

namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // MessageSender

    class MessageSender
    {
    // Operations
    public:
        MessageSender();
        ~MessageSender();

        bool sendReq( const unsigned int unId, const unsigned int ulToLocation, const int eType, 
            const unsigned int ulPlanLocation, const unsigned int ulPlanId, const unsigned int ulInstanceNum, 
            const unsigned int ulStepNum, const unsigned int ulHandler, const std::vector<std::string>& vecParameters );

        bool sendAck( const unsigned int unId, const unsigned int ulToLocation );

    private:
        MessageSender(const MessageSender& other);
        MessageSender& operator=(const MessageSender& other);

    // Attributes
    private:
        TA_Base_Core::CommsMessageSender* m_commsSender;
    };
}

#endif // !defined(MESSAGESENDER_H__INCLUDED_)
