#ifndef DECISIONSTEPPARAMETER_INCLUDE_H
#define DECISIONSTEPPARAMETER_INCLUDE_H

#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"

namespace TA_Base_App
{

const char* const ParamDecisionMessage = "Decision Message";
const char* const ParamResponseTimeout = "Response Timeout";
const char* const ParamYesStep = "Yes Step";
const char* const ParamNoStep = "No Step";
const char* const ParamYesButtonLabel = "Yes Button Label";
const char* const ParamNoButtonLabel = "No Button Label";
const char* const ParamPauseButtonLabel = "Pause Button Label";
const char* const ParamNoButtonEnabled = "No Button Enabled";
const char* const ParamPauseButtonEnabled = "Pause Button Enabled";

class DecisionStepParameter: public BaseStepParameter
{

public:
    DecisionStepParameter( const BasicParameter& detail ) : BaseStepParameter( detail ),
    responseTimeout( 60u ),
    yesStep( 0u ),
    noStep( 0u ),
    noButtonEnabled( true ),
    pauseButtonEnabled( false ),
    decisionMessage( "" ),
    yesButtonLabel( "Yes" ),
    noButtonLabel( "No" ),
    pauseButtonLabel( "Pause" )
    {
    }

    DecisionStepParameter( const unsigned long stepId, const int stepType ) : BaseStepParameter( stepId, stepType ),
    responseTimeout( 60u ),
    yesStep( 0u ),
    noStep( 0u ),
    noButtonEnabled( true ),
    pauseButtonEnabled( false ),
    decisionMessage( "" ),
    yesButtonLabel( "Yes" ),
    noButtonLabel( "No" ),
    pauseButtonLabel( "Pause" )
    {
    }

    virtual ~DecisionStepParameter()
    {
    }

protected:
    virtual void serializeImp( TA_Base_Bus::Serialization& ioSerialization )
    {
        ioSerialization.serialize( decisionMessage, ParamDecisionMessage );
        ioSerialization.serialize( responseTimeout, ParamResponseTimeout );
        ioSerialization.serialize( yesStep, ParamYesStep );
        ioSerialization.serialize( noStep, ParamNoStep );
        ioSerialization.serialize( yesButtonLabel, ParamYesButtonLabel );
        ioSerialization.serialize( noButtonLabel, ParamNoButtonLabel );
        ioSerialization.serialize( pauseButtonLabel, ParamPauseButtonLabel );
        ioSerialization.serialize( noButtonEnabled, ParamNoButtonEnabled );
        ioSerialization.serialize( pauseButtonEnabled, ParamPauseButtonEnabled );
    }

private:
    //DecisionStepParameter(const DecisionStepParameter&);
    DecisionStepParameter& operator=(const DecisionStepParameter&);

public:
    unsigned long responseTimeout;
    unsigned int yesStep;
    unsigned int noStep;
    bool noButtonEnabled;
    bool pauseButtonEnabled;
    std::string decisionMessage;
    std::string yesButtonLabel;
    std::string noButtonLabel;
    std::string pauseButtonLabel;
};

}

#endif //DECISIONSTEPPARAMETER_INCLUDE_H