#ifndef RUNPLANRESPONSEPARAMETER_INCLUDE_H
#define RUNPLANRESPONSEPARAMETER_INCLUDE_H

#include <string>
#include <vector>
#include "app/response_plans/plan_step_plugin/common/src/SerializableParameter.h"

namespace TA_Base_App
{
	const char* const ParamResponse = "Response";

    class RunPlanResponseParameter : public SerializableParameter
    {
    public:
		RunPlanResponseParameter( const unsigned int inResponse) : response(inResponse)
		{
		}

        RunPlanResponseParameter( const std::vector<std::string>& vecParameters )
		{
			deserialize(vecParameters);
		}

		~RunPlanResponseParameter() {}

    public:
        int response;

    protected:
        virtual void serializeImp( TA_Base_Bus::Serialization& ioSerialization)
	    {
	        ioSerialization.serialize( response, ParamResponse);
	    }

    private:
        RunPlanResponseParameter( const RunPlanResponseParameter& );
        RunPlanResponseParameter& operator=(const RunPlanResponseParameter&);
    };
}

#endif //LOGUSERPARAMETER_INCLUDE_H