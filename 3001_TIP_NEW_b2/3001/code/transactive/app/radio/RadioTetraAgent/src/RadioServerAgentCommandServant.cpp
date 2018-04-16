/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioServerAgentCommandServant.cpp $
 * @author:  Anggiono
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */


#include "app/radio/RadioTetraAgent/src/RadioServerAgentCommandServant.h"
#include "app/radio/RadioTetraAgent/src/RadioServerAgentFramework.h"
#include "app/radio/RadioTetraAgent/src/RadioSDS.h"
#include "app/radio/RadioTetraAgent/RadioTcpCommunication/src/TcpMethodTimeoutException.h"
#include "app/radio/RadioTetraAgent/RadioTcpCommunication/src/TcpException.h"

#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodDynamicGroup.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodCancelCallForwarding.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodProfileRequest.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodQueryCallForwarding.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodSubscriberMonitor.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventQueryCallForwarding.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventSubscriberMonitor.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/radio/radio_bus/src/radiotypes.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include <stdlib.h>
#include <time.h>
#include "omniORB4/CORBA.h"

#define DYNAMIC_GROUP_CONNECT	0
#define DYNAMIC_GROUP_RELEASE	1

#define AGENTCONNECTED		true
#define AGENTNOTCONNECTED	false

namespace TA_IRS_App
{

unsigned int RadioServerAgentCommandServant::OCC_LOCATION_KEY = 1;

RadioServerAgentCommandServant::RadioServerAgentCommandServant(TA_Base_Core::IEntityDataPtr EntityData,
RadioServerAgentFramework* parentFramework) :
   // m_agentCmd(EntityData),
	m_parentFramework(parentFramework),
	m_cmdSender(NULL),
	m_iscsStatus(false),
	m_pdsManager(NULL),
	m_lTrainID(0),
	m_sEndofTtisDownloadStatus(0),
	m_lEndofTtisDownloadTrainID(0),
	m_radioGatewayConnectionStatus (AGENTNOTCONNECTED),
	m_sdsLinkStatusDP(NULL),
	m_inControlMode(false)
{
    FUNCTION_ENTRY("constructor");

    m_objectName      = EntityData->getName();
	m_EntityKey       = EntityData->getKey();
	m_entityLocation  = EntityData->getLocation();
	m_locationKey     = EntityData->getLocation();

    FUNCTION_EXIT;
}


RadioServerAgentCommandServant::~RadioServerAgentCommandServant()
{
    FUNCTION_ENTRY("destructor");

	//clean up for RadioPDSManager
	if ( m_pdsManager )
	{	
		TA_Base_Core::Thread::sleep(1000);
		m_pdsManager->terminateAndWait();
		delete m_pdsManager;
		m_pdsManager = 0;
	}

	//set RadioSDShandler in m_serverMonitor to NULL
	m_serverMonitor.setRadioSDSHandler(NULL);

    FUNCTION_EXIT;
}

//void RadioServerAgentCommandServant::setSummary(RadioServerAgentSummary& summary)
//{
//    m_agentCmd.setSummary(summary);
//}

void RadioServerAgentCommandServant::setRadioSDS(RadioSDSSummary* pSdsSummary)
{
	//set RadioSDSHandler
	m_serverMonitor.setRadioSDSHandler(&m_sdsHandler);
    //m_radioSDS = pSDS;
	m_pRadioSDSSummary = pSdsSummary;
	m_pRadioSDSSummary->loadSDSParameters();

	RadioSDS* pPrimarySDS;
	RadioSDS* pSecondarySDS;	

	//std::vector<RadioSDS*> pSDSList;
	RadioSDSSummary::SDSConsoleDataMap consoleMap = m_pRadioSDSSummary->getConsoleDataMap();
	
	// for now, it always equal to 1
	if (consoleMap.size() == 1) 
	{
		RadioSDSSummary::SDSConsoleDataMap::iterator it=consoleMap.begin();
		RadioSDSSummary::SDSConsoleData consoleData = it->second;
		pPrimarySDS = m_sdsHandler.createRadioSDS( m_pRadioSDSSummary,
													  it->first, 
													  consoleData.passNumber, 
													  consoleData.clientSSI,
													  true,this);
		m_serverMonitor.setRadioSDS(pPrimarySDS, true);

		//Fixed: C955-RADIO-MultipleDeport
		//Check the location type if it is DEPOT or OCC location based on locationkey
		TA_Base_Core::ILocation::ELocationType locationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(m_locationKey);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "LocationKey : %d, LocationType : %d", 
			                    m_locationKey, static_cast<int>(locationType));
		
		// only if it is at depot or Occ
		if ( (locationType == TA_Base_Core::ILocation::DPT ) ||
			 (locationType == TA_Base_Core::ILocation::OCC ) )
		{
			pSecondarySDS = m_sdsHandler.createRadioSDS( m_pRadioSDSSummary,
															it->first, 
															consoleData.passNumber, 
															consoleData.clientSSI,
															false,this);
			m_serverMonitor.setRadioSDS(pSecondarySDS, false);
		}
		//pSDSList.push_back(pSDS);
	}

    //m_serverMonitor.setRadioSDS(pSDSList);
}

