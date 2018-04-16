#ifndef __CONDITIONAL_GATEWAY_PARAMETER_INCLUDE_H_
#define __CONDITIONAL_GATEWAY_PARAMETER_INCLUDE_H_

#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "bus/response_plans/PlanDataAccess/src/Serialization.h"

namespace TA_Base_App
{
    const char* const ParamStepExpressionPairs = "StepExpressionPairs";
    const char* const ParamPairNextStep = "NextStep";
    const char* const ParamPairExpression = "Expression";
    const char* const ParamPairStep = "Pair";

    struct ExpressionPair
    {
        unsigned int nextStep;
        std::string expression;

        ExpressionPair() : 
		nextStep( 0u ),
		expression( "" )
        {
        }
    };    

    class ConditionalGatewayParameter: public BaseStepParameter
    {
    public:
        ConditionalGatewayParameter( const BasicParameter& detail ) : BaseStepParameter( detail ), 
        stepExpressionPairs( 0u ),
        pairStep( 0u )
        {
        }

        ConditionalGatewayParameter( const unsigned long stepId, const int stepType ) : BaseStepParameter( stepId, stepType ), 
        stepExpressionPairs( 0u ),
        pairStep( 0u )
        {
        }

        ~ConditionalGatewayParameter()
        {
        }

        bool isConverge()
        {
            return stepExpressionPairs.empty() || ( stepExpressionPairs.size() == 1u && stepExpressionPairs.front().expression.empty() );
        }

    protected:
        virtual void serializeImp( TA_Base_Bus::Serialization& ioSerialization)
        {
            ioSerialization.serialize( stepExpressionPairs, ParamStepExpressionPairs );
            ioSerialization.serialize( pairStep, ParamPairStep );
        }

    private:
        //InclusiveGatewayParameter(const InclusiveGatewayParameter&);
        ConditionalGatewayParameter& operator=(const ConditionalGatewayParameter&);

    public:
        std::vector<ExpressionPair> stepExpressionPairs;
        unsigned int pairStep;
    };
}

namespace TA_Base_Bus
{
	TA_SERIALIZATION(TA_Base_App::ExpressionPair, (nextStep)(expression))
}

#endif //__CONDITIONAL_GATEWAY_PARAMETER_INCLUDE_H_
