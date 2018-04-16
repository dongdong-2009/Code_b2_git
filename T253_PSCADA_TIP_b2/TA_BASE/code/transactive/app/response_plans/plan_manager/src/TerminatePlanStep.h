//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TerminatePlanStep.h $
// @author:  Andy Parker
// @version: $Revision: #14 $
//
// Last modification: $DateTime: 2015/03/19 14:48:01 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#if !defined(AFX_TERMINATEPLANSTEP_H__8F6BCAA3_B30D_4FF0_8025_A7264924E915__INCLUDED_)
#define AFX_TERMINATEPLANSTEP_H__8F6BCAA3_B30D_4FF0_8025_A7264924E915__INCLUDED_

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/internal/src/TerminatePlanParameter.h"
#include "app/response_plans/plan_manager/src/TerminatePlanEditorParameter.h"

namespace TA_Base_App
{
    class PlanPathStepParameter;
    class CPlanStepEditParameterPage;

    /////////////////////////////////////////////////////////////////////////////
    // TerminatePlanStep

    class TerminatePlanStep : public PlanStep
    {
    // Operations
    public:
        TerminatePlanStep( IPlanNode* parent, const BasicParameter &stepDetail );
        TerminatePlanStep( IPlanNode* parent, const unsigned long stepId, const int stepType );  // For new plan steps
        virtual ~TerminatePlanStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;

        PlanPathStepParameter* getPathParameter() { return m_planParameter;};

        virtual IActivityParameter& getActivityParameter();

    protected:
        TerminatePlanStep(const TerminatePlanStep &other);
        virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        TerminatePlanStep &operator=(const TerminatePlanStep &other);


    // Attributes
    private:
        PlanPathStepParameter* m_planParameter;
		TerminatePlanParameter m_params;
		TerminatePlanEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_TERMINATEPLANSTEP_H__8F6BCAA3_B30D_4FF0_8025_A7264924E915__INCLUDED_)
