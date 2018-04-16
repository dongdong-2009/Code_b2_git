#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"

namespace TA_Base_App
{
    BaseStepParameter::BaseStepParameter( const BasicParameter& refSrc ) :
    basic( refSrc ),
    updated( false )
    {
    }

	BaseStepParameter::BaseStepParameter( const unsigned long stepId, const int stepType ) :
    basic( stepType, stepId, 0u, 0u, 0u, 0u, "New Step", "New Step", "", false, false, false ),
    updated( true )
	{
	}

    BaseStepParameter::~BaseStepParameter()
    {
    }
}