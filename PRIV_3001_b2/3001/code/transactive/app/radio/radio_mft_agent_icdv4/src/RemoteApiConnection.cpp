#include "app/radio/radio_mft_agent_icdv4/src/RemoteApiConnection.h"
#include"app/radio/radio_mft_agent_icdv4/src/RemoteAPI.h"
#include "ace/Get_Opt.h"

#include"app/radio/radio_mft_agent_icdv4/src/RemoteApiResponse.h"
#include"ace/Thread_Manager.h"
#include"app/radio/radio_mft_agent_icdv4/src/RemoteApiRadioCommand.h"
#include<string>
#include"app/radio/radio_mft_agent_icdv4/src/RadioEventDispatcher.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleStateManager.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleStateManager.h"
#include "Mca/mca.h"
#include "Mcdi/callback.h"
#include "mcdi/registration.h"
#include "mcdi/authentication.h"
using namespace MotorolaRemoteAPI;
using std::string;
wstring string_to_wstring(string str)
{
	std::wstring str2(str.length(), L' ');
	std::copy(str.begin(), str.end(), str2.begin());
	return str2;
}
void run(void *a)
{
	CORBA::ORB_var *p=(CORBA::ORB_var*)a;
	(*p)->run();
}
RemoteApiConnection::RemoteApiConnection( MCC7500SDK::IAPIRequest_ptr &request,const char *IOR,const char *username,const char*pass,RadioEventDispatcher &dispatcher):ior(IOR),cram_username(username),cram_password(pass),m_terminate(false),dispatch(dispatcher),apiRequest(request)
{
	start();
}
void RemoteApiConnection::terminate()
{
	m_terminate=true;
}

BYTE RemoteApiConnection::registerClient(MCC7500SDK::IAPIRequestServer_var& server,MCC7500SDK::IAPIResponse_var& m_APIResponseVar)
{	
	BYTE id = MCDI_NULL_CLIENT_ID;
	try {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[ %u ]",(int)(id=server->RegisterApp(m_APIResponseVar.in())));
		TA_IRS_App::TheConsoleStateManager::instance()->setClientID(id);
	}
	catch(const CORBA::Exception &ex)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba exception caught re-register in 20 secs");
		Sleep(20000);
		registerClient(server,m_APIResponseVar);
	}
	return id;
}

void RemoteApiConnection::authenticate(MCC7500SDK::IAPIRequestServer_var& server, BYTE clientid)
{
	try {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, server->AuthenticateUser(string_to_wstring(cram_username).c_str(),string_to_wstring(cram_password).c_str(),clientid,L"RemoteAPIConnection")?"Authentication sucess":"Authentication fail");
		MCC7500SDK::Version v;
		v.Major=1;
		v.Minor=0;
		MCC7500SDK::IBaseAPIRequest_ptr pBaseAPIRequest = server->GetAPIRequest(clientid, v);
		apiRequest = MCC7500SDK::IAPIRequest::_narrow(pBaseAPIRequest);
		Sleep(15000);//To wait for motorola to opendb
		dispatch.onRAPIReady();
	}
	catch(const CORBA::Exception &ex)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba exception caught: Retry in 20 secs");
		Sleep(20000);
		initialize();
		//return false;
	}
	//return true;
}

