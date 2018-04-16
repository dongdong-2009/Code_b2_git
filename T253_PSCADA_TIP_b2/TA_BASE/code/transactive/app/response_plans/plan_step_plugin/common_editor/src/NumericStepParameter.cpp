/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/NumericStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/06/03 09:20:18 $
  * Last modified by:  $Author: huangjian $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DataConversion.h"
#include "NumericStepParameter.h"
#include "PlanStepParameterNumericEditDlg.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

NumericStepParameter::NumericStepParameter(PlanStep& parent, const std::string& name, unsigned long& value, bool isHidden) :
PlanStepParameter( parent, name, value ),
m_isHiddenValue( isHidden )
{
    FUNCTION_ENTRY( "NumericStepParameter" );
    FUNCTION_EXIT;
}


NumericStepParameter::~NumericStepParameter()
{
    FUNCTION_ENTRY( "~NumericStepParameter" );
    FUNCTION_EXIT;
}

IPlanStepParameterForm* NumericStepParameter::getEditDialog( bool canEdit )
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new CPlanStepParameterNumericEditDlg(*this, canEdit);
}

void NumericStepParameter::hideValue(bool isHidden)
{
    FUNCTION_ENTRY( "hideValue" );

    m_isHiddenValue = isHidden;

    FUNCTION_EXIT;
}

std::vector<std::string> NumericStepParameter::getStepParameterValues()
{
	std::vector<std::string> parametersVect;
	parametersVect.push_back(displayValue());
	return parametersVect;
}

bool NumericStepParameter::isHiddenValue() const
{
    return m_isHiddenValue;
}

const std::string NumericStepParameter::displayValue() const
{
    return isHiddenValue() ? "" : TA_Base_Core::DataConversion::toString( m_refSrcValue );
}

}