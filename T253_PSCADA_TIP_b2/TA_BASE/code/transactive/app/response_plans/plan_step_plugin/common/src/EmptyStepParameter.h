#ifndef EMPTYPARAMETER_INCLUDE
#define EMPTYPARAMETER_INCLUDE

#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"

namespace TA_Base_App
{
	class EmptyStepParameter : public BaseStepParameter
	{
	public:
        EmptyStepParameter( const BasicParameter& detail ) : BaseStepParameter( detail ) {}
		EmptyStepParameter(const unsigned long stepId, const int stepType) : BaseStepParameter(stepId, stepType){}
        virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization) {}
	};
}

#endif