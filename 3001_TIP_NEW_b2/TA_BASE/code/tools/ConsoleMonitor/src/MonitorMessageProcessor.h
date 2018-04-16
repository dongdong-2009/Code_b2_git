#ifndef _MONITOR_MESSAGE_RECEIVER_H_
#define _MONITOR_MESSAGE_RECEIVER_H_

#include "MonitorCommonDataType.h"

#include "core/message/IDL/src/BindingRequestMessageCorbaDef.h"



namespace TA_Base_Core
{
    class MonitorMessageProcessor
    {
    public:
        MonitorMessageProcessor(bool isAgentSide);
        ~MonitorMessageProcessor(void);

    public:
        void processResponseMessage(TA_Base_Core::MonitorResponseCorbaDef* message, CommandStateInfo & cmdInfo);
        void processRequestMessage(TA_Base_Core::MonitorCommandCorbaDef* message, CommandStateInfo & cmdInfo);
    private:
        //TA_Base_Core::NonReEntrantThreadLockable m_lock;
        bool m_isAgentSide;

    };
}
#endif