void RadioServerAgentCommandServant::setCommandSender(const std::string &primaryAddress, const std::string &secondaryAddress,unsigned long timeout,TA_Base_Bus::DataPoint* radioDp)
{
	FUNCTION_ENTRY("RadioServerAgentCommandServant: setCommandSender");

	m_cmdSender = new BaseTcpServer(this, primaryAddress,secondaryAddress,timeout);

	if (OCC_LOCATION_KEY == m_locationKey) {
		m_mftBroadcast = new RadioResourceSync();
		m_mftProfileHandler = new RadioProfileHandler(m_cmdSender, m_mftBroadcast);
	}

	m_radioStatusHandler = new RadioStatusHandler(m_cmdSender, radioDp);

	FUNCTION_EXIT;
}

void RadioServerAgentCommandServant::setCommandPDSServer( const std::string& udpServer, const std::string& udpServicePort, const CommonTypes::SRadioPDSTimer& radioPDSTimer )
{
	try
	{
		m_pdsManager = new RadioPDSManager( udpServer,udpServicePort,radioPDSTimer, this );
		m_pdsManager->start();
	}
	catch (...)
	{
		LOG_GENERIC (SourceInfo,
			TA_Base_Core::DebugUtil::DebugError,
			"Cannot Instantiate Radio Manager");

		//make sure to remove the m_pdsManager object
		delete m_pdsManager;
		//set m_pdsManger to NULL
		m_pdsManager  = NULL;
	}
}

void RadioServerAgentCommandServant::setMccHandler(std::string username, std::string password, std::string ior)
{
	FUNCTION_ENTRY("setMccHandler");
	if (OCC_LOCATION_KEY == m_locationKey) {
		m_radioMftHandler = new RadioMFTHandler(ior, username, password, m_mftBroadcast, m_mftProfileHandler);
	}
	FUNCTION_EXIT;
}

//****************************
// IEntity Interface Methods *
//****************************

bool RadioServerAgentCommandServant::isValid(void)
{
    FUNCTION_ENTRY("isValid");
    FUNCTION_EXIT;
    return true;
}

void RadioServerAgentCommandServant::start(void)
{
    activateServantWithName(m_objectName);
	//m_cmdSender->start();
}

void RadioServerAgentCommandServant::stop(void)
{
    deactivateServant();
    //m_agentCmd.cleanupCADI();
	m_sdsHandler.clearSDS();
	m_sdsHandler.cleanUpSDS();
	m_cmdSender->onTerminate(true);
}

void RadioServerAgentCommandServant::update(const class TA_Base_Core::ConfigUpdateDetails &details)
{
}

void RadioServerAgentCommandServant::remove(void)
{
}

void RadioServerAgentCommandServant::setToControlMode(void)
{
//    m_serverMonitor.setCommandImpl(&m_agentCmd);
	//In Control mode already: data node & m_commandServant->setToMonitorMode();
	if ( m_inControlMode==true )
	{
		return;
	}
	m_inControlMode=true;
    m_serverMonitor.start();
	TA_ASSERT(m_cmdSender != NULL, "Command Sender (TCP Server) is null");
	m_cmdSender->setToControlMode();
	m_cmdSender->start();
	m_radioStatusHandler->start();

	if (OCC_LOCATION_KEY == m_locationKey) {
		m_radioMftHandler->start();
	}
}

void RadioServerAgentCommandServant::setToMonitorMode(void)
{
	//In monitor mode already: data node & m_commandServant->setToMonitorMode();
	if ( m_inControlMode==false )
	{
		return;
	}
	m_inControlMode=false;
	m_serverMonitor.stop();
	m_radioStatusHandler->stop();
    //m_agentCmd.cleanupCADI();
	
	m_sdsHandler.clearSDS(); //Make sure the socket connection can be closed.
	m_sdsHandler.cleanUpSDS();
	TA_ASSERT(m_cmdSender != NULL, "Command Sender (TCP Server) is null");
	m_cmdSender->setToMonitorMode();
	m_cmdSender->onTerminate(true);

	if (OCC_LOCATION_KEY == m_locationKey) {
		m_radioMftHandler->stop();
		m_mftProfileHandler->stop();
	}
}

void RadioServerAgentCommandServant::sendSDS(CORBA::ULong recipientSSI, const TA_IRS_Bus::RadioServerAgentIDL::SDSData& rawMessage)
{
	
	FUNCTION_ENTRY("sendSDS");

	checkOperationMode();

	
	if (m_radioGatewayConnectionStatus == AGENTNOTCONNECTED)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Cannot perform SDS Functionalities, gateway not connected to the agent");
		FUNCTION_EXIT;
		throw(TA_Base_Core::BadParameterException("Agent is not connected to gateway"));
	}

    bool sdsSent = false;
	try
	{
        std::vector<unsigned char> msg;
        msg.reserve(rawMessage.length());

        for (CORBA::ULong i=0; i < rawMessage.length(); ++i)
        {
            msg.push_back(rawMessage[i]);
        }
		
		m_sdsHandler.sendSDS(recipientSSI,msg);

        // TD18781 
        // ss++
        sdsSent = true;
        // ++ss

	}
	catch (const TA_Base_Core::TransactiveException& e) 
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Radio Agent Bad Param Exception");
		FUNCTION_EXIT;
		throw(TA_Base_Core::BadParameterException(e.what()));
	}
	catch (...) 
	{
		TA_Base_Core::BadParameterException ex;
		ex.reason = CORBA::string_dup("Exception occurred in call to sendSDS");
		FUNCTION_EXIT;
		throw ex;
	}

    if (!sdsSent)
    {
    	FUNCTION_EXIT;
		throw(TA_Base_Core::BadParameterException("Not connected to any SDS server"));
    }

	FUNCTION_EXIT;
}

