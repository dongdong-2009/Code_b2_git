//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PromptStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#ifndef AFX_PROMPTSTEP_H__INCLUDED_
#define AFX_PROMPTSTEP_H__INCLUDED_

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/prompt/common/PromptStepParameter.h"
#include "PromptStepEditorParameter.h"

namespace TA_Base_App
{
    class CPlanStepEditParameterPage;

    /////////////////////////////////////////////////////////////////////////////
    // PromptStep

    class PromptStep : public PlanStep
    {
    // Operations
    public:
        PromptStep(IPlanNode* parent,const BasicParameter &stepDetail);
        PromptStep(IPlanNode* parent,const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~PromptStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const;
        virtual bool canIgnoreFailure() const;
		virtual std::string getStepTypeName() const;

		virtual IActivityParameter& getActivityParameter();

    protected:
        PromptStep(const PromptStep &other);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
		virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        PromptStep &operator=(const PromptStep &other);

    // Attributes
    private:
        PromptStepParameter m_params;
        PromptStepEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_PROMPTSTEP_H__INCLUDED_)
