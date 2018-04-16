/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioServant.cpp $
  * @author  A. Parker
  * @version $Revision: #7 $
  * Last modification : $DateTime: 2009/11/25 09:18:00 $
  * Last modified by : $Author: builder $
  *
  * This class implements the radio servant which provides the required servant and radio interfaces
  * The radio interfaces simply delegate to the Radio class to enable it to be unit tested without
  * all the servant requirements.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif
#include <fstream>
#include <iomanip>
#include <ios>
static char * UNKNOWN_EXCEPTION = "Unknown exception";

#include "stdio.h"
#include "Radio.h"
#include "RadioServant.h"
#include "RadioSessionException.h"
#include "RadioSession.h"
#include "core/naming/src/NamedObject.h"
#include "core/naming/src/Naming.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/types/ControlStationComms_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/entity_access/src/IRadioEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/RadioAlarms_MessageTypes.h"
#include "core/utilities/src/RunParams.h"
#include "core/process_management/IDL/src/IControlStationCorbaDef.h"
#include "core/message/types/RadioAudit_MessageTypes.h"

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioSessionCorbaDef.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"


#include "app/radio/RadioTetraAgent/src/RadioCorbaTypeConverter.h"
#include "app/radio/RadioTetraAgent/src/RadioSubscriberMonitor.h"
#include "bus/generic_agent/src/GenericAgent.h"

using namespace TA_Base_Core;
using namespace TA_IRS_App;



// Forward declarations

TA_Base_Bus::IRadioCorbaDef::ECallStatusType convertCallStatus(TA_IRS_App::ECallStatusType status);
TA_Base_Bus::IRadioCorbaDef::ECallCategory   convertCallCategory(TA_IRS_App::ECallCategory category);
TA_Base_Bus::IRadioCorbaDef::ECallType       convertCallType(TA_IRS_App::ECallType callType);
TA_Base_Bus::IRadioCorbaDef::ECallSource     convertCallSource(TA_IRS_App::ECallSource source);
TA_Base_Bus::IRadioCorbaDef::EPTTState       convertPTTState(TA_IRS_App::EPTTState pttState);

RadioServant::RadioServant( /*TA_Base_Bus::ItaScada& scada*/ )
    : m_radio(NULL)
    , m_active(false)
    //, m_scada(scada)
	, m_auditMessageSender(NULL)
{
	FUNCTION_ENTRY("Constructor");
	
	m_serverLinkStatusThread = new RadioServerLinkStatusUpdateThread();

	m_radio = new Radio(m_serverLinkStatusThread, *this/*, m_scada*/);
	
	//m_serverLinkStatusThread->start();//limin

	FUNCTION_EXIT;
}

RadioServant::~RadioServant() 
{
	FUNCTION_ENTRY("Destructor"); 
	deactivateServant();
	delete m_radio;

	m_serverLinkStatusThread->terminateAndWait();
	delete m_serverLinkStatusThread;

	TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

	// TD13048
	if (m_auditMessageSender != NULL)
	{
		delete m_auditMessageSender;
		m_auditMessageSender = NULL;
	}

	FUNCTION_EXIT;
}

void RadioServant::setEntity(TA_Base_Core::IEntityData& EntityData, TA_Base_Core::RadioGlobalEntityData& globalEntity)
{
	FUNCTION_ENTRY("setEntity");
    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "setEntity(%s)", EntityData.getDescription().c_str());

    m_objectName      = EntityData.getName();
	m_EntityKey       = EntityData.getKey();
	m_entityLocation  = EntityData.getLocation();
	m_entitySubsystem = EntityData.getSubsystem();
	m_entityTypeKey   = EntityData.getTypeKey();

	TA_Base_Core::IRadioEntityData* entity = dynamic_cast<TA_Base_Core::IRadioEntityData*>(&EntityData);
	TA_ASSERT(entity!=NULL,"Entity data incorrect type.");

    // now read global parameters

	// obtain the asset name
	m_assetName = entity->getAssetName();
	m_radio->setAgentAssetName(m_assetName);

    unsigned long xonXoffTimeout = 140;

    try
    {    
        xonXoffTimeout = globalEntity.getXonXoffTimeout();
    }
    catch ( const TA_Base_Core::TransactiveException& e )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", e.what() );
    }
    
    unsigned long circuitDataFramesPerPacket = 1;

    try
    {    
        circuitDataFramesPerPacket = globalEntity.getCircuitDataFramesPerPacket();
    }
    catch ( const TA_Base_Core::TransactiveException& e )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", e.what() );
    }

    unsigned char serialiseTransactionLevel = 9;
    try
    {
        serialiseTransactionLevel = globalEntity.getSerialiseTransactionLevel();
    }
    catch ( const TA_Base_Core::TransactiveException& e )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", e.what() );
    }


    m_radio->setParameters( entity->getPrimaryTCPServers(),
                            entity->getSecondaryTCPServers(),
							entity->getDefaultVolume(),
                            serialiseTransactionLevel,
                            xonXoffTimeout,
                            circuitDataFramesPerPacket );

	m_radio->setEntity(EntityData);

	m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::RadioAudit::Context); // TD13048
	TA_ASSERT(m_auditMessageSender!=NULL, "Cannot create audit message sender");

	FUNCTION_EXIT;
}

