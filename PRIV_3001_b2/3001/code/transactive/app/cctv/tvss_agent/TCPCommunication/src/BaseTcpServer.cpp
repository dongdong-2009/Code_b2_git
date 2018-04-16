#include "app/cctv/tvss_agent/TCPCommunication/src/BaseTcpServer.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/ILogType.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEventReader.h"
#include "app/cctv/tvss_agent/TCPCommunication/src/TcpMethodTimeoutException.h"
#include "app/cctv/tvss_agent/TCPCommunication/src/TcpConnectionException.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSCommandResponseEventReader.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSTrainborneEventReader.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEquipmentResponseEventReader.h"

using namespace TA_IRS_App;
#define TRAINBORNE_ALARMEVENT_ALARMSTART_POS    7

BaseTcpServer::BaseTcpServer(ITCPMessageProcessor* processor, const std::string & primaryAddress,
                             const std::string & secondaryAddress, unsigned long timeout)
    :   m_processor(processor),
        m_transactionManager(0),
        m_connectionManager(0), m_timeout(timeout)
{
    m_connectionManager = new TcpConnectionManager(*this);
    m_transactionManager = new TcpTransactionManager();
    m_connectionManager->registForConnectionStatus(this);
    m_transactionManager->setWaitTimeout(m_timeout);
    m_connectionManager->setTcpHost(true, primaryAddress);
    m_connectionManager->setTcpHost(false, secondaryAddress);
}

void BaseTcpServer::setEntity(TA_Base_Core::IEntityData& entityData)
{
    m_connectionManager->setEntity(entityData);
}
BaseTcpServer::~BaseTcpServer(void)
{
    delete m_connectionManager;
    delete m_transactionManager;
}

void BaseTcpServer::updateControlConnectionState(ETcpControlConnectionStatus controlConnectionStatus)
{
    m_connectionManager->setControlConnection(controlConnectionStatus);
}

void BaseTcpServer::start()
{
    m_connectionManager->start();
}

void BaseTcpServer::setToControlMode()
{
    FUNCTION_ENTRY("setToControlMode");
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting TVSSTcpServer %d to control mode", 0);
    m_connectionManager->setToControlMode();
    FUNCTION_EXIT;
}

void BaseTcpServer::setToMonitorMode()
{
    FUNCTION_ENTRY("setToMonitorMode");
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting TvssTcpServer %d to monitor mode", 0);
    m_connectionManager->setToMonitorMode();
    FUNCTION_EXIT;
}

void BaseTcpServer::onTerminate(bool peerAvailable)
{
    FUNCTION_ENTRY("onTerminate");
    // unblock any threads waiting on transactions
    m_transactionManager->releaseAllTransactions();
    FUNCTION_EXIT;
}

boost::shared_ptr<TVSSEvent> BaseTcpServer::sendMethod(TVSSMethod& method, unsigned long transactionID, bool waitReply)
{
    FUNCTION_ENTRY("sendMethod");
    // before sending anything - be sure this agent is in control mode
    boost::shared_ptr<TVSSEvent> reply  ;
    try
    {
        if(0 == m_connectionManager->getCurrentConnection())
        {
            TA_THROW(TcpConnectionException());
        }
        // Get a new transaction ID
        if(transactionID == 0xFFFF)
        {
            transactionID = m_transactionManager->getNewTransactionID();
        }
        method.setTransactionId(transactionID);
        method.appendCRC();
        std::string hexText;
        std::string consoleText;
        method.getLogText(hexText, consoleText);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Tcp Server: %d,  transaction: %lu, sending tvss method: %s, tvss method data: %s",
                    0, transactionID, typeid(method).name(), hexText.c_str());
        // Send the message
        m_connectionManager->SendMethod(method);

        if(!waitReply)
        {
            return reply;
        }
        // Wait for the reply
        //reply = static_cast<boost::shared_ptr<TVSSEvent>>(m_transactionManager->waitForEvent(transactionID));
        reply = boost::static_pointer_cast < TVSSEvent, ITcpEvent > (m_transactionManager->waitForEvent(transactionID)) ;
        if((reply->getBufferLength() < 1) || (reply->getStatus() < 0))
        {
            std::string textstatus = "Connection reset while waiting for response event";
            long status = -1;
            if(reply->getBufferLength() > 0)
            {
                textstatus = reply->getTextStatus();
                status = reply->getStatus();
                // log the full verbose details
                // if not already logged - this detail is already logged at debug level
                // so if the debug level is not set that verbose then we'll log it
                if(TA_Base_Core::DebugUtil::getInstance().getLevel() <= TA_Base_Core::DebugUtil::DebugDebug)
                {
                    std::string hexText;
                    std::string consoleText;
                    method.getLogText(hexText, consoleText);
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                "tvss Tcp Server: %d, transaction: %lu, tvss method: %s, returned status: %ld",
                                0,  transactionID, typeid(method).name(), status);
                    LOGMORE(SourceInfo, "tvss method data: %s", hexText.c_str());
                }
            }
        }
    }
    catch(TcpMethodTimeoutException& te)
    {
        // log the method timeout
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "tvss Tcp Server: %d,  transaction: %lu, radio method: %s, TIMED OUT WHILE WAITING FOR RESPONSE",
                    0,  transactionID, typeid(method).name());
        //CLEAN_LOCK_AND_REPLY
        throw;
    }
    catch(...)
    {
        //CLEAN_LOCK_AND_REPLY
        throw;
    }
    // release lock
    // delete lock;
    //lock = NULL;
    FUNCTION_EXIT;
    return reply;
}

