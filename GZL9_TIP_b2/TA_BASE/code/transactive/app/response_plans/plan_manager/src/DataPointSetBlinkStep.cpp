//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointSetBlinkStep.cpp $
// @author:  Liu Yong
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2016/11/6 13:35:44 $
// Last modified by:  $Author: builder $
//
// This step is used for set PSCADA control datapoint blink control status in its holding plan. 

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "DataPointSetBlinkStep.h"
#include "core/exceptions/src/PlanStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

DataPointSetBlinkStep::DataPointSetBlinkStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail)
  : PlanStep(parent,stepDetail)
{
    FUNCTION_ENTRY( "DataPointSetBlinkStep" );
    FUNCTION_EXIT;
}

DataPointSetBlinkStep::DataPointSetBlinkStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::DATA_POINT_SET_BLINK_STEP)
{
    FUNCTION_ENTRY( "DataPointSetBlinkStep" );
    FUNCTION_EXIT;
}

DataPointSetBlinkStep::DataPointSetBlinkStep(const DataPointSetBlinkStep& planStep)
      : PlanStep(planStep)
{
    FUNCTION_ENTRY( "DataPointSetBlinkStep" );
    FUNCTION_EXIT;
}


DataPointSetBlinkStep::~DataPointSetBlinkStep()
{
    FUNCTION_ENTRY( "~DataPointSetBlinkStep" );
    FUNCTION_EXIT;
}


PlanStep* DataPointSetBlinkStep::clone()
{
    FUNCTION_ENTRY( "clone" );
    FUNCTION_EXIT;
    return new DataPointSetBlinkStep(*this);
}


void DataPointSetBlinkStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}


void DataPointSetBlinkStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                                PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::DATA_POINT_SET_BLINK_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for plan end step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::DATA_POINT_SET_BLINK_STEP));
    }

    FUNCTION_EXIT;
}


void DataPointSetBlinkStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    step.parameters.dataPointSetBlink(0);

    FUNCTION_EXIT;
}


bool DataPointSetBlinkStep::canDeleteStep()
{
    FUNCTION_ENTRY( "canDeleteStep" );
    FUNCTION_EXIT;
    return true;
}

/*
void DataPointSetBlinkStep::addToStepList(CSOCPlanStepListCtrl& listCtrl)
{
	FUNCTION_ENTRY( "addToStepList" );

	listCtrl.AddStep(m_stepDetail, this, true);	//TODO SOC a wrong output value

	FUNCTION_EXIT;
}
*/