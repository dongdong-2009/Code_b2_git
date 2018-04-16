//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/EndStep.h $
// @author:  Andy Parker
// @version: $Revision: #13 $
//
// Last modification: $DateTime: 2015/03/18 12:39:25 $
// Last modified by:  $Author: huang.wenbo $
//
// <description>

#if !defined(AFX_ENDSTEP_H__C032447B_A828_4C1B_9037_F502C1C49DD1__INCLUDED_)
#define AFX_ENDSTEP_H__C032447B_A828_4C1B_9037_F502C1C49DD1__INCLUDED_

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/common/src/EmptyStepParameter.h"
#include "app/response_plans/plan_step_plugin/common/src/EmptyEditorParamter.h"

namespace TA_Base_App
{
/////////////////////////////////////////////////////////////////////////////
// EndStep

class EndStep : public PlanStep
{
    // Operations
public:
    EndStep( IPlanNode* parent, const BasicParameter& stepDetail );
    EndStep(IPlanNode* parent, const unsigned long stepId, const int stepType);
    virtual ~EndStep();

    virtual PlanStep* clone();
    virtual bool hasCustomOutgoingStep() const;
    virtual bool canIgnoreFailure() const;
    virtual std::string getStepTypeName() const;

    virtual IActivityParameter& getActivityParameter();

protected:
    EndStep(const EndStep& other);
    virtual void createPlanStepParameters();
    virtual bool isParametersValid( const std::vector<std::string>& parameters );

    virtual bool editable();
    virtual bool hasIncoming();
    virtual bool hasOutgoing();

private:
    EndStep& operator=(const EndStep& other);


    // Attributes
private:
    EmptyStepParameter m_params;
    EmptyEditorParamter m_parActivityParameter;

};
}

#endif // !defined(AFX_ENDSTEP_H__C032447B_A828_4C1B_9037_F502C1C49DD1__INCLUDED_)
