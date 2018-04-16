
#include "app/radio/RadioTetraAgent/RadioTcpCommunication/src/BaseTcpServer.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethod.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/ILogType.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventReader.h"
#include "app/radio/RadioTetraAgent/RadioTcpCommunication/src/TcpMethodTimeoutException.h"

using namespace TA_IRS_App;

//#define TRAINBORNE_ALARMEVENT_ALARMSTART_POS	6

BaseTcpServer::BaseTcpServer( ITcpConnectionStatusObserver* commandServant, const std::string & primaryAddress,
                        const std::string & secondaryAddress ,unsigned long timeout)
	:	m_transactionManager(0),
	    m_connectionManager(0),
		m_commandServant(commandServant)
{
	FUNCTION_ENTRY("BaseTcpServer");
    m_connectionManager = new TcpConnectionManager(*this);
	m_transactionManager = new TcpTransactionManager();
    m_connectionManager->registForConnectionStatus(this);
//	m_transactionManager->setWaitTimeout( 200*1000 );
	m_transactionManager->setWaitTimeout( timeout );
	m_connectionManager->setTcpHost(true, primaryAddress);
	m_connectionManager->setTcpHost(false, secondaryAddress);
	FUNCTION_EXIT;
}

void BaseTcpServer::setEntity(TA_Base_Core::IEntityData& entityData)
{
	m_connectionManager->setEntity(entityData);
}
BaseTcpServer::~BaseTcpServer(void)
{
	FUNCTION_ENTRY("~BaseTcpServer");
	if (NULL != m_connectionManager) {
		delete m_connectionManager;
		m_connectionManager = NULL;
	}
	if (NULL != m_transactionManager) {
		delete m_transactionManager;
		m_transactionManager = NULL;
	}
	FUNCTION_EXIT;
}

void BaseTcpServer::updateControlConnectionState(ETcpControlConnectionStatus controlConnectionStatus )
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
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting TVSSTcpServer %d to control mode", 0 );
	
    m_connectionManager->setToControlMode();    

	FUNCTION_EXIT;
}

void BaseTcpServer::setToMonitorMode()
{
	FUNCTION_ENTRY("setToMonitorMode");
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting TvssTcpServer %d to monitor mode", 0 );
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

boost::shared_ptr<RadioEvent> BaseTcpServer::sendMethod(RadioMethod& method)
{
	FUNCTION_ENTRY("sendMethod");
    // before sending anything - be sure this agent is in control mode	

    unsigned long transactionID = 0;
	boost::shared_ptr<RadioEvent> reply  ;
    try
    {
             

	    // Get a new transaction ID
	    transactionID = m_transactionManager->getNewTransactionID();
	    method.setTransactionId(transactionID);
		

		std::string hexText;
        std::string consoleText;
                
         method.getLogText(hexText, consoleText);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"Tcp Server: %d,  transaction: %lu, sending Radio method: %s, Radio method data: %s",
                     0, transactionID, typeid(method).name(), hexText.c_str());
		
    
	    // Send the message
	    m_connectionManager->SendMethod(method);

	    // Wait for the reply

		//reply = static_cast<boost::shared_ptr<RadioEvent>>(m_transactionManager->waitForEvent(transactionID));
		reply = boost::static_pointer_cast < RadioEvent, ITcpEvent > ( m_transactionManager->waitForEvent( transactionID) ) ;
	    //if ( (reply->getBufferLength()<1) || (reply->getStatus()<0) )
		if ( reply->getBufferLength()<1 )
	    {
           
            std::string textstatus = "Connection reset while waiting for response event";
            long status = -1;

            if ( reply->getBufferLength()>0)
            {
                textstatus = reply->getTextStatus();
                status = reply->getStatus();
				
            
                // log the full verbose details
                // if not already logged - this detail is already logged at debug level
                // so if the debug level is not set that verbose then we'll log it
                if (TA_Base_Core::DebugUtil::getInstance().getLevel() <= TA_Base_Core::DebugUtil::DebugDebug)
                {
                    std::string hexText;
                    std::string consoleText;
                
                    method.getLogText(hexText, consoleText);
                
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                 "tvss Tcp Server: %d, transaction: %lu, tvss method: %s, returned status: %ld",
                                 0,  transactionID, typeid(method).name(), status);
                    LOGMORE(SourceInfo, "tvss method data: %s", hexText.c_str());
                    
                }
            }
			
	    }
    }
    catch( TcpMethodTimeoutException& te )
    {
        // log the method timeout
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                     "tvss Tcp Server: %d,  transaction: %lu, radio method: %s, TIMED OUT WHILE WAITING FOR RESPONSE",
                     0,  transactionID, typeid(method).name());
		//CLEAN_LOCK_AND_REPLY

		// exception occur, failure to send intends to remove the transaction to the 
		// transaction list for that lost transaction cannot be replied anymore
		m_transactionManager->waitListDeleteTransaction(transactionID);

        throw;
    }
    catch(...)
    {
		//CLEAN_LOCK_AND_REPLY
        // exception occur, failure to send intends to remove the transaction to the 
		// transaction list for that lost transaction cannot be replied anymore
		m_transactionManager->waitListDeleteTransaction(transactionID);
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

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Connection Lost, removing all transactions");
	m_transactionManager->releaseAllTransactions();
	m_commandServant->onConnectionLost();
	FUNCTION_EXIT;
}

