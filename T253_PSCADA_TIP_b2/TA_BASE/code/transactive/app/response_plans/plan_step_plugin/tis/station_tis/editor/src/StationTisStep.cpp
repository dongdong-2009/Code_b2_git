/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/StationTisStep.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2014/10/16 13:13:15 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Represents a Station Traveller Info System step.
  *
  **/

#include "StdAfx.h"

#if defined(_MSC_VER)
    #pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "StationTisStep.h"
#include "resource.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/StationStepParameter.h"
#include "TisPidListStepParameter.h"
#include "TisMessageStepParameter.h"
#include "TisPriorityStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeSpanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TextualStepParameter.h"
#include "PlanStepEditStationTisPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// StationTisStep

StationTisStep::StationTisStep(IPlanNode* parent, BasicParameter& stepDetail) :
    PlanStep(parent,m_params,false),
    m_stationParameter(NULL),
    m_pidListParameter(NULL),
    m_messageParameter(NULL),
    m_durationParameter(NULL),
    m_priorityParameter(NULL),
	m_params(stepDetail),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "StationTisStep" );
    FUNCTION_EXIT;
}


StationTisStep::StationTisStep(IPlanNode* parent,const unsigned long stepId, const int stepType) :
    PlanStep(parent,m_params,true),
    m_stationParameter(NULL),
    m_pidListParameter(NULL),
    m_messageParameter(NULL),
    m_durationParameter(NULL),
    m_priorityParameter(NULL),
	m_params(stepId, stepType),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "StationTisStep" );
    FUNCTION_EXIT;
}


StationTisStep::StationTisStep(const StationTisStep& planStep) :
    PlanStep(planStep.m_parent, m_params, true),
    m_stationParameter(NULL),
    m_pidListParameter(NULL),
    m_messageParameter(NULL),
    m_durationParameter(NULL),
    m_priorityParameter(NULL),
	m_params(planStep.m_params),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "StationTisStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


StationTisStep::~StationTisStep()
{
    FUNCTION_ENTRY( "~StationTisStep" );

	delete m_stationParameter;
	delete m_pidListParameter;
	delete m_messageParameter;
	delete m_priorityParameter;
	delete m_durationParameter;

    FUNCTION_EXIT;
}


PlanStep* StationTisStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new StationTisStep(*this);
}


void StationTisStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

	delete m_stationParameter;
	delete m_pidListParameter;
	delete m_messageParameter;
	delete m_priorityParameter;
	delete m_durationParameter;

	m_stationParameter = new StationStepParameter(*this, ParamStation, m_params.locationKey, m_params.station, m_params.allStations);
	m_pidListParameter = new TisPidListStepParameter(*this, ParamStationPids, m_params.stationPids);
	m_messageParameter = new TisMessageStepParameter(*this, ParamMessage, m_params.message, MT_STATION);
	m_priorityParameter = new TisPriorityStepParameter(*this, ParamPriority, m_params.priority);
	m_durationParameter = new TimeSpanStepParameter(*this, ParamDuration, m_params.duration, TimeSpanStepParameter::TR_SECONDS);

    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* StationTisStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditStationTisPage(this);
}

bool StationTisStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t StationTisParameterSize = 6u;

	bool bValid = (StationTisParameterSize == parameters.size( ));
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for station tis step [%u].", parameters.size(), StationTisParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

std::string StationTisStep::getStepTypeName() const
{
	return "Station traveler system";
}

IActivityParameter& TA_Base_App::StationTisStep::getActivityParameter()
{
    reloadParameters();
	return m_parActivityParameter;
}
