/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/YesNoStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/06/03 09:20:18 $
  * Last modified by:  $Author: huangjian $
  *
  * <description>
  *
  **/

#include "core/utilities/src/DebugUtil.h"
#include "PlanStepParameterYesNoEditDlg.h"
#include "YesNoStepParameter.h"

namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

YesNoStepParameter::YesNoStepParameter( PlanStep& parent, const std::string& name, bool& affirmative) :
PlanStepParameter( parent, name, affirmative )
{
    FUNCTION_ENTRY( "YesNoStepParameter" );
    FUNCTION_EXIT;
}

YesNoStepParameter::~YesNoStepParameter()
{
    FUNCTION_ENTRY( "~YesNoStepParameter" );
    FUNCTION_EXIT;
}

IPlanStepParameterForm* YesNoStepParameter::getEditDialog( bool canEdit )
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new CPlanStepParameterYesNoEditDlg(*this,canEdit);
}

std::vector<std::string> YesNoStepParameter::getStepParameterValues()
{
	std::vector<std::string> parametersVect;
	parametersVect.push_back(displayValue());
	return parametersVect;
}

const std::string YesNoStepParameter::displayValue() const
{
    return m_refSrcValue ? "Yes" : "No";
}

}