void RadioServerAgentCommandServant::sendData(const char* ITSI, ::CORBA::ULong trainID, const ::TA_IRS_Bus::RadioServerAgentIDL::PDSDataType& pdsData, ::CORBA::Boolean useXON, const char* sessionID)
{
	FUNCTION_ENTRY( "sendData" );

	if (!m_pdsManager)
	{
		LOG_GENERIC (SourceInfo,
			     TA_Base_Core::DebugUtil::DebugError,
			     "Cannnot Proceed Library Download, Manager not Initialize"	
			    );
		return;
	}

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"Library data transfer size: %d, trainID : %d", pdsData.length(),trainID );

	TA_Base_Bus::GenericAgent::ensureControlMode();

	m_lTrainID = trainID;
	//make sure that m_data is empty
	m_data.clear();
	m_data.reserve(pdsData.length());
	//map the TrainID with the corresponding ITSI
	//Make sure that their is no existing train ID.
	CommonTypes::TrainTSIMap::iterator Iter = m_trainTSIMap.find(m_lTrainID);
	if ( Iter != m_trainTSIMap.end() )
	{
		m_trainTSIMap.erase(Iter);
	}

	//Insert the trainID with the corresponding ITSI;
	m_trainTSIMap.insert( std::pair<unsigned long, std::string>(m_lTrainID,ITSI));

	for (unsigned long index=0;index < pdsData.length() ;index++)
	{
		m_data.push_back( pdsData[index] );
	}

	try
	{
		//TODO: Used RadioPDSManager to send the data.

		m_pdsManager->makeDataCall(m_lTrainID, m_data);

		//m_radio->getRadioCircuitModeManager()->addRadioCircuitTransfer( data, ITSI );
	}
	catch(const RadioPDSManagerException& rce )
	{
		//throw TA_Base_Bus::IRadioCorbaDef::dataCallInProgressException();

		throw TA_IRS_Bus::RadioServerAgentIDL::dataCallInProgressException();
	}

	FUNCTION_EXIT;
}

void RadioServerAgentCommandServant::notifyEndofTtisDownloadStatus(::CORBA::ULong trainID, ::CORBA::UShort notifyStatus)
{
	FUNCTION_ENTRY( "notifyEndofTtisDownloadStatus" );
	if (!m_pdsManager)
	{
		LOG_GENERIC (SourceInfo,
			     TA_Base_Core::DebugUtil::DebugError,
			     "Cannnot Proceed End Library Download, Manager not Initialize"	
			    );
		return;
	}

	m_sEndofTtisDownloadStatus = notifyStatus;
	m_lEndofTtisDownloadTrainID = trainID;

	try
	{
		m_pdsManager->NotifyEndOfTtisDownloadStatus( m_lEndofTtisDownloadTrainID, m_sEndofTtisDownloadStatus );
	}
	catch( const RadioPDSManagerException& rce )
	{
		throw TA_IRS_Bus::RadioServerAgentIDL::dataCallNotInProgressException();
	}

	FUNCTION_EXIT;
}

void RadioServerAgentCommandServant::cancelSendData(::CORBA::ULong trainID, const char* sessionID)
{
	FUNCTION_ENTRY( "cancelSendData" );
	
	if (!m_pdsManager)
	{
		LOG_GENERIC (SourceInfo,
			     TA_Base_Core::DebugUtil::DebugError,
			     "Cannnot Proceed Cancel Library Download, Manager not Initialize"	
			    );
		return;
	}

	std::string strSessionID(sessionID);
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
		"trainID : %d, sessionID : %s",trainID,strSessionID.c_str());
	TA_Base_Bus::GenericAgent::ensureControlMode();

	m_lTrainID = trainID;

	try
	{
		m_pdsManager->cancelDataCall( m_lTrainID );
	}
	catch( const RadioPDSManagerException& rce )
	{
		throw TA_IRS_Bus::RadioServerAgentIDL::dataCallNotInProgressException();
	}

	FUNCTION_EXIT;
}

