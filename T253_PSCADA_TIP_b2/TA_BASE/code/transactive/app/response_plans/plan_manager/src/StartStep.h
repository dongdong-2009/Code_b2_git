//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/StartStep.h $
// @author:  Andy Parker
// @version: $Revision: #8 $
//
// Last modification: $DateTime: 2014/12/30 18:50:08 $
// Last modified by:  $Author: huang.wenbo $
//
// <description>

#pragma once;

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common/src/EmptyStepParameter.h"
#include "app/response_plans/plan_step_plugin/common/src/EmptyEditorParamter.h"

namespace TA_Base_App
{
/////////////////////////////////////////////////////////////////////////////
// StartStep

class StartStep : public PlanStep
{
    // Operations
public:
    StartStep( IPlanNode* parent, const BasicParameter& stepDetail );
    StartStep( IPlanNode* parent, const unsigned long stepId, const int stepType );
    virtual ~StartStep();

    virtual PlanStep* clone();
    virtual bool hasCustomOutgoingStep() const;
    virtual bool canIgnoreFailure() const;
    virtual std::string getStepTypeName() const;

    virtual IActivityParameter& getActivityParameter();

protected:
    StartStep(const StartStep& other);
    virtual void createPlanStepParameters();
    virtual bool isParametersValid( const std::vector<std::string>& parameters );

    virtual bool editable();
    virtual bool hasIncoming();
    virtual bool hasOutgoing();

private:
    StartStep& operator=(const StartStep& other);


    // Attributes
private:
    EmptyStepParameter m_params;
    EmptyEditorParamter m_parActivityParameter;

};
}