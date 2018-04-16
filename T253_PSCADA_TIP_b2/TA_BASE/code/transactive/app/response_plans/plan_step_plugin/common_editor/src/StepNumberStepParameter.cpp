/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/StepNumberStepParameter.cpp $
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
#include "core/utilities/src/DataConversion.h"
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameterStepNumberEditDlg.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/StepNumberStepParameter.h"

namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

StepNumberStepParameter::StepNumberStepParameter(PlanStep& parent, const std::string& name, unsigned int& stepId) :
PlanStepParameter( parent, name, stepId )
{
    FUNCTION_ENTRY( "StepNumberStepParameter" );
    FUNCTION_EXIT;
}

StepNumberStepParameter::~StepNumberStepParameter()
{
    FUNCTION_ENTRY( "~StepNumberStepParameter" );
    FUNCTION_EXIT;
}

IPlanStepParameterForm* StepNumberStepParameter::getEditDialog( bool canEdit )
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new CPlanStepParameterStepNumberEditDlg(*this,canEdit);
}

std::vector<std::string> StepNumberStepParameter::getStepParameterValues()
{
	std::vector<std::string> parametersVect;
	parametersVect.push_back(displayValue().c_str());
	return parametersVect;
}

const std::string StepNumberStepParameter::displayValue() const
{
    FUNCTION_ENTRY( "getDestinationStepDescription" );

    std::string   stepDescription = "";

    // Get the step to which we will be going to
    PlanStep* destinationStep = getParentStep()->getParentPlan()->getPlanStepByStepId( m_refSrcValue );

    if ( destinationStep != NULL )
    {
        unsigned long stepNumber = destinationStep->getStepPosition();

        // Return step "<position> - <step name>"
        stepDescription = TA_Base_Core::DataConversion::toString( stepNumber );
        if ( stepNumber > 0 )
        {
            stepDescription += " - ";
            stepDescription += destinationStep->getStepName();
        }
    }

    FUNCTION_EXIT;
    return stepDescription;
}

}
