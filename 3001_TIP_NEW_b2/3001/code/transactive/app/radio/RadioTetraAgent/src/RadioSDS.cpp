/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioSDS.cpp $
 * @author:  albertus
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/RadioTetraAgent/src/RadioSDS.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSSendText.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSQueueManager.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSAckSendText.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSAckRecvText.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSRecvText.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSRegisterSession.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSUnregisterSession.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSMsgFactory.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_ReportEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioSDSHandler.h"


#include "bus/radio/radio_bus/src/radiotypes.h"
#include "bus/radio/radio_agent/IDL/src/RadioServerAgentIDL.h"
#include "bus/radio/radio_agent/IDL/src/CommonRadioAgentIDL.h"
#include "bus/trains/train_agent/idl/src/IRecvSDSTrainAgentCorbaDef.h"

#include "core/data_access_interface/radio/src/RadioSubscriberFactory.h"
#include "core/data_access_interface/radio/src/IRadioSubscriber.h"
#include "core/data_access_interface/radio/src/IRadioTextMessageInbox.h"
#include "core/data_access_interface/radio/src/RadioTextMessageInboxFactory.h"

#include "core/utilities/src/DebugUtil.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/types/RadioAlarms_MessageTypes.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DataException.h"

#include <sstream>


using namespace TA_IRS_App;

RadioSDS::RadioSDS(RadioSDSSummary* pSDSSummary,SDSOutputQueue* pOutputq,SDSQueueManager* pQMgr,SDTSTransactionManager* ptransactMgr,ISDSRegistationState& sdsRegState, bool bIsActiveSDS) :
    m_pSDSOutputQueue(pOutputq),
    m_pSDSQueueManager(pQMgr),
    m_pSDTSTransactionManager(ptransactMgr),
    m_registrationState(Radio::NOT_REGISTERED),
    m_commsSender(NULL),
    m_entityKey(0),
    m_entitySubsystem(0),
    m_entityLocation(0),
	m_bTrainborne(false),
	m_bIsActiveSDS(bIsActiveSDS),
	m_sdsRegistrationState(sdsRegState)
	