void RemoteApiConnection::initialize()
{
	CORBA::ORB_var orb;
	PortableServer::POA_var rootpoa;
	PortableServer::POAManager_var pman;
	try{

		int   nArgC    = 2;
		char *pszArgV[2] = {0};
		pszArgV[0]  = "RAPIClient";
		char ior[1024];
		strcpy(ior,this->ior.c_str());
		pszArgV[1]  = ior;
		const char* options[][2] = { { "offerBiDirectionalGIOP", "1" },{"traceLevel","40"},{"traceFile","ORB_LOG.txt"},{"acceptBiDirectionalGIOP","1"},{"endPoint","giop:tcp:mft:9020"}, { 0, 0 } };



		//Initialize ORB
		orb = CORBA::ORB_init(nArgC, pszArgV,"omniORB4",options);

		CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
		rootpoa = PortableServer::POA::_narrow(obj);
		pman = rootpoa->the_POAManager();
		pman->activate();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "POA Manager activated");
		// Create a POA with the Bidirectional policy
		CORBA::PolicyList pl;
		pl.length(1);
		::CORBA::Any a;
		a<<= BiDirPolicy::BOTH;
		pl[0] = orb->create_policy(BiDirPolicy::BIDIRECTIONAL_POLICY_TYPE, a);
		PortableServer::POA_var poa;
		try {
			poa = rootpoa->find_POA("childPOA", false);
		}			
		catch (const CORBA::Exception &ex) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "childPOA not found");
			poa = rootpoa->create_POA("childPOA", pman, pl);
		}
		//PortableServer::POA_var poa = rootpoa->create_POA("childPOA", pman,pl);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Portable server created.");

		// Get the reference the server.
		obj = orb->string_to_object(ior);
		// Register a CallBack object in this process.

		MCC7500SDK::IAPIRequestServer_var server = MCC7500SDK::IAPIRequestServer::_narrow(obj);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Server activated.");
		ACE_Thread_Manager::instance()->spawn_n(1,ACE_THR_FUNC(::run),&orb);
		TAPIResponse* mycallback = new TAPIResponse(*this);
		poa->activate_object(mycallback);

		MCC7500SDK::IAPIResponse_var m_APIResponseVar=mycallback->_this();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Callback activated. ");
		mycallback->_remove_ref();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Now trying to register: ");
		BYTE clientid = MCDI_NULL_CLIENT_ID;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[ %u ]",(int)(clientid=server->RegisterApp(m_APIResponseVar.in())));
		TA_IRS_App::TheConsoleStateManager::instance()->setClientID(clientid);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Motorola wants us to sleep for atleast 5 secs");
		Sleep(5000);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Authentication begins");
		//while (!authenticate(server,clientid)) {		
		//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Authentication fails, try to register again");
		//	server->DeregisterApp(clientid);
		//	registerClient(server,m_APIResponseVar);
		//}
		//authenticate(server,clientid);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, server->AuthenticateUser(string_to_wstring(cram_username).c_str(),string_to_wstring(cram_password).c_str(),clientid,L"RemoteAPIConnection")?"Authentication sucess":"Authentication fail");
		MCC7500SDK::Version v;
		v.Major=1;
		v.Minor=0;
		MCC7500SDK::IBaseAPIRequest_ptr pBaseAPIRequest = server->GetAPIRequest(clientid, v);
		apiRequest = MCC7500SDK::IAPIRequest::_narrow(pBaseAPIRequest);
		Sleep(15000);//To wait for motorola to opendb
		dispatch.onRAPIReady();

		while(!m_terminate) 
		{
			Sleep(20000);
			server->ServerVersion();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Call to server ok");			
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, (server->DeregisterApp(clientid)?"De-Registration Ok":"De-Grgistration failed"));

	}

	catch(const CORBA::Exception &ex)
	{
		The_StatusManager::instance()->failConnectToRadio();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba exception caught re-initializing the connection in 20 secs");
		orb = NULL;
		rootpoa = NULL;
		pman = NULL;
		Sleep(20000);
		initialize();
	}
}

void RemoteApiConnection::run()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Username: {%s}  Password: {%s}  IOR : {%s}",cram_username.c_str(),cram_password.c_str(),ior.c_str());
	while(!m_terminate)
	{
		initialize();
	}
}
void RemoteApiConnection::processEvent(ICDV4::RemoteAPIRadioEvent *event)
{
	dispatch.processEvent(event);
}
RemoteApiConnection::~RemoteApiConnection(void)
{
}
