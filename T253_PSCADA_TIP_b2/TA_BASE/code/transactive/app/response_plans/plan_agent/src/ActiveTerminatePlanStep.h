#ifndef ACTIVETERMINATEPLANSTEP_H_INCLUDED
#define ACTIVETERMINATEPLANSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveTerminatePlanStep.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #11 $
  *
  *  Last modification: $DateTime: 2015/03/19 14:48:01 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_step_plugin/internal/src/TerminatePlanParameter.h"

namespace TA_Base_App
{

    //Class declaration
    class ActiveTerminatePlanStep : public ActiveStep
    {
        // Public methods
    public:
        ActiveTerminatePlanStep(const BasicParameter& detail, IActivePlan& plan);

    protected:
        virtual bool executeImpl();
        virtual int waitImpl();

    private:
        ActiveTerminatePlanStep& operator=(const ActiveTerminatePlanStep&);
        ActiveTerminatePlanStep(const ActiveTerminatePlanStep&);

    private:
        TerminatePlanParameter m_params;
    }; // ActiveTerminatePlanStep

}; // TA_Base_App

#endif // ACTIVETERMINATEPLANSTEP_H_INCLUDED