//****************************
// IEntity Interface Methods *
//****************************

bool RadioServant::isValid(void)
{
	FUNCTION_ENTRY("isValid");
	FUNCTION_EXIT;
	return true;
}

void RadioServant::start(void)
{
	FUNCTION_ENTRY("start");
	m_radio->start();
	activateServantWithName(m_objectName);
	FUNCTION_EXIT;
}

void RadioServant::stop(void)
{
	FUNCTION_ENTRY("stop");
	deactivateServant();
	FUNCTION_EXIT;
}

void RadioServant::update(const class TA_Base_Core::ConfigUpdateDetails &details)
{
	FUNCTION_ENTRY("update");
	FUNCTION_EXIT;
}

void RadioServant::remove(void)
{
	FUNCTION_ENTRY("remove");
	FUNCTION_EXIT;
}

void RadioServant::setToControlMode(void)
{
	FUNCTION_ENTRY("setToControlMode");
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting radio servant %s to Control mode",m_objectName.c_str());
	m_active = true;	
	m_radio->setToControlMode();
	FUNCTION_EXIT;
}

void RadioServant::setToMonitorMode(void)
{
	FUNCTION_ENTRY("setToMonitorMode");
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting radio servant %s to Monitor mode",m_objectName.c_str());
	m_active = false;
	m_radio->setToMonitorMode();
	FUNCTION_EXIT;
}


void RadioServant::onTerminate(bool peerAvailable)
{
    FUNCTION_ENTRY("setToMonitorMode");
	m_radio->onTerminate(peerAvailable);
	FUNCTION_EXIT;
}


void RadioServant::cancelSendData(const char* ITSI)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
    try
    {
	    m_radio->getRadioCircuitModeManager()->removeRadioCircuitTransfer( ITSI );
    }
    catch( const RadioCircuitModeException& rce )
    {
        throw TA_Base_Bus::IRadioCorbaDef::dataCallNotInProgressException();
    }
}

// Call Driver Methods

void RadioServant::addCallDriverRequest(unsigned char trainID,const char* trainTSI)
{
	FUNCTION_ENTRY("addCallDriverRequest");
	m_radio->addCallDriverRequest(trainID,trainTSI);
	FUNCTION_EXIT;			
}

void RadioServant::removeCallDriverRequest(unsigned char trainID)
{
	FUNCTION_ENTRY("removeCallDriverRequest");
	m_radio->removeCallDriverRequest(trainID);
	FUNCTION_EXIT;			
}

bool RadioServant::inFallback()
{
    return m_radio->inFallback();
}


void RadioServant::resetConnection()
{
    m_radio->resetConnection();
}

void RadioServant::onCallStatusChange(const TA_IRS_App::CallDetailsType& details)
{
	FUNCTION_ENTRY("onCallStatusChange");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"onCallStatusChange()begin");

	if (m_active)
	{

		std::cout << "Sending status update message for call:" << details.callID << std::endl;

		TA_Base_Bus::IRadioCorbaDef::CallStatusEvent   event;
		CORBA::Any                                data;

		event.callID       = details.callID;
		event.callStatus   = convertCallStatus(details.callStatus);
		event.callCategory = convertCallCategory(details.callCategory);
		event.sessionRef = details.sessionRef;
		event.isInvisibleToGUIApps = false;
		event.consoleKey = details.consoleID;
		event.isInvisibleToGUIApps = details.isInvisibleToGUIApps;



		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::RadioComms::RadioCallStatus );
	
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"onCallStatusChange()sendCommsMessage(m_EntityKey=%d,m_EntityLocation=%d)",m_EntityKey,m_entityLocation);

		commsMessageSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
											 m_EntityKey,
											 data,
											 0,
											 m_entityLocation,
											 NULL);
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"onCallStatusChange()end");
	FUNCTION_EXIT;
}

void RadioServant::onTrainEvent(std::string trainTSI,std::vector<unsigned char> messageData)
{
	FUNCTION_ENTRY("onTrainEvent");

	if (m_active)
	{
		std::cout << "Sending train message notification from train:" << trainTSI << std::endl;

		TA_Base_Bus::IRadioCorbaDef::SDSNotificationEvent   event;
		CORBA::Any                                     data;

		event.sendingITSI = CORBA::string_dup(trainTSI.c_str());
		event.messageData.length(messageData.size());
		for (unsigned int index = 0;index<messageData.size();index++)
		{
			event.messageData[index] = messageData[index];
		}

		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::RadioComms::TrainMessage );

		commsMessageSender->sendCommsMessage(TA_Base_Core::RadioComms::TrainMessage,
											 m_EntityKey,
											 data,
											 0,                 // subsystem
											 m_entityLocation,  // region
											 NULL);             // filter
	}

	FUNCTION_EXIT;
}

