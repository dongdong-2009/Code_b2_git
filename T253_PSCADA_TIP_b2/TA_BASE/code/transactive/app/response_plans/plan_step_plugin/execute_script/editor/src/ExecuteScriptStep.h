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


#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/execute_script/common/ExecuteScriptStepParameter.h"
#include "ExecuteScriptStepEditorParameter.h"

namespace TA_Base_App
{
    class ExecuteScriptStep: public PlanStep
    {
    public:
        ExecuteScriptStep(IPlanNode* parent,const BasicParameter &stepDetail);
        ExecuteScriptStep(IPlanNode* parent,const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~ExecuteScriptStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;
		virtual IActivityParameter& getActivityParameter();
    protected:
        ExecuteScriptStep(const ExecuteScriptStep& planStep);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
		virtual CPlanStepEditParameterPage* getParameterEditPage();
    private:
        ExecuteScriptStep &operator=(const ExecuteScriptStep&);

		ExecuteScriptStepParameter m_params;
		ExecuteScriptStepEditorParameter m_parActivityParameter;
    };
}

#endif // AFX_DATAPOINTCHECKSTEP_H__da97775f_0c0e_4a8b_86a2_9ea163c89fa6__INCLUDED_