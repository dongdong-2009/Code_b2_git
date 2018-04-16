//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanCheckStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_ACTIVEPLANCHECKSTEP_H__F0B425B6_5C8B_4A24_B5A7_7B08792FB6C6__INCLUDED_)
#define AFX_ACTIVEPLANCHECKSTEP_H__F0B425B6_5C8B_4A24_B5A7_7B08792FB6C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
#include "app/response_plans/plan_step_plugin/plan_check/common/PlanCheckStepParameter.h"
#include "app/response_plans/plan_step_plugin/plan_check/editor/src/PlanCheckStepEditorParameter.h"

namespace TA_Base_App
{
    class StepNumberStepParameter;
    class PlanPathStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // PlanCheckStep

    class PlanCheckStep : public PlanStep
    {
    // Operations
    public:
        PlanCheckStep(IPlanNode* parent,const BasicParameter &stepDetail);
        PlanCheckStep(IPlanNode* parent,const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~PlanCheckStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const;
        virtual bool canIgnoreFailure() const;
		virtual std::string getStepTypeName() const;
		virtual void outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing );
		virtual bool retrieveOutgoings( std::vector<unsigned int>& vecOutgoings );
        virtual void clearFlowParameters();
		virtual bool allowMultipleIncoming();
		virtual bool allowMultipleOutgoing();

        PlanPathStepParameter*   getPathParameter() { return m_planPathParameter;};
        StepNumberStepParameter* getYesParameter() { return m_yesStepParameter;};
        StepNumberStepParameter* getNoParameter() { return m_noStepParameter;};
		virtual IActivityParameter& getActivityParameter();

    protected:
        PlanCheckStep(const PlanCheckStep &other);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
		virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        PlanCheckStep &operator=(const PlanCheckStep &other);


    // Attributes
    private:
        PlanPathStepParameter*   m_planPathParameter;
        StepNumberStepParameter* m_yesStepParameter;
        StepNumberStepParameter* m_noStepParameter;
       
		PlanCheckStepParameter m_params;
		PlanCheckStepEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_ACTIVEPLANCHECKSTEP_H__F0B425B6_5C8B_4A24_B5A7_7B08792FB6C6__INCLUDED_)
