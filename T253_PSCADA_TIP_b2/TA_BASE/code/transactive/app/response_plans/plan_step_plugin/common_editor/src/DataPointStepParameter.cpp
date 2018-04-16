/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/


#include "core/utilities/src/DebugUtil.h"
#include "PlanStepParameterDataPointEditDlg.h"
#include "DataPointStepParameter.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

DataPointStepParameter::DataPointStepParameter(PlanStep& parent, const std::string& name, std::string& dataPoint) :
PlanStepParameter( parent, name, dataPoint )
{
    FUNCTION_ENTRY( "DataPointStepParameter" );
    FUNCTION_EXIT;
}

DataPointStepParameter::~DataPointStepParameter()
{
    FUNCTION_ENTRY( "~DataPointStepParameter" );
    FUNCTION_EXIT;
}

IPlanStepParameterForm* DataPointStepParameter::getEditDialog( bool canEdit )
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new CPlanStepParameterDataPointEditDlg(*this,canEdit);
}

std::vector<std::string> DataPointStepParameter::getStepParameterValues()
{
	std::vector<std::string> parametersVect;
	parametersVect.push_back(m_refSrcValue);
    return parametersVect;
}

const std::string DataPointStepParameter::displayValue() const
{
    return m_refSrcValue.c_str();
}

}