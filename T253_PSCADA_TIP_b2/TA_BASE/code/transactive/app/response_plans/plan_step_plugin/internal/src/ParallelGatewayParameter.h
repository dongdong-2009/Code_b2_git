#ifndef PARALLELGATEWAY_STEP_PARAMETER_INCLUDE_H
#define PARALLELGATEWAY_STEP_PARAMETER_INCLUDE_H

#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"

namespace TA_Base_App
{
	const char* const ParamIncomingSteps = "Incoming steps";
    const char* const ParamOutgoingSteps = "Outgoing steps";
    const char* const ParamPairForkStep = "Fork pair";
    const char* const ParamPairJoinStep = "Join pair";

    class ParallelGatewayParameter: public BaseStepParameter
    {
    public:
		ParallelGatewayParameter( const BasicParameter& detail ) : BaseStepParameter( detail ) {}
		ParallelGatewayParameter( const unsigned long stepId, const int stepType ) : BaseStepParameter( stepId, stepType ) {}
        ~ParallelGatewayParameter() {};

    protected:
        virtual void serializeImp( TA_Base_Bus::Serialization& ioSerialization )
        {
            ioSerialization.serialize( incomingStepIds, ParamIncomingSteps );
            ioSerialization.serialize( outgoingStepIds, ParamOutgoingSteps );
            ioSerialization.serialize( pairForkStep, ParamPairForkStep );
            ioSerialization.serialize( pairJoinStep, ParamPairJoinStep );
        }

    private:
        ParallelGatewayParameter& operator=(const ParallelGatewayParameter&);

    public:
        std::vector<unsigned int> incomingStepIds;
        std::vector<unsigned int> outgoingStepIds;
        unsigned int pairForkStep;
        unsigned int pairJoinStep;
    };
}

#endif //PARALLELGATEWAY_STEP_PARAMETER_INCLUDE_H