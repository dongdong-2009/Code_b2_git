#include "app/radio/radio_mft_agent/src/RadioMFTAgentAliveHelper.h"

//std::string RadioMFTAgentAliveHelper::RADIO_FALLBACK_DATAPOINT_POSTFIX = ".SYS.ATS.ATSRCS.aiiRadio-NormalFallBack";

RadioMFTAgentAliveHelper::RadioMFTAgentAliveHelper(RadioMFTAgentRemoteApiEventImpl* event) : m_pRadioEvent(event)
{
	initialize();
}

RadioMFTAgentAliveHelper::~RadioMFTAgentAliveHelper()
{
	//delete m_pRadioEvent;
	//m_pRadioEvent = NULL;
	TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
}

void RadioMFTAgentAliveHelper::initialize()
{
	TA_Base_Core::ThreadGuard guard(m_statusLock);
	FUNCTION_ENTRY("initialize");

	// initialize all to normal mode
	//m_curIscsStatus = m_curRadioStatus = false;

	std::string locationName;

	TA_Base_Core::IEntityData* mftAgentEntity = RadioMFTAgentHelper::getRadioMFTAgentEntity();
	locationName = mftAgentEntity->getLocationName();
	delete mftAgentEntity;
	mftAgentEntity = NULL;

	//std::string dpName = locationName + RADIO_FALLBACK_DATAPOINT_POSTFIX;

	//TA_Base_Core::IEntityData* dpEntity = NULL;
	//
	//try {
	//	dpEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(dpName);
	//} 	
	//catch (TA_Base_Core::DataException& e) {
	//	delete dpEntity;
	//	dpEntity = NULL;
	//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception, datapoint not found in the database");
	//	return;
	//}

	//try
	//{				
	//	TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(dpEntity->getKey(), *this, m_dpProxy);
	//}
	//catch (...)
	//{
	//	delete dpEntity;
	//	dpEntity = NULL;
	//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Radio Fallback data point proxy not successfully created");
	//	FUNCTION_EXIT;
	//	return;
	//}
	//delete dpEntity;
	//dpEntity = NULL;

	// initialize observer of RadioComms_Message
	std::string radioEntityName;
	std::string convert;		
	
	convert = locationName.substr(1,locationName.size()-1);
	boost::to_lower(convert);
	locationName = locationName.at(0) + convert;

	radioEntityName = locationName + "RadioTetraAgent";
	TA_Base_Core::IEntityData* radioEntity;
	try {
		radioEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(radioEntityName);
	}
	catch (TA_Base_Core::TransactiveException& e) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TransactiveException");
		return;
	}

	unsigned long locationKey, entityKey, subsystemKey = 0;
	try
	{
		entityKey    = radioEntity->getKey();
		subsystemKey = radioEntity->getSubsystem();
		locationKey = radioEntity->getLocation();
	}
	catch(...)
	{
		delete radioEntity;
		radioEntity = NULL;
		return;
	}
	delete radioEntity;
	radioEntity = NULL;

	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
		TA_Base_Core::RadioComms::RadioSessionUpdateNofification,
		(SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*)(this),
		entityKey,
		subsystemKey,
		locationKey);

	// initialize status of iscs
	m_radioAgent.setEntityName(RadioMFTAgentHelper::getRadioServerAgentEntityName(),true);	
	bool iscsStatus = false;
	try
	{
		CORBA_CALL_RETURN(iscsStatus, m_radioAgent,getIscsStatus,());
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error connecting to the radio agent. Set mode as ISCS normal mode");
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ISCS is in %s mode", (iscsStatus) ? "Degraded" : "Normal");
	The_StatusManager::instance()->setIscsStatus(iscsStatus);
	
	// initialize the status of radio
	bool radioStatus = false;
	try
	{
		CORBA_CALL_RETURN(radioStatus, m_radioAgent,getRadioStatus,());
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error connecting to the radio agent. Set mode as ISCS normal mode");
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Radio is in %s mode", (radioStatus) ? "Degraded" : "Normal");
	The_StatusManager::instance()->setRadioStatus(radioStatus);

	FUNCTION_EXIT;
}

//void RadioMFTAgentAliveHelper::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
//{
//	TA_Base_Core::ThreadGuard guard(m_statusLock);
//	FUNCTION_ENTRY("processEntityUpdateEvent");
//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[temp] does not process dp value update, pass it");
	//if(TA_Base_Bus::ValueStateUpdate != updateType && TA_Base_Bus::ControlStateUpdate != updateType) {
	//	FUNCTION_EXIT;
	//	return;
	//}
	//bool curStatus = getDpValue();
	//bool prevStatus = The_StatusManager::instance()->getRadioStatus();
	//if (curStatus != prevStatus) {
	//	The_StatusManager::instance()->setRadioStatus(curStatus);
	//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Point ...SYS.ATS.ATSRCS.aiiRadio-NormalFallBack is changed. Updated value = %u (0-normal, 1-degraded)", curStatus);
	//	if (curStatus) {
	//		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Radio is in degraded mode. Resync resource with the radio");
	//		m_pRadioEvent->onMcdiLocalTrunking();
	//	} else {			
	//		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Radio is back to normal mode. Resync resource with the database");
	//		m_pRadioEvent->onMcdiNormalMode();
	//	}
	//} else {
	//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Processing update of data point value, but data point value is still the same (%u). No change", curStatus);
	//}
//	FUNCTION_EXIT;
//}

//bool RadioMFTAgentAliveHelper::getDpValue()
//{
//	FUNCTION_ENTRY("getDpValue");
//	bool value = false;
//	if (TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON == m_dpProxy->getStatus()) {
//		value = m_dpProxy->getValueAsBool();
//	}
//	FUNCTION_EXIT;
//	return value;
//}

//bool RadioMFTAgentAliveHelper::getRadioStatus()
//{
//	TA_Base_Core::ThreadGuard guard(m_statusLock);
//	return m_curRadioStatus;
//}
//
//bool RadioMFTAgentAliveHelper::getIscsStatus()
//{	
//	TA_Base_Core::ThreadGuard guard(m_statusLock);
//	return m_curIscsStatus;
//}

void RadioMFTAgentAliveHelper::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[temp] receiveSpecialisedMessage");
	TA_Base_Core::ThreadGuard guard(m_statusLock);
	FUNCTION_ENTRY("receiveSpecialisedMessage");
	CORBA::UShort statusC;
	if( (message.messageState >>= statusC) == 0 )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[temp] casting fail");
		FUNCTION_EXIT;
		return;
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[temp] casting successful");
	unsigned int status = statusC;
	bool prevIscsStatus = The_StatusManager::instance()->getIscsStatus();
	bool prevRadioStatus = The_StatusManager::instance()->getRadioStatus();

	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[temp] status = %u", status);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Processing update of ISCS/Radio status value = %d(0-ISCS_NORMAL, 1-ISCS_DEGRADED, 2-RADIO_NORMAL, 3-RADIO_DEGRADED", status);
	switch (status) {
		case ISCS_NORMAL_MODE:
			The_StatusManager::instance()->setIscsStatus(0);
			if (prevIscsStatus) {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ISCS changed into normal mode. Reconnect to MCC now");
				The_StatusManager::instance()->failConnectToRadio();
			}
			break;
		case ISCS_DEGRADED_MODE:
			The_StatusManager::instance()->setIscsStatus(1);
			if(!prevIscsStatus) {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ISCS changed into degraded mode. Reconnect to MCC now");
				The_StatusManager::instance()->failConnectToRadio();
			}
			break;
		case RADIO_NORMAL_MODE:
			The_StatusManager::instance()->setRadioStatus(0);
			if(prevRadioStatus) {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Radio hardware changed into normal mode. Reconnect to MCC now");
				The_StatusManager::instance()->failConnectToRadio();
			}
			break;
		case RADIO_DEGRADED_MODE:
			The_StatusManager::instance()->setRadioStatus(1);
			if(!prevRadioStatus) {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Radio hardware changed into degraded mode. Reconnect to MCC now");
				The_StatusManager::instance()->failConnectToRadio();
			}
			break;
	}
	

	//if (curStatus != prevStatus) {
	//	The_StatusManager::instance()->setIscsStatus(curStatus);
	//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ISCS status is changed. Updated value = %u (0-normal, 1-degraded)", curStatus);
	//} else {
	//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Processing update of ISCS status, but value is still the same(%u). No change", curStatus);
	//}
	FUNCTION_EXIT;
}