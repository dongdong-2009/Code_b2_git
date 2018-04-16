/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableLaunchSchematicStep.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Facilitates the launching of a display when executing a Launch Schematic step.
  *
  */

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "PlanManager.h"
#include "ExecutableLaunchSchematicStep.h"
#include "LaunchSchematicDlg.h"
#include "InteractiveStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// ExecutableLaunchSchematicStep

ExecutableLaunchSchematicStep::ExecutableLaunchSchematicStep(const TA_Base_Bus::IPlanManagerCorbaDef::CurrentPlanStepDetail& stepDetail) :
    InteractiveStep(stepDetail)
{
    FUNCTION_ENTRY( "ExecutableLaunchSchematicStep" );

    TA_ASSERT(stepDetail.data._d() == TA_Base_Core::LAUNCH_SCHEMATIC_STEP, "Invalid step parameter type");

    FUNCTION_EXIT;
}


ExecutableLaunchSchematicStep::~ExecutableLaunchSchematicStep()
{
    FUNCTION_ENTRY( "~ExecutableLaunchSchematicStep" );
    FUNCTION_EXIT;
}


void ExecutableLaunchSchematicStep::launch()
{
    FUNCTION_ENTRY( "launch" );

    // Prompt the user to acknowledge the launch of display.
    CLaunchSchematicDlg* launchSchematicDlg = new CLaunchSchematicDlg(getStepDetail());

    if (launchSchematicDlg->Create(IDD_LAUNCH_SCHEMATIC_DLG) == 0)
    {
        TA_THROW(InteractiveStepException("Failed to create Launch Schematic step confirmation dialog"));
    }

    FUNCTION_EXIT;
}


