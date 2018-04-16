#ifndef __STEPCOMPONENT_INCLUDE_H_
#define __STEPCOMPONENT_INCLUDE_H_

#include "IStepComponent.h"

namespace TA_Base_App
{

class IMessageSender;

class StepComponent : public IStepComponent
{
public:
    StepComponent();
    virtual ~StepComponent();

public:
    void init( const TA_Base_Core::IDllHandler& handler, IMessageSender& refMsgSender, TA_Base_Bus::RpcAgency& refRpcAgency );
};

}


#endif //__STEPCOMPONENT_INCLUDE_H_