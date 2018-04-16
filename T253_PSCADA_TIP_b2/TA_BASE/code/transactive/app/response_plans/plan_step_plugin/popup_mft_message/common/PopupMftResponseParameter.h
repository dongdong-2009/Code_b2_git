#ifndef POPUPMFTRESPONSEPARAMETER_INCLUDE_H
#define POPUPMFTRESPONSEPARAMETER_INCLUDE_H

#include <string>
#include <vector>
#include "app/response_plans/plan_step_plugin/common/src/SerializableParameter.h"

namespace TA_Base_App
{
class PopupMftResponseParameter : public SerializableParameter
{
public:
    PopupMftResponseParameter(const unsigned int inResponse) : response(inResponse)
    {
    }
    PopupMftResponseParameter(const std::vector<std::string>& vecParameters)
    {
        deserialize(vecParameters);
    }
    ~PopupMftResponseParameter() {}

    int response;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(response, "response");
    }

private:
    PopupMftResponseParameter(const PopupMftResponseParameter&);
    PopupMftResponseParameter& operator=(const PopupMftResponseParameter&);
};
}

#endif //LOGUSERPARAMETER_INCLUDE_H