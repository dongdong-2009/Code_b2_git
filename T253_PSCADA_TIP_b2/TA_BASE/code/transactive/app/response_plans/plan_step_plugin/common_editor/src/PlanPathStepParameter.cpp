/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanPathStepParameter.cpp $
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

#if defined(_MSC_VER)
    #pragma warning(disable:4250)
#endif // defined (_MSC_VER)

#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameterEditDlg.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanAgentAccessFactory.h"
#include "PlanStepParameterPlanPathEditDlg.h"
#include "PlanPathStepParameter.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

PlanPathStepParameter::PlanPathStepParameter( PlanStep& parent, const std::string& name, unsigned long& planNodeId ) :
    PlanStepParameter(parent, name, planNodeId)
{
    FUNCTION_ENTRY( "PlanPathStepParameter" );
    FUNCTION_EXIT;
}

PlanPathStepParameter::~PlanPathStepParameter()
{
    FUNCTION_ENTRY( "~PlanPathStepParameter" );
    FUNCTION_EXIT;
}

std::vector<std::string> PlanPathStepParameter::getStepParameterValues()
{
	std::vector<std::string> parametersVect;

    try
    {
        std::string planPath = displayValue();
        parametersVect.push_back(planPath);
    }
    catch(TA_Base_Core::TransactiveException&)
    {
        // Couldn't retrieve plan path. Try adding the parameter with the plan path set
        // to an error message.
        TA_Base_Bus::TransActiveMessage msg;
        CString param;
        param.LoadString(IDS_RETRIEVE_PARAMETER_VALUE);
        msg << param;
        CString error = msg.constructMessage(IDS_UE_010028);
        parametersVect.push_back(error.GetBuffer());
    }

	return parametersVect;
}

IPlanStepParameterForm* PlanPathStepParameter::getEditDialog( bool canEdit )
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new CPlanStepParameterPlanPathEditDlg(*this,canEdit);
}

const std::string PlanPathStepParameter::displayValue() const
{
    FUNCTION_ENTRY( "getPlanPath" );
    FUNCTION_EXIT;
    return PlanAgentAccessFactory::instance().getNodePath(m_refSrcValue);
}

}