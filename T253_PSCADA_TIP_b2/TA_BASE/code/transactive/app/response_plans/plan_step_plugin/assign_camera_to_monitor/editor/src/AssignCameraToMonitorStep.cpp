/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/AssignCameraToMonitorStep.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PlanStepException.h"
#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/editor/src/AssignCameraToMonitorStep.h"
#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/editor/src/PlanStepEditAssignCameraToMonitorPage.h"

namespace TA_Base_App
{
/////////////////////////////////////////////////////////////////////////////
// PlanStep

AssignCameraToMonitorStep::AssignCameraToMonitorStep( IPlanNode* parent, const BasicParameter& stepDetail ) :
    PlanStep( parent, m_params, false ),
    m_params( stepDetail ),
    m_parActivityParameter( m_params )
{
    FUNCTION_ENTRY( "AssignCameraToMonitorStep" );
    FUNCTION_EXIT;
}


AssignCameraToMonitorStep::AssignCameraToMonitorStep( IPlanNode* parent, const unsigned long stepId, const int stepType ) :
    PlanStep( parent, m_params, true ),
    m_params( stepId, stepType ),
    m_parActivityParameter( m_params )
{
    FUNCTION_ENTRY( "AssignCameraToMonitorStep" );
    FUNCTION_EXIT;
}


AssignCameraToMonitorStep::AssignCameraToMonitorStep( const AssignCameraToMonitorStep& planStep ) :
    PlanStep( planStep.m_parent, m_params, true ),
    m_params( planStep.m_params ),
    m_parActivityParameter( m_params )
{
    FUNCTION_ENTRY( "AssignCameraToMonitorStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


AssignCameraToMonitorStep::~AssignCameraToMonitorStep()
{
    FUNCTION_ENTRY( "~AssignCameraToMonitorStep" );
    FUNCTION_EXIT;
}


PlanStep* AssignCameraToMonitorStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new AssignCameraToMonitorStep( *this );
}


void AssignCameraToMonitorStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* AssignCameraToMonitorStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );

    FUNCTION_EXIT;
    return new CPlanStepEditAssignCameraToMonitorPage( this );
}

bool AssignCameraToMonitorStep::isParametersValid( const std::vector<std::string>& parameters )
{
    FUNCTION_ENTRY( "isParametersValid" );

    static const size_t AssignCameraToMonitorParameterSize = 2u;

    bool bValid = ( AssignCameraToMonitorParameterSize == parameters.size() );
    if( !bValid )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for AssignCameraToMonitor step [%u].", parameters.size(), AssignCameraToMonitorParameterSize );
    }

    FUNCTION_EXIT;
    return bValid;
}

std::string AssignCameraToMonitorStep::getStepTypeName() const
{
    return "Assign camera to monitor";
}

IActivityParameter& AssignCameraToMonitorStep::getActivityParameter()
{
    reloadParameters();
    return m_parActivityParameter;
}

} // namespace TA_Base_App