{
    FUNCTION_ENTRY("constructor");

    m_timeout = ACE_Time_Value(15, 0); // 15 seconds timeout
	
	cntNumRetires = 0;

	m_session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	
    m_entityKey       = pSDSSummary->getEntityKey();
    m_entityLocation  = pSDSSummary->getLocation();
    m_entitySubsystem = pSDSSummary->getSubsystem();
    m_entityTypeKey   = pSDSSummary->getEntityTypeKey();
    m_entityName      = pSDSSummary->getEntityName();

	m_MaxNumRetry = pSDSSummary->getSDSMaxNumRetry();

	//Build up: input/output queues, transaction manager & queue manager;

    m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().
        getCommsMessageSender(TA_Base_Core::RadioComms::Context);


    // Should be just one entity of the agent
  //  std::vector<TA_Base_Core::IEntityData*> entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType("RadioTetraAgent");
	unsigned long locationKey = atol( TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
     std::vector<TA_Base_Core::IEntityData*> entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation("RadioServerCommand" , locationKey , true);
    TA_ASSERT(entityList.size()>0,"RadioServer entity not found for this location.");
    TA_ASSERT(entityList.size()<2,"More than one radio entity defined for this location.");

	initialiseSDSNamedObject();

	m_serverAgentKey = entityList[0]->getKey();

    FUNCTION_EXIT;
}


RadioSDS::~RadioSDS()
{
    FUNCTION_ENTRY("destructor");

    delete m_commsSender;
    m_commsSender = NULL;

	//TD17994, Haipeng refactoring the alarm libary;
	TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
    FUNCTION_EXIT;
}

bool RadioSDS::needReconnection()
{
	if(cntNumRetires >= m_MaxNumRetry)
	{
		return true;
	}

	return false;
	
}

void RadioSDS::resetRetryCount()
{
	cntNumRetires = 0;
}

bool RadioSDS::initializeSDS()
{
    FUNCTION_ENTRY("initializeSDS");

    bool ret = false;

    switch (m_registrationState)
    {
    case Radio::REGISTERED:
		// registration already done, just return false to indicate no change
		resetRetryCount();	
        
        break;

    case Radio::REGISTRATION_STARTED:
        if (ACE_OS::gettimeofday() - m_registrationTime > m_timeout)
        {	
            submitAlarm(TA_Base_Core::RadioAlarms::RadioSDSAlarm);
            // reset state to re-register
            m_registrationState = Radio::NOT_REGISTERED;
			
			cntNumRetires++;

			//if actual number of retries equal/more than Maximum number of retries
			//disconnect from SDS server and reconnect
        }

        // wait longer

        break;

    case Radio::NOT_REGISTERED:
		
        try
        {
            TA_IRS_App::SDSRegisterSession session( *m_pSDTSTransactionManager, 
                *m_pSDSQueueManager,
                m_host,
                m_port,
                m_passNumber, 
                m_clientSSI, CMREGISTRATIONTYPE_DEFAULT);
    
	        session.call();

            m_registrationTime = ACE_OS::gettimeofday();
            m_registrationState = Radio::REGISTRATION_STARTED;
        }
        catch (...)
        {
        }
        ret = true;
        break;
    }
	 
    FUNCTION_EXIT;
    return ret;
}


void RadioSDS::cleanupSDS()
{
    if (m_registrationState != Radio::NOT_REGISTERED)
    {
        //Let all SDS activities stop receiving notifications from Motorola SDS API; 
        TA_IRS_App::SDSUnregisterSession sdsUnregisterSession( *m_pSDTSTransactionManager,*m_pSDSQueueManager);
    
        // The following "call" does the following things via QueueManager;
        // Let m_queueManager stop monitoring the specified TcpSocket;
        // The speicified TcpSocket will call "terminateAndWait()";
        // Let m_queueManager stop inputQueue & outputQueue;
    
        sdsUnregisterSession.call();
        m_registrationState = Radio::NOT_REGISTERED;
    }
}

void RadioSDS::sendSDS(unsigned long recipientSSI, std::vector<unsigned char> rawMessage)
{
    FUNCTION_ENTRY("sendSDS");

	if (!getActiveSDSConnection())
	{
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Not an active connection, returning");
		return;
	}

    if (m_registrationState != Radio::REGISTERED)
    {
        TA_THROW(TA_Base_Core::TransactiveException("SDTS not initialized"));
    }

    TA_IRS_App::SDSSendText* sender = new TA_IRS_App::SDSSendText(*m_pSDTSTransactionManager, *m_pSDSQueueManager, 
        m_clientSSI, recipientSSI, CMPROTOCOLIDENTIFIER_TRAINBORNEMSG);

   
    sender->setSDSMessage(rawMessage);
    m_pSDSOutputQueue->enqueue(sender);

    FUNCTION_EXIT;
}

void RadioSDS::sendText(unsigned long recipientSSI, TextCodingScheme encoding, std::string rawMessage, bool isBroadcast/* =false */)
{
    FUNCTION_ENTRY("sendText");
    FUNCTION_EXIT;
}


void RadioSDS::notify( TA_IRS_App::SDSEvent* sdsEvent)
{
	FUNCTION_ENTRY("notify");
    // handle events received from SDTS protocol
    try
    {
		
		if (!getActiveSDSConnection())
		{
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						 "Not an active connection, returning");
			return;
		}

		// process only when SDS is an active connection
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "create SDSRecvText from sdsEvent");
        // TODO: using the SSI, create a RadioResource
        TA_IRS_App::SDSRecvText* pSDSRecvText = dynamic_cast<TA_IRS_App::SDSRecvText*>(sdsEvent);
        if( pSDSRecvText != NULL )
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "create dataEvent from SDSRecvText");
            SDTSProtocolEvent* event = pSDSRecvText->getProtocolEvent();

            TL_DataEvent* dataEvent = dynamic_cast<TL_DataEvent*>(event);

			//TD14310 Huangxin: send acknowledgement
            unsigned long sourceSSI = 0;
            if (dataEvent != NULL)
            {
				if (dataEvent->getCMHeader()->getDestSSI() != m_clientSSI) // TD18525
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Incoming Text not intended for %s", m_consoleName.c_str());
					return;
				}
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ProcessTLDataEvent");
				bool recFlag = processTLDataEvent(dataEvent);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "After ProcessTLDataEvent");
                sourceSSI = dataEvent->getCMHeader()->getSourceSSI();
				unsigned char cmMsgRef = dataEvent->getCMHeader()->getMessageRef();

				// TD16359 - add checking if sending of delivery receipt is required
				// remove hardcode of Protocol Identifier
                if ((sourceSSI != 0) && (recFlag)) 
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sourceSSI is not zero");
					// send acknowledgement
					TA_IRS_App::SDSAckRecvText* ack = new TA_IRS_App::SDSAckRecvText(
								*m_pSDTSTransactionManager, *m_pSDSQueueManager, 
								m_clientSSI, sourceSSI, dataEvent->getCMHeader()->getProtocolId(), cmMsgRef);
					ack->setDeliveryStatus(TLDELIVERYSTATUS_RECEIVED);
					m_pSDSOutputQueue->enqueue(ack);
				} else {
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sourceSSI is zero");
				}
			}
			// TD14310 Huangxin
			else
            {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data event is null");
				CM_RegisterResponse* pResponse = dynamic_cast<CM_RegisterResponse*>(event);
                if (pResponse != NULL)
			    {
                    sourceSSI = pResponse->getClientSSI();
					if (sourceSSI == m_clientSSI) // TD18525
					{
						processCMResponse(pResponse);
					}
			    }
			    else
                {
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, " pResponse is null");
				    CM_ReportEvent* pReport = dynamic_cast<CM_ReportEvent*>(event);
                    if (pReport != NULL)
			        {
                        sourceSSI = pReport->getSourceSSI();

						if (pReport->getDestSSI() == m_clientSSI) // TD18525
						{
							std::string logStr = SDTSHelper::CM_DeliveryStatusToString(pReport->getDeliveryStatus());
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CM_ReportEvent: %s", logStr.c_str());
						} else {
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "destinationSSI != clientSSI");
						}
					} else {
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "pReport is null");
					}
                }
            }
		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sdsEvent is null");
		}
    }
    catch (TA_Base_Core::TransactiveException& e)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, e.what());
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unknown exception");
    }
	FUNCTION_EXIT;
}

