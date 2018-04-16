//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ExpressionStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_ExpressionSTEP_H__6FB752F8_A4CB_4F92_B486_C933B5041E39__INCLUDED_)
#define AFX_ExpressionSTEP_H__6FB752F8_A4CB_4F92_B486_C933B5041E39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/evaluate_expression/common/EvaluateExpressionStepParameter.h"
#include "app/response_plans/plan_step_plugin/evaluate_expression/editor/src/EvaluateExpressionStepEditorParameter.h"


namespace TA_Base_App
{
/////////////////////////////////////////////////////////////////////////////
// ExpressionStep

class ExpressionStep : public PlanStep
{
    // Operations
public:
    ExpressionStep( IPlanNode* parent, const BasicParameter& stepDetail );
    ExpressionStep( IPlanNode* parent, const unsigned long stepId, const int stepType ); // For new plan steps
    virtual ~ExpressionStep();

    virtual PlanStep* clone();
    virtual bool hasCustomOutgoingStep() const;
    virtual bool canIgnoreFailure() const;
    virtual std::string getStepTypeName() const;
    virtual void outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing );
	virtual bool retrieveOutgoings( std::vector<unsigned int>& vecOutgoings );
    virtual void clearFlowParameters();
    virtual bool allowMultipleIncoming();
    virtual bool allowMultipleOutgoing();
    virtual IActivityParameter& getActivityParameter();

protected:
    ExpressionStep( const ExpressionStep& other );
    virtual void createPlanStepParameters();
    virtual bool isParametersValid( const std::vector<std::string>& parameters );
    virtual CPlanStepEditParameterPage* getParameterEditPage();

private:
    ExpressionStep& operator=( const ExpressionStep& other );


    // Attributes
private:
    EvaluateExpressionStepParameter m_params;
    EvaluateExpressionStepEditorParameter m_parActivityParameter;
};
}

#endif // !defined(AFX_ExpressionSTEP_H__6FB752F8_A4CB_4F92_B486_C933B5041E39__INCLUDED_)
