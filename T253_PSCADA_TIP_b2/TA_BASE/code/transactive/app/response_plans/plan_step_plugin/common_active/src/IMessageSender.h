/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_step_plugin/common_active/src/IMessageSender.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2014/09/10 15:13:41 $
  *  Last modified by:  $Author: huangjian $
  *
  *  <description>
  */
#ifndef IMESSAGESENDER_H
#define IMESSAGESENDER_H

#include <string>
#include <vector>

namespace TA_Base_Core
{
    class MessageType;
    struct NameValuePair;
}

namespace TA_Base_App
{

class IMessageSender
{
public:
    virtual void sendAuditMessage(const std::string& session, const TA_Base_Core::MessageType& auditMessageType, const std::vector<TA_Base_Core::NameValuePair*>& description) = 0;
};

}

#endif // IMESSAGESENDER_H