void RadioServerAgentCommandServant::sendText(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource, TA_IRS_Bus::Radio::ERadioServerAgentEncodeType encoding, const char* rawMessage)
{
	checkOperationMode();

	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[%s] SendText [%s]",session,rawMessage );

 //   bool textSent = false;

	//try
	//{
	//	switch(encoding)
	//	{
	//		case TA_IRS_Bus::Radio::ET_ENCODED:
	//			if (resource.type == TA_IRS_Bus::Radio::RR_RADIO)
 //               {
	//				m_sdsHandler.sendText(session,resource.ssi, TEXTCODING_ISO_UCS2, rawMessage);
	//				textSent = true;
	//			}
	//			else if ((resource.type == TA_IRS_Bus::Radio::RR_TALKGROUP) || (resource.type == TA_IRS_Bus::Radio::RR_ANNGROUP))
	//			{
	//				m_sdsHandler.sendText(session,resource.ssi, TEXTCODING_ISO_UCS2, rawMessage,true);
	//				textSent = true;
	//			}
	//			break;

	//		case TA_IRS_Bus::Radio::ET_UNENCODED:
	//			if (resource.type == TA_IRS_Bus::Radio::RR_RADIO)
 //               {
	//				m_sdsHandler.sendText(session,resource.ssi, TEXTCODING_ISO_88591, rawMessage);
	//				textSent = true;
	//			}
	//			else if ((resource.type == TA_IRS_Bus::Radio::RR_TALKGROUP) || (resource.type == TA_IRS_Bus::Radio::RR_ANNGROUP))
	//			{
	//				m_sdsHandler.sendText(session,resource.ssi, TEXTCODING_ISO_88591, rawMessage,true);
	//				textSent = true;
	//			}
	//			break;

	//		default:
	//			throw(TA_Base_Core::BadParameterException("invalid encoding type"));
	//	}
	//}
	//catch (const TA_Base_Core::TransactiveException& e) 
	//{
	//	throw(TA_Base_Core::BadParameterException(e.what()));
	//}
	//catch (...) 
	//{
	//	TA_Base_Core::BadParameterException ex;
	//	ex.reason = CORBA::string_dup("Exception occurred in call to sendText");
	//	throw ex;
	//}

 //   if (!textSent)
 //   {
 //   	throw(TA_Base_Core::BadParameterException("Not connected to any SDS server"));
 //   }

}

void RadioServerAgentCommandServant::sendCmdText(RadioMethod& method)
{
	
	FUNCTION_ENTRY("sendCmdText");
	boost::shared_ptr<RadioEvent> reply;
	std::string strErrorMsg="RadioServer sendMethod error:";
	try
	{
		reply = m_cmdSender->sendMethod(method);
		//0 = Negative/failed , 1 = Positive/ Success, 2 = No available video channels
		switch(reply->getStatus())
		{
		case 0:
			strErrorMsg.append("No response from TVSS hardware before time-out");
			throw(TA_Base_Core::TransactiveException(strErrorMsg.c_str()));
			break;
		case 1:
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"sendMethod Success");
			break;
		case 2:
			strErrorMsg.append("No available video channels");
			throw(TA_Base_Core::TransactiveException(strErrorMsg.c_str()));
			break;
		default:
			break;
		}
	}
	catch( TcpMethodTimeoutException& te )
	{
		strErrorMsg.append("No response from hardware before time-out");
		//	strErrorMsg.append(te.what());
		throw(TA_Base_Core::TransactiveException(strErrorMsg.c_str()));
	}
	catch(TcpException& e)
	{
		strErrorMsg.append("TCP connection error");
		//	strErrorMsg.append(te.what());
		throw(TA_Base_Core::TransactiveException(strErrorMsg.c_str()));
	}
	FUNCTION_EXIT;

}

void RadioServerAgentCommandServant::radioStatusMsg(CORBA::ULong recipientSSI, CORBA::ULong statusMsg)
{
	checkOperationMode();

	try
	{
		//m_agentCmd.radioStatusMsg(recipientSSI, statusMsg);
	}
	catch (const TA_Base_Core::TransactiveException& e) 
	{
		throw(TA_Base_Core::BadParameterException(e.what()));
	}
	catch (...) 
	{
		TA_Base_Core::BadParameterException ex;
		ex.reason = CORBA::string_dup("Exception occurred in call to radioStatusMsg");
		throw ex;
	}
}

bool RadioServerAgentCommandServant::connectDynamicRegroup(const TA_IRS_Bus::Radio::RadioResource& resource,const char* talkgroupAlias)
{
	FUNCTION_ENTRY("connectDynamicRegroup");
	checkOperationMode();
	bool status = false;

	try
	{
		// Call to m_agentCmd to connect to radio gateway to add talkgroup to radio
		// input: radioSSI(unsigned long), talkgroupAlias(std::string)
		// output: status (bool)
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (connectDynamicRegroup), with radio SSI = %u, talkgroup Alias = %s", resource.ssi, talkgroupAlias);
		RadioMethodDynamicGroup method(resource.ssi, talkgroupAlias,DYNAMIC_GROUP_CONNECT);
		boost::shared_ptr<RadioEvent> radioEvent = m_cmdSender->sendMethod(method);
		radioEvent->setEventLength( EVENT_LENGTH_DGNA_QUERY);
		RadioEventReader* eventReader = radioEvent->getEventReader();
		if(NULL == eventReader)
		{
			throw(TA_Base_Core::BadParameterException("failed to initialize event reader"));
		}
		RadioEventDynamicGroup* radioEventDyncamicGroup = dynamic_cast<RadioEventDynamicGroup*>(eventReader);
		status =(radioEventDyncamicGroup->getStatus()=='0');
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (connectDynamicRegroup) result [%d]",status);
		
	}
	catch (const TA_IRS_App::TcpMethodTimeoutException& e) {
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpMethodTimeoutException", e.what());
		throw (TA_IRS_App::TcpMethodTimeoutException());
	}
	catch (const TA_IRS_App::TcpException& e) {
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
		throw (TA_IRS_App::TcpException(e.what()));
	}
	catch (const TA_Base_Core::BadParameterException& e) 
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", "Bad Parameter Exception");
		throw(TA_IRS_App::TcpException(e.reason.in()));
	}
	catch (const TA_Base_Core::TransactiveException& e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
		throw(TA_Base_Core::TransactiveException(e.what()));
	}
	catch (...) 
	{
		TA_Base_Core::TransactiveException ex("Unknown Exception");
		throw ex;
	}
	FUNCTION_EXIT;
	return status;
}