/**
* Connection resumed.
*/
void BaseTcpServer::onConnectionEstablish(bool isPrimary)
{
	FUNCTION_ENTRY("onConnectionEstablish");

    //
    // if at least one connection was already up, no need to go around
    // switching stuff back "on"
    int numConnected = m_connectionManager->numConnected();
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                 "TVSSTcpServer %d Number of active connections: %d", 0, numConnected);
    // TODO: What if both connections resume simultaneously ?   

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Radio Gateway port 8088 is connected to %s, informing Radio SDS now",
		(isPrimary) ? "Primary" : "Secondary");
	m_commandServant->onConnectionEstablish(isPrimary);

	FUNCTION_EXIT;
}

void BaseTcpServer::onSwitchConnection(bool toPrimary)
{
	FUNCTION_ENTRY("onSwitchConnection");

	//delete the all the transaction
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Connection Switch, removing all transactions");
	m_transactionManager->releaseAllTransactions();	

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Radio Gateway port 8088 is being switch to connect to %s, informing Radio SDS now",
		(toPrimary) ? "Primary" : "Secondary");
	m_commandServant->onSwitchConnection(toPrimary);

	FUNCTION_EXIT;
}

/**
* createCompatibleTcpEvent
*/
ITcpEvent * BaseTcpServer::createCompatibleTcpEvent()
{
	RadioEvent * event = new TA_IRS_App::RadioEvent();

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
		// For BaseTcpServer the ITcpEvent must be a RadioEvent.
		RadioEvent& event = dynamic_cast<RadioEvent&>(itcpEvent);
		
		long eventType = event.getEventType();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				 "Event: %ld, transaction: %lu",				 
				 eventType,				
				 event.getTransactionId());
		// [end of logging code]

		

		//Call try to create the event reader. if it is a invalid message, 
		//it will throw RadioProtocolException
		event.getEventReader();

		// Pass to the handle for they type of message
		if (event.getTransactionId()==0)
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
	catch (...)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,  "RadioProtocolException. the received event is invalid, will ignore it.");
	}

	FUNCTION_EXIT;
}

/**
* Handler for solicited events - This will send them to the RadioTransactionManager for processing
* @param    event
*/
void BaseTcpServer::onSolicitedEvent(TA_IRS_App::RadioEvent& event)
{
	FUNCTION_ENTRY("onSolicitedEvent");


	if (!m_transactionManager->receivedEvent(event, event.getTransactionId () ))
	{
		
		// Solicited event not handled !!
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
		     "Solicited event not handled, transaction: %lu, event: %lu ",
			 event.getTransactionId(), event.getEventType() );

		onUnhandledSolicitedEvent(event);
	}
	
	FUNCTION_EXIT;
}

