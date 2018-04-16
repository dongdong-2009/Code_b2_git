/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointTimeCheckStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_DATA_POINT_TIME_CHECK)

#include "core/utilities/src/TAAssert.h"
#include "resource.h"
#include "DataPointTimeCheckStep.h"
#include "DataPointStepParameter.h"
#include "StepNumberStepParameter.h"
#include "TimeSpanStepParameter.h"
#include "PlanStepEditDataPointTimeCheckPage.h"
#include "core/exceptions/src/PlanStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

DataPointTimeCheckStep::DataPointTimeCheckStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent,stepDetail),
    m_dataPoint(NULL),
    m_timeSpanParameter(NULL),
    m_successParameter(NULL),
    m_failureParameter(NULL)
{
    FUNCTION_ENTRY( "DataPointTimeCheckStep" );
    FUNCTION_EXIT;
}


DataPointTimeCheckStep::DataPointTimeCheckStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::DATA_POINT_TIME_CHECK_STEP),
    m_dataPoint(NULL),
    m_timeSpanParameter(NULL),
    m_successParameter(NULL),
    m_failureParameter(NULL)
{
    FUNCTION_ENTRY( "DataPointTimeCheckStep" );
    FUNCTION_EXIT;
}


DataPointTimeCheckStep::DataPointTimeCheckStep(const DataPointTimeCheckStep& planStep) :
    PlanStep(planStep),
    m_dataPoint(NULL),
    m_timeSpanParameter(NULL),
    m_successParameter(NULL),
    m_failureParameter(NULL)
{
    FUNCTION_ENTRY( "DataPointTimeCheckStep" );

    m_dataPoint = new DataPointStepParameter(*this, *planStep.m_dataPoint);
    m_timeSpanParameter = new TimeSpanStepParameter(*this, *planStep.m_timeSpanParameter);
    m_successParameter = new StepNumberStepParameter(*this, *planStep.m_successParameter);
    m_failureParameter = new StepNumberStepParameter(*this, *planStep.m_failureParameter);

    addPlanStepParameter(*m_dataPoint);
    addPlanStepParameter(*m_timeSpanParameter);
    addPlanStepParameter(*m_successParameter);
    addPlanStepParameter(*m_failureParameter);

    FUNCTION_EXIT;
}


DataPointTimeCheckStep::~DataPointTimeCheckStep()
{
    FUNCTION_ENTRY( "~DataPointTimeCheckStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* DataPointTimeCheckStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new DataPointTimeCheckStep(*this);
}


void DataPointTimeCheckStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_dataPoint;
    delete m_timeSpanParameter;
    delete m_successParameter;
    delete m_failureParameter;

    m_dataPoint = new DataPointStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NAME), "");
    m_timeSpanParameter = new TimeSpanStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIME_SPAN), 0, TimeSpanStepParameter::TR_SECONDS);
    m_successParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_SUCCESS_STEP), 0);
    m_failureParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_FAILURE_STEP), 0);

    stepParameters.clear();
    stepParameters.push_back(m_dataPoint);
    stepParameters.push_back(m_timeSpanParameter);
    stepParameters.push_back(m_successParameter);
    stepParameters.push_back(m_failureParameter);

    FUNCTION_EXIT;
}


void DataPointTimeCheckStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                                  PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::DATA_POINT_TIME_CHECK_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for data point time check step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::DATA_POINT_TIME_CHECK_STEP));
    }
    delete m_dataPoint;
    delete m_timeSpanParameter;
    delete m_successParameter;
    delete m_failureParameter;

    m_dataPoint = new DataPointStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_DATA_POINT_NAME), parameterSet.dataPointTimeCheck().dataPoint.in());
    m_timeSpanParameter = new TimeSpanStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIME_SPAN), parameterSet.dataPointTimeCheck().timeSpan, TimeSpanStepParameter::TR_SECONDS);
    m_successParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_SUCCESS_STEP), parameterSet.dataPointTimeCheck().successStep);
    m_failureParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_FAILURE_STEP), parameterSet.dataPointTimeCheck().failureStep);

    stepParameters.clear();
    stepParameters.push_back(m_dataPoint);
    stepParameters.push_back(m_timeSpanParameter);
    stepParameters.push_back(m_successParameter);
    stepParameters.push_back(m_failureParameter);

    FUNCTION_EXIT;
}


void DataPointTimeCheckStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::DataPointTimeCheckParameters  parameter;

    parameter.dataPoint = CORBA::string_dup(m_dataPoint->getDataPoint().c_str());
    parameter.timeSpan = m_timeSpanParameter->getNumericValue();
    parameter.successStep = m_successParameter->getStepId();
    parameter.failureStep = m_failureParameter->getStepId();

    step.parameters.dataPointTimeCheck(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* DataPointTimeCheckStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditDataPointTimeCheckPage(this);
}


#endif // defined (STEPTYPE_DATA_POINT_TIME_CHECK)
