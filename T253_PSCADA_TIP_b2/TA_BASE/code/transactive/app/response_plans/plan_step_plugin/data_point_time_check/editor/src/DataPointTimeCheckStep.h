//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointTimeCheckStep.h $
// @author:  Andy Parker
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2012/02/06 16:15:14 $
// Last modified by:  $Author: haijun.li $
//
// <description>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/data_point_time_check/common/DataPointTimeCheckStepParameter.h"
#include "app/response_plans/plan_step_plugin/data_point_time_check/editor/src/DataPointTimeCheckStepEditorParameter.h"


namespace TA_Base_App
{
/////////////////////////////////////////////////////////////////////////////
// DataPointTimeCheckStep

class DataPointTimeCheckStep : public PlanStep
{
    // Operations
public:
    DataPointTimeCheckStep( IPlanNode* parent, const BasicParameter& stepDetail );
    DataPointTimeCheckStep( IPlanNode* parent, const unsigned long stepId, const int stepType ); // For new plan steps
    virtual ~DataPointTimeCheckStep();

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
    DataPointTimeCheckStep( const DataPointTimeCheckStep& other );
    virtual void createPlanStepParameters();
    virtual bool isParametersValid( const std::vector<std::string>& parameters );
    virtual CPlanStepEditParameterPage* getParameterEditPage();

private:
    DataPointTimeCheckStep& operator=( const DataPointTimeCheckStep& other );


    // Attributes
private:
    DataPointTimeCheckStepParameter m_params;
    DataPointTimeCheckStepEditorParameter m_parActivityParameter;
};
}