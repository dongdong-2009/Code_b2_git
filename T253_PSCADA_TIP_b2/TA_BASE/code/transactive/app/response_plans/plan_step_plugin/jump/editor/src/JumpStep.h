//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/JumpStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_JUMPSTEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
#define AFX_JUMPSTEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/jump/common/JumpStepParameter.h"
#include "app/response_plans/plan_step_plugin/jump/editor/src/JumpStepEditorParameter.h"

namespace TA_Base_App
{
    class StepNumberStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // JumpStep

    class JumpStep : public PlanStep
    {
    // Operations
    public:
        JumpStep(IPlanNode* parent, const BasicParameter &stepDetail);
        JumpStep(IPlanNode* parent, const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~JumpStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return true; }
        virtual bool canIgnoreFailure() const { return false; }
		virtual std::string getStepTypeName() const;
        virtual void outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing );
        virtual bool retrieveOutgoings( std::vector<unsigned int>& vecOutgoings );
        virtual void clearFlowParameters();
        
		virtual IActivityParameter& getActivityParameter();

    protected:
        JumpStep(const JumpStep &other);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
		virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        JumpStep &operator=(const JumpStep &other);

    // Attributes
    private:
		JumpStepParameter m_params;
		JumpStepEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_JUMPSTEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
