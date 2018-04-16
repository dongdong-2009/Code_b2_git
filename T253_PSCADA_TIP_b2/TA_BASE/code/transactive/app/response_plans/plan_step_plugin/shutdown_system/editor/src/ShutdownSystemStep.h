#if !defined(AFX_DECISIONSTEP_H__01DDAB17_3CEB_4c7b_B5C1_B14A7AE8BDC5__INCLUDED_)
#define AFX_DECISIONSTEP_H__01DDAB17_3CEB_4c7b_B5C1_B14A7AE8BDC5__INCLUDED_
//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ShutdownSystemStep.h $
// @author:  Andy Parker
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2014/05/19 15:59:20 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/shutdown_system/common/ShutdownSystemStepParameter.h"
#include "ShutdownSystemStepEditorParameter.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // ShutdownSystemStep

    class ShutdownSystemStep : public PlanStep
    {
        // Operations
    public:
        ShutdownSystemStep(IPlanNode* parent,const BasicParameter &stepDetail);
        ShutdownSystemStep(IPlanNode* parent,const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~ShutdownSystemStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;

		virtual IActivityParameter& getActivityParameter();

    protected:
        ShutdownSystemStep(const ShutdownSystemStep &other);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
		virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        ShutdownSystemStep &operator=(const ShutdownSystemStep &other);


        // Attributes
    private:

		ShutdownSystemStepParameter m_params;

		ShutdownSystemStepEditorParameter m_parActivityParameter;
    };
}

#endif // defined (AFX_DECISIONSTEP_H__01DDAB17_3CEB_4c7b_B5C1_B14A7AE8BDC5__INCLUDED_)