void BaseTcpServer::onConnectionLost()
{
    FUNCTION_ENTRY("onConnectionLost");
    m_transactionManager->releaseAllTransactions();
    FUNCTION_EXIT;
}

/**
* Connection resumed.
*/
void BaseTcpServer::onConnectionEstablish()
{
    FUNCTION_ENTRY("onConnectionEstablish");
    //
    // if at least one connection was already up, no need to go around
    // switching stuff back "on"
    int numConnected = m_connectionManager->numConnected();
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "TVSSTcpServer %d Number of active connections: %d", 0, numConnected);
    // TODO: What if both connections resume simultaneously ?
    FUNCTION_EXIT;
}

void BaseTcpServer::onSwitchConnection()
{
    FUNCTION_ENTRY("onConnectionEstablish");
    //delete the all the transaction
    m_transactionManager->releaseAllTransactions();
    FUNCTION_EXIT;
}

/**
* createCompatibleTcpEvent
*/
ITcpEvent * BaseTcpServer::createCompatibleTcpEvent()
{
    TVSSEvent * event = new TA_IRS_App::TVSSEvent();
    return event;
}



/**
* This method will receive the incoming events from the radio system.
* It will notify waiting RadioSession of a new event through the RadioTransactionManager.
* It will process all unsolited events.
* @param    event
*/
void BaseTcpServer::onReceiveEvent(TA_IRS_App::ITcpEvent& itcpEvent)
{
    FUNCTION_ENTRY("onReceiveEvent");
    try
    {
		if(!itcpEvent.crcCheck())
		{
	        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,  "Crc check failure");
			return;
		}
		itcpEvent.peelOffCrc();
        // For BaseTcpServer the ITcpEvent must be a TVSSEvent.
        TVSSEvent& event = dynamic_cast<TVSSEvent&>(itcpEvent);
        unsigned char eventType = event.getEventType();
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Event: %d, transaction: %lu",
                    eventType,
                    event.getTransactionId());
        // [end of logging code]
        //Call try to create the event reader. if it is a invalid message,
        //it will throw RadioProtocolException
        event.getEventReader();
        // Pass to the handle for they type of message
        //if (event.getTransactionId()==0)
        if(eventType == EVENT_TRAINBORNE_EVENT)
        {
            onUnsolicitedEvent(event);
        }
        else
        {
            onSolicitedEvent(event);
        }
        // Do not block normal process
    }
    catch(std::bad_cast)
    {
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,  "RadioProtocolException. the received event is invalid, will ignore it.");
    }
    FUNCTION_EXIT;
}

/**
* Handler for solicited events - This will send them to the RadioTransactionManager for processing
* @param    event
*/
void BaseTcpServer::onSolicitedEvent(TA_IRS_App::TVSSEvent& event)
{
    FUNCTION_ENTRY("onSolicitedEvent");
    if(!m_transactionManager->receivedEvent(event, event.getTransactionId()))
    {
        // Solicited event not handled !!
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
            "Solicited event not handled, transaction: %lu, event: %lu ",
            event.getTransactionId(), event.getEventType());
        onUnhandledSolicitedEvent(event);
    }
    FUNCTION_EXIT;
}

