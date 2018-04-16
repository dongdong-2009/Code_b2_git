//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/RunPlanStep.h $
// @author:  Andy Parker
// @version: $Revision: #15 $
//
// Last modification: $DateTime: 2015/04/24 17:26:58 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#if !defined(AFX_RUNPLANSTEP_H__81809A68_7564_4F2D_8B23_F7B4DDCB695B__INCLUDED_)
#define AFX_RUNPLANSTEP_H__81809A68_7564_4F2D_8B23_F7B4DDCB695B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/internal/src/RunPlanParameter.h"
#include "app/response_plans/plan_manager/src/RunPlanStepEditorParameter.h"

namespace TA_Base_App
{
    class YesNoStepParameter;
    class PlanPathStepParameter;
    class CPlanStepEditRunPlanPage;

    /////////////////////////////////////////////////////////////////////////////
    // RunPlanStep

    class RunPlanStep : public PlanStep
    {
    // Operations
    public:
        RunPlanStep( IPlanNode* parent, const BasicParameter &stepDetail );
        RunPlanStep(IPlanNode* parent, const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~RunPlanStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;

        PlanPathStepParameter* getPathParameter() { return m_planParameter;};
        YesNoStepParameter* getLaunchParameter() { return m_launchParameter;};
        YesNoStepParameter* getWaitParameter() { return m_waitParameter;};

        virtual IActivityParameter& getActivityParameter();

    protected:
        RunPlanStep(const RunPlanStep &other);
        virtual void createPlanStepParameters();
        virtual bool isParametersValid( const std::vector<std::string>& parameters );
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        RunPlanStep &operator=(const RunPlanStep &other);

    // Attributes
    private:
        PlanPathStepParameter* m_planParameter;
        YesNoStepParameter* m_launchParameter;
        YesNoStepParameter* m_waitParameter;
        
        RunPlanParameter m_params;
		RunPlanStepEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_RUNPLANSTEP_H__81809A68_7564_4F2D_8B23_F7B4DDCB695B__INCLUDED_)
