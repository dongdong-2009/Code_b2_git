#include "MonitorMessageProcessor.h"

#include <sys/types.h>
#include <sys/timeb.h>

#include "core/utilities/src/RunParams.h"
#include "core/message/src/TA_CosUtility.h"







namespace TA_Base_Core
{
    MonitorMessageProcessor::MonitorMessageProcessor(bool isAgentSide):
        m_isAgentSide(isAgentSide)
    {

    }

    MonitorMessageProcessor::~MonitorMessageProcessor(void)
    {
    }

    void MonitorMessageProcessor::processRequestMessage(TA_Base_Core::MonitorCommandCorbaDef* message, CommandStateInfo & cmdInfo)
    {
        if (!m_isAgentSide)
        {
            LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "only agent side could receive monitor request message");
            return;
        }

        TA_ASSERT(m_isAgentSide, "Only Agent receive Request Message!");

        TA_ASSERT(cmdInfo.nextStep == NEXTSTEP_NONE, "Next STEP should be set to NONE");

        std::string commandLine = message->commandLine.in();
        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "received request message command line=[%s]", commandLine.c_str());
        {
            std::cout << commandLine.c_str() << std::endl;
        }
    }

    void MonitorMessageProcessor::processResponseMessage(TA_Base_Core::MonitorResponseCorbaDef* message, CommandStateInfo & cmdInfo)
    {
        if (m_isAgentSide)
        {
            LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "only console monitor could receive monitor response message");
            return;
        }

        TA_ASSERT(cmdInfo.nextStep == NEXTSTEP_NONE, "Next STEP should be set to NONE");

        TA_Base_Core::MonitorCommandCorbaDef * pCommand = &(message->commandInfo);
        std::string commandLine = pCommand->commandLine.in();

        std::string messageResponse = message->response.in();
        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "received response message command line=[%s]", commandLine.c_str());
        {
            std::cout << " command: " << commandLine.c_str() << ", result: " << messageResponse.c_str() << std::endl;
        }
    }

    
}