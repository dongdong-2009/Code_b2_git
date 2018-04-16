#ifndef LAUNCHGUIRESPONSEPARAMETER_INCLUDE_H
#define LAUNCHGUIRESPONSEPARAMETER_INCLUDE_H

#include <string>
#include <vector>
#include "app/response_plans/plan_step_plugin/common/src/SerializableParameter.h"

namespace TA_Base_App
{
    class LaunchGuiResponseParameter : public SerializableParameter
    {
    public:
		LaunchGuiResponseParameter( const unsigned int inResponse) : response(inResponse)
		{
		}
        LaunchGuiResponseParameter( const std::vector<std::string>& vecParameters )
	    {
	        deserialize( vecParameters );
	    }
		~LaunchGuiResponseParameter() {}

        int response;

    protected:
        virtual void serializeImp( TA_Base_Bus::Serialization& ioSerialization)
	    {
	        ioSerialization.serialize( response, "response" );
	    }

    private:
        LaunchGuiResponseParameter( const LaunchGuiResponseParameter& );
        LaunchGuiResponseParameter& operator=(const LaunchGuiResponseParameter&);
    };
}

#endif //LOGUSERPARAMETER_INCLUDE_H