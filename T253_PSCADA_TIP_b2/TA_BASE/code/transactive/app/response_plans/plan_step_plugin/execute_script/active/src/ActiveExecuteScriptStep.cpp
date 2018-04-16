#include <ace/OS_NS_stdlib.h>
#include "ActiveExecuteScriptStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"
//#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"
#include "core/uuid/src/TAuuid.h"

namespace TA_Base_App
{
    ActiveExecuteScriptStep::ActiveExecuteScriptStep(const BasicParameter& detail, IActivePlan& flowControl)
        : ActiveStep(m_params, flowControl),
          m_params(detail)
    {
    }

    ActiveExecuteScriptStep::~ActiveExecuteScriptStep()
    {
    }

    bool ActiveExecuteScriptStep::executeImpl()
    {
        bool bExecSucceed = false;

        std::string commandline = m_params.commands;
        std::string fileName = TA_Base_Core::TA_Uuid::generateUUID();
        std::string commandlineRefined = commandline + " 1>" + fileName + " 2>&1";
        int status = ACE_OS::system(commandlineRefined.c_str());
        
		if(0 != WEXITSTATUS(status))
        {
			std::ifstream fileOpen(fileName.c_str(), std::ios::in | std::ios::binary);
            std::stringstream fileContent;

            fileContent << fileOpen.rdbuf();

            m_remark = fileContent.str();

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Plan step execute script error: command '%s' error msg: %s", commandline.c_str(), fileContent.str().c_str());

            fileOpen.close();

            if(0 == remove(fileName.c_str()))
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Del temp file  %s", fileContent.str().c_str());
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Del temp file  %s fail", fileContent.str().c_str());
            }

            bExecSucceed = true;
        }

        return bExecSucceed;
    }
}