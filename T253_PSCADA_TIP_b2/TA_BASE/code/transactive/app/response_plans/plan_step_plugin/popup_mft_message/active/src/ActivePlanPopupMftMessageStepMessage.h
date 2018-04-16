#ifndef ACTIVE_PLAN_PUPOP_MFT_MESSAGE_STEP_HEADER_INCLUDED
#define ACTIVE_PLAN_PUPOP_MFT_MESSAGE_STEP_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanPopupMftMessageStepMessage.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2014/10/24 16:32:43 $
  *  Last modified by:  $Author: huang.wenbo $
  *
  *  <description>
  */

#include "app/response_plans/plan_step_plugin/common_active/src/IActiveStepMessage.h"

namespace TA_Base_App
{

class ActivePlanPopupMftMessageStepMessage : public IActiveStepMessage
{
//public methods
public:
    ActivePlanPopupMftMessageStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData );
    virtual void execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const;
private:
    ActivePlanPopupMftMessageStepMessage& operator=( const ActivePlanPopupMftMessageStepMessage& );
    ActivePlanPopupMftMessageStepMessage( const ActivePlanPopupMftMessageStepMessage& );
	unsigned long m_ulStepNum;
	std::vector<std::string> m_vecData;
};

}

#endif