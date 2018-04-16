#include "app/ats/AtsAgent/AtsTCPCommunication/src/AtsTcpServer.h"
#include "app/ats/AtsAgent/AtsTCPCommunication/src/TcpMethodTimeoutException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/ILogType.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"




using namespace TA_IRS_App;

AtsTcpServer::AtsTcpServer( const std::string & primaryAddress,
                        const std::string & secondaryAddress )
	:	m_transactionManager(0),
	    m_connectionManager(0)
{
    m_connectionManager = new TcpConnectionManager(*this);
	m_transactionManager = new TcpTransactionManager();
    m_connectionManager->registForConnectionStatus(this);
	m_transactionManager->setWaitTimeout( 200*1000 );
	m_connectionManager->setTcpHost(true, primaryAddress);
	m_connectionManager->setTcpHost(false, secondaryAddress);
}

void AtsTcpServer::setEntity(TA_Base_Core::IEntityData& entityData)
{
	m_connectionManager->setEntity(entityData);
}
AtsTcpServer::~AtsTcpServer(void)
{
	delete m_connectionManager;
	delete m_transactionManager;
}

void AtsTcpServer::updateControlConnectionState(ETcpControlConnectionStatus controlConnectionStatus )
{
	m_connectionManager->setControlConnection(controlConnectionStatus);
}

void AtsTcpServer::start()
{
	m_connectionManager->start();
}

void AtsTcpServer::setToControlMode()
{
	FUNCTION_ENTRY("setToControlMode");
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting TVSSTcpServer %d to control mode", 0 );
	
    m_connectionManager->setToControlMode();    

	FUNCTION_EXIT;
}

void AtsTcpServer::setToMonitorMode()
{
	FUNCTION_ENTRY("setToMonitorMode");
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting TvssTcpServer %d to monitor mode", 0 );
	m_connectionManager->setToMonitorMode();
	FUNCTION_EXIT;
}

void AtsTcpServer::onTerminate(bool peerAvailable)
{
    FUNCTION_ENTRY("onTerminate");

   
    // unblock any threads waiting on transactions
    m_transactionManager->releaseAllTransactions();   
    
    FUNCTION_EXIT;
}

void AtsTcpServer::sendMethod(AtsITcpMethod& method)
{
	FUNCTION_ENTRY("sendMethod");
	//unsigned char trainId;
	try
	{
		// Send the message
		m_connectionManager->SendMethod(method);

		//std::string hexText;
		//std::string consoleText;
  //              
		//method.getLogText(hexText, consoleText);
		//trainId = method.getTrainId();

		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		//	"AtsTcp Server: %d,  train Id: %x, sending ats method: %s, ats method data: %s",
		//	0, trainId, typeid(method).name(), hexText.c_str());

		
	}
    catch(...)
    {
		//CLEAN_LOCK_AND_REPLY
        throw;
    }

	FUNCTION_EXIT;
}

void AtsTcpServer::onConnectionLost()
{
	FUNCTION_ENTRY("onConnectionLost");

	m_transactionManager->releaseAllTransactions();
	FUNCTION_EXIT;
}

/**
* Connection resumed.
*/
void AtsTcpServer::onConnectionEstablish()
{
	FUNCTION_ENTRY("onConnectionEstablish");

    //
    // if at least one connection was already up, no need to go around
    // switching stuff back "on"
    int numConnected = m_connectionManager->numConnected();
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                 "TVSSTcpServer %d Number of active connections: %d", 0, numConnected);
    // TODO: What if both connections resume simultaneously ?   

	FUNCTION_EXIT;
}

void AtsTcpServer::onSwitchConnection()
{
	FUNCTION_ENTRY("onConnectionEstablish");

	//delete the all the transaction
	m_transactionManager->releaseAllTransactions();	

	FUNCTION_EXIT;
}

/**
* createCompatibleTcpEvent
*/
   
AtsITcpEvent * AtsTcpServer::createCompatibleTcpEvent()
{
	AtsEvent * event = new TA_IRS_App::AtsEvent();

	return event;
}



