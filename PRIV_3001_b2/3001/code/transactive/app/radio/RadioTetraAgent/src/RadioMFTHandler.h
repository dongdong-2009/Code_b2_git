/**
*
* To access MCC to retrieve data and synchronize with ISCS database 
*/
#ifndef RADIO_MFT_HANDLER_H_INCLUDED
#define RADIO_MFT_HANDLER_H_INCLUDED

#include "ace/Task.h"
#include "app/radio/RadioTetraAgent/src/RemoteAPI.h"
#include "bus/radio/radio_bus/src/radiotypes.h"
#include "app/radio/RadioTetraAgent/src/RadioResourceSync.h"
#include "app/radio/RadioTetraAgent/src/RadioProfileHandler.h"

#include "core/data_access_interface/radio/src/RadioSubscriberFactory.h"
#include "core/data_access_interface/radio/src/IRadioSubscriber.h"
#include "core/data_access_interface/radio/src/ConfigRadioSubscriber.h"
#include "core/data_access_interface/radio/src/RadioDirectoryMemberFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/TransactiveException.h"

#include "core/utilities/src/IAlarmClockHelper.h"
#include "core/utilities/src/AlarmClock.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "ace/OS_NS_unistd.h"
#include <string>

using namespace std;
class RadioMFTResponse;

class RadioMFTHandler : public virtual ACE_Task_Base, public TA_Base_Core::IAlarmClockHelper {
	friend class RadioMFTResponse;
public:
	RadioMFTHandler(string ior, string username, string password, RadioResourceSync* sync, TA_IRS_App::RadioProfileHandler* profile);
	~RadioMFTHandler();

	void initialize();
	void start();
	void stop();
	virtual int svc();
	bool connect();
	//void clearConnection();

	virtual void executeAlarm(TA_Base_Core::AlarmClock* clock);

	bool initializeDatabase();
	void syncDatabase();
	void getMccDatabase();
	void getIscsDatabase();

	// For API Calls
	void acceptDatabaseUpdate();
	void rejectDatabaseUpdate();

	// Update ISCS Database
	bool updateIscsDatabase(const ::MCC7500SDK::Parameters &param);
	void getCurMccDatabase(RadioResourceType type);
	void getCurIscsDatabase(RadioResourceType type);
	void notifyMftAgents(RadioResourceType type);

private:
	wstring string_to_wstring(string str);
	string m_ior, m_username, m_password;
	TA_Base_Core::AlarmClock* m_timeoutThread;
	unsigned int m_privateCallResource;
	
	unsigned char m_clientId;
	CORBA::ORB_var m_orb;
	CORBA::Object_var m_orbObj;
	PortableServer::POA_var m_rootPoa;
	PortableServer::POAManager_var m_poaManager;
	MCC7500SDK::IAPIRequest_ptr m_IAPIRequest;
	MCC7500SDK::IAPIResponse_var m_response;
	MCC7500SDK::IAPIRequestServer_var m_server;
	MCC7500SDK::IBaseAPIRequest_ptr m_baseAPIRequest;
	RadioMFTResponse* m_callback;

	RadioResourceSync* m_syncHandler;

	TA_IRS_App::RadioProfileHandler* m_profileHandler;

	std::vector<RadioResource> m_iscsCollection;
	std::vector<RadioResource> m_mccCollection;
	
	// lock for accessing the collections
	TA_Base_Core::ReEntrantThreadLockable m_db;

	static unsigned int WAIT_TIME;
	static std::string PRIVATE_CALL_RESOURCE_IDENTIFIER1;
	static std::string PRIVATE_CALL_RESOURCE_IDENTIFIER2;
};

#endif