void BaseTcpServer::onUnhandledSolicitedEvent(TVSSEvent& event)
{
    FUNCTION_ENTRY("onUnhandledSolicitedEvent");
    // Get the reader for the event
    TVSSEventReader* eventReader = event.getEventReader();
    FUNCTION_EXIT;
}

void BaseTcpServer::onUnsolicitedEvent(TVSSEvent& event)
{
    FUNCTION_ENTRY("onUnsolicitedEvent");
    // Get the reader for the event
    TVSSEventReader* eventReader = event.getEventReader();
    if(eventReader == NULL)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
            "Unsolicited event not handled, transaction: %lu, event: %lu",
            event.getTransactionId(), event.getEventType());
        return;
    }
    //Make sure that EventHandlerData is empty
    if(!EventHandlerData.empty())
        EventHandlerData.clear();
    //Handle incoming Command Received function
    TVSSCommandEventReader* commandEventReader = dynamic_cast<TVSSCommandEventReader*>(eventReader);
    if(commandEventReader != NULL)
    {
        onIncomingCommandEventReader(*commandEventReader);
    }
    //Handle Trainborne Event Trigger / Normalization
    TVSSTrainborneEventReader* trainborneEventReader = dynamic_cast<TVSSTrainborneEventReader*>(eventReader);
    if(trainborneEventReader != NULL)
    {
        onIncomingTrainborneEventReader(*trainborneEventReader);
    }
    //Handle Equipment Alarm Response Event
    TVSSEquipmentResponseEventReader* equipmentAlarmResponse = dynamic_cast<TVSSEquipmentResponseEventReader*>(eventReader);
    if(equipmentAlarmResponse != NULL)
    {
        onIncomingEquipmentAlarmResponse(*equipmentAlarmResponse);
    }
    FUNCTION_EXIT;
}

void BaseTcpServer::onIncomingCommandEventReader(TA_IRS_App::TVSSCommandEventReader &eventReader)
{
    FUNCTION_ENTRY("onIncomingCommandEventReader");
    //getStatus Data
    EventHandlerData.push_back(eventReader.getCommandStatus());
    //Log the event that receive.
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "MESSAGE: from TVSS Command Received for Function <Received> Data1: %ld", EventHandlerData[0]);
    FUNCTION_EXIT;
}

void BaseTcpServer::onIncomingTrainborneEventReader(TA_IRS_App::TVSSTrainborneEventReader& eventReader)
{
    FUNCTION_ENTRY("onIncomingTrainborneEventReader");
    //getStatus Data
    int length = eventReader.getTrainborneDataLength();
    char alarmStr[128], alarmStrHex[16];
    std::string strAlarm;
    memset(alarmStr, 0, sizeof(alarmStr));
    memset(alarmStrHex, 0, sizeof(alarmStrHex));
    eventReader.getEvent();
    for(int x = 0; x < length; x++)
    {
        EventHandlerData.push_back(eventReader.getTrainborneData(x));
        //LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        //"MESSAGE: from TVSS Trainborne Event Trigger / Normalization <Received> Data%d: %ld",x+1,EventHandlerData[x]);
        alarmStr[x] = eventReader.getTrainborneData(x);
        sprintf(alarmStrHex, "%d:%x ", x + 1, eventReader.getTrainborneData(x));
        strAlarm.append(alarmStrHex);
    }
    unsigned long trainID = (unsigned long)eventReader.getTrainID();
    LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "onIncomingTrainborneEventReader TrainID:%ld,AlarmStr:[%s]", trainID, strAlarm.c_str());
    m_processor->HandleTVSSTrainBorneEvent(trainID, alarmStr);
    FUNCTION_EXIT;
}

void BaseTcpServer::onIncomingEquipmentAlarmResponse(TA_IRS_App::TVSSEquipmentResponseEventReader &eventReader)
{
    FUNCTION_ENTRY("onIncomingEquipmentAlarmResponse");
    int length = eventReader.getLengthAlarmResponseData();
    for(int x = 0; x < length; x++)
    {
        EventHandlerData.push_back(eventReader.getAlarmResponseData(x));
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "MESSAGE: from TVSS Equipment Alarm Response <Received> Data%d: %ld", x + 1, EventHandlerData[x]);
    }
    FUNCTION_EXIT;
}