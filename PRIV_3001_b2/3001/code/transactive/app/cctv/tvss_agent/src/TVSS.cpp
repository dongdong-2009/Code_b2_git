
#include "app/cctv/tvss_agent/src/TVSS.h"
#include "app/cctv/tvss_agent/src/TVSSServant.h"
#include "app/cctv/tvss_agent/src/TVSSHelper.h"
#include "app/cctv/tvss_agent/TCPCommunication/src/ITCPMessageProcessor.h"

//#include "app/cctv/tvss_agent/src/TVSSException.h"
#include "bus/cctv/tvss_agent/TVSSManager/IDL/src/TVSSManagerCorbaDef.h""
#include "app/cctv/tvss_agent/TCPCommunication/src/TcpMethodTimeoutException.h"
#include "app/cctv/tvss_agent/TCPCommunication/src/TcpConnectionException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSTrainBorneHeartbeatMethod.h"
using namespace TA_IRS_App;

TVSS::TVSS(TVSSServant* servant, TVSSServerLinkStatusUpdateThread *linkStatusThread)
: m_servant(servant),
  m_active(false),	m_serverLinkStatusThread(linkStatusThread),
  m_backgroundProcessor(0),server(NULL),trainborneServer(NULL)
{
	FUNCTION_ENTRY("TVSS: Constructor");

	m_backgroundProcessor = new TVSSBackgroundProcessor(this);

	FUNCTION_EXIT;
}

TVSS::~TVSS()
{
	FUNCTION_ENTRY("TVSS: Destructor");
	delete m_backgroundProcessor;
	if(server){
		delete server;
		server = NULL;
	}
	if(trainborneServer){
		delete trainborneServer;
		trainborneServer = NULL;
	}
}

void TVSS::setEntity(TA_Base_Core::IEntityData &entityData)
{
	server->setEntity(entityData);
	trainborneServer->setEntity(entityData);
}

void TVSS::setParameters(const std::string &primaryAddress, const std::string &secondaryAddress,const std::string & primaryTrainborneAddress,const std::string & secondaryTrainborneAddress,unsigned long timeout)
{
	FUNCTION_ENTRY("TVSS: setParameters");

	server = new BaseTcpServer(dynamic_cast<ITCPMessageProcessor*>(m_servant),primaryAddress,secondaryAddress,timeout);
	trainborneServer = new BaseTcpServer(dynamic_cast<ITCPMessageProcessor*>(m_servant),primaryTrainborneAddress,secondaryTrainborneAddress,timeout);

	FUNCTION_EXIT;
}
void TVSS::start()
{
	FUNCTION_ENTRY("TVSS: start");
	//start BaseTCPServer
	server->start();
	trainborneServer->start();

	FUNCTION_EXIT;
}

void TVSS::setToControlMode()
{
	FUNCTION_ENTRY("setToControlMode");

	m_active = true;
	server->setToControlMode();
	trainborneServer->setToControlMode();

	FUNCTION_EXIT;
}

void TVSS::setToMonitorMode()
{
	FUNCTION_ENTRY("setToMonitorMode");

	m_active = false;
	server->setToMonitorMode();
	trainborneServer->setToMonitorMode();

	FUNCTION_EXIT
}

void TVSS::onTerminate(bool peerAvailable)
{
	FUNCTION_ENTRY("onTerminate");

	server->onTerminate(peerAvailable);
	trainborneServer->onTerminate(peerAvailable);
	
	FUNCTION_EXIT;
}

bool TVSS::getIsActive()
{
	return m_active;
}

void TVSS::sendMethod(TA_IRS_App::TVSSMethod &Method)
{
	FUNCTION_ENTRY("sendMethod");
	boost::shared_ptr<TVSSEvent> reply;
	std::string strErrorMsg="";
	try
	{
		reply = server->sendMethod(Method);
		TVSSEquipmentQueryMethod* equipmentQueryMethod = dynamic_cast<TVSSEquipmentQueryMethod*>( &Method);
		if(NULL!=equipmentQueryMethod)
		{
			TVSSEquipmentResponseEventReader equipmentAlarmReader(reply);
			for(int i=0;i<4;i++)
				char ch =equipmentAlarmReader.getAlarmResponseData(i);
		}
		//0 = Negative/failed , 1 = Positive/ Success, 2 = No available video channels
		switch(reply->getStatus())
		{
		case 0:
			strErrorMsg.append("No response from TVSS hardware before time-out");
			throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
			break;
		case 1:
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"sendMethod Success");
			break;
		case 2:
			strErrorMsg.append("No available video channels");
			throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
			break;
		default:
			break;
		}
	}
	catch( TcpException& te)
	{
		strErrorMsg.append("No response from TVSS hardware before time-out");
	//	strErrorMsg.append(te.what());
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	catch( TcpConnectionException& te)
	{
		strErrorMsg.append("No response from TVSS hardware before time-out");
	//	strErrorMsg.append(te.what());
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	catch( TcpMethodTimeoutException& te )
	{
		strErrorMsg.append("No response from TVSS hardware before time-out");
	//	strErrorMsg.append(te.what());
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	catch( TA_Base_Core::TransactiveException& e)
	{
		strErrorMsg.append("No response from TVSS hardware before time-out");
		//	strErrorMsg.append(te.what());
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	FUNCTION_EXIT;
}

void TVSS::sendEquipmentAlarmMethod(TA_IRS_App::TVSSMethod &Method,bool alarmStatus[])
{
	FUNCTION_ENTRY("sendEquipmentAlarmMethod");
	boost::shared_ptr<TVSSEvent> reply;
	std::string strErrorMsg="";
	try
	{
		reply = server->sendMethod(Method);
		TVSSEquipmentResponseEventReader equipmentAlarmReader(reply);
		for(int i=0;i<m_servant->GetDPCount();i++)
		{
			alarmStatus[i] =(bool)equipmentAlarmReader.getAlarmResponseData(i);
		}
	}
	catch( TcpException& te)
	{
		strErrorMsg.append("No response from TVSS hardware before time-out");
		//	strErrorMsg.append(te.what());
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	catch( TcpConnectionException& te)
	{
		strErrorMsg.append("No response from TVSS hardware before time-out");
		//	strErrorMsg.append(te.what());
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	catch( TcpMethodTimeoutException& te )
	{
		strErrorMsg.append("No response from TVSS hardware before time-out");
		//	strErrorMsg.append(te.what());
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	catch( TA_Base_Core::TransactiveException& e)
	{
		strErrorMsg.append("No response from TVSS hardware before time-out");
		//	strErrorMsg.append(te.what());
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	FUNCTION_EXIT;
}
/**
 * Corba Call Implemetation
 */

void TA_IRS_App::TVSS::sendTrainbornHeartbeatMethod()
{
	FUNCTION_ENTRY("sendTrainbornHeartbeatMethod");
	std::string strErrorMsg="";
	try
	{
        TVSSTrainBorneHeartbeatMethod method;
		trainborneServer->sendMethod(method, 0, false);
	}
	catch( TcpException& te)
	{
		strErrorMsg.append("No response from TVSS hardware before time-out");
		//	strErrorMsg.append(te.what());
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	catch( TcpConnectionException& te)
	{
		strErrorMsg.append("No response from TVSS hardware before time-out");
		//	strErrorMsg.append(te.what());
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	catch( ... )
	{
		strErrorMsg.append("Unknown exception");
		//	strErrorMsg.append(te.what());
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	FUNCTION_EXIT;
}