//
// Helper methods
//

void RadioSDS::initialiseSDSNamedObject()
{       
    FUNCTION_ENTRY("initialiseSDSNamedObject");
    
    try        
    {  
      //  TA_Base_Core::IEntityDataList entityNames = 
      //      TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType("TrainAgent");
		unsigned long locationKey = atol(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
		TA_Base_Core::CorbaNameList trainAgentCorbaNameList = 
			TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( "TrainAgent", locationKey,true);

        TA_ASSERT(trainAgentCorbaNameList.size() == 1,"No or more than one Radio entity found in OCC");
        m_trainAgent.setCorbaName(trainAgentCorbaNameList[0]);
		m_trainAgent.setObjectTimeout(5);  //should not more than 5 seconds
        // there is only the OCC train Agent and it can be obtained locally
		/*
        if (entityNames.size() == 1)
        {        
            std::string trainAgentEntityName(entityNames[0]->getName());            

            // clean up the returned result
            for (TA_Base_Core::IEntityDataList::iterator iter = entityNames.begin();
                 iter != entityNames.end(); ++iter)
            {
                delete *iter;
            }

            m_trainAgent.setEntityName(trainAgentEntityName,true);
			// set corbatimeout to 2 seconds instead of default 20 seconds
			m_trainAgent.setObjectTimeout(2);
        }
        else if (entityNames.size() > 1)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "More that one trainAgent entity were found.");
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "No entity of type trainAgent was locally.");
        }

        entityNames.clear();
		*/

    }
    catch (const TA_Base_Core::DatabaseException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
    }
    catch (const TA_Base_Core::DataException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
    }
	catch (...)
	{
        LOG_EXCEPTION_CATCH(SourceInfo, "...", 
            "Exception caught while attempting to resolve the trainAgent.");
    }   

    FUNCTION_EXIT;
}

bool RadioSDS::processTLDataEvent(TA_IRS_App::TL_DataEvent* dataEvent)
{
    FUNCTION_ENTRY("processTLDataEvent");
    
    msgDataFormat_TrainborneSDS* sdsData = dataEvent->getTrainborneSDSData();
	
    if (sdsData != NULL)
    {

		//TD18825 marc
		//no need to send comms message if current sds object is not for trainborne
		if (!m_bTrainborne)
		{
			return dataEvent->getRECflag();
		}
		
		//TD18825
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TLDataEvent (TrainborneSDS)");
		// get Source
		
		unsigned long sourceSSI = dataEvent->getCMHeader()->getSourceSSI();
		
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Log : Source SSI %d",sourceSSI);
		
		TA_IRS_Bus::Radio::RadioResource train;
        std::string sourceAlias;

		TA_IRS_Core::IRadioSubscriber* data = NULL;
		
		
		try
		{
			data = TA_IRS_Core::RadioSubscriberFactory::getInstance().getRadioSubscriberBySSI(sourceSSI);
		}
		catch (...)
		{
		}

        if (data == NULL)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot find radio subscriber for SSI %d", sourceSSI);
			
			std::ostringstream srcData;
			srcData << sourceSSI;
            sourceAlias = srcData.str();

            train.id = 0;
            train.type = TA_IRS_Bus::Radio::RR_RADIO;
        }
        else
        {
		    train.id = data->getResourceId();
		    train.type = (TA_IRS_Bus::Radio::ERadioResourceType)data->getResourceType();
            sourceAlias = data->getAlias();
            delete data;
            data = NULL;
        }

		train.ssi = sourceSSI;
		train.alias = CORBA::string_dup(sourceAlias.c_str());

		//modified by hongzhi, use corba call instead of comms message
		TA_IRS_Bus_4677::IRecvSDSTrainAgentCorbaDef::SDSData sData;
        std::vector<unsigned char> content(sdsData->getSDSMessage());
        sData.length(content.size());

		std::ostringstream dump;
		dump << std::hex;		//dump is only used for log

        for (int i=0; i < content.size(); ++i)
        {
            sData[i] = content[i];
			//for debugging purposes
			dump << static_cast<int>(content[i]) << " ";
        }
		
		unsigned long dest_SSI=dataEvent->getCMHeader()->getDestSSI();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						"[SDS recv from radio %d to radio %d][Hex] %s", 
						sourceSSI, dest_SSI,
						dump.str().c_str());

		try
		{
			//m_trainAgent->ReceiveSDSData(sData,sourceSSI,dest_SSI);
			CORBA_CALL ( m_trainAgent, ReceiveSDSData , (sData,sourceSSI,dest_SSI) ) ;
		}//TD18706,jianghp, handle the operation mode
		catch (TA_Base_Core::OperationModeException &) 
		{
			//m_trainAgent.clear();
		//	m_trainAgent->ReceiveSDSData(sData,sourceSSI,dest_SSI);
		}
		catch (const TA_Base_Core::ObjectResolutionException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
		}
		catch(const CORBA::Exception& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", 
				TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "unknown", "unknown exception.");
		} 
		// create notification data
