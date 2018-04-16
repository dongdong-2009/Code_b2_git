#ifndef __I_STEP_COMPONENT_H_
#define __I_STEP_COMPONENT_H_

#include <string>
#include <vector>

namespace TA_Base_Core
{
    class IDllHandler;
}

namespace TA_Base_Bus
{
    class RpcAgency;
}

namespace TA_Base_App
{
    class IActiveStepMessage;
    class IActivePlan;
    class ActiveStep;
    class IMessageSender;
    class BasicParameter;

    class IStepComponent
    {
    public:
        virtual ActiveStep* createActiveStep( const BasicParameter& detail, IActivePlan& plan ) = 0;
        virtual IActiveStepMessage* createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& parameters ) { return NULL; };
        virtual void validateStepParameter( const std::vector<std::string>& parameterSeq ) = 0;
        virtual void init( const TA_Base_Core::IDllHandler& handler, IMessageSender& refMsgSender, TA_Base_Bus::RpcAgency& refRpcAgency ) = 0;
    };

    typedef IStepComponent& (*GetStepComponentFuncPtr)();
}

#endif