void RadioServant::onTXChange(const TXStateInfo txInfo)
{
	FUNCTION_ENTRY("onTXChange");

	if (m_active)
	{
		TA_Base_Bus::IRadioCorbaDef::PTTStatusNotification   event;
		CORBA::Any                                      data;
		
		switch (txInfo.state)
		{
			case TX_GRANTED:
				event.state = TA_Base_Bus::IRadioCorbaDef::PTT_GRANTED;
				break;
			case TX_QUEUE:
				event.state = TA_Base_Bus::IRadioCorbaDef::PTT_QUEUE;
				break;
			case TX_CEASE:
				event.state = TA_Base_Bus::IRadioCorbaDef::PTT_CEASE;
				break;
			case TX_TIMEOUT:
				event.state = TA_Base_Bus::IRadioCorbaDef::PTT_TIMEOUT;
				break;
			default:
				TA_ASSERT(false,"Unknown TX state.");
		}

		event.callID        = txInfo.callID;
		event.sessionRef    = txInfo.sessionRef;
		event.transactionID = txInfo.transactionID;
		event.userITSI      = CORBA::string_dup(txInfo.userITSI.c_str());
		event.tpiITSI       = CORBA::string_dup(txInfo.tpiITSI.c_str());

		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::RadioComms::RadioPTTNotification );

		commsMessageSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioPTTNotification,
											 m_EntityKey,
											 data,
											 0,                 // subsystem
											 m_entityLocation,  // region
											 NULL);             // filter
	}

	FUNCTION_EXIT;
}


void RadioServant::updateGX(std::string sessionId, std::string message, ConsoleID consoleId)
{
	FUNCTION_ENTRY("updateGX");

	if (m_active)
	{
		// Notify the HMI of GX status being set for this session
		TA_Base_Core::IControlStationCorbaDef::TAToGXMessage   event;
		CORBA::Any                                data;
		event.targetSessionID = CORBA::string_dup(sessionId.c_str());
		event.targetSchematicID = CORBA::string_dup("");
		event.sourceEntityName = CORBA::string_dup((TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME)).c_str());
		event.message = CORBA::string_dup(message.c_str());

		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::ControlStationComms::TAToGXMessageNotification );

		commsMessageSender->sendCommsMessage(TA_Base_Core::ControlStationComms::TAToGXMessageNotification,
											 consoleId,
											 data,
											 0,
											 m_entityLocation,
											 NULL);
	}

	FUNCTION_EXIT;
}

void RadioServant::submitAlarm( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& dp )
{
    FUNCTION_ENTRY("submitAlarm()");
    TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset( type, 
        m_EntityKey, 
        m_entityTypeKey, 
        dp,
        m_objectName,
        m_entityLocation,
        m_entitySubsystem,
		m_assetName,
		TA_Base_Core::AlarmConstants::defaultTime );
    FUNCTION_EXIT;
}

void RadioServant::onRadioFallback(bool isFallback)
{
    FUNCTION_ENTRY("onRadioFallback()");
    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Attempting to inform TrainAgent of Radio Fallback");

    // the train agent doesnt care about radio fallback status changing
    // if the session it uses is not initialised and ready to go,
    // so only tell it if the iscs session is logged in
    RadioSession* iscsRadioSession = NULL;
	iscsRadioSession = m_radio->getISCSRadioSession();

    if ( (false == iscsRadioSession->isLoggedIn()) )
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            "Not informing TrainAgent of Radio Fallback yet, iscs radio session still not logged in");

        FUNCTION_EXIT;
        //return;
    }

	// initialise m_trainAgentProxy if it isnt already
	if ( !m_trainAgentProxy)
	{
		m_trainAgentProxy = TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainOperationModeCorbaProxy();
    }
	
	try
	{
		m_trainAgentProxy->inFallback(isFallback);
		
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"Successfully informed TrainAgent of Radio Fallback" );
	}
	catch ( TA_IRS_Bus::AgentCommunicationException& ex )
	{
		std::ostringstream message;
		message << "While sending isFallback command to local train agent. " << ex.what();
		LOG_EXCEPTION_CATCH(SourceInfo, "AgentCommunicationException", message.str().c_str() );
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "...", "While sending isFallback command to local train agent." );
    }

    FUNCTION_EXIT;
}

void RadioServant::setDirectoryCallback(IRadioDirectoryCallback * directoryCallback)
{
    m_radio->setDirectoryCallback(directoryCallback);
}


