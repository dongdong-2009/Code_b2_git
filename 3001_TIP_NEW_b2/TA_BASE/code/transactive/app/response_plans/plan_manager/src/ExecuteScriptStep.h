//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/LogStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2009/12/29 14:17:48 $
// Last modified by:  $Author: grace.koh $
//
// <description>
#if !defined(AFX_DATAPOINTCHECKSTEP_H__da97775f_0c0e_4a8b_86a2_9ea163c89fa6__INCLUDED_)
#define AFX_DATAPOINTCHECKSTEP_H__da97775f_0c0e_4a8b_86a2_9ea163c89fa6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_EXECUTE_SCRIPT)


#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"

namespace TA_Base_App
{
    class TextualStepParameter;

    class ExecuteScriptStep: public PlanStep
    {
    public:
        ExecuteScriptStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        ExecuteScriptStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~ExecuteScriptStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return true; }
        virtual bool canIgnoreFailure() const { return true; }

        TextualStepParameter*           getCommandLineParameter() { return m_commandLineParameter; }

    protected:
        ExecuteScriptStep(const ExecuteScriptStep& planStep);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
            PlanStepParameterList &stepParameters);

        // Get parameters for update back to agent 
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();
    private:
        ExecuteScriptStep &operator=(const ExecuteScriptStep&);

        TextualStepParameter* m_commandLineParameter;
    };
}

#endif // (STEPTYPE_EXECUTE_SCRIPT)
#endif // AFX_DATAPOINTCHECKSTEP_H__da97775f_0c0e_4a8b_86a2_9ea163c89fa6__INCLUDED_