bool RadioServerAgentCommandServant::releaseDynamicRegroup(const TA_IRS_Bus::Radio::RadioResource& resource,const char* talkgroupAlias)
{
	FUNCTION_ENTRY("releaseDynamicRegroup");
	checkOperationMode();
	bool status = false;

	try
	{
		// Call to m_agentCmd to connect to radio gateway to remove talkgroup from radio
		// input: radioSSI(unsigned long), talkgroupAlias(std::string)
		// output: status (bool)
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (releaseDynamicRegroup), with radio SSI = %u, talkgroup Alias = %s", resource.ssi, talkgroupAlias);
		RadioMethodDynamicGroup method(resource.ssi, talkgroupAlias,DYNAMIC_GROUP_RELEASE);
		boost::shared_ptr<RadioEvent> radioEvent = m_cmdSender->sendMethod(method);
		radioEvent->setEventLength( EVENT_LENGTH_DGNA_QUERY);
		RadioEventReader* eventReader = radioEvent->getEventReader();
		if(NULL == eventReader)
		{
			throw(TA_Base_Core::BadParameterException("failed to initialize event reader"));
		}
		RadioEventDynamicGroup* radioEventDyncamicGroup = dynamic_cast<RadioEventDynamicGroup*>(eventReader);
		status =(radioEventDyncamicGroup->getStatus()=='0');
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (releaseDynamicRegroup) result [%d]",status);

	}
	catch (const TA_IRS_App::TcpMethodTimeoutException& e) {
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpMethodTimeoutException", e.what());
		throw (TA_IRS_App::TcpMethodTimeoutException());
	}
	catch (const TA_IRS_App::TcpException& e) {
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
		throw (TA_IRS_App::TcpException(e.what()));
	}
	catch (const TA_Base_Core::BadParameterException& e) 
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", "Bad Parameter Exception");
		throw(TA_IRS_App::TcpException(e.reason.in()));
	}
	catch (const TA_Base_Core::TransactiveException& e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
		throw(TA_Base_Core::TransactiveException(e.what()));
	}
	catch (...) 
	{
		TA_Base_Core::TransactiveException ex("Unknown Exception");
		throw ex;
	}
	FUNCTION_EXIT;
	return status;
}

void RadioServerAgentCommandServant::checkOperationMode()
{
	TA_ASSERT(m_parentFramework != NULL, "Parente class framework should not be null");
	m_parentFramework->checkOperationMode();
}

void RadioServerAgentCommandServant::requestRadioCheck(const TA_IRS_Bus::Radio::RadioResource& resource, ::CORBA::String_out baseStation, ::CORBA::String_out talkgroupAlias)
{
	FUNCTION_ENTRY("requestRadioCheck");

	checkOperationMode();

	RadioResource res;
	res.alias = resource.alias;
	res.id = resource.id;
	res.ssi = resource.ssi;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (requestRadioCheck), with ISSI = %u)", resource.ssi);

	try
	{
		// Call to m_agentCmd to connect to radio gateway
		// input parameters: Radio SSI(unsigned long)
		// output parameters: baseStation alias(std::string), talkgroup alias(std::string)
		

		// testing_code for testing
		//baseStation = ::CORBA::string_dup("DTL");
		//srand(time(NULL));
		//if (rand()%10+1 < 6) talkgroupAlias = ::CORBA::string_dup("TG1"); else talkgroupAlias = ::CORBA::string_dup("TG2");
		// end of testing_code
		talkgroupAlias = ::CORBA::string_dup("TG1");
		baseStation = ::CORBA::string_dup("DTL");
		RadioMethodSubscriberMonitor method(resource.ssi);
		boost::shared_ptr<RadioEvent> radioEvent = m_cmdSender->sendMethod(method);
		radioEvent->setEventLength( EVENT_LENGTH_SUBSCRIBER_MONITORING);
		RadioEventReader* eventReader = radioEvent->getEventReader();
		if(NULL == eventReader)
		{
			throw(TA_Base_Core::BadParameterException("failed to initialize event reader"));
		}
		RadioEventSubscriberMonitor* radioEventSubscriberMonitor = dynamic_cast<RadioEventSubscriberMonitor*>(eventReader);
		baseStation =::CORBA::string_dup( radioEventSubscriberMonitor->getBaseStaionAlias().c_str());
		talkgroupAlias = ::CORBA::string_dup(radioEventSubscriberMonitor->getTalkgroupAlias().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (requestRadioCheck), with ISSI = %u,result:(baseStation:%s,talkgroupAlias:%s)", 
			resource.ssi,radioEventSubscriberMonitor->getBaseStaionAlias().c_str(),radioEventSubscriberMonitor->getTalkgroupAlias().c_str());
	//	RadioEventSubscriberMonitor* eventReply =(RadioEventSubscriberMonitor*)(radioEvent.get() );
		
	//	baseStation = eventReply->getBaseStaionAlias().c_str();
	//	talkgroupAlias = eventReply->getTalkgroupAlias().c_str();
	//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (requestRadioCheck), with ISSI = %u,result:(status:%d,baseStation:%s,talkgroupAlias:%s)", resource.ssi,radioEvent->getStatus(),baseStation,talkgroupAlias);
	}
	catch (const TA_IRS_App::TcpMethodTimeoutException& e) {
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpMethodTimeoutException", e.what());
		throw (TA_IRS_App::TcpMethodTimeoutException());
	}
	catch (const TA_IRS_App::TcpException& e) {
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
		throw (TA_IRS_App::TcpException(e.what()));
	}
	catch (const TA_Base_Core::BadParameterException& e) 
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", "Bad Parameter Exception");
		throw(TA_IRS_App::TcpException(e.reason.in()));
	}
	catch (const TA_Base_Core::TransactiveException& e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
		throw(TA_Base_Core::TransactiveException(e.what()));
	}
	catch (...) 
	{
		TA_Base_Core::TransactiveException ex("Unknown Exception");
		throw ex;
	}
	FUNCTION_EXIT;
}

