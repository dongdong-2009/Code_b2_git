/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TrainStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents the Train parameter of a Station TIS/PA step.
  *
  **/

#include <sstream>
#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameterEditDlg.h"
#include "TrainStepParameter.h"


namespace TA_Base_App
{
const unsigned short TIS_UNDEFINED_TRAIN_ID = 0xFFFF;

/////////////////////////////////////////////////////////////////////////////
// TrainStepParameter

TrainStepParameter::TrainStepParameter(PlanStep& parent, const std::string& name, unsigned short& trainId, const ETrainGroup trainGroup) :
PlanStepParameter( parent, name, trainId ),
m_trainGroup(trainGroup)
{
    FUNCTION_ENTRY( "TrainStepParameter" );
    FUNCTION_EXIT;
}

TrainStepParameter::~TrainStepParameter()
{
    FUNCTION_ENTRY( "~TrainStepParameter" );
    FUNCTION_EXIT;
}

void TrainStepParameter::setTrainGroup(ETrainGroup trainGroup)
{
    FUNCTION_ENTRY( "setTrainGroup" );

    m_trainGroup = trainGroup;

    FUNCTION_EXIT;
}

IPlanStepParameterForm* TrainStepParameter::getEditDialog( bool canEdit )
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new CPlanStepParameterEditDlg(*this, canEdit);
}

std::vector<std::string> TrainStepParameter::getStepParameterValues()
{
	std::ostringstream destStr;

	if (m_trainGroup == TG_UNDEFINED)
	{
		if (m_trainId != TIS_UNDEFINED_TRAIN_ID)
		{
			destStr << "ID " << m_trainId;
		}
	}
	else
	{
		destStr << convertToString(m_trainGroup);
	}

	std::vector<std::string> parametersVect;
	parametersVect.push_back(destStr.str());
	return parametersVect;
}

ETrainGroup TrainStepParameter::getTrainGroup() const
{
	return m_trainGroup;
}

const std::string TrainStepParameter::convertToString(const ETrainGroup trainGroup) const
{
	FUNCTION_ENTRY( "convertToString" );

	switch (trainGroup)
	{
	case TG_INBOUND_TRAINS:
		FUNCTION_EXIT;
		return "Inbound trains";

	case TG_OUTBOUND_TRAINS:
		FUNCTION_EXIT;
		return "Outbound trains";

	case TG_ALL_TRAINS:
		FUNCTION_EXIT;
		return "All trains";

	default:
		;
	}

	FUNCTION_EXIT;
	return "Undefined train group";
}

const std::string TrainStepParameter::displayValue() const
{
	std::ostringstream destStr;

	if (m_trainGroup == TG_UNDEFINED)
	{
		if (m_trainId != TIS_UNDEFINED_TRAIN_ID)
		{
			destStr << "ID " << m_trainId;
		}
	}
	else
	{
		destStr << convertToString(m_trainGroup);
	}

	return destStr.str();
}

}