//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanCheckTimerStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2018/03/14 13:51:20 $
// Last modified by:  $Author: lim.cy $
//
// <description>
#if !defined(AFX_PLANCHECKTIMERSTEP_H__INCLUDED_)
#define AFX_PLANCHECKTIMERSTEP_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_PLAN_CHECK_TIMER)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"

namespace TA_Base_App
{
    class TimeSpanStepParameter;
    class StepNumberStepParameter;

    class PlanCheckTimerStep: public PlanStep
    {
    public:
        PlanCheckTimerStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        PlanCheckTimerStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~PlanCheckTimerStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return true; }
        virtual bool canIgnoreFailure() const { return true; }

        TimeSpanStepParameter* getTimeSpanParameter() { return m_timeSpanParameter; }
        StepNumberStepParameter* getSuccessParameter() { return m_successParameter;}
        StepNumberStepParameter* getFailParameter() { return m_failureParameter;}

    protected:
        PlanCheckTimerStep(const PlanCheckTimerStep& planStep);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
            PlanStepParameterList &stepParameters);

        // Get parameters for update back to agent 
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();
    private:
        PlanCheckTimerStep &operator=(const PlanCheckTimerStep&);

        TimeSpanStepParameter* m_timeSpanParameter;
        StepNumberStepParameter* m_successParameter;
        StepNumberStepParameter* m_failureParameter;
    };
}

#endif // (STEPTYPE_PLAN_CHECK_TIMER)
#endif // AFX_PLANCHECKTIMERSTEP_H__INCLUDED_