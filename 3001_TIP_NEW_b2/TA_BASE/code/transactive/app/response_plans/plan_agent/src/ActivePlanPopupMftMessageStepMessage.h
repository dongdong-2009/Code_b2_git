#if defined ( STEPTYPE_POPUP_MFT_MESSAGE )

#ifndef ACTIVE_PLAN_PUPOP_MFT_MESSAGE_STEP_HEADER_INCLUDED
#define ACTIVE_PLAN_PUPOP_MFT_MESSAGE_STEP_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanPopupMftMessageStepMessage.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2013/01/31 20:24:40 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include "bus/response_plans/plan_agent/src/SortableActiveStepId.h"
#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{

class ActivePlanPopupMftMessageStepMessage : public TA_Base_App::IActivePlanMessage
{
//public methods
public:
    ActivePlanPopupMftMessageStepMessage(const TA_Base_Core::CurrentPlanStepDetail& step, const int status);
    virtual void execute( ActivePlan& activePlan ) const;

private:
    ActivePlanPopupMftMessageStepMessage& operator=( const ActivePlanPopupMftMessageStepMessage& );
    ActivePlanPopupMftMessageStepMessage( const ActivePlanPopupMftMessageStepMessage& );

    const int m_status;
    TA_Base_Bus::SortableActiveStepId m_srtStepId;
};

}

#endif

#endif // STEPTYPE_POPUP_MFT_MESSAGE