void RadioServant::applyFullState(const RadioServantType& state)
{
    m_radio->applyFullState(state.radio);
}

void RadioServant::clearFullState()
{
    m_radio->clearFullState();
}

void RadioServant::getFullState(RadioServantType& state)
{
	state.entityKey = m_EntityKey; // TD9016
    m_radio->getFullState(state.radio);
}

void RadioServant::updateCallInRadioCallStack(const CallDetailsType& callDetails)
{
    m_radio->updateCallInRadioCallStack(callDetails);
}

void RadioServant::updateRadioMonitoredCallProgression(unsigned long monitorReference)
{
	this->m_radio->updateRadioMonitoredCallProgression(monitorReference);
}

void RadioServant::removeMonitoredSubscriberDetails(unsigned long monitorReference,	long serverIndex)
{
	this->m_radio->removeMonitoredSubscriberDetails(monitorReference, serverIndex);
}


void RadioServant::removeSessionFromAudioMap(SessionID sessionReference, long serverIndex)
{
	
	m_radio->removeSessionFromAudioMap(sessionReference, serverIndex);
}

void RadioServant::updateAudioEntryInSessionMap(SessionID sessionReference, 
												  AudioReference audioReference,
												  const AudioEntryType& audioEntry)
{
	m_radio->updateAudioEntryInSessionMap(sessionReference, audioReference, audioEntry);
}
void RadioServant::updateRadioResourceManagerVars(SessionID sessionReference, CallReference callReference,
												  int currentVolume, long serverIndex)
{
	m_radio->updateRadioResourceManagerVars(sessionReference, callReference, currentVolume, serverIndex);
}

void RadioServant::updateAuthorisationRequest(const AuthorisationRequestType& authorisationRequest, long serverIndex)
{
	m_radio->updateAuthorisationRequest(authorisationRequest, serverIndex);
}

//***************************
// IRadio interface methods *
//***************************

void RadioServant::removeAuthorisationRequest(CallID callID, long serverIndex)
{
	m_radio->removeAuthorisationRequest(callID, serverIndex);
}


TA_Base_Bus::IRadioCorbaDef::CallList* RadioServant::getCallList(ConsoleID consoleId)
{
	FUNCTION_ENTRY("getCallList");

	TA_Base_Bus::GenericAgent::ensureControlMode();
	// Get a list of calls
	TA_IRS_App::CallList  callList;
	m_radio->getCallList(consoleId, callList);

	// Get size of call stack
	CORBA::ULong iCallCount  = callList.size();
	CORBA::ULong iCallNumber = 0;

	// CORBA will delete this object
	TA_Base_Bus::IRadioCorbaDef::CallList* corbaCallList = new TA_Base_Bus::IRadioCorbaDef::CallList();
    corbaCallList->length(iCallCount);

	TA_IRS_App::CallList::iterator it;
	TA_IRS_App::CallDetailsType    callDetails;

	for (it=callList.begin();it!=callList.end();it++)
	{
		if (m_radio->getCallDetailsPartial(*it,consoleId,callDetails))
		{
			(*corbaCallList)[iCallNumber].callID       = callDetails.callID;
			(*corbaCallList)[iCallNumber].sessionRef   = callDetails.sessionRef;
            (*corbaCallList)[iCallNumber].consoleKey   = callDetails.consoleID;
			(*corbaCallList)[iCallNumber].callStatus   = convertCallStatus(callDetails.callStatus);
			(*corbaCallList)[iCallNumber].callCategory = convertCallCategory(callDetails.callCategory);
            (*corbaCallList)[iCallNumber].isInvisibleToGUIApps = callDetails.isInvisibleToGUIApps;
			iCallNumber++;
		}
	}

	FUNCTION_EXIT;
	return corbaCallList;
}