void RadioServerAgentCommandServant::updateOperatorProfile(const char* opName, TA_IRS_Bus::Radio::OpTalkgroups_out talkgroups)
{
	FUNCTION_ENTRY("updateOperatorProfile");
	checkOperationMode();


	try
	{
		// Call to m_agentCmd to connect to radio gateway to update operatorprofile
		// input parameters: operator Alias (std::string)
		// output parameters: vector of 12 talkgroup alias (std::vector<std::string>)
		

		// testing_code for testing
		//for (int i = 0; i < 12; i++) {
		//	talkgroups[(CORBA::ULong)i] = "TG1";
		//}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (updateOperatorProfile), with opName = %s", opName);
		std::string strOpName(opName);
		RadioMethodProfileRequest method(strOpName);
		boost::shared_ptr<RadioEvent> radioEvent = m_cmdSender->sendMethod(method);
		radioEvent->setEventLength( EVENT_LENGTH_PROFILE_REQUEST);
		RadioEventReader* eventReader = radioEvent->getEventReader();
		if(NULL == eventReader)
		{
			throw(TA_Base_Core::BadParameterException("failed to initialize event reader"));
		}
		RadioEventProfileRequest* radioEventProfile = dynamic_cast<RadioEventProfileRequest*>(eventReader);
		std::vector<std::string> talkgroupList = radioEventProfile->getTalkgroup();	
		talkgroups = new TA_IRS_Bus::Radio::OpTalkgroups();
		talkgroups->length(12);
		for(CORBA::ULong i=0;i<talkgroupList.size();i++)
			talkgroups[i] = talkgroupList[i].c_str();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (updateOperatorProfile), with opName = %s,result:(talkgroups.size:%d)", opName,talkgroupList.size());


		
		
		// end of testing_code
	}
	catch (const TA_IRS_App::TcpMethodTimeoutException& e) {
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpMethodTimeoutException", e.what());
		throw (TA_IRS_App::TcpMethodTimeoutException());
	}
	catch (const TA_IRS_App::TcpException& e) {
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
		throw (TA_IRS_App::TcpException(e.what()));
	}
	catch (const TA_Base_Core::BadParameterException& e) 
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", "Bad Parameter Exception");
		throw(TA_IRS_App::TcpException(e.reason.in()));
	}
	catch (const TA_Base_Core::TransactiveException& e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
		throw(TA_Base_Core::TransactiveException(e.what()));
	}
	catch (...) 
	{
		TA_Base_Core::TransactiveException ex("Unknown Exception");
		throw ex;
	}
	FUNCTION_EXIT;
}

bool RadioServerAgentCommandServant::callForwarding(CORBA::Boolean condition, CORBA::ULong  forwardingISSI, CORBA::ULong  forwardedISSI)
{
	FUNCTION_ENTRY("callForwarding");
	checkOperationMode();
	bool status = false;

	try
	{
		// Call to m_agentCmd to connect to radio gateway to call forwarding
		// input: condition(boolean), forwardingISSI(unsigned long), forwardedISSI(unsigned long)
		// output: status(boolean)

		// testing_code
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (callForwarding), with condition = %lu, forwardingISSI = %lu, forwardedISSI = %lu",condition, forwardingISSI, forwardedISSI);

		
		RadioMethodCallForwardRequest method(condition, forwardingISSI, forwardedISSI);
		//RadioMethodCancelCallFowarding method(forwardingISSI);
		sendCmdText(method);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (callForwarding) done");
		status = true;
		//srand(time(NULL));
		//if (rand()%10+1 < 6) status = true; else status = false;
		// end of testing_code
	}
	catch (const TA_IRS_App::TcpMethodTimeoutException& e) {
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpMethodTimeoutException", e.what());
		throw (TA_IRS_App::TcpMethodTimeoutException());
	}
	catch (const TA_IRS_App::TcpException& e) {
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
		throw (TA_IRS_App::TcpException(e.what()));
	}
	catch (const TA_Base_Core::BadParameterException& e) 
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", "Bad Parameter Exception");
		throw(TA_IRS_App::TcpException(e.reason.in()));
	}
	catch (const TA_Base_Core::TransactiveException& e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
		throw(TA_Base_Core::TransactiveException(e.what()));
	}
	catch (...) 
	{
		TA_Base_Core::TransactiveException ex("Unknown Exception");
		throw ex;
	}
	FUNCTION_EXIT;
	return status;
}

