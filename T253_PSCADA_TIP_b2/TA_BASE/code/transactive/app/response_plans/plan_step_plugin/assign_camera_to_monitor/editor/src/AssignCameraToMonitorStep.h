//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/AssignCameraToMonitorStep.h $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
//

#if !defined(AFX_ASSIGN_CAMERA_TO_MONITOR_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
#define AFX_ASSIGN_CAMERA_TO_MONITOR_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/common/AssignCameraToMonitorStepParameter.h"
#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/editor/src/AssignCameraToMonitorStepEditorParameter.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // AssignCameraToMonitorStep

    class AssignCameraToMonitorStep : public PlanStep
    {
    // Operations
    public:
        AssignCameraToMonitorStep(IPlanNode* parent,const BasicParameter &stepDetail);
        AssignCameraToMonitorStep(IPlanNode* parent,const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~AssignCameraToMonitorStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;

		virtual IActivityParameter& getActivityParameter();

    protected:
        AssignCameraToMonitorStep(const AssignCameraToMonitorStep &other);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        AssignCameraToMonitorStep &operator=(const AssignCameraToMonitorStep &other);


    // Attributes
    private:
		AssignCameraToMonitorStepParameter m_params;
		AssignCameraToMonitorStepEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_ASSIGN_CAMERA_TO_MONITOR_STEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
