
#pragma once

#include "RemoteApiConnectionHelper.h"
#include "app\radio\radio_mft_agent\src\RadioMCCOrbSingleton.h"
#include "app/radio/radio_mft_agent/src/RemoteApiResponse.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleStateManager.h"
#include "app\radio\radio_mft_agent\src\RemoteApiConnection.h"

#include "Mca/mca.h"
#include "Mcdi/callback.h"
#include "mcdi/registration.h"

using std::string;

wstring string_to_wstring1(string str)
{
	std::wstring str2(str.length(), L' ');
	std::copy(str.begin(), str.end(), str2.begin());
	return str2;
}

namespace TA_IRS_App
{	
	RemoteApiConnectionHelper::RemoteApiConnectionHelper(RemoteApiConnection * remoteApiConnection) : 
	m_remoteApiConnection (remoteApiConnection),
	m_server(NULL)
	{
		setOrbObjects ();
	}

	RemoteApiConnectionHelper::~RemoteApiConnectionHelper(void)
	{
		
		if (m_server)
		{
			try 
			{
				m_server->DeregisterApp(m_clientid);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"Registration deactivated");
			}
			catch(const CORBA::Exception &ex)
			{
				LOG_GENERIC(SourceInfo, 
					TA_Base_Core::DebugUtil::DebugError, 
					"Corba Exception Encountered : %s", ex._name());

			}
			
		}

		m_server = NULL;
	}

	void RemoteApiConnectionHelper::setOrbObjects ()
	{
		orb = TA_IRS_App::RadioMCCOrbSingleton::getInstance()->getOrb();
		rootpoa = TA_IRS_App::RadioMCCOrbSingleton::getInstance()->getRootPoa();
		pman = TA_IRS_App::RadioMCCOrbSingleton::getInstance()->getPoaMngr();
		m_child_poa = TA_IRS_App::RadioMCCOrbSingleton::getInstance()->getChildPoa();
		obj = TA_IRS_App::RadioMCCOrbSingleton::getInstance()->getObject();
	}

	bool RemoteApiConnectionHelper::establishConnectionToMcc (MCC7500SDK::IAPIRequest_ptr &request, TA_Base_Core::ReEntrantThreadLockable &apiLock)
	{
		
		bool bResult = false;

		try{

			m_server = MCC7500SDK::IAPIRequestServer::_narrow(obj.in());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Server activated.");

			//ACE_Thread_Manager::instance()->spawn_n(1,ACE_THR_FUNC(::run),&orb);
			MotorolaRemoteAPI::TAPIResponse* mycallback = new MotorolaRemoteAPI::TAPIResponse(*m_remoteApiConnection);
			m_child_poa->activate_object(mycallback);

			MCC7500SDK::IAPIResponse_var m_APIResponseVar=mycallback->_this();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Callback activated. ");
			mycallback->_remove_ref();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Now trying to register: Calling RegisterApp to MCC..");		

			m_clientid = MCDI_NULL_CLIENT_ID;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC returned, ClientID:[ %u ] ",(int)(m_clientid=m_server->RegisterApp(m_APIResponseVar.in())));
			TA_IRS_App::TheConsoleStateManager::instance()->setClientID(m_clientid);
	
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Motorola wants us to sleep for atleast 5 secs");
			Sleep(5000);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Authentication begins");

			bool authOk = m_server->AuthenticateUser(string_to_wstring1(m_remoteApiConnection->getMccUserName()).c_str(),
				string_to_wstring1(m_remoteApiConnection->getMccPassword()).c_str(),
				m_clientid,
				L"RemoteAPIConnection");

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, authOk ?"Authentication success":"Authentication fail");

			if (!authOk) 
			{
				throw 1;
			}

			MCC7500SDK::Version v;
			v.Major=1;
			v.Minor=0;
			MCC7500SDK::IBaseAPIRequest_ptr pBaseAPIRequest = m_server->GetAPIRequest(m_clientid, v);
			{
				TA_Base_Core::ThreadGuard guard(apiLock);
				request = MCC7500SDK::IAPIRequest::_narrow(pBaseAPIRequest);
			}
			//m_connecting = false;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC Initialization finish successfully");
			bResult = true;

			//Set into Normal state once the registration is completed
			The_StatusManager::instance()->setRadioMFTState(RadioMFTAgentStatusManager::NORMAL_STATE);

		}

		catch(const CORBA::Exception &ex)
		{
			LOG_GENERIC(SourceInfo, 
						TA_Base_Core::DebugUtil::DebugError, 
						"Corba Exception Encountered : %s", ex._name());

			{
				TA_Base_Core::ThreadGuard guard(apiLock);
				request = NULL;
			}

			//Set to unknown state whenever there is an exception occur.
			The_StatusManager::instance()->setRadioMFTState(RadioMFTAgentStatusManager::UNKNOWN_STATE);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, 
						TA_Base_Core::DebugUtil::DebugError, 
						"Unknown Exception Encountered");
			{
				TA_Base_Core::ThreadGuard guard(apiLock);
				request = NULL;
			}

			//Set to unknown state whenever there is an exception occur.
			The_StatusManager::instance()->setRadioMFTState(RadioMFTAgentStatusManager::UNKNOWN_STATE);
		}
		
		LOG_GENERIC(SourceInfo, 
					TA_Base_Core::DebugUtil::DebugInfo, 
					"establishConnectionToMcc : returning : %d",bResult);

		return bResult;
	}
}