/*        TA_IRS_Bus::RadioServerAgentIDL::recvSDS sData;

        std::vector<unsigned char> content(sdsData->getSDSMessage());

        sData.message.length(content.size());
        for (int i = 0; i < content.size(); ++i)
        {
            sData.message[i] = content[i];
        }

		sData.originatingTrain = train;
		sData.mftName = CORBA::string_dup(m_consoleName.c_str());

		// send notification
		CORBA::Any msg;
        msg <<= sData;
        
        m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::ReceiveSDS,
            m_serverAgentKey, msg, m_entitySubsystem, m_entityLocation);
*/
    }
    else
    {
        msgDataFormat_TextMsg* textData = dataEvent->getTextMessage();
        if (textData != NULL)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TLDataEvent (ReceiveText)");

			// sending resource
    		unsigned long sourceSSI = dataEvent->getCMHeader()->getSourceSSI();
            std::string sourceAlias;

			TA_IRS_Bus::Radio::RadioResource origin;

			TA_IRS_Core::IRadioSubscriber* dataSrc = NULL;

			try
			{
				dataSrc = TA_IRS_Core::RadioSubscriberFactory::getInstance().getRadioSubscriberBySSI(sourceSSI);
			}
			catch (...)
			{
			}

            if (dataSrc == NULL)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot find radio subscriber for SSI %d", sourceSSI);

				std::ostringstream srcData;
				srcData << sourceSSI;
				sourceAlias = srcData.str();

			    origin.id = 0;
			    origin.type = TA_IRS_Bus::Radio::RR_RADIO;
            }
            else
            {
			    origin.id = dataSrc->getResourceId();
			    origin.type = (TA_IRS_Bus::Radio::ERadioResourceType)dataSrc->getResourceType();
                sourceAlias = dataSrc->getAlias();
                delete dataSrc;
                dataSrc = NULL;
            }

			origin.ssi = sourceSSI;
			origin.alias = CORBA::string_dup(sourceAlias.c_str());


			// receiving resource
            unsigned long destSSI = dataEvent->getCMHeader()->getDestSSI();
            std::string destAlias;
			TA_IRS_Bus::Radio::RadioResource destination;

			TA_IRS_Core::IRadioSubscriber* dataDest = NULL;

			try
			{
				dataDest = TA_IRS_Core::RadioSubscriberFactory::getInstance().getRadioSubscriberBySSI(destSSI);
			}
			catch (...)
			{
			}

            if (dataDest == NULL)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot find radio subscriber for SSI %d", destSSI);

				std::ostringstream destData;
				destData << destSSI;
				destAlias = destData.str();

			    destination.id = 0;
			    destination.type = TA_IRS_Bus::Radio::RR_RADIO;
            }
            else
            {
			    destination.id = dataDest->getResourceId();
			    destination.type = (TA_IRS_Bus::Radio::ERadioResourceType)dataDest->getResourceType();
                destAlias = dataDest->getAlias();
                delete dataDest;
                dataDest = NULL;
            }

			destination.ssi = destSSI;
			destination.alias = CORBA::string_dup(destAlias.c_str());

			// update DB
            time_t currentTime = ACE_OS::time();

			TA_IRS_Core::IRadioTextMessageInbox* pText = TA_IRS_Core::RadioTextMessageInboxFactory::getInstance().addNew(
				currentTime,m_session,false,textData->getText(),sourceAlias, m_consoleName);
			
			// create Notification data
			TA_IRS_Bus::RadioServerAgentIDL::recvText tData;			
			tData.originator = origin;
			tData.receivingOperator = destination;

			TA_IRS_Bus::Radio::Message msgData;
			if (pText != NULL)
			{
				msgData.key = pText->getKey();
				msgData.time = currentTime;
				msgData.isRead = pText->isRead();
				msgData.messageText = CORBA::string_dup(textData->getText().c_str());
			}

			tData.messageData = msgData;
			tData.mftName = CORBA::string_dup(m_consoleName.c_str());
			
			// send notification
			CORBA::Any msg;
            msg <<= tData;

            m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::ReceiveText,
                m_serverAgentKey, msg,
                m_entitySubsystem, m_entityLocation);
			
			delete pText;
			delete dataSrc;
			delete dataDest;
        }
    }

    FUNCTION_EXIT;
	return dataEvent->getRECflag(); // flag whether to send delivery receipt or not
}