TA_Base_Bus::IRadioCorbaDef::CallDetailsType* RadioServant::getCallDetails(long callID,ConsoleID consoleId) //throw (TA_IRS_App::IRadioCorbaDef::invalidCallID)
{
	FUNCTION_ENTRY("getCallDetails");

	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_IRS_App::CallDetailsType   callDetails;

	    if (!m_radio->getCallDetails(callID,consoleId,callDetails))
	{
		// Cannot find required call ID so throw an exception
		FUNCTION_EXIT;
		throw TA_Base_Bus::IRadioCorbaDef::invalidCallIDException();  
	}
	
	// CORBA will delete this object
	TA_Base_Bus::IRadioCorbaDef::CallDetailsType* corbaCallDetails = new TA_Base_Bus::IRadioCorbaDef::CallDetailsType();
	
	corbaCallDetails->callID         = callDetails.callID;
	corbaCallDetails->sessionRef     = callDetails.sessionRef;
	corbaCallDetails->time           = callDetails.time;
	corbaCallDetails->callStatus     = convertCallStatus(callDetails.callStatus);
	corbaCallDetails->callCategory   = convertCallCategory(callDetails.callCategory);
	corbaCallDetails->callType       = convertCallType(callDetails.callType);
	corbaCallDetails->callSource     = convertCallSource(callDetails.callSource);;
	corbaCallDetails->callTime       = CORBA::string_dup(callDetails.callTime.c_str());
	corbaCallDetails->callerName     = CORBA::string_dup(callDetails.callerName.c_str());
	corbaCallDetails->callerTSI      = CORBA::string_dup(callDetails.callerTSI.c_str());
	corbaCallDetails->callerLocation = CORBA::string_dup(callDetails.callerLocation.c_str());
	corbaCallDetails->calledName     = CORBA::string_dup(callDetails.calledName.c_str());
	corbaCallDetails->calledTSI      = CORBA::string_dup(callDetails.calledTSI.c_str());
	corbaCallDetails->calledLocation = CORBA::string_dup(callDetails.calledLocation.c_str());
	corbaCallDetails->isEmergency    = callDetails.isEmergency;
	//TD 10234
    corbaCallDetails->isInvisibleToGUIApps = callDetails.isInvisibleToGUIApps;

    if (!callDetails.participants.empty())
    {
        int numParticipants = callDetails.participants.size();
        corbaCallDetails->participants.length(numParticipants);

        for ( int pNum=0 ; pNum<numParticipants ; pNum++ )
        {
            corbaCallDetails->participants[pNum].name       = CORBA::string_dup(callDetails.participants[pNum].name.c_str());
            corbaCallDetails->participants[pNum].TSI        = CORBA::string_dup(callDetails.participants[pNum].TSI.c_str());
            corbaCallDetails->participants[pNum].location   = CORBA::string_dup(callDetails.participants[pNum].location.c_str());
            corbaCallDetails->participants[pNum].isAcknowledged = callDetails.participants[pNum].isAcknowledged;
            corbaCallDetails->participants[pNum].pttState   = convertPTTState(callDetails.participants[pNum].pttState);
        }
    }
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getCallDetails() Got Call Details: ID=%d, console=%lu, Caller=%s (%s), Called=%s (%s)",
                                                                callID, consoleId,
                                                                callDetails.callerName.c_str(),
                                                                callDetails.callerTSI.c_str(),
                                                                callDetails.calledName.c_str(),
                                                                callDetails.calledTSI.c_str());

	// Return the details
	FUNCTION_EXIT;
	return corbaCallDetails;
}

TA_Base_Bus::IRadioCorbaDef::ECallCategory RadioServant::getCallCategory(long callID,ConsoleID consoleId) //throw (TA_IRS_App::IRadioCorbaDef::invalidCallID)
{
	FUNCTION_ENTRY("getCallCategory");

	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_IRS_App::CallDetailsType   callDetails;

	if (!m_radio->getCallDetailsPartial(callID,consoleId,callDetails))
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "getCallCategory() Failed to locate call ID=%d (console=%lu)", callID, consoleId);
		// Cannot find required call ID so throw an exception
		FUNCTION_EXIT;
		throw TA_Base_Bus::IRadioCorbaDef::invalidCallIDException();  
	}

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getCallCategory() Got Call Details: ID=%d, console=%lu, Caller=%s (%s), Called=%s (%s)",
                                                                callID, consoleId,
                                                                callDetails.callerName.c_str(),
                                                                callDetails.callerTSI.c_str(),
                                                                callDetails.calledName.c_str(),
                                                                callDetails.calledTSI.c_str());
	FUNCTION_EXIT;
	return convertCallCategory(callDetails.callCategory);  
}

char * RadioServant::getCallText(long callID,ConsoleID consoleId)
{
	FUNCTION_ENTRY("getCallText");

	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_IRS_App::CallDetailsType   callDetails;

	// Find the call in the stack
	if (!m_radio->getCallDetails(callID,consoleId,callDetails))
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "getCallText() Failed to locate call ID=%d (console=%lu)", callID, consoleId);
		// Cannot find required call ID so throw an exception
		FUNCTION_EXIT;
		throw TA_Base_Bus::IRadioCorbaDef::invalidCallIDException();  
	}

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getCallText() Got Call Details: ID=%d, console=%lu, Caller=%s (%s), Called=%s (%s)",
                                                                callID, consoleId,
                                                                callDetails.callerName.c_str(),
                                                                callDetails.callerTSI.c_str(),
                                                                callDetails.calledName.c_str(),
                                                                callDetails.calledTSI.c_str());

	FUNCTION_EXIT;			
	return CORBA::string_dup(callDetails.callText.c_str());
}


