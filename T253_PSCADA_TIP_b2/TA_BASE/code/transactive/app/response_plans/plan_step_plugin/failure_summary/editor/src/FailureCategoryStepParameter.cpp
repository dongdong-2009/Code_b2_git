/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/FailureCategoryStepParameter.cpp $
  * @author:  Lizette Lingo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "FailureCategoryStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameterEditDlg.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
//#include "Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

FailureCategoryStepParameter::FailureCategoryStepParameter(PlanStep& parent, const std::string& name, FailureSummaryStepParameter::EStepFailureCategory category)
    : PlanStepParameter(parent,name,category)
{
    FUNCTION_ENTRY( "FailureCategoryStepParameter" );
    FUNCTION_EXIT;
}

std::vector<std::string> FailureCategoryStepParameter::getStepParameterValues()
{
	std::vector<std::string> parameterVect;
	parameterVect.push_back(convertToString(m_refSrcValue));
	return parameterVect;
}

IPlanStepParameterForm* FailureCategoryStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new CPlanStepParameterEditDlg(*this,canEdit);
}

const std::string FailureCategoryStepParameter::convertToString(const FailureSummaryStepParameter::EStepFailureCategory failureCategory) const
{
    FUNCTION_ENTRY( "convertToString" );

    switch(failureCategory)
    {
    case TA_Base_App::FailureSummaryStepParameter::SFC_ALL_STEPS:
        FUNCTION_EXIT;
        return "All step failures";
    case TA_Base_App::FailureSummaryStepParameter::SFC_SET_DATA_POINT_STEPS:
        FUNCTION_EXIT;
        return "BRC Step";
    default:
        ;
    }

    FUNCTION_EXIT;
    return "Undefined failure category";
}

const std::string FailureCategoryStepParameter::displayValue() const
{
    return convertToString(m_refSrcValue);;
}
