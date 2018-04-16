#include "app/radio/RadioTetraAgent/src/RadioMFTResponse.h"
#include "app/radio/RadioTetraAgent/src/RadioMFTHandler.h"
#include "app/radio/RadioTetraAgent/src/MccProtocolHelper.h"
#include "core/utilities/src/DebugUtil.h"

unsigned int RadioMFTResponse::KEEPALIVE_TIMEOUT = 65000;
// temporary,

/*unsigned short RadioMFTResponse::PERIPHERAL_STATUS = 0;*/ 
unsigned short RadioMFTResponse::AUDIO_PROCESSING_STATUS = 0;
unsigned short RadioMFTResponse::DATABASE_STATUS = 1; 
unsigned short RadioMFTResponse::SYSTEM_NOT_READY = 2;

RadioMFTResponse::RadioMFTResponse(RadioMFTHandler& rm, TA_Base_Core::AlarmClock& timeout) 
: m_connection(rm), 
m_timeoutThread(timeout), 
operationMode(false),
initialized(false) {
	mccReady[AUDIO_PROCESSING_STATUS] = mccReady[DATABASE_STATUS] = false;
}

RadioMFTResponse::~RadioMFTResponse() {
}

void RadioMFTResponse::CallBack(const ::MCC7500SDK::Parameters& message)
{
	if (operationMode) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Restarting timer");
		m_timeoutThread.start(KEEPALIVE_TIMEOUT);
	} else {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Ignoring message, in initialization mode");
	}
	if (message.length() == 0) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Received Keep-ALIVE message from MCC");
	} else {
		//RadioAgentMcc::MccRadioEvent* pEventM;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Received message from MCC: %s", RadioAgentMcc::MccProtocolHelper::getResponseName(message[0].V_UINT16()));
		
		switch (message[0].V_UINT16()) {
			case RadioAgentMcc::eMsgMcaPeripheralsStatusID:
				{
					RadioAgentMcc::McaPeripheralStatus* pEventM = new RadioAgentMcc::McaPeripheralStatus(message);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Event name = %s", pEventM->getEventName().c_str());
					pEventM->printMessage();
					//PERIPHERAL is not needed anymore in MCC Initialization process
					/*if (pEventM->isReady()) setMccReady(PERIPHERAL_STATUS);*/
					delete pEventM;
				}
				break;
			case RadioAgentMcc::eMsgMcdiDatabaseStatusID:
				{
					RadioAgentMcc::McdiDatabaseStatus* pEventM = new RadioAgentMcc::McdiDatabaseStatus(message);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Event name = %s", pEventM->getEventName().c_str());
					pEventM->printMessage();
					if (pEventM->isReady()) setMccReady(DATABASE_STATUS);
					delete pEventM;
				}
				break;
			case RadioAgentMcc::eMsgMcdiAudioProcessingStatusID:
				{
					RadioAgentMcc::McdiAudioProcessingStatus* pEventM = new RadioAgentMcc::McdiAudioProcessingStatus(message);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Event name = %s", pEventM->getEventName().c_str());
					pEventM->printMessage();
					if (pEventM->isReady()) setMccReady(AUDIO_PROCESSING_STATUS);
					delete pEventM;
				}
				break;
			case RadioAgentMcc::eMsgMcdiNewDatabaseAvailableID:
				{
					m_connection.acceptDatabaseUpdate();
				}
				break;
			case RadioAgentMcc::eMsgMcdiDatabaseUpdateNotificationID:
				{
					m_connection.updateIscsDatabase(message);
				}
				break;
			default:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Event discarded");
		}
	}
}

void RadioMFTResponse::setOperationMode(bool mode) 
{
	operationMode = mode;
}

void RadioMFTResponse::setMccReady(int id)
{
	if (SYSTEM_NOT_READY == id) {
		mccReady[AUDIO_PROCESSING_STATUS] = mccReady[DATABASE_STATUS] = false;
		initialized = false;
	}
	else if (initialized) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Not processing message, DB is already initialized");
		return;
	}
	else 
		mccReady[id] = true;
	

	if ( mccReady[AUDIO_PROCESSING_STATUS] && mccReady[DATABASE_STATUS])
	{
		// system is ready
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Now that MCC is ready, prepare to synchronize the database");
		m_connection.initializeDatabase();

		initialized = true;
	}
}