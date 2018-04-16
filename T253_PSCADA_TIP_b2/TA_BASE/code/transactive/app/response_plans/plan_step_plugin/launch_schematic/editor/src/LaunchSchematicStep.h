/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/LaunchSchematicStep.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#if !defined(AFX_LaunchSchematicSTEP_H__0A8BB324_82D4_43BE_8670_61A6B2B15048__INCLUDED_)
#define AFX_LaunchSchematicSTEP_H__0A8BB324_82D4_43BE_8670_61A6B2B15048__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/launch_schematic/common/LaunchSchematicStepParameter.h"
#include "LaunchSchematicStepEditorParameter.h"

namespace TA_Base_App
{
    class NumericStepParameter;
    class TextualStepParameter;


    /////////////////////////////////////////////////////////////////////////////
    // LaunchSchematicStep

    class LaunchSchematicStep : public PlanStep
    {
    // Operations
    public:
        LaunchSchematicStep(IPlanNode* parent, const BasicParameter &stepDetail);
        LaunchSchematicStep(IPlanNode* parent, const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~LaunchSchematicStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;

        TextualStepParameter* getSchematicNameParameter()  { return m_schematicNameParameter; }
        TextualStepParameter* getCommandLineParameter()    { return m_commandLineParameter; }
        NumericStepParameter* getScreenNumberParameter()   { return m_screenNumberParameter; }

		virtual IActivityParameter& getActivityParameter();
    protected:
        LaunchSchematicStep(const LaunchSchematicStep &other);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
		virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        LaunchSchematicStep &operator=(const LaunchSchematicStep &other);


    // Attributes
    private:
        TextualStepParameter* m_schematicNameParameter;
        TextualStepParameter* m_commandLineParameter;
        NumericStepParameter* m_screenNumberParameter;

		LaunchSchematicStepParameter m_params;
		LaunchSchematicStepEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_LaunchSchematicSTEP_H__0A8BB324_82D4_43BE_8670_61A6B2B15048__INCLUDED_)
