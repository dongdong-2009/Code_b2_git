/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/StationTisStep.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Represents a Station Traveller Info System step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#if defined(_MSC_VER)
    #pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "StationTisStep.h"
#include "resource.h"
#include "StationStepParameter.h"
#include "TisPidListStepParameter.h"
#include "TisMessageStepParameter.h"
#include "TisPriorityStepParameter.h"
#include "TimeSpanStepParameter.h"
#include "TextualStepParameter.h"
#include "PlanStepEditStationTisPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// StationTisStep

StationTisStep::StationTisStep(PlanNode* parent,const TA_Base_Core::StepDetail& stepDetail) :
    PlanStep(parent,stepDetail),
    m_stationParameter(NULL),
    m_stationLevelParameter(NULL),
    m_pidListParameter(NULL),
    m_messageParameter(NULL),
    m_durationParameter(NULL),
    m_priorityParameter(NULL)
{
    FUNCTION_ENTRY( "StationTisStep" );
    FUNCTION_EXIT;
}


StationTisStep::StationTisStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP),
    m_stationParameter(NULL),
    m_stationLevelParameter(NULL),
    m_pidListParameter(NULL),
    m_messageParameter(NULL),
    m_durationParameter(NULL),
    m_priorityParameter(NULL)
{
    FUNCTION_ENTRY( "StationTisStep" );
    FUNCTION_EXIT;
}


StationTisStep::StationTisStep(const StationTisStep& planStep) :
    PlanStep(planStep),
    m_stationParameter(NULL),
    m_stationLevelParameter(NULL),
    m_pidListParameter(NULL),
    m_messageParameter(NULL),
    m_durationParameter(NULL),
    m_priorityParameter(NULL)
{
    FUNCTION_ENTRY( "StationTisStep" );

    m_stationParameter = new StationStepParameter(*this, *planStep.m_stationParameter);
    m_stationLevelParameter = new TextualStepParameter(*this, *planStep.m_stationLevelParameter);
    m_pidListParameter = new TisPidListStepParameter(*this, *planStep.m_pidListParameter);
    m_messageParameter = new TisMessageStepParameter(*this, *planStep.m_messageParameter);
    m_priorityParameter = new TisPriorityStepParameter(*this, *planStep.m_priorityParameter);
    m_durationParameter = new TimeSpanStepParameter(*this, *planStep.m_durationParameter);

    addPlanStepParameter(*m_stationParameter);
    addPlanStepParameter(*m_stationLevelParameter);
    addPlanStepParameter(*m_pidListParameter);
    addPlanStepParameter(*m_messageParameter);
    addPlanStepParameter(*m_priorityParameter);
    addPlanStepParameter(*m_durationParameter);

    FUNCTION_EXIT;
}


StationTisStep::~StationTisStep()
{
    FUNCTION_ENTRY( "~StationTisStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* StationTisStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new StationTisStep(*this);
}


void StationTisStep::createPlanStepParameters(PlanStepParameterList& stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    m_stationParameter = new StationStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_STATION));
    m_stationLevelParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_STATION_LEVEL), "");
    m_pidListParameter = new TisPidListStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_PID_LIST));
    m_messageParameter = new TisMessageStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_MESSAGE), TisMessageStepParameter::MT_STATION);
    m_priorityParameter = new TisPriorityStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_PRIORITY), 0, true);
    m_durationParameter = new TimeSpanStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_DURATION), 0, TimeSpanStepParameter::TR_SECONDS, true);

    stepParameters.clear();
    stepParameters.push_back(m_stationParameter);
    stepParameters.push_back(m_stationLevelParameter);
    stepParameters.push_back(m_pidListParameter);
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_priorityParameter);
    stepParameters.push_back(m_durationParameter);

    FUNCTION_EXIT;
}


void StationTisStep::createPlanStepParameters(const TA_Base_Core::StepParameters& parameterSet,
                                              PlanStepParameterList& stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for Station TIS step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP));
    }

    const TA_Base_Core::StationTravellerInfoSystemParameters& stationTisParameter = parameterSet.stationTravellerInfoSystem();

    m_stationParameter = new StationStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_STATION), stationTisParameter.destination);
    m_stationLevelParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_STATION_LEVEL), stationTisParameter.stationLevel.in());
    m_pidListParameter = new TisPidListStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_PID_LIST), stationTisParameter.stationPids);
    m_messageParameter = new TisMessageStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_MESSAGE), stationTisParameter.message, TisMessageStepParameter::MT_STATION);
    m_priorityParameter = new TisPriorityStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_PRIORITY), stationTisParameter.priority);
    m_durationParameter = new TimeSpanStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_DURATION), stationTisParameter.duration, TimeSpanStepParameter::TR_SECONDS);

    stepParameters.clear();
    stepParameters.push_back(m_stationParameter);
    stepParameters.push_back(m_stationLevelParameter);
    stepParameters.push_back(m_pidListParameter);
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_priorityParameter);
    stepParameters.push_back(m_durationParameter);

    FUNCTION_EXIT;
}


void StationTisStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::StationTravellerInfoSystemParameters stationTisParameter;

    stationTisParameter.destination = m_stationParameter->getDestination();
    stationTisParameter.stationLevel = CORBA::string_dup(m_stationLevelParameter->getMessageText().c_str());
    stationTisParameter.stationPids = m_pidListParameter->getPids();
    stationTisParameter.message = m_messageParameter->getMessage();
    stationTisParameter.priority = m_priorityParameter->getPriority();
    stationTisParameter.duration = m_durationParameter->getNumericValue();

    step.parameters.stationTravellerInfoSystem(stationTisParameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* StationTisStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditStationTisPage(this);
}


#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)
