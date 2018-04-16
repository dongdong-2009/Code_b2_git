#ifndef DECISIONRESPONSEPARAMETER_INCLUDE_H
#define DECISIONRESPONSEPARAMETER_INCLUDE_H

#include "app/response_plans/plan_step_plugin/common/src/SerializableParameter.h"

namespace TA_Base_App
{

class DecisionResponseParameter : public SerializableParameter
{
public:
    DecisionResponseParameter( const unsigned int inResponse ) : response( inResponse )
    {
    }

    DecisionResponseParameter( const std::vector<std::string>& vecParameters )
    {
        deserialize( vecParameters );
    }

    ~DecisionResponseParameter()
    {
    }

protected:
    virtual void serializeImp( TA_Base_Bus::Serialization& ioSerialization )
    {
        ioSerialization.serialize( response, "response" );
    }

public:
    int response;
};

}

#endif //LOGUSERPARAMETER_INCLUDE_H