bool RadioServerAgentCommandServant::cancelCallForwarding(const CORBA::ULong ISSI)
{
	FUNCTION_ENTRY("cancelCallForwarding");
	
	/*   //these part used to test queryCallForwarding()
	unsigned short condition;
	unsigned long forwardingISSI,forwardedISSI;
	unsigned short& _condition=condition;
	unsigned long& _forwardingISSI=forwardingISSI;
	unsigned long& _forwardedISSI= forwardedISSI;
	queryCallForwarding(ISSI,_condition,_forwardingISSI,_forwardedISSI);
	return true;
	
	*/
	/*//these part used to test updateOperatorProfile()
	
	TA_IRS_Bus::Radio::OpTalkgroups_var talkgroupsCorba;
	updateOperatorProfile("operator",talkgroupsCorba);
	return true;
	*/
	checkOperationMode();
	bool status = false;

	try
	{
		// Call to m_agentCmd to connect to radio gateway to cancel call forward a radio
		// input: condition(boolean), forwardingISSI(unsigned long), forwardedISSI(unsigned long)
		// output: status(boolean)
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (cancelCallForwarding), with ISSI = %lu", ISSI);

		RadioMethodCancelCallFowarding method(ISSI);
		sendCmdText(method);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (cancelCallForwarding) done");
		status = true;
	}
	catch (const TA_IRS_App::TcpMethodTimeoutException& e) {
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpMethodTimeoutException", e.what());
		throw (TA_IRS_App::TcpMethodTimeoutException());
	}
	catch (const TA_IRS_App::TcpException& e) {
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
		throw (TA_IRS_App::TcpException(e.what()));
	}
	catch (const TA_Base_Core::BadParameterException& e) 
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", "Bad Parameter Exception");
		throw(TA_IRS_App::TcpException(e.reason.in()));
	}
	catch (const TA_Base_Core::TransactiveException& e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
		throw(TA_Base_Core::TransactiveException(e.what()));
	}
	catch (...) 
	{
		TA_Base_Core::TransactiveException ex("Unknown Exception");
		throw ex;
	}
	FUNCTION_EXIT;
	return status;
}

void RadioServerAgentCommandServant::queryCallForwarding(CORBA::ULong ISSI, CORBA::UShort& condition, CORBA::ULong& forwardingISSI, CORBA::ULong& forwardedISSI)
{	
	FUNCTION_ENTRY("queryCallForwarding");
	checkOperationMode();

	try
	{
		// Call to m_agentCmd to connect to radio gateway to query a radio on call forwarding status
		// input: ISSI (unsigned long)
		// output: condition(unsigned short), forwardingISSI(unsigned long), forwardedISSI(unsigned long)

		//condition = 1;
		//forwardingISSI = 1122;
		//forwardedISSI = 91111;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (queryCallForwarding), with ISSI = %u", ISSI);
		RadioMethodQueryCallFowarding method(ISSI);
		boost::shared_ptr<RadioEvent> radioEvent = m_cmdSender->sendMethod(method);
		radioEvent->setEventLength( EVENT_LENGTH_DGNA_QUERY);
		RadioEventReader* eventReader = radioEvent->getEventReader();
		if(NULL == eventReader)
		{
			throw(TA_Base_Core::BadParameterException("failed to initialize event reader"));
		}
		RadioEventQueryCallForwarding* radioEventQueryCallForwarding = dynamic_cast<RadioEventQueryCallForwarding*>(eventReader);
		condition = radioEventQueryCallForwarding->getCondition();
		forwardingISSI = radioEventQueryCallForwarding->getForwardingISSI();
		forwardedISSI  = radioEventQueryCallForwarding->getForwardedISSI();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing corba call (queryCallForwarding), with ISSI = %u,result:(condition:%d,forwardingISSI:%lu,forwardedISSI:%lu)", ISSI,condition,forwardingISSI,forwardedISSI);
		switch(condition)
		{
		case 0:
		case 1:
			break;
		case 9:
			//throw(TA_Base_Core::BadParameterException("ISSI not found"));
			break;
		default:
			throw(TA_Base_Core::BadParameterException("Unknown reply status"));
			break;
		}
	}
	catch (const TA_IRS_App::TcpMethodTimeoutException& e) {
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpMethodTimeoutException", e.what());
		throw (TA_IRS_App::TcpMethodTimeoutException());
	}
	catch (const TA_IRS_App::TcpException& e) {
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
		throw (TA_IRS_App::TcpException(e.what()));
	}
	catch (const TA_Base_Core::BadParameterException& e) 
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", "Bad Parameter Exception");
		throw(TA_IRS_App::TcpException(e.reason.in()));
	}
	catch (const TA_Base_Core::TransactiveException& e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
		throw(TA_Base_Core::TransactiveException(e.what()));
	}
	catch (...) 
	{
		TA_Base_Core::TransactiveException ex("Unknown Exception");
		throw ex;
	}
	FUNCTION_EXIT;
}

void RadioServerAgentCommandServant::notifyMftAgent(short status)
{
	m_radioStatusHandler->notifyMftAgent(status);
	m_iscsStatus = status;
};