void BaseTcpServer::onUnhandledSolicitedEvent(RadioEvent& event)
{
    FUNCTION_ENTRY("onUnhandledSolicitedEvent");
    
	// Get the reader for the event
    RadioEventReader* eventReader = event.getEventReader();
	
	if (eventReader == NULL)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
		     "Unsolicited event not handled, transaction: %lu, event: %lu",
			 event.getTransactionId(), event.getEventType());
		return;
	}
	
	//Handle incoming MFT User Profile Request
	RadioEventProfileRequest* radioEventProfile = dynamic_cast<RadioEventProfileRequest*>(eventReader);
	if(radioEventProfile != NULL)
	{
		onIncomingEventProfileRequest(*radioEventProfile);
	}

	//Handle incoming Subscriber Monitor
	RadioEventSubscriberMonitor* radioEventSubscriberMonitor = dynamic_cast<RadioEventSubscriberMonitor*>(eventReader);
	if (radioEventSubscriberMonitor != NULL)
	{
		onIncomingEventSubscriberMonitor(*radioEventSubscriberMonitor);
	}

	//Handle incoming Dynamic Group Number Assignment
	RadioEventDynamicGroup* radioEventDyncamicGroup = dynamic_cast<RadioEventDynamicGroup*>(eventReader);
	if (radioEventDyncamicGroup != NULL)
	{
		onIncomingEventDynamicGroup(*radioEventDyncamicGroup);
	}
	
	//Handle incoming Call Forward Request
	RadioEventCallForwardRequest* radioEventCallFowardRequest = dynamic_cast<RadioEventCallForwardRequest*>(eventReader);
	if (radioEventCallFowardRequest != NULL)
	{
		onIncomingEventCallFowardRequest(*radioEventCallFowardRequest);
	}

	//Handle incoming Cancel Call Forwarding
	RadioEventCancelCallForwarding* radioEventCancelCallForwarding = dynamic_cast<RadioEventCancelCallForwarding*>(eventReader);
	if (radioEventCancelCallForwarding != NULL)
	{
		onIncomingEventCancelCallForwarding(*radioEventCancelCallForwarding);
	}

	////Handle incoming Query Call Forwarding
	RadioEventQueryCallForwarding* radioEventQueryCallForwarding = dynamic_cast<RadioEventQueryCallForwarding*>(eventReader);
	if (radioEventQueryCallForwarding != NULL)
	{
		onIncomingEventQueryCallForwarding(*radioEventQueryCallForwarding);
	}


	FUNCTION_EXIT;
}

void BaseTcpServer::onUnsolicitedEvent(RadioEvent& event)
{
	FUNCTION_ENTRY("onUnsolicitedEvent");
    
	// Get the reader for the event
 //   RadioEventReader* eventReader = event.getEventReader();
	//
	//if (eventReader == NULL)
	//{
	//	LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
	//	     "Unsolicited event not handled, transaction: %lu, event: %lu",
	//		 event.getTransactionId(), event.getEventType());
	//	return;
	//}
	//
	////Make sure that EventHandlerData is empty
	//if(!EventHandlerData.empty())
	//	EventHandlerData.clear();

	////Handle incoming Command Received function
	////TVSSCommandEventReader* commandEventReader = dynamic_cast<TVSSCommandEventReader*>(eventReader);
	//if (commandEventReader != NULL)
	//{
	//	onIncomingCommandEventReader(*commandEventReader);

	//}

	////Handle Trainborne Event Trigger / Normalization
	//TVSSTrainborneEventReader* trainborneEventReader = dynamic_cast<TVSSTrainborneEventReader*>(eventReader);
	//if ( trainborneEventReader != NULL)
	//{
	//	onIncomingTrainborneEventReader(*trainborneEventReader);
	//}

	////Handle Equipment Alarm Response Event
	//TVSSEquipmentResponseEventReader* equipmentAlarmResponse = dynamic_cast<TVSSEquipmentResponseEventReader*>(eventReader);
	//if ( equipmentAlarmResponse != NULL )
	//{
	//	onIncomingEquipmentAlarmResponse(*equipmentAlarmResponse);
	//}
	FUNCTION_EXIT;
}

