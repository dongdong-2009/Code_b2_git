//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanCheckTimerStep.h $
// @author:  Andy Parker
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2014/05/19 15:59:20 $
// Last modified by:  $Author: qi.huang $
//
// <description>
#if !defined(AFX_PLANCHECKTIMERSTEP_H__INCLUDED_)
#define AFX_PLANCHECKTIMERSTEP_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/plan_check_timer/common/PlanCheckTimerStepParameter.h"
#include "app/response_plans/plan_step_plugin/plan_check_timer/editor/src/PlanCheckTimerStepEditorParameter.h"

namespace TA_Base_App
{
class PlanCheckTimerStep: public PlanStep
{
public:
    PlanCheckTimerStep( IPlanNode* parent, const BasicParameter& stepDetail );
    PlanCheckTimerStep( IPlanNode* parent, const unsigned long stepId, const int stepType ); // For new plan steps
    virtual ~PlanCheckTimerStep();

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
    PlanCheckTimerStep( const PlanCheckTimerStep& planStep );
    virtual void createPlanStepParameters();
    virtual bool isParametersValid( const std::vector<std::string>& parameters );
    virtual CPlanStepEditParameterPage* getParameterEditPage();
private:
    PlanCheckTimerStep& operator=( const PlanCheckTimerStep& );
    PlanCheckTimerStepParameter m_params;
    PlanCheckTimerStepEditorParameter m_parActivityParameter;
};
}

#endif // AFX_PLANCHECKTIMERSTEP_H__INCLUDED_