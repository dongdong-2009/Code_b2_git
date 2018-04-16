#ifndef __BASE_STEP_PARAMETER_INCLUDE_H_
#define __BASE_STEP_PARAMETER_INCLUDE_H_

#include "app/response_plans/plan_step_plugin/common/src/BasicParameter.h"
#include "app/response_plans/plan_step_plugin/common/src/SerializableParameter.h"

namespace TA_Base_App
{
    class BaseStepParameter : public SerializableParameter
    {
    public:
        BaseStepParameter( const BasicParameter& refSrc );
		BaseStepParameter( const unsigned long stepId, const int stepType );
        virtual ~BaseStepParameter();
        
    public:
        BasicParameter basic;
        bool updated;
    };
}


#endif //__BASE_STEP_PARAMETER_INCLUDE_H_