void RadioServant::sendSDSMessage(const char* trainITSI, const TA_Base_Bus::IRadioCorbaDef::MessageDataType& messageData)
{
	FUNCTION_ENTRY("sendSDSMessage");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "sendSDSMessage [trainITSI=%s]", trainITSI );

	TA_Base_Bus::GenericAgent::ensureControlMode();
    // TD12462 In fallback, cannot send SDS to train if audio already established
    if (m_radio->inFallback())
    {
        TA_IRS_App::CallList callList;
        std::string tsi = trainITSI;
        m_radio->getTsiCallList(tsi, true, callList);
        // therefore if ANY calls on the list they are
        if ( callList.size() > 0 )
        {
            char errorstr[128] = {0};
            sprintf(errorstr, "Attempt to send SDS to TSI %s with active audio (callId %d) while in fallback", tsi.c_str(), callList[0]);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, errorstr );
            throw TA_Base_Bus::IRadioCorbaDef::radioException(errorstr);  
        }
    }

	std::vector<unsigned char>  message;
	for (unsigned int index=0;index<messageData.length();index++)
	{
		message.push_back(messageData[index]);
	}

    try
    {
	    m_radio->sendTrainCommand(trainITSI,message);
    }
    catch (RadioSessionException rse)
    {
        // pass on via corba back to the TrainAgent or whomever
		throw TA_Base_Bus::IRadioCorbaDef::radioException(rse.what());  
    }

	FUNCTION_EXIT;			
}

// Circuit data transfers

void RadioServant::sendData(const char* ITSI,const class TA_Base_Bus::IRadioCorbaDef::CircuitDataType& circuitData, bool useXON, const char * sessionId)
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Circuit mode data transfer size: %d", circuitData.length() );
	TA_Base_Bus::GenericAgent::ensureControlMode();
    RadioCircuitTransfer::Data data;

    for (unsigned long index=0;index < circuitData.length() ;index++)
	{
		data.push_back( circuitData[index] );
	}

    try
    {
	    m_radio->getRadioCircuitModeManager()->addRadioCircuitTransfer( data, ITSI );
    }
    catch(const RadioCircuitModeException& rce )
    {
        throw TA_Base_Bus::IRadioCorbaDef::dataCallInProgressException();
    }
}


void RadioServant::removeCallFromRadioCallStack(CallID callID, unsigned long consoleID)
{
    m_radio->removeCallFromRadioCallStack(callID,consoleID);
}

void RadioServant::closeAlarm( const TA_Base_Core::MessageType& type)
{
	TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(type,m_EntityKey,m_entityLocation);
}


//***************************
// Private helper functions *
//***************************

TA_Base_Bus::IRadioCorbaDef::ECallStatusType convertCallStatus(TA_IRS_App::ECallStatusType  status)
{
	switch (status)
	{
		case TA_IRS_App::Ringing:
			return TA_Base_Bus::IRadioCorbaDef::Ringing;
		case TA_IRS_App::Connected:
			return TA_Base_Bus::IRadioCorbaDef::Connected;
		case TA_IRS_App::ConnectedGroup:
			return TA_Base_Bus::IRadioCorbaDef::ConnectedGroup;
		case TA_IRS_App::Disconnected:
			return TA_Base_Bus::IRadioCorbaDef::Disconnected;
		case TA_IRS_App::OnHold:
			return TA_Base_Bus::IRadioCorbaDef::OnHold;
		case TA_IRS_App::Deleted:
			return TA_Base_Bus::IRadioCorbaDef::Deleted;
		default:
			TA_ASSERT(false, "No conversion specified for call status");
	}

    return ((TA_Base_Bus::IRadioCorbaDef::ECallStatusType)-1);
}

TA_Base_Bus::IRadioCorbaDef::ECallCategory convertCallCategory(TA_IRS_App::ECallCategory category)
{
	switch (category)
	{
		case TA_IRS_App::HalfDuplex:
			return TA_Base_Bus::IRadioCorbaDef::HalfDuplex;
		case TA_IRS_App::FullDuplex:
			return TA_Base_Bus::IRadioCorbaDef::FullDuplex;
		case TA_IRS_App::TextMessage:
			return TA_Base_Bus::IRadioCorbaDef::TextMessage;
		case TA_IRS_App::CallDriver:
			return TA_Base_Bus::IRadioCorbaDef::CallDriver;
		default:
			TA_ASSERT(false, "No conversion specified for call status");
	}

    return ((TA_Base_Bus::IRadioCorbaDef::ECallCategory)-1);
}

