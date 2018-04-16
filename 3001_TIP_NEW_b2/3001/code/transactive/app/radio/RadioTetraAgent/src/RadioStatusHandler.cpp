#include "app/radio/RadioTetraAgent/src/RadioStatusHandler.h"


using namespace TA_IRS_App;

unsigned short RadioStatusHandler::OCC_SYSTEM_STATUS = 0;
unsigned short RadioStatusHandler::STATION_SYSTEM_STATUS = 1;

RadioStatusHandler::RadioStatusHandler(BaseTcpServer* sender, TA_Base_Bus::DataPoint* dp) : m_tcpSender(sender), m_dataPoint(dp)
{
	FUNCTION_ENTRY("constructor");
	initialize();
	FUNCTION_EXIT;
}

RadioStatusHandler::~RadioStatusHandler()
{
	FUNCTION_ENTRY("destructor");
	if (NULL != m_tcpSender) {
		delete m_tcpSender;
		m_tcpSender = NULL;
	}
	if (NULL != m_dpUpdateThread) {
		delete m_dpUpdateThread;
		m_dpUpdateThread = NULL;
	}
	if (NULL != m_dataPoint) {
		delete m_dataPoint;
		m_dataPoint = NULL;
	}
	FUNCTION_EXIT;
}

void RadioStatusHandler::initialize()
{
	FUNCTION_ENTRY("initialize");
	m_status = -1;
	m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().
		getCommsMessageSender(TA_Base_Core::RadioComms::Context);
	std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
	
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

	m_radioEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);
    m_entityKey = m_radioEntity->getKey();
	m_subsystemKey = m_radioEntity->getSubsystem();
	m_locationKey = m_radioEntity->getLocation();

	m_dpUpdateThread = new RadioDpStatusUpdateThread();

	if (NULL != m_dataPoint) {
		m_dpUpdateThread->setDataPoint(m_dataPoint);
    	m_dpUpdateThread->start();
	} else {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Radio Agent is not able to update datapoint");
	}

	if (0 == m_locationKey || 1 == m_locationKey) {
		m_messageType = OCC_SYSTEM_STATUS;
	} else {
		m_messageType = STATION_SYSTEM_STATUS;
	}

	
	FUNCTION_EXIT;
}

void RadioStatusHandler::start()
{
	FUNCTION_ENTRY("start");
	m_terminate = false;
	activate();
	FUNCTION_EXIT;
}

void RadioStatusHandler::stop()
{
	FUNCTION_ENTRY("stop");
	m_terminate = true;
	wait();
	FUNCTION_EXIT;
}

int RadioStatusHandler::svc()
{
	while (!m_terminate) {
		try
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Polling data from gateway server with interval 3 seconds");

			bool isOcc = (m_messageType==OCC_SYSTEM_STATUS) ? true : false;
			RadioMethodSystemStatus method(isOcc);
			boost::shared_ptr<RadioEvent> radioEvent = m_tcpSender->sendMethod(method);
			radioEvent->setEventLength(EVENT_LENGTH_RADIO_SYSTEM_STATUS);
			RadioEventReader* eventReader = radioEvent->getEventReader();

			if (NULL == eventReader)
				throw(TA_Base_Core::TransactiveException("Failed to initialized Event Reader"));

			RadioEventSystemStatus* radioEventSystemStatus = dynamic_cast<RadioEventSystemStatus*>(eventReader);
			char stat = radioEventSystemStatus->getStatus();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Current Gateway radio status: %c", stat);

			if (-1 == m_status) {				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Initializing radio datapoint");
				if (stat == '1') {
					m_status = 1;
					
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Informing MFT Agent that radio is in degraded mode");
					notifyMftAgent(RADIO_DEGRADED_MODE);

					notifyTrainAgent(true);
					m_dpUpdateThread->setLinkStatus(RadioDpStatusUpdateThread::ServerFallback, true);
				} else if (stat == '0') {
					m_status = 0;
					
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Informing MFT Agent that radio is in normal mode");
					notifyMftAgent(RADIO_NORMAL_MODE);

					notifyTrainAgent(false);
					m_dpUpdateThread->setLinkStatus(RadioDpStatusUpdateThread::ServerFallback, false);
				}
			}else if (stat == '1' && m_status == 0) {
				m_status = 1;
				CORBA::Any msg;
				msg <<= (CORBA::ULong)m_status;

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Informing MFT Agent that radio is in degraded mode");
				notifyMftAgent(RADIO_DEGRADED_MODE);

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Updating radio datapoint to degraded mode");
				notifyTrainAgent(true);
				m_dpUpdateThread->setLinkStatus(RadioDpStatusUpdateThread::ServerFallback, true);
				
			} else if (stat == '0' && m_status == 1) {
				m_status = 0;
				CORBA::Any msg;
				msg <<= (CORBA::ULong)m_status;

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Informing MFT Agent that radio is in normal mode");
				notifyMftAgent(RADIO_NORMAL_MODE);
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Updating radio datapoint to normal mode");
				notifyTrainAgent(false);
				m_dpUpdateThread->setLinkStatus(RadioDpStatusUpdateThread::ServerFallback, false);
			}
		}
		catch (const TA_Base_Core::TransactiveException& e) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occur: %s", e.what());
		}
		catch (...) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred in call to radio system status update");
		}
		
		ACE_OS::sleep(3);
	}
	return 0;
}

void RadioStatusHandler::notifyMftAgent(short status)
{
	FUNCTION_ENTRY("notifyMftAgent");

	CORBA::Any msg;
	msg <<= (CORBA::UShort)status;	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Notify MFT Agent update of ISCS station connection status with OCC: %u", status);
	m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioSessionUpdateNofification,
		m_entityKey, msg,
		m_subsystemKey, m_locationKey);
	FUNCTION_EXIT;
}

void RadioStatusHandler::notifyTrainAgent(bool status)
{
	FUNCTION_ENTRY("notifyTrainAgent");

	// initialise m_trainAgentProxy if it isnt already
	if ( !m_trainAgentProxy)
	{
		m_trainAgentProxy = TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainOperationModeCorbaProxy();
	}

	try
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Informing TrainAgent of Radio Fallback" );
		m_trainAgentProxy->inFallback(status);

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Successfully informed TrainAgent of Radio Fallback" );
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

bool RadioStatusHandler::getStatus()
{
	return m_status;
}