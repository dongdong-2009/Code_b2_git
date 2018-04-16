/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/TextualStepParameter.cpp $
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
#include "PlanStepParameterTextualEditDlg.h"
#include "TextualStepParameter.h"


namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

TextualStepParameter::TextualStepParameter(PlanStep& parent, const std::string& name, std::string& text) :
PlanStepParameter(parent, name, text)
{
    FUNCTION_ENTRY( "TextualStepParameter" );
    FUNCTION_EXIT;
}

TextualStepParameter::~TextualStepParameter()
{
    FUNCTION_ENTRY( "~TextualStepParameter" );
    FUNCTION_EXIT;
}

IPlanStepParameterForm* TextualStepParameter::getEditDialog( bool canEdit )
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new CPlanStepParameterTextualEditDlg(*this,canEdit);
}

std::vector<std::string> TextualStepParameter::getStepParameterValues()
{
	std::vector<std::string> parametersVect;
	parametersVect.push_back(m_refSrcValue.c_str());
	return parametersVect;
}

const std::string TextualStepParameter::displayValue() const
{
    return m_refSrcValue.c_str();
}

}