/**
* This method will receive the incoming events from the radio system.  
* It will notify waiting RadioSession of a new event through the RadioTransactionManager.  
* It will process all unsolited events.
* @param    event
*/
void AtsTcpServer::onReceiveEvent(TA_IRS_App::AtsITcpEvent& itcpEvent)
{
	FUNCTION_ENTRY("onReceiveEvent");
	std::vector<unsigned char> crc;
	crc = itcpEvent.getCRC();
	unsigned short crcTemp;
	crcTemp = ((crc[0] << 8) & 0xff00) + (crc[1] & 0x00ff); 
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"\n\n\t\t\t **** Reply ****" 
		"\n\n\t\t\tData Received : %s\n" 
				 "\n\n\t\t\t*******************" 
				   "\n\t\t\t  Received Data    "
				   "\n\t\t\t*******************"
				 "\n\t\t\tFlag         : %.2X"
				 "\n\t\t\tCommand Type : %.2X"
				 "\n\t\t\tTrain ID     : %.2X"
				 "\n\t\t\tLength       : %.2X"
				 "\n\t\t\tCRC          : %.2X\n\n"
				 ,itcpEvent.getBufferDump().c_str(),itcpEvent.getFlag(), itcpEvent.getCommand(),itcpEvent.getTrainId(),itcpEvent.getLength(),crcTemp);				
				 

	//try
	//{
	//	
	//	AtsEvent& event = dynamic_cast<AtsEvent&>(itcpEvent);
	//	
	//	long eventType = event.getCommandType();

 //       LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
	//			 "Command Event: %ld, train ID: %c",				 
	//			 eventType,				
	//			 event.getTrainId());
	//			 //event.getStatus());
	//	// [end of logging code]

	//	

	//	//Call try to create the event reader. if it is a invalid message, 
	//	//it will throw RadioProtocolException
	//	event.getEventReader();

	//	// Pass to the handle for they type of message
	//	if (event.getTrainId()==0)
	//	{
	//		onUnsolicitedEvent(event);
	//	}
	//	else
	//	{
	//		onSolicitedEvent(event);
	//	}

	//	// Do not block normal process
	//	
	//}
	//catch(std::bad_cast)
	//{
	//}
	//catch (...)
	//{
	//	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,  "RadioProtocolException. the received event is invalid, will ignore it.");
	//}

	FUNCTION_EXIT;
}


/**
* Handler for solicited events - This will send them to the RadioTransactionManager for processing
* @param    event
*/
void AtsTcpServer::onSolicitedEvent(TA_IRS_App::AtsEvent& event)
{
	FUNCTION_ENTRY("onSolicitedEvent");


	if (!m_transactionManager->receivedEvent(event, event.getTrainId () ))
	{
		
		// Solicited event not handled !!
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
		     "Solicited event not handled, transaction: %x, event: %x ",
			 event.getTrainId(), event.getCommandType() );

		onUnhandledSolicitedEvent(event);
	}
	
	FUNCTION_EXIT;
}

void AtsTcpServer::onUnhandledSolicitedEvent(AtsEvent& event)
{
    FUNCTION_ENTRY("onUnhandledSolicitedEvent");
    
	// Get the reader for the event
    AtsEventReader* eventReader = event.getEventReader();

	FUNCTION_EXIT;
}

void AtsTcpServer::onUnsolicitedEvent(AtsEvent& event)
{
	FUNCTION_ENTRY("onUnsolicitedEvent");
    
	// Get the reader for the event
    AtsEventReader* eventReader = event.getEventReader();
	
	if (eventReader == NULL)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
		     "Unsolicited event not handled, transaction: %x, Command event: %x",
			 event.getTrainId(), event.getCommandType());
		return;
	}
	
	//Make sure that EventHandlerData is empty
	if(!EventHandlerData.empty())
		EventHandlerData.clear();

	//Handle incoming Command Received function
	/*AtsEventOa1Table* atsEventOa1Table = dynamic_cast<AtsEventOa1Table*>(eventReader);
	if (atsEventOa1Table != NULL)
	{
		onIncomingAtsEventOa1Table(*atsEventOa1Table);

	}*/

	FUNCTION_EXIT;
}


//
//void BaseTcpServer::onIncomingCommandEventReader(TA_IRS_App::TVSSCommandEventReader &eventReader)
//{
//	FUNCTION_ENTRY("onIncomingCommandEventReader");
//	
//	//getStatus Data
//	EventHandlerData.push_back(eventReader.getCommandStatus());
//	
//	//Log the event that receive.
//	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
//		"MESSAGE: from TVSS Command Received for Function <Received> Data1: %ld",EventHandlerData[0]);
//
//	FUNCTION_EXIT;
//
//}
//
//void BaseTcpServer::onIncomingTrainborneEventReader(TA_IRS_App::TVSSTrainborneEventReader& eventReader)
//{
//	FUNCTION_ENTRY("onIncomingTrainborneEventReader");
//
//	//getStatus Data
//	int length = eventReader.getTrainborneDataLength();
//	for(int x = 0; x < length; x++)
//	{
//		EventHandlerData.push_back(eventReader.getTrainborneData(x));
//		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
//		"MESSAGE: from TVSS Trainborne Event Trigger / Normalization <Received> Data%d: %ld",x+1,EventHandlerData[x]);
//	}
//
//	FUNCTION_EXIT;
//}
//
//void BaseTcpServer::onIncomingEquipmentAlarmResponse(TA_IRS_App::TVSSEquipmentResponseEventReader &eventReader)
//{
//	FUNCTION_ENTRY("onIncomingEquipmentAlarmResponse");
//
//	int length = eventReader.getLengthAlarmResponseData();
//	for(int x = 0; x < length; x++)
//	{
//		EventHandlerData.push_back(eventReader.getAlarmResponseData(x));
//		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
//		"MESSAGE: from TVSS Equipment Alarm Response <Received> Data%d: %ld",x+1,EventHandlerData[x]);
//	}
//	FUNCTION_EXIT;
//}