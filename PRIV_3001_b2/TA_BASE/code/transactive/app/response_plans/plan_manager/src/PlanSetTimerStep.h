//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanSetTimerStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2018/03/14 13:51:20 $
// Last modified by:  $Author: lim.cy $
//
// <description>
#if !defined(AFX_PLANSETTIMERSTEP_H__INCLUDED_)
#define AFX_PLANSETTIMERSTEP_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_PLAN_SET_TIMER)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"

namespace TA_Base_App
{
    class TimeSpanStepParameter;
    class StepNumberStepParameter;

    class PlanSetTimerStep: public PlanStep
    {
    public:
        PlanSetTimerStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        PlanSetTimerStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~PlanSetTimerStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return true; }
        virtual bool canIgnoreFailure() const { return true; }

        TimeSpanStepParameter*          getTimeSpanParameter() { return m_timeSpanParameter; }

    protected:
        PlanSetTimerStep(const PlanSetTimerStep& planStep);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
            PlanStepParameterList &stepParameters);

        // Get parameters for update back to agent 
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();
    private:
        PlanSetTimerStep &operator=(const PlanSetTimerStep&);

        TimeSpanStepParameter* m_timeSpanParameter;
    };
}

#endif // (STEPTYPE_PLAN_SET_TIMER)
#endif // AFX_PLANSETTIMERSTEP_H__INCLUDED_