void RadioSDS::processCMResponse(TA_IRS_App::CM_RegisterResponse* pResponse)
{
    FUNCTION_ENTRY("processCMResponse");

	if (pResponse != NULL)
	{
		int regStatus = pResponse->getRegistrationStatus();
		if ( regStatus == 0)
		{
			m_registrationState = Radio::REGISTERED;
            closeAlarm(TA_Base_Core::RadioAlarms::RadioSDSAlarm);

			//Callback interface that would till that the SDS Registration is successful.
			m_sdsRegistrationState.setSDSRegistrationState(Radio::REGISTERED);
		}
		else
		{
			std::ostringstream logStr; 
			logStr << "SDTS Registration Status code[" << regStatus <<"]" <<std::endl;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logStr.str().c_str());
		}
	}
	
    FUNCTION_EXIT;
	
}



void RadioSDS::submitAlarm(const TA_Base_Core::MessageType& pAlarm)
{
 	FUNCTION_ENTRY("submitAlarm");

    // Threadsafe 
    TA_Base_Core::ThreadGuard guard(m_lock);
	
    try
    {   
        // construct the DescriptionParameters list
        TA_Base_Core::DescriptionParameters parameters;

        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarm ( 
            pAlarm, 
            m_entityKey,	        	// entity key
            m_entityTypeKey,	    	// entity type key
            parameters,                 // parameters
		    m_entityName,               // entity name
            m_entityLocation, 		    // region key
            m_entitySubsystem);	        // subystem key
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to raise alarm");
    }

    FUNCTION_EXIT;
}

void RadioSDS::closeAlarm(const TA_Base_Core::MessageType& pAlarm)
{
 	FUNCTION_ENTRY("closeAlarm");

    // Threadsafe 
    TA_Base_Core::ThreadGuard guard(m_lock);
	
    try
    {   
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(pAlarm, m_entityKey, m_entityLocation);
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to close the alarm");
    }
    
    FUNCTION_EXIT;
}


