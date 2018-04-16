//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TrainPAStep.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "StdAfx.h"

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "TrainPAStep.h"
#include "resource.h"
#include "PlanStepEditPATrainPage.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TrainStepParameter.h"
#include "PaMessageStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeSpanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/YesNoStepParameter.h"

namespace TA_Base_App
{

const unsigned long PA_UNDEFINED_MESSAGE_ID = 0;

/////////////////////////////////////////////////////////////////////////////
// PlanStep

TrainPAStep::TrainPAStep(IPlanNode* parent, BasicParameter&stepDetail)
    : PlanStep(parent,m_params,false),
    m_trainParameter (NULL),
    m_messageParameter (NULL),
    m_cyclicParameter (NULL),
    m_durationParameter (NULL),
	m_params(stepDetail),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "TrainPAStep" );
    FUNCTION_EXIT;
}


TrainPAStep::TrainPAStep(IPlanNode* parent,const unsigned long stepId, const int stepType)
   : PlanStep(parent,m_params, true),
    m_trainParameter (NULL),
    m_messageParameter (NULL),
    m_cyclicParameter (NULL),
    m_durationParameter (NULL),
	m_params(stepId, stepType),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "TrainPAStep" );
    FUNCTION_EXIT;
}


TrainPAStep::TrainPAStep(const TrainPAStep& planStep)
   : PlanStep(planStep.m_parent, m_params, true),
    m_trainParameter (NULL),
    m_messageParameter (NULL),
    m_cyclicParameter (NULL),
    m_durationParameter (NULL),
	m_params(planStep.m_params),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "TrainPAStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


TrainPAStep::~TrainPAStep()
{
    FUNCTION_ENTRY( "~TrainPAStep" );

	delete m_trainParameter;
	delete m_messageParameter;
	delete m_cyclicParameter;
	delete m_durationParameter;

    FUNCTION_EXIT;
}


PlanStep* TrainPAStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new TrainPAStep(*this);
}


void TrainPAStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

	delete m_trainParameter;
	delete m_messageParameter;
	delete m_cyclicParameter;
	delete m_durationParameter;

    m_trainParameter    = new TrainStepParameter(*this, ParamTrainId, m_params.trainId, static_cast<ETrainGroup>(m_params.trainGroup));
    m_messageParameter  = new PaMessageStepParameter(*this, ParamMessage, m_params.message, true);
    m_cyclicParameter   = new YesNoStepParameter(*this, ParamCyclic, m_params.cyclic);
    m_durationParameter = new TimeSpanStepParameter(*this, ParamDuration, m_params.duration, TimeSpanStepParameter::TR_MINUTES, true);

    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* TrainPAStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditPATrainPage(this);
}

bool TrainPAStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t TrainPAParameterSize = 5u;

	bool bValid = (TrainPAParameterSize == parameters.size( ));
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for train PA step [%u].", parameters.size(), TrainPAParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

std::string TrainPAStep::getStepTypeName() const
{
	return "Train public address";
}

IActivityParameter& TrainPAStep::getActivityParameter()
{
    reloadParameters();
	return m_parActivityParameter;
}

}
