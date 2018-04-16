#ifndef __RUN_PLAN_PARAMETER_INCLUDE_H_
#define __RUN_PLAN_PARAMETER_INCLUDE_H_

#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "bus/response_plans/PlanDataAccess/src/Serialization.h"

namespace TA_Base_App
{
	const char* const ParamPlan = "Plan";
	const char* const ParamManualLaunch = "Manual Launch";
	const char* const ParamWaitForCompletion = "Wait For Completion";

    class RunPlanParameter: public BaseStepParameter
    {
    public:
        RunPlanParameter( const BasicParameter& detail ) : BaseStepParameter( detail ),
        plan( 0u ),
        manualLaunch( false ),
        waitForCompletion( false )
        {
        }

        RunPlanParameter( const unsigned long stepId, const int stepType ) : BaseStepParameter( stepId, stepType ),
        plan( 0u ),
        manualLaunch( false ),
        waitForCompletion( false )
        {
        }

        virtual ~RunPlanParameter() {}

    public:
        unsigned long plan;
        bool manualLaunch;
        bool waitForCompletion;

    protected:
        virtual void serializeImp( TA_Base_Bus::Serialization& ioSerialization)
        {
            ioSerialization.serialize( plan, ParamPlan );
            ioSerialization.serialize( manualLaunch, ParamManualLaunch );
            ioSerialization.serialize( waitForCompletion, ParamWaitForCompletion );
        }

    private:
        //RunPlanParameter(const RunPlanParameter&);
        RunPlanParameter& operator=(const RunPlanParameter&);

    };
}

#endif //__RUN_PLAN_PARAMETER_INCLUDE_H_