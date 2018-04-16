//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/DecisionStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#ifndef AFX_DECISIONSTEP_H__INCLUDED_
#define AFX_DECISIONSTEP_H__INCLUDED_

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/decision/common/DecisionStepParameter.h"
#include "app/response_plans/plan_step_plugin/decision/editor/src/DecisionStepEditorParameter.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // DecisionStep

    class DecisionStep : public PlanStep
    {
    // Operations
    public:
        DecisionStep(IPlanNode* parent, BasicParameter& stepDetail);
        DecisionStep(IPlanNode* parent, const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~DecisionStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return true; }
        virtual bool canIgnoreFailure() const { return false; }
        virtual std::string getStepTypeName() const;
        virtual void outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing );
        virtual bool retrieveOutgoings( std::vector<unsigned int>& vecOutgoings );
        virtual void clearFlowParameters();
        virtual bool allowMultipleIncoming();
        virtual bool allowMultipleOutgoing();

		virtual IActivityParameter& getActivityParameter();

    protected:
        DecisionStep(const DecisionStep &other);
        virtual void createPlanStepParameters();
        virtual bool isParametersValid( const std::vector<std::string>& parameters );
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        DecisionStep &operator=(const DecisionStep &other);


    // Attributes
    private:
		DecisionStepParameter m_params;
		DecisionStepEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_DECISIONSTEP_H__INCLUDED_)
