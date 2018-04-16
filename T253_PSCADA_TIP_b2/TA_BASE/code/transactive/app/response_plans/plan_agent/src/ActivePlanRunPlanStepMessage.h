#ifndef ACTIVE_PLAN_RUN_PLAN_STEP_HEADER_INCLUDED
#define ACTIVE_PLAN_RUN_PLAN_STEP_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanRunPlanStepMessage.h $
  *  @author:  Bart Golab
  *  @version: $Revision: #9 $
  *
  *  Last modification: $DateTime: 2015/03/19 14:48:01 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Generated when the result of a Run Plan interactive step execution request
  *  is received back from the Plan Manager.
  */

#include <vector>
#include <string>

#include "app/response_plans/plan_step_plugin/common_active/src/IActiveStepMessage.h"
#include "app/response_plans/plan_step_plugin/internal/src/RunPlanResponseParameter.h"

namespace TA_Base_App
{
    class ActivePlanRunPlanStepMessage : public IActiveStepMessage
    {
    //public methods
    public:
        ActivePlanRunPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& parameters );
        virtual ~ActivePlanRunPlanStepMessage();
        virtual void execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const;

    private:
        ActivePlanRunPlanStepMessage& operator=( const ActivePlanRunPlanStepMessage& );
        ActivePlanRunPlanStepMessage( const ActivePlanRunPlanStepMessage& );

    private:
        unsigned long m_ulStepNum;
		RunPlanResponseParameter m_responseParam;
    };

}

#endif //ACTIVE_PLAN_RUN_PLAN_STEP_HEADER_INCLUDED