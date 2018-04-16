#include "MonitorMessageSender.h"
#include "MonitorCommonDataType.h"

#include <sys/types.h>
#include <sys/timeb.h>

#include "core/utilities/src/Hostname.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/IDL/src/BindingRequestMessageCorbaDef.h"


namespace TA_Base_Core
{

    unsigned long MonitorMessageSender::m_sCommandId = 2;
    unsigned long COMMAND_ID_NONE = 1;


    MonitorMessageSender::MonitorMessageSender(void):
        m_channelname(TA_Base_Core::InnerMonitorMessage::Context.getChannelName()),
        m_contextName(TA_Base_Core::InnerMonitorMessage::Context.getContextName())
    {
        m_entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
        m_locationKey = TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);

        m_hostName = Hostname::getHostname();

        TA_ASSERT(m_entityName.length()>0, "EntityName should not be empty");
        TA_ASSERT(m_locationKey.length()>0, "locationKey should be larger than 0");
        m_supplier = TA_Base_Core::gGetStructuredEventSupplierForChannel(m_channelname); 

        createAllFilter();
    }

    MonitorMessageSender::~MonitorMessageSender(void)
    {
        if (m_supplier!=NULL)
        {
            delete m_supplier;
            m_supplier = NULL;
        }
    }

    void MonitorMessageSender::sendResponseMessage(const std::string & msgTypeName, const std::string & commandLine, 
                       const int commandType, const char * responseResult, unsigned long commandID)
    {
        TA_Base_Core::FilterData varHeader;

        TA_Base_Core::MonitorResponseCorbaDef* responseMessage = new TA_Base_Core::MonitorResponseCorbaDef;

        struct timeb nowTime;
        ftime(&nowTime);
        memset(responseMessage,0, sizeof(*responseMessage));
        responseMessage->commandInfo.sendDateTime.time =  nowTime.time;
        responseMessage->commandInfo.sendDateTime.millitm = nowTime.millitm;
        responseMessage->commandInfo.commandID = commandID;

        responseMessage->commandInfo.commandLine = commandLine.c_str();
        responseMessage->commandInfo.optionData = (const char*) "";
        responseMessage->commandInfo.commandType = commandType;

        responseMessage->locationKey = atoi(m_locationKey.c_str());
        responseMessage->entityName = m_entityName.c_str();
        responseMessage->hostName = m_hostName.c_str();

        responseMessage->response = responseResult;

        // Create Structured Event, inject Any data
        CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
        event->remainder_of_body <<= responseMessage;


        TA_Base_Core::gPopulateStructuredEventHeader( *event, 
            TA_Base_Core::InnerMonitorMessage::Context.getContextName(),
            msgTypeName, 
            &varHeader, 
            &m_commonFilter ); 

        m_supplier->publishEvent(*event);

    }
    unsigned long MonitorMessageSender::getNextCommandID()
    {
        return m_sCommandId++;
    }

    void MonitorMessageSender::sendRequestMessage(const std::string & msgTypeName, 
        const std::string & commandLine, const int commandType, const std::string & commandOptionInfo)
    {
        TA_Base_Core::FilterData varHeader;

        // Create CommsMessageCorbaDef object 
        TA_Base_Core::MonitorCommandCorbaDef* commandMessage = new TA_Base_Core::MonitorCommandCorbaDef;
        
        struct timeb nowTime;
        ftime(&nowTime);
        memset(commandMessage, 0, sizeof(*commandMessage));
        commandMessage->sendDateTime.time =  nowTime.time;
        commandMessage->sendDateTime.millitm = nowTime.millitm;
        commandMessage->commandID = getNextCommandID();

        commandMessage->commandLine = commandLine.c_str();
        commandMessage->optionData = commandOptionInfo.c_str();
        commandMessage->commandType = commandType;
        
        // Create Structured Event, inject Any data
        CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
        event->remainder_of_body <<= commandMessage;

        
        TA_Base_Core::gPopulateStructuredEventHeader( *event, 
            TA_Base_Core::InnerMonitorMessage::Context.getContextName(),
            msgTypeName, 
            &varHeader, 
            &m_commonFilter ); 

        m_supplier->publishEvent(*event);
    }

    void MonitorMessageSender::createAllFilter()
    {
        NameValuePair * pLocationPair = new TA_Base_Core::NameValuePair("Location", m_locationKey);
        m_commonFilter.push_back(pLocationPair);
        NameValuePair * pEntityNamePair = new TA_Base_Core::NameValuePair("EntityName", m_entityName);
        m_commonFilter.push_back(pEntityNamePair);
       
    }


}