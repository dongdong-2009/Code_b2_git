//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/StationPAStep.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "StdAfx.h"

#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "StationPAStep.h"
#include "core/exceptions/src/PlanStepException.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/StationStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/NumericStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/YesNoStepParameter.h"
#include "PAMessageStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeSpanStepParameter.h"
#include "PlanStepStationPAPage.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep
StationPAStep::StationPAStep(IPlanNode* parent, BasicParameter &stepDetail) :
    PlanStep(parent, m_params, false),
    m_station(NULL),
    m_duration(NULL),
    m_interval(NULL),
    m_chime(NULL),
    m_cyclic(NULL),
    m_TISSynch(NULL),
    m_message(NULL),
    m_zones(NULL),
	m_params(stepDetail),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "StationPAStep" );
    FUNCTION_EXIT;
}


StationPAStep::StationPAStep(IPlanNode* parent, const unsigned long stepId, const int stepType) :
    PlanStep(parent,m_params,true),
    m_station(NULL),
	m_duration(NULL),
	m_interval(NULL),
	m_chime(NULL),
	m_cyclic(NULL),
	m_TISSynch(NULL),
	m_message(NULL),
    m_zones(NULL),
	m_params(stepId, stepType),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "StationPAStep" );
    FUNCTION_EXIT;
}


StationPAStep::StationPAStep(const StationPAStep& planStep) :
    PlanStep(planStep.m_parent, m_params, true),
    m_station(NULL),
    m_duration(NULL),
    m_interval(NULL),
    m_chime(NULL),
    m_cyclic(NULL),
    m_TISSynch(NULL),
    m_message(NULL),
    m_zones(NULL),
    m_params(planStep.m_params),
	m_parActivityParameter(m_params)
{
	FUNCTION_ENTRY( "StationPAStep" );

	createPlanStepParameters();

    FUNCTION_EXIT;
}


StationPAStep::~StationPAStep()
{
    FUNCTION_ENTRY( "~StationPAStep" );

	delete m_station;
	delete m_duration;
	delete m_interval;
	delete m_chime;
	delete m_cyclic;
	delete m_TISSynch;
	delete m_message;

    FUNCTION_EXIT;
}


PlanStep* StationPAStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new StationPAStep(*this);
}

void StationPAStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_station;
    delete m_duration;
    delete m_interval;
    delete m_chime;
    delete m_cyclic;
    delete m_TISSynch;
    delete m_message;

    m_station    = new StationStepParameter    (*this, ParamStation, m_params.locationKey, m_params.station, m_params.allStations);
    m_duration     = new TimeSpanStepParameter    (*this, ParamDuration, m_params.duration, TimeSpanStepParameter::TR_SECONDS, true);
    m_interval     = new NumericStepParameter    (*this, ParamInterval, m_params.interval);
    m_chime         = new YesNoStepParameter        (*this, ParamChime, m_params.chime);
    m_cyclic         = new YesNoStepParameter        (*this, ParamCyclic, m_params.cyclic);
    m_TISSynch   = new YesNoStepParameter     (*this, ParamTISSynch, m_params.TISSynch);
	m_message    = new PaMessageStepParameter (*this, ParamMessage, m_params.message);
	m_zones         = new PaZonesStepParameter    (*this, ParamZones,m_params.zones);

    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* StationPAStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepStationPAPage (this);
}

bool StationPAStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t StationPAParameterSize = 9u;

	bool bValid = (StationPAParameterSize == parameters.size( ));
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for station PA step [%u].", parameters.size(), StationPAParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

std::string StationPAStep::getStepTypeName() const
{
	return "Station public address";
}

IActivityParameter& StationPAStep::getActivityParameter()
{
    reloadParameters();
	return m_parActivityParameter;
}

}
