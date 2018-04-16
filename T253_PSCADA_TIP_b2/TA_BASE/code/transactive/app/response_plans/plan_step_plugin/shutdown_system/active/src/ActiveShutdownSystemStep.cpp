#include <ace/OS.h>
#include "ActiveShutdownSystemStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"
//#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"
#include "core/utilities/src/RunParams.h"
#include "core/process_management/IDL/src/IShutdownCorbaDef.h"

namespace TA_Base_App
{
    ActiveShutdownSystemStep::ActiveShutdownSystemStep(const BasicParameter& detail, IActivePlan& flowControl)
		: ActiveStep(m_params, flowControl),
          m_params(detail)
    {
    }

    ActiveShutdownSystemStep::~ActiveShutdownSystemStep()
    {
    }

    bool ActiveShutdownSystemStep::executeImpl()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Shutdown system step %lu", id() );

        bool bExecSucceed = false;
        bool bNotifyPeer = m_params.notifyPeer;

        int port = atoi(TA_Base_Core::RunParams::getInstance().get(RPARAM_MGRPORT).c_str());
        std::string myHostname = getHostname();

        // Create a string in the form: "corbaloc::hostname:port/ShutdownExecutor"
        std::stringstream shutdownExecutorIor;
        shutdownExecutorIor << "corbaloc::" << myHostname << ":" << port << "/ShutdownExecutor";

        // Attempt to resolve an IShutdownCorbaDef object
        CORBA::Object_var obj = TA_Base_Core::CorbaUtil::getInstance().stringToObject( shutdownExecutorIor.str() );

        if ( !CORBA::is_nil(obj) )
        {
            try
            {
                TA_Base_Core::IShutdownCorbaDef_var shutdownExecutorVar = TA_Base_Core::IShutdownCorbaDef::_narrow( obj );
                shutdownExecutorVar->shutdown(CORBA::Boolean(bNotifyPeer));
                bExecSucceed = true;
            }
            catch (CORBA::SystemException& e)
            {
                m_remark = RemarkStepExecutionFailed;
                std::string desc = "System exception occurred while resolving peer shutdown executor ";
                desc += shutdownExecutorIor.str() + " - ";
                desc += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
                LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", desc);
            }
            catch ( ... )
            {
                m_remark = RemarkStepExecutionFailed;
                std::string desc = "Error occurred narrowing ";
                desc += shutdownExecutorIor.str();
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", desc);
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot resolve peer Shutdown Executor %s", shutdownExecutorIor.str().c_str());
        }

        return bExecSucceed;
    }
    
    std::string ActiveShutdownSystemStep::getHostname()
    {
        char name[32];
        int length = 32;
		ACE_OS::hostname(name, length);
        return name;
    }

}