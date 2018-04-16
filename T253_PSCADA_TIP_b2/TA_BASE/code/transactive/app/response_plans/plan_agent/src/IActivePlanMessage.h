#ifndef IACTIVEPLANMESSAGE_H_INCLUDED
#define IACTIVEPLANMESSAGE_H_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/IActivePlanMessage.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #6 $
  *
  *  Last modification: $DateTime: 2014/12/16 15:03:52 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

namespace TA_Base_App
{

class ActivePlan;

//Class declaration
class IActivePlanMessage
{
// Public methods
public:
    virtual ~IActivePlanMessage() {};
	virtual void execute( ActivePlan& activePlan ) const = 0;
}; // IActivePlanMessage

}; // TA_Base_App

#endif // IACTIVEPLANMESSAGE_H_INCLUDED
