
#include "app/ats/AtsAgent/AtsTCPCommunication/src/AtsTcpServerWrapper.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_IRS_App;

AtsTcpServerWrapper::AtsTcpServerWrapper(AtsServerLinkStatusUpdateThread *linkStatusThread)
: m_active(false),	m_serverLinkStatusThread(linkStatusThread),
  m_backgroundProcessor(0), server(NULL)
{
	FUNCTION_ENTRY("AtsTcpServerWrapper: Constructor");

	m_backgroundProcessor = new AtsBackgroundProcessor(this);

	FUNCTION_EXIT;
}

AtsTcpServerWrapper::~AtsTcpServerWrapper()
{
	FUNCTION_ENTRY("AtsTcpServerWrapper: Destructor");
	delete m_backgroundProcessor;
	if(server){
		delete server;
		server = NULL;
	}
}

void AtsTcpServerWrapper::setEntity(TA_Base_Core::EntityData &entityData)
{
	FUNCTION_ENTRY("TVAtsTcpServerWrapperSS: setEntity");

	server->setEntity(entityData);

	FUNCTION_EXIT;
}

void AtsTcpServerWrapper::setParameters(const std::string &primaryAddress, const std::string &secondaryAddress)
{
	FUNCTION_ENTRY("AtsTcpServerWrapper: setParameters");

	server = new AtsTcpServer(primaryAddress,secondaryAddress);

	FUNCTION_EXIT;
}
void AtsTcpServerWrapper::start()
{
	FUNCTION_ENTRY("AtsTcpServerWrapper: start");
	//start BaseTCPServer
	server->start();

	FUNCTION_EXIT;
}

void AtsTcpServerWrapper::setToControlMode()
{
	FUNCTION_ENTRY("AtsTcpServerWrapper: setToControlMode");

	m_active = true;
	server->setToControlMode();

	FUNCTION_EXIT;
}

void AtsTcpServerWrapper::setToMonitorMode()
{
	FUNCTION_ENTRY("AtsTcpServerWrapper: setToMonitorMode");

	m_active = false;
	server->setToMonitorMode();

	FUNCTION_EXIT
}

void AtsTcpServerWrapper::onTerminate(bool peerAvailable)
{
	FUNCTION_ENTRY("AtsTcpServerWrapper: onTerminate");

	server->onTerminate(peerAvailable);
	
	FUNCTION_EXIT;
}

bool AtsTcpServerWrapper::getIsActive()
{
	return m_active;
}

void AtsTcpServerWrapper::sendMethod(TA_IRS_App::AtsITcpMethod &Method)
{
	FUNCTION_ENTRY("sendMethod");
	server->sendMethod(Method);
	FUNCTION_EXIT;
}
