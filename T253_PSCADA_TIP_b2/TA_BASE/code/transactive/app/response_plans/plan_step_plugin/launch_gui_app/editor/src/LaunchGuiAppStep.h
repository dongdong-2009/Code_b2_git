//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/LaunchGuiAppStep.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_LaunchGuiAppStepSTEP_H__ED768651_B2BC_415D_8DEF_B66C76BE0231__INCLUDED_)
#define AFX_LaunchGuiAppStepSTEP_H__ED768651_B2BC_415D_8DEF_B66C76BE0231__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/launch_gui_app/common/LaunchGuiAppStepParameter.h"
#include "LaunchGuiAppStepEditorParameter.h"


namespace TA_Base_App
{
    class GuiAppTypeStepParameter;
    class NumericStepParameter;
    class TextualStepParameter;


    /////////////////////////////////////////////////////////////////////////////
    // LaunchGuiAppStep

    class LaunchGuiAppStep : public PlanStep
    {
    // Operations
    public:
        LaunchGuiAppStep(IPlanNode* parent, const BasicParameter &stepDetail);
        LaunchGuiAppStep(IPlanNode* parent, const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~LaunchGuiAppStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;

        GuiAppTypeStepParameter* getAppTypeParameter()  { return m_appTypeParameter; }
        TextualStepParameter* getCommandLineParameter() { return m_commandLineParameter; }
        NumericStepParameter* getXPositionParameter()   { return m_xPositionParameter; }
        NumericStepParameter* getYPositionParameter()   { return m_yPositionParameter; }
        NumericStepParameter* getWidthParameter()       { return m_widthParameter; }
        NumericStepParameter* getHeightParameter()      { return m_heightParameter; }

		virtual IActivityParameter& getActivityParameter();

    protected:
        LaunchGuiAppStep(const LaunchGuiAppStep &other);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        LaunchGuiAppStep &operator=(const LaunchGuiAppStep &other);

	// Attributes
	public:
		static const unsigned long DEFAULT_X_POSITION;
		static const unsigned long DEFAULT_Y_POSITION;
		static const unsigned long DEFAULT_WIDTH;
		static const unsigned long DEFAULT_HEIGHT;

    private:
        GuiAppTypeStepParameter* m_appTypeParameter;
        TextualStepParameter* m_commandLineParameter;
        NumericStepParameter* m_xPositionParameter;
        NumericStepParameter* m_yPositionParameter;
        NumericStepParameter* m_widthParameter;
        NumericStepParameter* m_heightParameter;

		LaunchGuiAppStepParameter m_params;
		LaunchGuiAppStepEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_LaunchGuiAppStepSTEP_H__ED768651_B2BC_415D_8DEF_B66C76BE0231__INCLUDED_)
