/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/CODE_4677_T00060500/transactive/app/radio/RadioTetraAgent/src/RadioSDSHandler.cpp $
 * @author:  alexis
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2006/12/08 11:20:56 $
 * Last modified by:  $Author: buildteam $
 * 
 */

#include "app/radio/RadioTetraAgent/src/RadioSDSHandler.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/types/RadioAlarms_MessageTypes.h"
#include "core/utilities/src/RunParams.h"


#include <sstream>


using namespace TA_IRS_App;

RadioSDSHandler::RadioSDSHandler():
    m_pSDSInputQueue(NULL),
    m_pSDSOutputQueue(NULL),
    m_pSDSQueueManager(NULL),
    m_pSDTSTransactionManager(NULL),
	m_pPrimaryRadioSDS(NULL),
	m_pSecondaryRadioSDS(NULL),
	m_pCurrentRadioSDS(NULL)
{
    FUNCTION_ENTRY("constructor");

	//Build up: input/output queues, transaction manager & queue manager;

	// transaction manager
	m_pSDTSTransactionManager = new TA_IRS_App::SDTSTransactionManager();

	// queue manager which connects to transaction manager
	m_pSDSQueueManager = new TA_IRS_App::SDSQueueManager( *m_pSDTSTransactionManager );

	// an input queue to cache the events/requests which is under a queue manager
	m_pSDSInputQueue = new TA_IRS_App::SDSInputQueue( *m_pSDSQueueManager );

    // an output queue to cache the events/requests which is under a queue manager
	m_pSDSOutputQueue = new TA_IRS_App::SDSOutputQueue( *m_pSDSQueueManager );


    // start queuing events
    m_pSDSQueueManager->setQueues(m_pSDSInputQueue, m_pSDSOutputQueue);


    FUNCTION_EXIT;
}


RadioSDSHandler::~RadioSDSHandler()
{
    FUNCTION_ENTRY("destructor");


	if ( m_pPrimaryRadioSDS != NULL) 
	{
		m_pSDSQueueManager->deregisterObserver(m_pPrimaryRadioSDS);
		m_pSDSInputQueue->deregisterObserver(m_pPrimaryRadioSDS);
		delete m_pPrimaryRadioSDS;
		m_pPrimaryRadioSDS = NULL;
	}

	if ( m_pSecondaryRadioSDS != NULL)
	{
		m_pSDSQueueManager->deregisterObserver(m_pSecondaryRadioSDS);
		m_pSDSInputQueue->deregisterObserver(m_pSecondaryRadioSDS);
		delete m_pSecondaryRadioSDS;
		m_pSecondaryRadioSDS = NULL;
	}

	if( m_pSDSInputQueue != NULL )
	{
        
        m_pSDSInputQueue->stop();
		delete m_pSDSInputQueue;
		m_pSDSInputQueue = NULL;
	}

	if( m_pSDSOutputQueue != NULL )
	{
        m_pSDSOutputQueue->stop();
		delete m_pSDSOutputQueue;
		m_pSDSOutputQueue = NULL;
	}

	if( m_pSDSQueueManager != NULL )
	{
		m_pSDSQueueManager->disconnect();
		delete m_pSDSQueueManager;
		m_pSDSQueueManager = NULL;
	}

	if( m_pSDTSTransactionManager != NULL )
	{
		delete m_pSDTSTransactionManager;
		m_pSDTSTransactionManager = NULL;
	}


    FUNCTION_EXIT;
}

void RadioSDSHandler::sendSDS(unsigned long recipientSSI, std::vector<unsigned char> rawMessage)
{
    FUNCTION_ENTRY("sendSDS");
	//TA_Base_Core::ThreadGuard lock(m_lock); //the lock cannot ensure m_pCurrentRadioSDS is the active one during this request.it may be closed/swtiched already.
	if ( NULL == m_pCurrentRadioSDS ) {
		throw(TA_Base_Core::BadParameterException("Initialization to SDS server not done yet."));
	}

	if ( m_pCurrentRadioSDS->isTrainborne() )
	{
		m_pCurrentRadioSDS->sendSDS(recipientSSI,rawMessage);
	}

    FUNCTION_EXIT;
}

void RadioSDSHandler::sendText(const char* session,unsigned long recipientSSI, TextCodingScheme encoding, std::string rawMessage, bool isBroadcast/* =false */)
{
    FUNCTION_ENTRY("sendText");

	//TA_Base_Core::ThreadGuard lock(m_lock); //the lock cannot ensure m_pCurrentRadioSDS is the active one during this request.it may be closed/swtiched already.

	if ( NULL == m_pCurrentRadioSDS ) {
		throw(TA_Base_Core::BadParameterException("Initialization to SDS server not done yet."));
	}

	if ( ( m_pCurrentRadioSDS->getConsoleName() == session ) && ( !( m_pCurrentRadioSDS->isTrainborne() ) ) ) {
		m_pCurrentRadioSDS->sendText(recipientSSI,encoding,rawMessage,isBroadcast);
	}

    FUNCTION_EXIT;
}

