//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointSetStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_DATAPOINTSETSTEP_H__E1BCCBDF_F2D1_4A63_8C98_0DA66344B8BB__INCLUDED_)
#define AFX_DATAPOINTSETSTEP_H__E1BCCBDF_F2D1_4A63_8C98_0DA66344B8BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/data_point_set/common/DataPointSetStepParameter.h"
#include "app/response_plans/plan_step_plugin/data_point_set/editor/src/DataPointSetStepEditorParameter.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // DataPointSetStep

    class DataPointSetStep : public PlanStep
    {
    // Operations
    public:
        DataPointSetStep(IPlanNode* parent,const BasicParameter &stepDetail);
        DataPointSetStep(IPlanNode* parent,const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~DataPointSetStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;

		virtual IActivityParameter& getActivityParameter();
    protected:
        DataPointSetStep(const DataPointSetStep &other);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
		virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        DataPointSetStep &operator=(const DataPointSetStep &other);

    // Attributes
    private:
		DataPointSetStepParameter m_params;
		DataPointSetStepEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_DATAPOINTSETSTEP_H__E1BCCBDF_F2D1_4A63_8C98_0DA66344B8BB__INCLUDED_)