TA_Base_Bus::IRadioCorbaDef::ECallType convertCallType(TA_IRS_App::ECallType callType)
{
    switch (callType)
    {
        case TA_IRS_App::SingleCall:
            return TA_Base_Bus::IRadioCorbaDef::SingleCall;
            break;
        case TA_IRS_App::GroupAcknowledgeCall:
            return TA_Base_Bus::IRadioCorbaDef::GroupAcknowledgeCall;
            break;
        case TA_IRS_App::GroupUnacknowledgeCall:
            return TA_Base_Bus::IRadioCorbaDef::GroupUnacknowledgeCall;
            break;
        case TA_IRS_App::GroupBroadcastCall:
            return TA_Base_Bus::IRadioCorbaDef::GroupBroadcastCall;
            break;
        default:
			// TA_ASSERT(false, "No conversion specified for call type");
            break;
    }

    // to avoid compiler warnings about not returning a value
    return TA_Base_Bus::IRadioCorbaDef::SingleCall;
}

TA_Base_Bus::IRadioCorbaDef::ECallSource convertCallSource(TA_IRS_App::ECallSource source)
{
	switch (source)
	{
		case TA_IRS_App::IncomingCall:
			return TA_Base_Bus::IRadioCorbaDef::IncomingCall;
		case TA_IRS_App::OutgoingCall:
			return TA_Base_Bus::IRadioCorbaDef::OutgoingCall;
		default:
			TA_ASSERT(false, "No conversion specified for call source");
	}

    return ((TA_Base_Bus::IRadioCorbaDef::ECallSource)-1);
}


TA_Base_Bus::IRadioCorbaDef::EPTTState convertPTTState(TA_IRS_App::EPTTState pttState)
{ 
	switch (pttState)
	{
		case PTT_GRANTED: return TA_Base_Bus::IRadioCorbaDef::PTT_GRANTED;
		case PTT_QUEUE: return TA_Base_Bus::IRadioCorbaDef::PTT_QUEUE;
		case PTT_CEASE: return TA_Base_Bus::IRadioCorbaDef::PTT_CEASE;
		case PTT_TIMEOUT: return TA_Base_Bus::IRadioCorbaDef::PTT_TIMEOUT;
		default:
			TA_ASSERT(false,"Invalid PTT State.");
	}

	return TA_Base_Bus::IRadioCorbaDef::PTT_CEASE;
}



void RadioServant::onCircuitModeActivity(const char * ITSI, bool successful, const char * failReason)
{
	FUNCTION_ENTRY("onCircuitModeActivity");

	if (m_active)
	{

		std::cout << "Sending circuit mode notification for datacall to itsi " << ITSI << std::endl;

		TA_Base_Bus::IRadioCorbaDef::SendDataNotification event;
		CORBA::Any data;

		event.ITSI          = CORBA::string_dup(ITSI);
		event.isSuccessful  = successful;
		event.failReason    = CORBA::string_dup(failReason);

		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::RadioComms::RadioCircuitModeNotification );

		commsMessageSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCircuitModeNotification,
											 m_EntityKey,
											 data,
											 0,
											 m_entityLocation,
											 NULL);
	}

	FUNCTION_EXIT;

}

void RadioServant::onSubscriberActivity(TA_IRS_App::MonitoredCallDetailsType* subscriberActivityCallDetails)
{
    FUNCTION_ENTRY("onSubscriberActivity()");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Sending RadioMonitor notification of SubscriberActivity (%p) for Monitor %lu Call %lu",
                subscriberActivityCallDetails, subscriberActivityCallDetails->monitorID, subscriberActivityCallDetails->callID);

	TA_Base_Bus::IRadioCorbaDef::MonitoredCallDetailsType subscriberActivityCorbaDetails;

    subscriberActivityCorbaDetails.callID           = subscriberActivityCallDetails->callID;
    subscriberActivityCorbaDetails.sessionRef       = subscriberActivityCallDetails->sessionRef;
    subscriberActivityCorbaDetails.monitorID        = subscriberActivityCallDetails->monitorID;
    subscriberActivityCorbaDetails.callTime         = CORBA::string_dup(subscriberActivityCallDetails->callTime.c_str());
    subscriberActivityCorbaDetails.callerName       = CORBA::string_dup(subscriberActivityCallDetails->callerName.c_str());
    subscriberActivityCorbaDetails.callerTSI        = CORBA::string_dup(subscriberActivityCallDetails->callerTSI.c_str());
    subscriberActivityCorbaDetails.callerLocation   = CORBA::string_dup(subscriberActivityCallDetails->callerLocation.c_str());
    subscriberActivityCorbaDetails.calledName       = CORBA::string_dup(subscriberActivityCallDetails->calledName.c_str());
    subscriberActivityCorbaDetails.calledTSI        = CORBA::string_dup(subscriberActivityCallDetails->calledTSI.c_str());
    subscriberActivityCorbaDetails.calledLocation   = CORBA::string_dup(subscriberActivityCallDetails->calledLocation.c_str());
    subscriberActivityCorbaDetails.callStatus       = RadioCorbaTypeConverter::convertMonitorStatusTypeToCorba(subscriberActivityCallDetails->monitorStatus);
    RadioCorbaTypeConverter::convertECallCategoryNormalToCorba(subscriberActivityCallDetails->callCategory, subscriberActivityCorbaDetails.callCategory);
    RadioCorbaTypeConverter::convertECallTypeNormalToCorba(subscriberActivityCallDetails->callType, subscriberActivityCorbaDetails.callType);
	
    // standard corba notification tricks
    CORBA::Any subscriberActivityCorbaData;
    subscriberActivityCorbaData <<= subscriberActivityCorbaDetails;
    TA_Base_Core::CommsMessageSender * commsMessageSender = 
        TA_Base_Core::MessagePublicationManager::getInstance()
            .getCommsMessageSender( TA_Base_Core::RadioComms::RadioMonitorNotification );

    commsMessageSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioMonitorNotification,
                                         m_EntityKey,
                                         subscriberActivityCorbaData,
                                         0,
                                         m_entityLocation);
    FUNCTION_EXIT;
}
//*************************************************
// IRadioDirectory - Speed Dial Interface Methods *
//*************************************************            
TA_Base_Bus::IRadioCorbaDef::SpeedDialList* RadioServant::getSpeedDialSet(const char* setName)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
    return m_radio->getSpeedDialSet(setName);
}

