//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/AbortStep.h $
// @author:  Andy Parker
// @version: $Revision: #12 $
//
// Last modification: $DateTime: 2015/03/18 12:39:25 $
// Last modified by:  $Author: huang.wenbo $
//
// <description>

#if !defined(AFX_ABORTSTEP_H__E40AE63B_4A40_4DC8_992E_240935C7BBA9__INCLUDED_)
#define AFX_ABORTSTEP_H__E40AE63B_4A40_4DC8_992E_240935C7BBA9__INCLUDED_


#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/common/src/EmptyStepParameter.h"
#include "app/response_plans/plan_step_plugin/common/src/EmptyEditorParamter.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // AbortStep

    class AbortStep : public PlanStep
    {
    // Operations
    public:
        AbortStep( IPlanNode* parent, const BasicParameter &stepDetail );
        AbortStep( IPlanNode* parent, const unsigned long stepId, const int stepType );  // For new plan steps
        virtual ~AbortStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;

        virtual IActivityParameter& getActivityParameter();

    protected:
        virtual void createPlanStepParameters();
        virtual bool isParametersValid( const std::vector<std::string>& parameters );

    private:
        AbortStep(const AbortStep &other);
        AbortStep &operator=(const AbortStep &other);

		EmptyStepParameter m_params;

		EmptyEditorParamter m_parActivityParameter;
    };
}
#endif // !defined(AFX_ABORTSTEP_H__E40AE63B_4A40_4DC8_992E_240935C7BBA9__INCLUDED_)
