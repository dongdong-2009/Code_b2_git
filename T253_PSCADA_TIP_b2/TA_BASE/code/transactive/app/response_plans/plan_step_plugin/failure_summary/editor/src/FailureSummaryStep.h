//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/FailureSummaryStep.h $
// @author:  Lizette Lingo
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/failure_summary/common/FailureSummaryStepParameter.h"
#include "FailureSummaryStepEditorParameter.h"


namespace TA_Base_App
{
    class FailureCategoryStepParameter;
    class CPlanStepEditParameterPage;
    /////////////////////////////////////////////////////////////////////////////
    // FailureSummaryStep

    class FailureSummaryStep : public PlanStep
    {
    // Operations
    public:
        FailureSummaryStep(IPlanNode* parent, const BasicParameter &stepDetail);
        FailureSummaryStep(IPlanNode* parent, const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~FailureSummaryStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;

        FailureCategoryStepParameter* getFailureCategoryParameter() { return m_failureCategoryParameter; }

		virtual IActivityParameter& getActivityParameter();
    protected:
        FailureSummaryStep(const FailureSummaryStep &other);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
		virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        FailureSummaryStep &operator=(const FailureSummaryStep &other);

        FailureCategoryStepParameter* m_failureCategoryParameter;

		FailureSummaryStepParameter m_params;
		FailureSummaryStepEditorParameter m_parActivityParameter;
    };
}

