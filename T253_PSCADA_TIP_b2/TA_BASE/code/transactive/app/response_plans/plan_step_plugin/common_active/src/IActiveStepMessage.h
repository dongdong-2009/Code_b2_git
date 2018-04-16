#ifndef IACTIVESTEPMESSAGE_H_INCLUDED
#define IACTIVESTEPMESSAGE_H_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/IActivePlanMessage.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2014/05/19 16:28:29 $
  *  Last modified by:  $Author: huangjian $
  *
  *  <description>
  */

namespace TA_Base_App
{

class IActivePlan;
class IMessageSender;

//Class declaration
class IActiveStepMessage
{
// Public methods
public:
    virtual ~IActiveStepMessage() {};
    virtual void execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const = 0;
}; // IActivePlanMessage

}; // TA_Base_App

#endif // IACTIVESTEPMESSAGE_H_INCLUDED
