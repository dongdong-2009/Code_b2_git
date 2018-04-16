/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/FailureCategoryStepParameter.h $
  * @author:  Lizette Lingo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
//#include "PlanStepParameterListCtrl.h"
#include "app/response_plans/plan_step_plugin/failure_summary/common/FailureSummaryStepParameter.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // FailureCategoryStepParameter

    class FailureCategoryStepParameter : public PlanStepParameter<FailureSummaryStepParameter::EStepFailureCategory>
    {
    // Operations
    public:
        FailureCategoryStepParameter(PlanStep& parent, const std::string& name, FailureSummaryStepParameter::EStepFailureCategory category);
        virtual ~FailureCategoryStepParameter() {}

		virtual std::vector<std::string> getStepParameterValues();
		virtual const std::string displayValue() const;

		TA_Base_App::FailureSummaryStepParameter::EStepFailureCategory getCategory() const { return m_refSrcValue; }


    protected:
        virtual IPlanStepParameterForm* getEditDialog(bool canEdit);

    private:
        FailureCategoryStepParameter& operator=(const FailureCategoryStepParameter& parameter);
		const std::string convertToString(const FailureSummaryStepParameter::EStepFailureCategory failureCategory) const;

    };
}

