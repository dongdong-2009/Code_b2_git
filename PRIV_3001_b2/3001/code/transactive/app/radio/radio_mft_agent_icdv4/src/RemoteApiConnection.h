#pragma once
#include"app/radio/radio_mft_agent_icdv4/src/RemoteAPIRadioEvent.h"
#include "core/threads/src/Thread.h"
#include<string>
#include"app/radio/radio_mft_agent_icdv4/src/RadioEventDispatcher.h"
using std::string;
class RemoteApiConnection : public virtual TA_Base_Core::Thread
{
private:
string ior,cram_username,cram_password;
bool m_terminate;	
RadioEventDispatcher &dispatch;
MCC7500SDK::IAPIRequest_ptr &apiRequest;
public:
	RemoteApiConnection(MCC7500SDK::IAPIRequest_ptr &request,const char *IOR,const char *username,const char*pass,RadioEventDispatcher &dispatcher);
	~RemoteApiConnection(void);
	virtual void run();
	virtual void terminate();
	void processEvent(ICDV4::RemoteAPIRadioEvent* event);
	void authenticate(MCC7500SDK::IAPIRequestServer_var& server, BYTE clientid);
	BYTE registerClient(MCC7500SDK::IAPIRequestServer_var& server, MCC7500SDK::IAPIResponse_var& m_APIResponseVar);
	void initialize(void);
};
