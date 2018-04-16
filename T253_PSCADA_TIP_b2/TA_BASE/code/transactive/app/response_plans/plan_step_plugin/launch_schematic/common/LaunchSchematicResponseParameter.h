#ifndef LAUNCHSCHEMATICRESPONSEPARAMETER_INCLUDE_H
#define LAUNCHSCHEMATICRESPONSEPARAMETER_INCLUDE_H

#include <string>
#include <vector>
#include "app/response_plans/plan_step_plugin/common/src/SerializableParameter.h"

namespace TA_Base_App
{
    class LaunchSchematicResponseParameter : public SerializableParameter
    {
    public:
		LaunchSchematicResponseParameter( const unsigned int inResponse) : response(inResponse)
		{
		}
        LaunchSchematicResponseParameter( const std::vector<std::string>& vecParameters )
		{
			deserialize(vecParameters);
		}
		~LaunchSchematicResponseParameter() {}

        int response;

    protected:
        virtual void serializeImp( TA_Base_Bus::Serialization& ioSerialization)
	    {
	        ioSerialization.serialize( response, "response" );
	    }

    private:
        LaunchSchematicResponseParameter( const LaunchSchematicResponseParameter& );
        LaunchSchematicResponseParameter& operator=(const LaunchSchematicResponseParameter&);
    };
}

#endif //LOGUSERPARAMETER_INCLUDE_H