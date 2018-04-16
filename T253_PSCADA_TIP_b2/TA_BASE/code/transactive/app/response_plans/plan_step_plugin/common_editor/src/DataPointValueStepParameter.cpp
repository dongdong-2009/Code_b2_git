/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointValueStepParameter.cpp $
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
#include "DataPointValueStepParameter.h"
#include "PlanStepParameterEditDlg.h"

namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

DataPointValueStepParameter::DataPointValueStepParameter(PlanStep& parent, const std::string& name, const int valueType, std::string& value) :
PlanStepParameter(parent,name, value),
m_valueType(valueType)
{
    FUNCTION_ENTRY( "DataPointValueStepParameter" );
    FUNCTION_EXIT;
}

DataPointValueStepParameter::~DataPointValueStepParameter()
{
    FUNCTION_ENTRY( "~DataPointValueStepParameter" );
    FUNCTION_EXIT;
}

const std::string DataPointValueStepParameter::displayValue() const
{
    FUNCTION_ENTRY( "getDataPointValueAsString" );

    switch (m_valueType)
    {
        case BOOLEAN_DATA_POINT:
        case NUMERIC_DATA_POINT:
        case TEXT_DATA_POINT:
            FUNCTION_EXIT;
            return m_refSrcValue;

		case UNDEFINED_DATA_POINT:
        default:
            // Log ERROR
            break;
    }

    FUNCTION_EXIT;
    return "Invalid Value Type";
}

void DataPointValueStepParameter::setDataPointValueType(const int valueType)
{
	FUNCTION_ENTRY( "setDataPointValueType" );

	m_valueType = valueType;

	FUNCTION_EXIT;
}

std::vector<std::string> DataPointValueStepParameter::getStepParameterValues()
{
	std::vector<std::string> parametersVect;
	parametersVect.push_back(displayValue());
	return parametersVect;
}

IPlanStepParameterForm* DataPointValueStepParameter::getEditDialog( bool canEdit )
{
    return NULL;
}

}