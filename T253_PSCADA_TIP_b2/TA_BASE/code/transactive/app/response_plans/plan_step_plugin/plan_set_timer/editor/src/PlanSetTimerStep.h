//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanSetTimerStep.h $
// @author:  Andy Parker
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2014/05/19 15:59:20 $
// Last modified by:  $Author: qi.huang $
//
// <description>
#if !defined(AFX_PLANSETTIMERSTEP_H__INCLUDED_)
#define AFX_PLANSETTIMERSTEP_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/plan_set_timer/common/PlanSetTimerStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "PlanSetTimerStepEditorParameter.h"

namespace TA_Base_App
{
    class TimeSpanStepParameter;
    class StepNumberStepParameter;

    class PlanSetTimerStep: public PlanStep
    {
    public:
        PlanSetTimerStep(IPlanNode* parent,const BasicParameter &stepDetail);
        PlanSetTimerStep(IPlanNode* parent,const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~PlanSetTimerStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;

        TimeSpanStepParameter*          getTimeSpanParameter() { return m_timeSpanParameter; }

		virtual IActivityParameter& getActivityParameter();
    protected:
        PlanSetTimerStep(const PlanSetTimerStep& planStep);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
		virtual CPlanStepEditParameterPage* getParameterEditPage();
    private:
        PlanSetTimerStep &operator=(const PlanSetTimerStep&);

        TimeSpanStepParameter* m_timeSpanParameter;
		PlanSetTimerStepParameter m_params;
		PlanSetTimerStepEditorParameter m_parActivityParameter;
    };
}
#endif // AFX_PLANSETTIMERSTEP_H__INCLUDED_