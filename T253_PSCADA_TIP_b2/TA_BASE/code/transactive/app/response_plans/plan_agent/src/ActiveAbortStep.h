#ifndef ACTIVEABORTSTEP_H_INCLUDED
#define ACTIVEABORTSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveAbortStep.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #10 $
  *
  *  Last modification: $DateTime: 2015/03/18 12:39:25 $
  *  Last modified by:  $Author: huang.wenbo $
  *
  *  <description>
  */

#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_step_plugin/common/src/EmptyStepParameter.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{

    //Class declaration
    class ActiveAbortStep : public ActiveStep
    {
        // Public methods
    public:
        ActiveAbortStep(const BasicParameter& detail, IActivePlan& plan);

    protected:
        virtual bool executeImpl();
        virtual int waitImpl();

    private:
        ActiveAbortStep& operator=(const ActiveAbortStep&);
        ActiveAbortStep(const ActiveAbortStep&);
        EmptyStepParameter m_params;
    }; // ActiveAbortStep

}; // TA_Base_App


#endif // ACTIVEABORTSTEP_H_INCLUDED