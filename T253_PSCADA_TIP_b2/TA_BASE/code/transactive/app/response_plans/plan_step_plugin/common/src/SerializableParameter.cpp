#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_step_plugin/common/src/SerializableParameter.h"

namespace TA_Base_App
{

    SerializableParameter::SerializableParameter()
    {
    }

    SerializableParameter::~SerializableParameter()
    {
    }

    std::vector<std::string> SerializableParameter::serialize()
    {
		TA_Base_Bus::Serialization iSerialization;

        try
        {
            serializeImp( iSerialization );
        }
        catch ( std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when serialize" );
        }

        return iSerialization.stringVector();
    }

    void SerializableParameter::deserialize( const std::vector<std::string>& vecSource )
    {
		TA_Base_Bus::Serialization iSerialization(vecSource);

        try
        {
            serializeImp( iSerialization );
        }
        catch ( std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when deserialize" );
        }
    }

}