

#include "core/utilities/src/DllHandler.h"
#include "bus/response_plans/common/src/RpcAgency.h"

#include "MessageSender.h"
#include "StepComponent.h"

namespace TA_Base_App
{

StepComponent::StepComponent()
{
}

StepComponent::~StepComponent()
{
}

void StepComponent::init( const TA_Base_Core::IDllHandler& handler, IMessageSender& refMsgSender, TA_Base_Bus::RpcAgency& refRpcAgency )
{
    TA_Base_Core::CorbaUtil::setInstance( handler.getCorbaUtil() );
	TA_Base_Core::DebugUtil::setInstance( handler.getDebugUtil() );

    TA_Base_Core::RunParams& dllRunParams = TA_Base_Core::RunParams::getInstance();
    TA_Base_Core::RunParams::ParamVector paraVec = handler.getRunParams();
    TA_Base_Core::RunParams::ParamVector::iterator it = paraVec.end();
    for ( it = paraVec.begin(); it != paraVec.end(); it++ )
    {
        if ( !dllRunParams.isSet( it->name.c_str() ) )
        {
            dllRunParams.set( it->name.c_str(), it->value.c_str() );
        }
    }

    MessageSender::init( refMsgSender );

    TA_Base_Bus::RpcAgency::setInstance( refRpcAgency );
}

}