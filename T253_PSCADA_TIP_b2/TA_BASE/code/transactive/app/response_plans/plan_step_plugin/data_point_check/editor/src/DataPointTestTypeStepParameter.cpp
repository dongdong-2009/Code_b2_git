/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointTestTypeStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "DataPointTestTypeStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameterEditDlg.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

DataPointTestTypeStepParameter::DataPointTestTypeStepParameter(PlanStep& parent,const std::string& name, int testType) :
    PlanStepParameter(parent,name,testType)
{
    FUNCTION_ENTRY( "DataPointTestTypeStepParameter" );
    FUNCTION_EXIT;
}

DataPointTestTypeStepParameter::~DataPointTestTypeStepParameter()
{
    FUNCTION_ENTRY( "~DataPointTestTypeStepParameter" );
    FUNCTION_EXIT;
}

IPlanStepParameterForm* DataPointTestTypeStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new CPlanStepParameterEditDlg(*this,canEdit);
}

std::vector<std::string> DataPointTestTypeStepParameter::getStepParameterValues()
{
	std::vector<std::string> parametersVect;
    parametersVect.push_back(displayValue());
	return parametersVect;
}

const std::string DataPointTestTypeStepParameter::displayValue() const
{
    return convertToString( m_refSrcValue );
}

std::string DataPointTestTypeStepParameter::convertToString( const int eType )
{
    FUNCTION_ENTRY( "convertToString" );

    switch ( eType )
    {
    case EQUAL_TO_TEST:
        FUNCTION_EXIT;
        return"Equal";

    case NOT_EQUAL_TO_TEST:
        FUNCTION_EXIT;
        return "Not Equal";

    case LESS_THAN_TEST:
        FUNCTION_EXIT;
        return "Less Than";

    case GREATER_THAN_TEST:
        FUNCTION_EXIT;
        return "Greater Than";

    case LESS_THAN_OR_EQUAL_TO_TEST:
        FUNCTION_EXIT;
        return "Less Than or Equal";

    case GREATER_THAN_OR_EQUAL_TO_TEST:
        FUNCTION_EXIT;
        return "Greater Than or Equal";

    default:
        ;
    }

    FUNCTION_EXIT;
    return "Undefined test type";
}