void BaseTcpServer::onIncomingEventProfileRequest(TA_IRS_App::RadioEventProfileRequest &eventReader)
{
	FUNCTION_ENTRY("onIncomingEventProfileRequest");
	std::vector<std::string> TALKGROUP_ALIAS;
	TALKGROUP_ALIAS = eventReader.getTalkgroup();
	char szArray[1024] = {0};
	std::string strGroup_alias;
	for (int x=0;x<TALKGROUP_ALIAS.size();x++)
	{
		sprintf(szArray, "\t\t\tTalkgroup Alias %d = %s",x+1,TALKGROUP_ALIAS[x].c_str());
		strGroup_alias += szArray;
		strGroup_alias +="\n";
	}
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
		"\n\n\t\t\t%s"
		"\n\t\t\t**** REPLY ****"
		"\n\n%s\n\n",typeid(eventReader).name(),strGroup_alias.c_str());
	

	FUNCTION_EXIT;
}

void BaseTcpServer::onIncomingEventSubscriberMonitor(TA_IRS_App::RadioEventSubscriberMonitor &eventReader)
{
	FUNCTION_ENTRY("onIncomingEventSubscriberMonitor");
	std::string talkGroup = eventReader.getTalkgroupAlias();
	std::string baseStation = eventReader.getBaseStaionAlias();
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
		"\n\n\t\t\t%s"
		"\n\t\t\t**** REPLY ****"
		"\n\t\t\tTalkgroup Alias    = %s"
		"\n\t\t\tBase Station Alias = %s",typeid(eventReader).name(),talkGroup.c_str(),baseStation.c_str());

	FUNCTION_EXIT;
	
}

void BaseTcpServer::onIncomingEventDynamicGroup(TA_IRS_App::RadioEventDynamicGroup &eventReader)
{
	FUNCTION_ENTRY("onIncomingEventDynamicGroup");
	bool status = eventReader.getStatus();
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
		"\n\n\t\t\t%s"
		"\n\t\t\t**** REPLY ****"
		"\n\t\t\tSTATUS = %d\n\n",typeid(eventReader).name(),status);
	FUNCTION_EXIT;
}

void BaseTcpServer::onIncomingEventCallFowardRequest(TA_IRS_App::RadioEventCallForwardRequest& eventReader)
{
	FUNCTION_ENTRY("onIncomingEventCallFowardRequest");
	bool status = eventReader.getStatus();
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
		"\n\n\t\t\t%s"
		"\n\t\t\t**** REPLY ****"
		"\n\t\t\tSTATUS = %d\n\n",typeid(eventReader).name(),status);
	FUNCTION_EXIT;
}

void BaseTcpServer::onIncomingEventCancelCallForwarding(TA_IRS_App::RadioEventCancelCallForwarding &eventReader)
{
	FUNCTION_ENTRY("onIncomingEventCancelCallForwarding");
	bool status = eventReader.getStatus();
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
		"\n\n\t\t\t%s"
		"\n\t\t\t**** REPLY ****"
		"\n\t\t\tSTATUS = %d\n\n",typeid(eventReader).name(),status);
	FUNCTION_EXIT;
}

void BaseTcpServer::onIncomingEventQueryCallForwarding(TA_IRS_App::RadioEventQueryCallForwarding &eventReader)
{
	FUNCTION_ENTRY("onIncomingEventQueryCallForwarding");
	unsigned short condition = eventReader.getCondition();
	unsigned long forwardingISSI = eventReader.getForwardingISSI();
	unsigned long forwardedISSI = eventReader.getForwardedISSI();
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
		"\n\n\t\t\t%s"
		"\n\t\t\t**** REPLY ****"
		"\n\t\t\tCondition       = %u"
		"\n\t\t\tForwarding ISSI = %u"
		"\n\t\t\tForwarded ISSI  = %u\n\n",typeid(eventReader).name(),condition,forwardingISSI,forwardedISSI);
	FUNCTION_EXIT;
}
