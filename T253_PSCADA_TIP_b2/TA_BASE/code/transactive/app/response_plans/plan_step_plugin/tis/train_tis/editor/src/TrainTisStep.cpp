/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TrainTisStep.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents a Train Traveller Info System step.
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "TrainTisStep.h"
#include "resource.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TrainStepParameter.h"
#include "TisMessageStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeSpanStepParameter.h"
#include "TisPriorityStepParameter.h"
#include "PlanStepEditTrainTisPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// TrainTisStep

TrainTisStep::TrainTisStep(IPlanNode* parent,const BasicParameter& stepDetail) :
    PlanStep(parent,m_params,false),
    m_trainParameter(NULL),
    m_messageParameter(NULL),
    m_durationParameter(NULL),
    m_priorityParameter(NULL),
	m_params(stepDetail),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "TrainTisStep" );
    FUNCTION_EXIT;
}


TrainTisStep::TrainTisStep(IPlanNode* parent,const unsigned long stepId, const int stepType) :
    PlanStep(parent,m_params,true),
    m_trainParameter(NULL),
    m_messageParameter(NULL),
    m_durationParameter(NULL),
    m_priorityParameter(NULL),
	m_params(stepId, stepType),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "TrainTisStep" );
    FUNCTION_EXIT;
}


TrainTisStep::TrainTisStep(const TrainTisStep& planStep) :
    PlanStep(planStep.m_parent, m_params, true),
    m_trainParameter(NULL),
    m_messageParameter(NULL),
    m_durationParameter(NULL),
    m_priorityParameter(NULL),
	m_params(planStep.m_params),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "TrainTisStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


TrainTisStep::~TrainTisStep()
{
    FUNCTION_ENTRY( "~TrainTisStep" );

	delete m_trainParameter;
	delete m_messageParameter;
	delete m_priorityParameter;
	delete m_durationParameter;

    FUNCTION_EXIT;
}


PlanStep* TrainTisStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new TrainTisStep(*this);
}


void TrainTisStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

	delete m_trainParameter;
	delete m_messageParameter;
	delete m_priorityParameter;
	delete m_durationParameter;

	m_trainParameter = new TrainStepParameter(*this, ParamTrainId, m_params.trainId, static_cast<ETrainGroup>(m_params.trainGroup));
	m_messageParameter = new TisMessageStepParameter(*this, ParamMessage, m_params.message, MT_TRAIN);
	m_priorityParameter = new TisPriorityStepParameter(*this, ParamPriority, m_params.priority);
	m_durationParameter = new TimeSpanStepParameter(*this, ParamDuration, m_params.duration, TimeSpanStepParameter::TR_MINUTES);

    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* TrainTisStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditTrainTisPage(this);
}

bool TrainTisStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t TrainTisParameterSize = 5u;

	bool bValid = (TrainTisParameterSize == parameters.size( ));
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for train tis step [%u].", parameters.size(), TrainTisParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

std::string TrainTisStep::getStepTypeName() const
{
	return "Train traveler system";
}

IActivityParameter& TA_Base_App::TrainTisStep::getActivityParameter()
{
    reloadParameters();
	return m_parActivityParameter;
}