void RadioSDS::setClientSSI(unsigned long clientSSI)
{
	m_clientSSI = clientSSI;
}

unsigned long RadioSDS::getClientSSI()
{
	return m_clientSSI;
}


void RadioSDS::setConsoleName(const std::string& name)
{
	m_consoleName = name;
}

std::string RadioSDS::getConsoleName()
{
	return m_consoleName;
}

void RadioSDS::setSDSHost(const std::string& host)
{
	m_host = host;
}

std::string RadioSDS::getSDSHost()
{
	return m_host;
}


void RadioSDS::setSDSPort(const std::string& port)
{
	m_port = port;
}

std::string RadioSDS::getSDSPort()
{
	return m_port;
}

void RadioSDS::setSDSPassNumber(unsigned long passnumber)
{
	m_passNumber = passnumber;
}

bool RadioSDS::isTrainborne()
{
	return m_bTrainborne;
}

void RadioSDS::setTrainborne(bool isTrain)
{
	m_bTrainborne = isTrain;
}

void RadioSDS::setUnregistrationState()
{
	//SDS Server connection is terminated.
	//Registration State must changed into NOT_REGISTERED state

	m_registrationState = Radio::NOT_REGISTERED;

	//Change Registration State to NOT_REGISTERED state.
	m_sdsRegistrationState.setSDSRegistrationState(Radio::NOT_REGISTERED);
}

void RadioSDS::testSDS()
{
    FUNCTION_ENTRY("test");
    
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin test Radio-train SDS Interface");
	// get Source
	unsigned long sourceSSI = 91137;
	
	TA_IRS_Bus::Radio::RadioResource train;
	std::string sourceAlias;
	
	TA_IRS_Core::IRadioSubscriber* data = NULL;
	
	sourceAlias = "hongzhi";
	
	train.id = 12;
	train.type = TA_IRS_Bus::Radio::RR_RADIO;
	
	train.alias = CORBA::string_dup(sourceAlias.c_str());
	std::vector<unsigned char> sdsMessage;
	unsigned char SDS_Ori1[] = {0x57, 0x31, 0x39, 0x31, 0x31, 0x31, 0x31, 0x33, 0x37, 0x0, 0x0, 0x0, 0x0, 0x7, 0x77, 0x4, 0x1, 0x42, 0x1, 0x0, 0x79, 0x3a};
	unsigned char SDS_Ori2[] = {0x41, 0x80, 0x0, 0x97, 0x0, 0x56, 0x4, 0x0, 0x0, 0x55, 0x4, 0x0, 0x0, 0x52, 0x0, 0xff, 0x1, 0xe, 0xa, 0x1, 0x0, 0x50};
	for(int k=0; k<100000; k++)
	{
		sdsMessage.clear();

		if(k%2==0)
		{
			sourceSSI=91137;
			for(int j=0;j<sizeof(SDS_Ori1);j++)
			{
				sdsMessage.push_back(SDS_Ori1[j]);
			}
		}
		else
		{
			sourceSSI=91110;
			for(int j=0;j<sizeof(SDS_Ori1);j++)
			{
				sdsMessage.push_back(SDS_Ori2[j]);
			}
		}
		//modified by hongzhi, use corba call instead of comms message
		TA_IRS_Bus_4677::IRecvSDSTrainAgentCorbaDef::SDSData sData;
		std::vector<unsigned char> content(sdsMessage);
		sData.length(content.size());
		
		std::ostringstream dump;
		dump << std::hex;		//dump is only used for log
		
		for (int i = 0; i < content.size(); ++i)
		{
			sData[i] = content[i];
			//for debugging purposes
			dump << static_cast<int>(content[i]) << " ";
		}
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"[hongzhi, send SDS to Train %d][Hex] %s", 
			sourceSSI, 
			dump.str().c_str());
		
		m_trainAgent->ReceiveSDSData(sData,sourceSSI,91139);
		TA_Base_Core::Thread::sleep(1000);
	}
	FUNCTION_EXIT;
}

void RadioSDS::setActiveSDSConnection (const bool & bStateValue)
{
	FUNCTION_ENTRY("setActiveSDSConnection");
	
	m_bIsActiveSDS = bStateValue;
	
	FUNCTION_EXIT;
}

bool RadioSDS::getActiveSDSConnection ()
{
	FUNCTION_ENTRY("getActiveSDSConnection");
	FUNCTION_EXIT;

	return m_bIsActiveSDS;
}