bool RadioServerAgentCommandServant::getIscsStatus()
{
	return m_iscsStatus;
}

bool RadioServerAgentCommandServant::getRadioStatus()
{
	bool status = false;
	if ( NULL != m_radioStatusHandler ) {
		status = m_radioStatusHandler->getStatus();
	} else {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Radio Status Handler is not initialized. Cannot get the radio status");
	}
	return status;
}

void RadioServerAgentCommandServant::notifyPDSDownloadStatus(const TA_IRS_App::TtisTypes::SPDSDownloadDataResults &pdsDownloadResult)
{
	FUNCTION_ENTRY( "notifyPDSDownloadStatus" );

	checkOperationMode();

	std::cout << "Sending PDS Download notification with trainID " << pdsDownloadResult.lTrainID << std::endl;

	LOG_GENERIC (SourceInfo,
		TA_Base_Core::DebugUtil::DebugInfo,
		"NotifyPDSDownloadStatus - TrainID : %d , Download Status : %d, Reason: %s",
		pdsDownloadResult.lTrainID,pdsDownloadResult.bStatus, pdsDownloadResult.strReason.c_str() );

	CommonTypes::TrainTSIMap::iterator Iter = m_trainTSIMap.find(pdsDownloadResult.lTrainID);
	std::string ITSI;

	//Make sure that Train ID exist in the map, else return
	if ( Iter == m_trainTSIMap.end())
	{
		return;
	}

	ITSI = (*Iter).second;
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"TrainID: %d , ITSI: %s",(*Iter).first,(*Iter).second.c_str() );
		

	TA_Base_Bus::IRadioCorbaDef::SendDataNotification event;
	CORBA::Any data;

	event.ITSI          = CORBA::string_dup(ITSI.c_str());
	event.isSuccessful   = pdsDownloadResult.bStatus;
	event.failReason    = CORBA::string_dup(pdsDownloadResult.strReason.c_str());

	
	data <<= event;

	TA_Base_Core::CommsMessageSender* commsMessageSender =
		TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::RadioComms::RadioCircuitModeNotification );

	commsMessageSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCircuitModeNotification,
		m_EntityKey,
		data,
		0,
		m_entityLocation,
		NULL);

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"ITSI: %s, isSuccessfull: %d, failReason: %s, m_EntityKey: %d, m_entityLocation: %d", 
		ITSI.c_str(), pdsDownloadResult.bStatus, pdsDownloadResult.strReason.c_str(), m_EntityKey, m_entityLocation );

	FUNCTION_EXIT;
}

void RadioServerAgentCommandServant::switchSDSConnection(bool toPrimary) {
	FUNCTION_ENTRY("switchSDSConnection");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Switch SDS Connection to %s connection", (toPrimary) ? "Primary" : "Secondary");

	m_sdsHandler.clearSDS();
	m_sdsHandler.cleanUpSDS();
	m_sdsHandler.switchConnection(toPrimary);

	if (toPrimary) 
	{
		m_sdsHandler.connectToSDS(m_pRadioSDSSummary->getSDTSHost(),m_pRadioSDSSummary->getSDTSPort());
	} else
	{
		m_sdsHandler.connectToSDS(m_pRadioSDSSummary->getSDTSSecondaryHost(),m_pRadioSDSSummary->getSDTSSecondaryPort());
	}

	//Registration request must perform after the SDS Connection.
	m_serverMonitor.switchRadioSDS(toPrimary);
	FUNCTION_EXIT;
}

void RadioServerAgentCommandServant::onConnectionLost()
{
	FUNCTION_ENTRY("onConnectionLost");

	m_radioGatewayConnectionStatus = AGENTNOTCONNECTED;

	//Both Gateway Server 1 & 2 are down.
	//Set SDS Link Status Datapoint
	m_sdsLinkStatusDP->setLinkStatus(false);

	//Need to force terminate any SDS Server connection
	m_sdsHandler.clearSDS();
	m_sdsHandler.cleanUpSDS();

	FUNCTION_EXIT;
}

void RadioServerAgentCommandServant::onConnectionEstablish(bool isPrimary)
{
	FUNCTION_ENTRY("onConnectionEstablish");
	switchSDSConnection(isPrimary);
	
	m_radioGatewayConnectionStatus = AGENTCONNECTED;
	
	FUNCTION_EXIT;
}

void RadioServerAgentCommandServant::onSwitchConnection(bool toPrimary)
{
	FUNCTION_ENTRY("onSwitchConnection");
	switchSDSConnection(toPrimary);
	FUNCTION_EXIT;
}

void RadioServerAgentCommandServant::setSDSLinkStatusDp(IRadioSDSLinkStatus* sdsLinkStatusDp)
{
	m_sdsLinkStatusDP = sdsLinkStatusDp;
}

void RadioServerAgentCommandServant::setSDSRegistrationState(Radio::ERegistrationState regState)
{
	//For SDS Server Registration response is successful
	if (Radio::REGISTERED == regState )
	{
		m_sdsLinkStatusDP->setLinkStatus(true);
	}
	//For SDS Server Connection Lost
	else if (Radio::NOT_REGISTERED == regState )
	{
		m_sdsLinkStatusDP->setLinkStatus(false);
	}

}

}
