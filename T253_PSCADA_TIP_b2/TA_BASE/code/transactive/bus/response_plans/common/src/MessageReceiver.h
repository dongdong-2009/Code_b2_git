/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/bus/response_plans/common/src/MessageReceiver.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/10/30 19:47:26 $
  * Last modified by:  $Author: qi.huang $
  *
  **/

#ifndef MESSAGERECEIVER_H__INCLUDED_
#define MESSAGERECEIVER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <set>

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

namespace TA_Base_Bus
{
    class IAgency;
    /////////////////////////////////////////////////////////////////////////////
    // MessageReceiver

    class MessageReceiver : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {
    // Operations
    public:
        MessageReceiver( IAgency& refCallback );
        ~MessageReceiver();

        void subscribeReq();
        void subscribeAck();
        void unsubscribe();

    protected:
        void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message );

    private:
        MessageReceiver(const MessageReceiver& other);
        MessageReceiver& operator=(const MessageReceiver& other);

    // Attributes
    private:
        IAgency& m_refAgency;
    };
}

#endif // !defined(MESSAGERECEIVER_H__INCLUDED_)
