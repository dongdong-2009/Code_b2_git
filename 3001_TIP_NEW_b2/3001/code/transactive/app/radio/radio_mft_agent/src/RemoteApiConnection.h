#pragma once

#include"app/radio/radio_mft_agent/src/RemoteAPIRadioEvent.h"
#include"app/radio/radio_mft_agent/src/RadioEventDispatcher.h"
#include "app\radio\radio_mft_agent\src\RemoteApiConnectionHelper.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "core/threads/src/Thread.h"
#include<string>

using std::string;
class RemoteApiConnection : public virtual TA_Base_Core::Thread 
{
	private:
		string ior,cram_username,cram_password;
		bool m_bTerminate;	
		bool m_connecting;
		RadioEventDispatcher &m_dispatch;
		MCC7500SDK::IAPIRequest_ptr &m_apiRequest;
		TA_Base_Core::ReEntrantThreadLockable &m_apiLock;

					
	public:
		RemoteApiConnection(MCC7500SDK::IAPIRequest_ptr &request,const char *IOR,const char *username,const char*pass,
							RadioEventDispatcher &dispatcher, TA_Base_Core::ReEntrantThreadLockable &apiLock);
		~RemoteApiConnection(void);
		virtual void run();
		virtual void terminate();
		void processEvent(TA_IRS_App::RemoteAPIRadioEvent* event);
		void authenticate(MCC7500SDK::IAPIRequestServer_var& server, BYTE clientid);
		BYTE registerClient(MCC7500SDK::IAPIRequestServer_var& server, MCC7500SDK::IAPIResponse_var& m_APIResponseVar);
		void initialize(void);
		bool isConnecting();

		
		//MCC7500SDK::IAPIRequest_ptr & getIApiReferencePointer ();
		std::string getMccIor ();
		std::string getMccUserName();
		std::string getMccPassword();
		RadioEventDispatcher & getRadioEventDispatcher ();

		void connect ();

	private:
		TA_IRS_App::RemoteApiConnectionHelper * m_remoteConnectionHelper;
};