void RadioServant::updateSpeedDialSet(const TA_Base_Bus::IRadioCorbaDef::SpeedDialSet& setDetails, const char* sessionID)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
    m_radio->updateSpeedDialSet(setDetails, sessionID);
}

void RadioServant::updateSpeedDial(const char* setName, const TA_Base_Bus::IRadioCorbaDef::SpeedDialEntry& newEntry, const char* sessionID)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
    m_radio->updateSpeedDial(setName, newEntry, sessionID);
}

char* RadioServant::getUserSetName(const char* sessionID,TA_Base_Bus::IRadioCorbaDef::EUserSetTypeMatch setTypeMatch)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
    return CORBA::string_dup(m_radio->getUserSetName(sessionID).c_str());
}

//**********************************
// IRadioSession Interface Methods *
//**********************************
TA_Base_Bus::IRadioCorbaDef::SubscriberList*     RadioServant::getGroupSubscribers()
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_Base_Bus::IRadioCorbaDef::SubscriberList* list;

	try
	{
		list = m_radio->getGroupSubscribers();    
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());  
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);  
	}

    return list;
}

TA_Base_Bus::IRadioCorbaDef::SubscriberList*     RadioServant::getPatchSubscribers()
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_Base_Bus::IRadioCorbaDef::SubscriberList* list;

	try
	{
		list = m_radio->getPatchSubscribers();
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());  
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);  
	}

    return list;
}

TA_Base_Bus::IRadioCorbaDef::SubscriberList*     RadioServant::getIndividualSubscribers()
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_Base_Bus::IRadioCorbaDef::SubscriberList* list;

	try
	{  
		list = m_radio->getIndividualSubscribers();
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());  
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);  
	}

    return list;
}

TA_Base_Bus::IRadioCorbaDef::SubscriberDetails*  RadioServant::getSubscriberDetails(const char* TSI)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	if (strlen(TSI)<1)
	{
		throw TA_Base_Bus::IRadioCorbaDef::radioInvalidSubscriberException();  
	}

    TA_Base_Bus::IRadioCorbaDef::SubscriberDetails* details;
		
	try
	{
		details = m_radio->getSubscriberDetails(TSI);
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());  
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);  
	}

    return details;
}

TA_Base_Bus::IRadioCorbaDef::SubscriberList*     RadioServant::getGroupMembers(const char* TSI)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	if (strlen(TSI)<1)
	{
		throw TA_Base_Bus::IRadioCorbaDef::radioInvalidSubscriberException();  
	}

    TA_Base_Bus::IRadioCorbaDef::SubscriberList* list;
		
	try
	{
		list = m_radio->getGroupMembers(TSI);
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());  
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);  
	}

    return list;
}

TA_Base_Bus::IRadioCorbaDef::SubscriberList*     RadioServant::getPatchMembers(const char* TSI)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	if (strlen(TSI)<1)
	{
		throw TA_Base_Bus::IRadioCorbaDef::radioInvalidSubscriberException();  
	}

    TA_Base_Bus::IRadioCorbaDef::SubscriberList* list;
		
	try
	{
		 list = m_radio->getPatchMembers(TSI);
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());  
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);  
	}

    return list;
}

TA_Base_Bus::IRadioCorbaDef::SubscriberList*     RadioServant::searchSubscriber(const char* searchText)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
    TA_Base_Bus::IRadioCorbaDef::SubscriberList* list;

	try
	{
		list = m_radio->searchSubscriber(searchText);
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());  
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);  
	}

    return list;
}

void RadioServant::setDataPointLink( TA_Base_Bus::DataPoint* dp )
{
	m_serverLinkStatusThread->setDataPoint(dp);
}
