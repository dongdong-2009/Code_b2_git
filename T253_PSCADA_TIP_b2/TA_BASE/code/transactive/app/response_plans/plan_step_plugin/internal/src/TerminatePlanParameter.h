#ifndef __TERMINATE_PLAN_PARAMETER_INCLUDE_H_
#define __TERMINATE_PLAN_PARAMETER_INCLUDE_H_

#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"

namespace TA_Base_App
{
    const char* const ParamPlan = "Plan";

    class TerminatePlanParameter: public BaseStepParameter
    {
    public:
        TerminatePlanParameter( const BasicParameter& detail ) : BaseStepParameter( detail ),
        plan( 0u )
        {
        }

		TerminatePlanParameter( const unsigned long stepId, const int stepType ): BaseStepParameter( stepId, stepType ),
		plan( 0u )
        {
        }

        ~TerminatePlanParameter() {}

    protected:
        virtual void serializeImp( TA_Base_Bus::Serialization& ioSerialization)
        {
            ioSerialization.serialize( plan, ParamPlan );
        }

    private:
        //TerminatePlanParameter(const TerminatePlanParameter&);
        TerminatePlanParameter& operator=(const TerminatePlanParameter&);

    public:
        unsigned long plan;
    };
}

#endif //__TERMINATE_PLAN_PARAMETER_INCLUDE_H_