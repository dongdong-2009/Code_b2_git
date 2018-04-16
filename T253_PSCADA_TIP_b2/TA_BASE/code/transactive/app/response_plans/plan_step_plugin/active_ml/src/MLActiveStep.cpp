/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/active_ml/src/MLActiveStep.cpp $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/22 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Base class for ML Algorithms' step classes.
*/


#include "app/response_plans/plan_step_plugin/active_ml/src/MLActiveStep.h"

#include "core/utilities/src/DebugUtil.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"

 
namespace TA_Base_App
{

	MLActiveStep::MLActiveStep( BaseStepParameter& refParameter, IActivePlan& flowControl ) : ActiveStep(refParameter, flowControl)
	{
	}

	MLActiveStep::~MLActiveStep()
	{
	}

	void MLActiveStep::initImpl()
	{
		// Any common initialisation goes here:
	}

} // TA_Base_App
