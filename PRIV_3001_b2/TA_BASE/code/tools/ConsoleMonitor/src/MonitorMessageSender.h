#ifndef _MONITOR_MESSAGE_SENDER_H__
#define _MONITOR_MESSAGE_SENDER_H__

#include "core/message/src/NameValuePair.h"
#include "core/message/src/StructuredEventSupplier.h"

namespace TA_Base_Core
{
    class MonitorMessageSender
    {
    public:
        MonitorMessageSender(void);
        ~MonitorMessageSender(void);
    public:
        void sendRequestMessage(const std::string & msgTypeName, 
                    const std::string & commandLine, const int commandType, const std::string & commandOptionInfo);
        void sendResponseMessage(const std::string & msgTypeName, 
                 const std::string & commandLine, const int commandType, const char * responseResult, unsigned long commandID);
        void createAllFilter();
    private:
        unsigned long getNextCommandID();
    private:
        TA_Base_Core::StructuredEventSupplier * m_supplier;
        std::string m_contextName;
        std::string m_channelname;
        std::string m_locationKey;
        std::string m_entityName;
        std::string m_hostName;
        TA_Base_Core::FilterData m_commonFilter;

        static unsigned long m_sCommandId;
    };
}

#endif