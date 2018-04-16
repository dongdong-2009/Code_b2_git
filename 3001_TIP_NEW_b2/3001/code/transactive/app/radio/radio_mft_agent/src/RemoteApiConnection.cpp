#include "app/radio/radio_mft_agent/src/RemoteApiConnection.h"
#include"app/radio/radio_mft_agent/src/RemoteAPI.h"
#include "ace/Get_Opt.h"

#include"app/radio/radio_mft_agent/src/RemoteApiResponse.h"
#include"ace/Thread_Manager.h"
#include "ace/corba_macros.h"
//#include"RemoteApiRadioCommand.h"
#include<string>
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleStateManager.h"
#include "Mca/mca.h"
#include "Mcdi/callback.h"
#include "mcdi/registration.h"
#include "mcdi/authentication.h"
#include "core/utilities/src/RunParams.h"

#include "app\radio\radio_mft_agent\src\RadioMCCOrbSingleton.h"

using namespace MotorolaRemoteAPI;
using std::string;

wstring string_to_wstring(string str)
{
	std::wstring str2(str.length(), L' ');
	std::copy(str.begin(), str.end(), str2.begin());
	return str2;
}

RemoteApiConnection::RemoteApiConnection( MCC7500SDK::IAPIRequest_ptr &request,const char *IOR,
										 const char *username,const char*pass,RadioEventDispatcher &dispatcher,
										 TA_Base_Core::ReEntrantThreadLockable &apiLock)

					:ior(IOR),
					 cram_username(username),
					 cram_password(pass),
					 m_bTerminate(false),
					 m_dispatch(dispatcher),
					 m_apiRequest(request), 
					 m_apiLock(apiLock),
					 m_connecting(false)
					 
{

	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"RadioMFTAgent ORB Details UserName : %s Password : %s StringIOR : %s",
				 cram_username.c_str(),
				 cram_password.c_str(),
				 ior.c_str());

	m_remoteConnectionHelper =  new TA_IRS_App::RemoteApiConnectionHelper(this);
}
void RemoteApiConnection::terminate()
{
	m_bTerminate=true;
}

BYTE RemoteApiConnection::registerClient(MCC7500SDK::IAPIRequestServer_var& server,MCC7500SDK::IAPIResponse_var& m_APIResponseVar)
{	
	BYTE id = MCDI_NULL_CLIENT_ID;
	try 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[ %u ]",(int)(id=server->RegisterApp(m_APIResponseVar.in())));
		TA_IRS_App::TheConsoleStateManager::instance()->setClientID(id);
	}
	catch(const CORBA::Exception &ex)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba exception caught re-register in 25 secs");
		The_StatusManager::instance()->failConnectToRadio();
	}
	return id;
}

void RemoteApiConnection::authenticate(MCC7500SDK::IAPIRequestServer_var& server, BYTE clientid)
{
	try 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, server->AuthenticateUser(string_to_wstring(cram_username).c_str(),string_to_wstring(cram_password).c_str(),clientid,L"RemoteAPIConnection")?"Authentication sucess":"Authentication fail");
		MCC7500SDK::Version v;
		v.Major=1;
		v.Minor=0;
		MCC7500SDK::IBaseAPIRequest_ptr pBaseAPIRequest = server->GetAPIRequest(clientid, v);
		{
			TA_Base_Core::ThreadGuard guard(m_apiLock);
			m_apiRequest = MCC7500SDK::IAPIRequest::_narrow(pBaseAPIRequest);
		}
		
		Sleep(15000);//To wait for motorola to opendb
		m_dispatch.onRAPIReady();
	}
	catch(const CORBA::Exception &ex)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba exception caught: Retry in 25 secs");
		The_StatusManager::instance()->failConnectToRadio();
	}
}

void RemoteApiConnection::initialize()
{	
	// do process results here
	m_connecting = true;
	bool bResult = m_remoteConnectionHelper->establishConnectionToMcc(m_apiRequest, m_apiLock);

	//If RadioMFT cannot able to establish the connection successfully to MCC, then call reconnection
	if (!bResult)
	{
		LOG_GENERIC(SourceInfo, 
					TA_Base_Core::DebugUtil::DebugInfo, 
					"Failed to connect to Radio Hardware System, Calling reconnection to MCC..");
		m_connecting = false;
		m_dispatch.reconnect();
		The_StatusManager::instance()->setMccConnectionStatus(false);
		The_StatusManager::instance()->failConnectToRadio();
		

	}
	else
	{
		The_StatusManager::instance()->setMccConnectionStatus(true);
		LOG_GENERIC(SourceInfo, 
					TA_Base_Core::DebugUtil::DebugInfo, 
					"Connected Radio Hardware System");
		
	}


}

void RemoteApiConnection::run()
{
	bool bInit = false;

	while (!m_bTerminate)
	{
		TA_Base_Core::Thread::sleep(100);
		if (!bInit)
		{
			initialize();
			// if already true, it means 
			// that it is just waiting for termination
			// and properly dispose of all thread resource
			bInit = true;
		}
	}
	
}
void RemoteApiConnection::processEvent(TA_IRS_App::RemoteAPIRadioEvent* event)
{
	m_dispatch.processEvent(event);
}
bool RemoteApiConnection::isConnecting()
{
	return m_connecting;
}

/*MCC7500SDK::IAPIRequest_ptr & RemoteApiConnection::getIApiReferencePointer ()
{
	return m_apiRequest;
}*/
std::string RemoteApiConnection::getMccIor ()
{
	return ior;
}

std::string RemoteApiConnection::getMccUserName()
{
	return cram_username;
}

std::string RemoteApiConnection::getMccPassword()
{
	return cram_password;
}

RadioEventDispatcher & RemoteApiConnection::getRadioEventDispatcher ()
{
	return m_dispatch;
}

RemoteApiConnection::~RemoteApiConnection(void)
{
	
	LOG_GENERIC(SourceInfo, 
				TA_Base_Core::DebugUtil::DebugInfo, 
				"Deconstruct RemoteApiConnection");

	if (m_remoteConnectionHelper)
	{
		delete m_remoteConnectionHelper;
		m_remoteConnectionHelper = NULL;
	}

	{
		TA_Base_Core::ThreadGuard guard(m_apiLock);
		if(!CORBA::is_nil(m_apiRequest))
		{
			CORBA::release(m_apiRequest);
		}
		m_apiRequest = NULL;
	}
	
	terminateAndWait();

	LOG_GENERIC(SourceInfo, 
				TA_Base_Core::DebugUtil::DebugInfo, 
				"Done Deconstructing RemoteApiConnection");
}


void RemoteApiConnection::connect ()
{
	start ();
}
