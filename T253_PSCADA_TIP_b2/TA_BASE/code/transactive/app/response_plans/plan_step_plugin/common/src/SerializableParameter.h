#ifndef __SERIALIZABLEPARAMETER_INCLUDE_H_
#define __SERIALIZABLEPARAMETER_INCLUDE_H_

#include "bus/response_plans/PlanDataAccess/src/Serialization.h"

namespace TA_Base_App
{

class SerializableParameter
{
public:
    SerializableParameter();
    virtual ~SerializableParameter();

public:
    std::vector<std::string> serialize();
    void deserialize( const std::vector<std::string>& vecSource );

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization) = 0;
};

}


#endif //__SERIALIZABLEPARAMETER_INCLUDE_H_