RadioSDS* RadioSDSHandler::createRadioSDS(RadioSDSSummary* pSummary,const std::string& name, unsigned long passNumber,unsigned long clientSSI, bool isPrimary,ISDSRegistationState* sdsRegState)
{
	std::ostringstream sdsLog;
	RadioSDS* p_newRadioSDS = NULL;

	if (isPrimary) 
	{
		sdsLog << "Create Primary RadioSDS => console: " << name << " ClientSSI: " << clientSSI << " PassNumber: " << passNumber;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,sdsLog.str().c_str());

		p_newRadioSDS = new RadioSDS(pSummary,m_pSDSOutputQueue,m_pSDSQueueManager,m_pSDTSTransactionManager,*sdsRegState,true);
		p_newRadioSDS->setSDSHost(pSummary->getSDTSHost());
		p_newRadioSDS->setSDSPort(pSummary->getSDTSPort());
		p_newRadioSDS->setSDSPassNumber(passNumber);
		p_newRadioSDS->setConsoleName(name);
		p_newRadioSDS->setClientSSI(clientSSI);

		// TODO: 'TRN' is to indicate that this SSI is for trainborne, 
		// Please remove hardcoded string and change it to a const string value
		if (name == "TRN") 
		{
			p_newRadioSDS->setTrainborne(true);
		}

		m_pPrimaryRadioSDS = p_newRadioSDS;

		m_pSDSQueueManager->registerObserver(p_newRadioSDS);
		m_pSDSInputQueue->registerObserver(p_newRadioSDS);
	} 
	else 
	{
		sdsLog << "Create Secondary RadioSDS => console: " << name << " ClientSSI: " << clientSSI << " PassNumber: " << passNumber;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,sdsLog.str().c_str());

		p_newRadioSDS = new RadioSDS(pSummary,m_pSDSOutputQueue,m_pSDSQueueManager,m_pSDTSTransactionManager,*sdsRegState);
		p_newRadioSDS->setSDSHost(pSummary->getSDTSSecondaryHost());
		p_newRadioSDS->setSDSPort(pSummary->getSDTSSecondaryPort());
		p_newRadioSDS->setSDSPassNumber(passNumber);
		p_newRadioSDS->setConsoleName(name);
		p_newRadioSDS->setClientSSI(clientSSI);

		// TODO: 'TRN' is to indicate that this SSI is for trainborne, 
		// Please remove hardcoded string and change it to a const string value
		if (name == "TRN") 
		{
			p_newRadioSDS->setTrainborne(true);
		}

		m_pSecondaryRadioSDS = p_newRadioSDS;

 		m_pSDSQueueManager->registerObserver(p_newRadioSDS);
 		m_pSDSInputQueue->registerObserver(p_newRadioSDS);
	}

	return p_newRadioSDS;
}

void RadioSDSHandler::cleanUpSDS()
{
	if (NULL != m_pPrimaryRadioSDS) {
		m_pPrimaryRadioSDS->cleanupSDS();
	}

	if (NULL != m_pSecondaryRadioSDS) {
		m_pSecondaryRadioSDS->cleanupSDS();
	}
}

void RadioSDSHandler::connectToSDS(const std::string& host, const std::string& port)
{
	try
	{
    	m_pSDSQueueManager->connect(host,port);	
	}
	catch(TA_Base_Core::TransactiveException e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unknown error while connecting to SDS host");
	}
}

void RadioSDSHandler::switchConnection(bool toPrimary)
{
	FUNCTION_ENTRY("switchConnection");

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"SDTS Connection about to switch connection to %s connection",toPrimary? "Primary":"Secondary");
	TA_Base_Core::ThreadGuard lock(m_lock);
	m_pCurrentRadioSDS = (toPrimary) ? m_pPrimaryRadioSDS : m_pSecondaryRadioSDS;
	
	if (toPrimary)
	{
		if (m_pPrimaryRadioSDS)
		{
			m_pPrimaryRadioSDS->setActiveSDSConnection(true);
		}
		
		if (m_pSecondaryRadioSDS)
		{
			m_pSecondaryRadioSDS->setActiveSDSConnection(false);
		}
		
	}
	else
	{
		if (m_pSecondaryRadioSDS)
		{
			m_pSecondaryRadioSDS->setActiveSDSConnection(true);
		}
		
		if (m_pPrimaryRadioSDS)
		{
			m_pPrimaryRadioSDS->setActiveSDSConnection(false);
		}
		
	}
	

	FUNCTION_EXIT;
}

void RadioSDSHandler::clearSDS()
{
	FUNCTION_ENTRY("clearSDS");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Clearing all transaction, input and output messages");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Disconnect SDS Queue Manager now");
	m_pSDSQueueManager->disconnect();	

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Clear all messages from SDS Input Queue");
	m_pSDSInputQueue->clearQueue();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Clear all messages from SDS Output Queue");
	m_pSDSOutputQueue->clearQueue();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Clear all transactions from SDS Transaction Manager");
	m_pSDTSTransactionManager->clearTransaction();

	FUNCTION_EXIT;
}