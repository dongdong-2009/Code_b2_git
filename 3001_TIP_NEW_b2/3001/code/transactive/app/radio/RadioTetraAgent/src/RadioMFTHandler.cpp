#include "app/radio/RadioTetraAgent/src/RadioMFTHandler.h"
#include "app/radio/RadioTetraAgent/src/RadioMFTResponse.h"
#include "app/radio/RadioTetraAgent/src/MccProtocolHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include <vector>
#include <string>

using namespace TA_IRS_App;

unsigned int RadioMFTHandler::WAIT_TIME = 25;
std::string RadioMFTHandler::PRIVATE_CALL_RESOURCE_IDENTIFIER1 = "Private Call";
std::string RadioMFTHandler::PRIVATE_CALL_RESOURCE_IDENTIFIER2 = "PVC";

RadioMFTHandler::RadioMFTHandler(std::string ior, std::string username, std::string password, RadioResourceSync* sync, TA_IRS_App::RadioProfileHandler* profile) : 
m_ior(ior),
m_username(username),
m_password(password),
m_callback(NULL),
m_clientId(0),
m_syncHandler(sync),
m_profileHandler(profile)
{
}

RadioMFTHandler::~RadioMFTHandler()
{
}

void RadioMFTHandler::initialize()
{
}

void RadioMFTHandler::start()
{
	activate();
}

void RadioMFTHandler::stop()
{
}

int RadioMFTHandler::svc()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Connecting to MCC");	
	while(!connect()) { 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Reconnect to MCC in 25 secs");
		ACE_OS::sleep(WAIT_TIME);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Reconnect to MCC now");
	}
	return 0;
}

bool RadioMFTHandler::connect() 
{
	FUNCTION_ENTRY("connect");
	try {
		int nArgC = 1;
		char *pszArgV[1] = {0};
		char ior[1024];
		strcpy(ior, m_ior.c_str());
		pszArgV[0] = ior;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: IOR:%s, username:%s, password:%s", ior, m_username.c_str(), m_password.c_str());

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Initialize ORB");
		m_orb = CORBA::ORB_init(nArgC, pszArgV, "omniORB4");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Resolve Root POA");
		m_orbObj = m_orb->resolve_initial_references("RootPOA");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Initialize Root POA");
		m_rootPoa = PortableServer::POA::_narrow(m_orbObj.in());

		if (NULL != m_rootPoa) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Root POA created successfully.");
		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Root POA not created successfully.");
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Initialize and activate POA Manager");
		m_poaManager = m_rootPoa->the_POAManager();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Activating POA manager now");
		m_poaManager->activate();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Creating IOR orb object");
		
		m_orbObj = m_orb->string_to_object(ior);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Initialize MCC Server object reference");
		m_server = MCC7500SDK::IAPIRequestServer::_narrow(m_orbObj.in());
		if (NULL != m_server) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Server Object created successfully.");
		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Server Object not created successfully.");
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Creating timeout thread");
		m_timeoutThread = new TA_Base_Core::AlarmClock(this);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Creating callback class");
		m_callback = new RadioMFTResponse(*this,*m_timeoutThread);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Activate callback class");
		m_rootPoa->activate_object(m_callback);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Get the pointer to callback");
		m_response = m_callback->_this();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Removing the reference of pointer");
		m_callback->_remove_ref();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: MFT Response object created");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Registering to MCC");
		m_clientId = m_server->RegisterApp(m_response);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Registration successful, ClientId = %u", m_clientId);

		ACE_OS::sleep(5);

		int result = m_server->AuthenticateUser(string_to_wstring(m_username).c_str(),string_to_wstring(m_password).c_str(),m_clientId,L"RemoteAPIConnection");
		if (result) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC Authentication success");
		} else {				
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC Authentication fail");
			return 0;
		}

		MCC7500SDK::Version v;
		v.Major = 1;
		v.Minor = 0;
		m_baseAPIRequest = m_server->GetAPIRequest(m_clientId, v);
		m_IAPIRequest = MCC7500SDK::IAPIRequest::_narrow(m_baseAPIRequest);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Successful getting the API Request object");
		m_callback->setOperationMode(true);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Now wait MCC ready (Audio Processing, Peripheral Status, Database Status)");
		
		// only applicable to internal testing
		//Sleep(15000);
		//initializeDatabase();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Finished obtaining data from database, start profile thread now");
		m_profileHandler->start();
	}
	catch(MCC7500SDK::IAPIRequestServer::AnotherUserAlreadyRegistered& e) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MCC: AnotherUserAlreadyRegistered Exception");
		if (0 != m_clientId && !CORBA::is_nil(m_server)) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Deregistering previous user");
			m_server->DeregisterApp(m_clientId);
		}
		return 0;
	}
	catch(const ::CORBA::ORB::InvalidName& ex) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MCC: CORBA::ORB::InvalidName exception caught, re-initializing the connection: %s", ex._name());
		return 0;
	}
	catch(const ::CORBA::INITIALIZE& ex) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MCC: CORBA::Initialize exception caught, re-initializing the connection: %s", ex._name());
		return 0;
	}
	catch(const ::CORBA::Exception& ex){
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MCC: Corba exception caught re-initializing the connection: %s", ex._name());
		return 0;
	}
	FUNCTION_EXIT;
	return 1;
}

wstring RadioMFTHandler::string_to_wstring(string str)
{
	std::wstring str2(str.length(), L' ');
	std::copy(str.begin(), str.end(), str2.begin());
	return str2;
}


void RadioMFTHandler::executeAlarm(TA_Base_Core::AlarmClock* clock)
{
	FUNCTION_ENTRY("executeAlarm");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "MCC: Keep-ALIVE message not received in 60 secs.");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Reconnect to MCC in 25 secs");
	ACE_OS::sleep(WAIT_TIME);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Reconnect to MCC now");
	while (!connect()) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Reconnect to MCC in 25 secs");
		ACE_OS::sleep(WAIT_TIME);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Reconnect to MCC now");
	}
	FUNCTION_EXIT;
}

// Todo: deinitialize ORB objects related to MCC without affecting ISCS..
//void RadioMFTHandler::clearConnection()
//{
//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Clearing existing connection");

	//m_timeoutThread->cancel();
	/*if(NULL != m_callback)
	{
		delete m_callback;
	}
	m_callback = NULL;*/
	
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Shutdown orb");
	//if (!CORBA::is_nil(m_orb)) m_orb->shutdown(1);
	
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Delete root POA");
	//if (!CORBA::is_nil(m_rootPoa)){ 
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Delete root POA");
	//	m_rootPoa->destroy(0,1);
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Delete root POA");
	//}
	//m_rootPoa = NULL;

	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Delete orb");
	//if (!CORBA::is_nil(m_orb)) m_orb->destroy();
	//m_orb = NULL;
	//
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Delete orb object");
	//if (!CORBA::is_nil(m_orbObj)) CORBA::release(m_orbObj);
	//m_orbObj = NULL;

	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Delete POA Manager");
	//if (!CORBA::is_nil(m_poaManager)) m_poaManager->deactivate(1,1);
	//m_poaManager = NULL;
	//
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Delete MCC server object reference");
	//if (!CORBA::is_nil(m_server)) CORBA::release(m_server);
	//m_server = NULL;
	//
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Delete MCC response object reference");
	//if (!CORBA::is_nil(m_response)) CORBA::release(m_response);
	//m_response = NULL;
	
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Delete MCC baseAPI request object reference");
	//if (!CORBA::is_nil(m_baseAPIRequest)) CORBA::release(m_baseAPIRequest);
	//m_baseAPIRequest = NULL;

	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Delete MCC API request object reference");
	//if (!CORBA::is_nil(m_IAPIRequest)) CORBA::release(m_IAPIRequest);
	//m_IAPIRequest = NULL;
//
//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Finish reset connection");
//}


bool RadioMFTHandler::initializeDatabase()
{
	FUNCTION_ENTRY("initializeDatabase");
	try {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Now getting resources from MCC database");
		getMccDatabase();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Now getting resources from ISCS database");
		getIscsDatabase();

		syncDatabase();
	}
	catch (TA_Base_Core::DataException& ex) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MCC: DataException, %s", ex.what());
		return false;
	}
	catch (RadioAgentMcc::MccDbException& ex) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MCC: MCC database access error, %s", ex.what());
		return false;
	}
	catch (TA_Base_Core::TransactiveException& ex) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MCC: Transactive exception %s", ex.what());
		return false;
	}
	FUNCTION_EXIT;
	return true;
}

void RadioMFTHandler::syncDatabase()
{
	TA_Base_Core::ThreadGuard guard(m_db);
	FUNCTION_ENTRY("syncDatabase");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Now synchronizing ISCS and MCC database");
	unsigned int iscsNumRec = m_iscsCollection.size();
	//unsigned int mccNumRec = m_mccCollection.size();
	unsigned int index = 0;
	std::vector<unsigned int> deletedIndex;
	deletedIndex.clear();
	RadioResource r;
	bool found;
	std::vector<RadioResource>::iterator mccIter;
	std::vector<TA_IRS_Core::IRadioSubscriber*> datas;

	RadioResource emptyRes;
	emptyRes.initialise();
	try {
		for (; index < iscsNumRec; index++) {
			r = m_iscsCollection[index];
			mccIter = m_mccCollection.begin();
			found = false;
			for (;mccIter!=m_mccCollection.end();mccIter++) {
				if (r.id == (*mccIter).id) {
					if (r.alias == (*mccIter).alias) {
						// no change
					} else {
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Updating resource with id = %u on DB, from alias = %s to alias = %s", r.id, r.alias.c_str(), (*mccIter).alias.c_str());
						datas = TA_IRS_Core::RadioSubscriberFactory::getInstance().getRadioSubscribersByResourceAlias(r.alias,false);
						if (datas.size() > 1) {
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "MCC: Not updating this resource. DB has two entries with the same alias");
							continue;
						}

						// broadcast first
						// In case need to broadcast change to agents
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Broadcasting update notification to agents");
						m_syncHandler->onResourceChange(r,(*mccIter));
						// update the database
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Committing the change to the database");
						TA_IRS_Core::RadioSubscriberFactory::getInstance().updateRadioSubscriber((*datas.begin())->getKey(), (*mccIter));
						datas.clear();
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Finished updating resource. Continue to the next resource");
					}
					// mark it, as we dont want to use it anymore
					(*mccIter).initialise();
					found = true;
					break;
				} 
				if (r.alias == (*mccIter).alias) {
					if (r.id == (*mccIter).id) {
						// no change
					} else {
						// TODO: update the database
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Updating resource with alias = %s on DB, from id = %u to id = %u", r.alias.c_str(), r.id, (*mccIter).id);						
						datas = TA_IRS_Core::RadioSubscriberFactory::getInstance().getRadioSubscribersByResourceAlias(r.alias,false);
						if (datas.size() > 1) {
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "MCC: Not updating this resource. DB has two entries with the same alias");
							continue;
						}

						// broadcast first
						// In case need to broadcast change to agents
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Broadcasting update notification to agents");
						m_syncHandler->onResourceChange(r,(*mccIter));
						// update the database
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Committing the change to the database");
						TA_IRS_Core::RadioSubscriberFactory::getInstance().updateRadioSubscriber((*datas.begin())->getKey(), (*mccIter));
						TA_IRS_Core::RadioDirectoryMemberFactory::getInstance().updateMemberIds((*mccIter).type, r.id, (*mccIter).id);						
						datas.clear();
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Finished updating resource. Continue to the next resource");
					}
					// mark it, as we dont want to use it anymore
					(*mccIter).initialise();
					found = true;
					break;
				}
			} 
			if (!found) {
				// delete from the database
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Deleting resource with alias = %s from the DB", r.alias.c_str());
				// if want to broadcast
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Broadcasting update notification to agents");
				m_syncHandler->onResourceChange(r,emptyRes);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Committing the change to the database");
				TA_IRS_Core::RadioSubscriberFactory::getInstance().deleteRadioSubscriber(r);
				TA_IRS_Core::RadioDirectoryMemberFactory::getInstance().deleteMembers(r.type, r.id);
			}
		}

//TA_IRS_Core::RadioSubscriberFactory::getInstance().test1();
		
		// remaining mccCollection is the new resources, add to the database
		mccIter = m_mccCollection.begin();
		for (; mccIter != m_mccCollection.end(); mccIter++) {
			// add to the database
			if (RR_NIL != (*mccIter).type) {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Adding resource with alias = %s to the DB", (*mccIter).alias.c_str());
				// if want to broadcast
				//if (RR_TALKGROUP == (*mccIter).type) {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Broadcasting update notification to agents");
				m_syncHandler->onResourceChange(emptyRes,(*mccIter));
				//}
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Committing the change to the database");
				TA_IRS_Core::RadioSubscriberFactory::getInstance().createNew(*mccIter);
			}
		}

		// testing purpose
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: updating key 1");
		//RadioResource re;
		//re.alias = "Radio Test";
		//re.id = 1;
		//re.ssi = 1;
		//re.type = RR_RADIO;
		//TA_IRS_Core::RadioSubscriberFactory::getInstance().updateRadioSubscriber(1, re);
	}
	catch(TA_Base_Core::DataException & e) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DataException happen while updating database. Stop updating.");
		RadioAgentMcc::MccDbException ex(e.what());
		TA_THROW(ex);
	}
	catch(TA_Base_Core::TransactiveException& ex) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TransactiveException: %s", ex.what());
		RadioAgentMcc::MccDbException e(ex.what());
		TA_THROW(e);
	}

	FUNCTION_EXIT;
}

void RadioMFTHandler::getMccDatabase()
{
	TA_Base_Core::ThreadGuard guard(m_db);
	FUNCTION_ENTRY("getMccDatabase");
	m_mccCollection.clear();

	unsigned int hDb = 0;
	unsigned int tgHq = 0;
	unsigned int tgNumRec = 0;
	m_privateCallResource = 0;

	RadioAgentMcc::OpenDatabase openDatabaseCommand(m_IAPIRequest);
	openDatabaseCommand.execute();
	openDatabaseCommand.printStatus();
	if (openDatabaseCommand.getStatus()) {
		RadioAgentMcc::MccDbException ex("openDatabaseCommand error");
		TA_THROW(ex);
	}
	hDb = openDatabaseCommand.getHandle();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Getting talkgroup resources from MCC");
	RadioAgentMcc::CreateTalkChannelQuery createTalkChannelQueryCommand(m_IAPIRequest, hDb);
	createTalkChannelQueryCommand.execute();
	createTalkChannelQueryCommand.printStatus();
	if (createTalkChannelQueryCommand.getStatus()) {
		RadioAgentMcc::MccDbException ex("createTalkChannelQueryCommand error");
		TA_THROW(ex);
	}
	tgNumRec = createTalkChannelQueryCommand.getNumRecs();
	tgHq = createTalkChannelQueryCommand.getQueryHdb();

	for (; tgNumRec > 0; tgNumRec--) {
		RadioResource resource;
		unsigned int tgUrid = 0;
		unsigned int rcHq = 0;
		std::string tgLabel = "";
		std::string rcLabel = "";
		unsigned int rcNumRec = 0;

		RadioAgentMcc::GetNextTalkChannel getNextTalkChannelCommand(m_IAPIRequest, tgHq);
		getNextTalkChannelCommand.execute();
		getNextTalkChannelCommand.printStatus();
		if (getNextTalkChannelCommand.getStatus()) {
			RadioAgentMcc::MccDbException ex("getNextTalkChannelCommand error");
			TA_THROW(ex);
		}
		tgUrid = getNextTalkChannelCommand.getUrid();
		tgLabel = getNextTalkChannelCommand.getLabel();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: getNextTalkChannel, Urid = %u, alias = %s", tgUrid, tgLabel.c_str());
		resource.id = tgUrid;
		resource.alias = tgLabel;
		resource.ssi = 0;
		if (tgLabel.find(PRIVATE_CALL_RESOURCE_IDENTIFIER1) != std::string::npos || tgLabel.find(PRIVATE_CALL_RESOURCE_IDENTIFIER2) != std::string::npos)
		{
			resource.type = RR_PRIVATECALL;
			m_privateCallResource = resource.id;
		} else 
			resource.type = RR_TALKGROUP;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Adding Talkgroup Resource: id = %u, alias = %s", resource.id, resource.alias.c_str());
		m_mccCollection.push_back(resource);

		RadioAgentMcc::CreateResCapabilityIndexQuery createResCapabilityIndexQueryCommand(m_IAPIRequest, hDb, tgUrid);
		createResCapabilityIndexQueryCommand.execute();
		createResCapabilityIndexQueryCommand.printStatus();
		if (createResCapabilityIndexQueryCommand.getStatus()) {
			RadioAgentMcc::MccDbException ex("createResCapabilityIndexQueryCommand error");
			TA_THROW(ex);
		}
		rcHq = createResCapabilityIndexQueryCommand.getQueryHdb();
		rcNumRec = createResCapabilityIndexQueryCommand.getNumRecs();

		if (rcNumRec > 0) {
			RadioAgentMcc::GetNextResCapabilityIndexRec getNextResCapabilityIndexRecCommand(m_IAPIRequest, rcHq);
			getNextResCapabilityIndexRecCommand.execute();
			getNextResCapabilityIndexRecCommand.printStatus();
			if (getNextResCapabilityIndexRecCommand.getStatus()) {
				RadioAgentMcc::MccDbException ex("getNextResCapabilityIndexRecCommand error");
				TA_THROW(ex);
			}
			rcLabel = getNextResCapabilityIndexRecCommand.getLabel();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: RC alias = %s", rcLabel.c_str());
		}

		RadioAgentMcc::DestroyQuery rcDestroyQueryCommand(m_IAPIRequest, rcHq);
		rcDestroyQueryCommand.execute();
		rcDestroyQueryCommand.printStatus();
		if (rcDestroyQueryCommand.getStatus()) {
			RadioAgentMcc::MccDbException ex("rcDestroyQueryCommand error");
			TA_THROW(ex);
		}
	}

	RadioAgentMcc::DestroyQuery tgDestroyQueryCommand(m_IAPIRequest, tgHq);
	tgDestroyQueryCommand.execute();
	tgDestroyQueryCommand.printStatus();
	if (tgDestroyQueryCommand.getStatus()) {
		RadioAgentMcc::MccDbException ex("tgDestroyQueryCommand error");
		TA_THROW(ex);
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Getting radio resources from MCC");
	if (0 == m_privateCallResource) {
		RadioAgentMcc::MccDbException ex("Private call resource not found");
		TA_THROW(ex);
	} 
	unsigned int radioNumRec = 0;
	unsigned int radioHq = 0;
	RadioAgentMcc::CreateUnitAliasListQuery createUnitAliasListQueryCommand(m_IAPIRequest, hDb, m_privateCallResource);
	createUnitAliasListQueryCommand.execute();
	createUnitAliasListQueryCommand.printStatus();
	if (createUnitAliasListQueryCommand.getStatus()) {
		RadioAgentMcc::MccDbException ex("createUnitAliasListQueryCommand error");
		TA_THROW(ex);
	}
	radioNumRec = createUnitAliasListQueryCommand.getNumRecs();
	radioHq = createUnitAliasListQueryCommand.getQueryHdb();
	for (; radioNumRec > 0; radioNumRec--)
	{
		RadioAgentMcc::GetNextUnitAlias getNextUnitAliasCommand(m_IAPIRequest, radioHq);
		getNextUnitAliasCommand.execute();
		getNextUnitAliasCommand.printStatus();
		if (getNextUnitAliasCommand.getStatus()) {
			RadioAgentMcc::MccDbException ex("getNextUnitAliasCommand error");
			TA_THROW(ex);
		}
		RadioResource resource;
		resource.ssi = getNextUnitAliasCommand.getUnitId();
		resource.id = resource.ssi;
		resource.alias = getNextUnitAliasCommand.getUnitAlias();
		resource.type = RR_RADIO;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Adding Radio Resource: ssi = %u, alias = %s", resource.ssi, resource.alias.c_str());
		m_mccCollection.push_back(resource);
	}
	RadioAgentMcc::DestroyQuery radioDestroyQueryCommand(m_IAPIRequest, radioHq);
	radioDestroyQueryCommand.execute();
	radioDestroyQueryCommand.printStatus();
	if (radioDestroyQueryCommand.getStatus()) {
		RadioAgentMcc::MccDbException ex("radioDestroyQueryCommand error");
		TA_THROW(ex);
	}

	RadioAgentMcc::CloseDatabase closeDatabaseCommand(m_IAPIRequest, hDb);
	closeDatabaseCommand.execute();
	closeDatabaseCommand.printStatus();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Restarting timer");
	m_timeoutThread->start(RadioMFTResponse::KEEPALIVE_TIMEOUT);
	FUNCTION_EXIT;
}

void RadioMFTHandler::getIscsDatabase()
{	
	TA_Base_Core::ThreadGuard guard(m_db);
	FUNCTION_ENTRY("getIscsDatabase");
	m_iscsCollection.clear();
	std::vector<TA_IRS_Core::IRadioSubscriber*> raSubscribers;
	try {
		 raSubscribers = TA_IRS_Core::RadioSubscriberFactory::getInstance().getAllRadioSubscribers(false);
	}
	catch (TA_Base_Core::TransactiveException& ex) {
		raSubscribers.clear();
		RadioAgentMcc::MccDbException e(ex.what());
		TA_THROW(e);
	}
	std::vector<TA_IRS_Core::IRadioSubscriber*>::iterator iter = raSubscribers.begin();

	for (; iter != raSubscribers.end(); iter++) {
		RadioResource resource;
		resource.type = (RadioResourceType)(*iter)->getResourceType();
		if (RR_RADIO == resource.type) {
			resource.ssi = (*iter)->getSSI();
			resource.id = resource.ssi;
			resource.alias = (*iter)->getAlias();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Adding radio resource, ssi = %u, alias = %s", resource.ssi, resource.alias.c_str());
		} else {
			resource.id = (*iter)->getResourceId();
			resource.alias = (*iter)->getAlias();
			resource.ssi = 0;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Adding talkgroup resource, id = %u, alias = %s", resource.id, resource.alias.c_str());
		}
		m_iscsCollection.push_back(resource);
		delete (*iter);
	}
	raSubscribers.clear();
	FUNCTION_EXIT;
}

// API Functions
void RadioMFTHandler::acceptDatabaseUpdate()
{
	FUNCTION_ENTRY("acceptDatabaseUpdate");
	RadioAgentMcc::DatabaseUpdateGrant databaseUpdateGrantCommand(m_IAPIRequest);
	databaseUpdateGrantCommand.execute();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DatabaseUpdateGrant Execute Status: %u", databaseUpdateGrantCommand.getStatus());
	FUNCTION_EXIT;
}

void RadioMFTHandler::rejectDatabaseUpdate()
{
	FUNCTION_ENTRY("rejectDatabaseUpdate");
	RadioAgentMcc::DatabaseUpdateNoGrant databaseUpdateNoGrantCommand(m_IAPIRequest);
	databaseUpdateNoGrantCommand.execute();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DatabaseUpdateNoGrant Execute Status: %u", databaseUpdateNoGrantCommand.getStatus());
	FUNCTION_EXIT;
}

bool RadioMFTHandler::updateIscsDatabase(const ::MCC7500SDK::Parameters &param)
{
	FUNCTION_ENTRY("updateIscsDatabase");

	RadioAgentMcc::McdiDatabaseUpdateNotification* pMessage = new RadioAgentMcc::McdiDatabaseUpdateNotification(param);
	RadioResourceType resourceType = RR_NIL;
	
	m_mccCollection.clear();
	m_iscsCollection.clear();
	int datagroup = pMessage->get_wDataGroup();
	if (RadioAgentMcc::McdiDatabaseUpdateNotification::DB_UPDATE_TALKGROUP == datagroup)
	{
	
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Preparing to synchronize Talkgroup Resource (\"B\") and Private Call Resource (\"P\") on ISCS Database");
		resourceType = RR_TALKGROUP;
		try {
			getCurMccDatabase(resourceType);
			getCurIscsDatabase(resourceType);

			syncDatabase();
			notifyMftAgents(resourceType);
		}
		catch (RadioAgentMcc::MccDbException &ex) 
		{
			RadioAgentMcc::MccDbException e("MCC: Database error. Not continuing processing this update");
			TA_THROW(e);
		}
		catch(TA_Base_Core::DataException& ex)
		{
			RadioAgentMcc::MccDbException e("MCC: Database error. Not continuing processing this update");
			TA_THROW(e);
		}
		catch(TA_Base_Core::TransactiveException& ex)
		{
			RadioAgentMcc::MccDbException e("MCC: Database error. Not continuing processing this update");
			TA_THROW(e);
		}
	} else if (RadioAgentMcc::McdiDatabaseUpdateNotification::DB_UNIT_ALIASES == datagroup)
	{

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Preparing to synchronize Radio Resource (\"A\") on ISCS Database");
		resourceType = RR_RADIO;
		try {
			getCurMccDatabase(resourceType);
			getCurIscsDatabase(resourceType);

			syncDatabase();
			notifyMftAgents(resourceType);
		}
		catch (RadioAgentMcc::MccDbException &ex) 
		{
			RadioAgentMcc::MccDbException e("MCC: Database error. Not continuing processing this update");
			TA_THROW(e);
		}
		catch(TA_Base_Core::DataException& ex)
		{
			RadioAgentMcc::MccDbException e("MCC: Database error. Not continuing processing this update");
			TA_THROW(e);
		}
		catch(TA_Base_Core::TransactiveException& ex)
		{
			RadioAgentMcc::MccDbException e("MCC: Database error. Not continuing processing this update");
			TA_THROW(e);
		}
	} else 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Not processing this update type");
	}
	FUNCTION_EXIT;
	return true;
}

void RadioMFTHandler::getCurMccDatabase(RadioResourceType type)
{
	TA_Base_Core::ThreadGuard guard(m_db);
	FUNCTION_ENTRY("getCurMccDatabase");
	if (RR_TALKGROUP == type)
	{
		unsigned int hDb = 0;
		unsigned int tgHq = 0;
		unsigned int tgNumRec = 0;
		m_privateCallResource = 0;

		RadioAgentMcc::OpenDatabase openDatabaseCommand(m_IAPIRequest);
		openDatabaseCommand.execute();
		openDatabaseCommand.printStatus();
		if (openDatabaseCommand.getStatus()) {
			RadioAgentMcc::MccDbException ex("openDatabaseCommand error");
			TA_THROW(ex);
		}
		hDb = openDatabaseCommand.getHandle();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Getting talkgroup resources from MCC");
		RadioAgentMcc::CreateTalkChannelQuery createTalkChannelQueryCommand(m_IAPIRequest, hDb);
		createTalkChannelQueryCommand.execute();
		createTalkChannelQueryCommand.printStatus();
		if (createTalkChannelQueryCommand.getStatus()) {
			RadioAgentMcc::MccDbException ex("createTalkChannelQueryCommand error");
			TA_THROW(ex);
		}
		tgNumRec = createTalkChannelQueryCommand.getNumRecs();
		tgHq = createTalkChannelQueryCommand.getQueryHdb();

		for (; tgNumRec > 0; tgNumRec--) {
			RadioResource resource;
			unsigned int tgUrid = 0;
			unsigned int rcHq = 0;
			std::string tgLabel = "";
			std::string rcLabel = "";
			unsigned int rcNumRec = 0;

			RadioAgentMcc::GetNextTalkChannel getNextTalkChannelCommand(m_IAPIRequest, tgHq);
			getNextTalkChannelCommand.execute();
			getNextTalkChannelCommand.printStatus();
			if (getNextTalkChannelCommand.getStatus()) {
				RadioAgentMcc::MccDbException ex("getNextTalkChannelCommand error");
				TA_THROW(ex);
			}
			tgUrid = getNextTalkChannelCommand.getUrid();
			tgLabel = getNextTalkChannelCommand.getLabel();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: getNextTalkChannel, Urid = %u, alias = %s", tgUrid, tgLabel.c_str());
			resource.id = tgUrid;
			resource.alias = tgLabel;
			resource.ssi = 0;
			if (tgLabel.find(PRIVATE_CALL_RESOURCE_IDENTIFIER1) != std::string::npos || tgLabel.find(PRIVATE_CALL_RESOURCE_IDENTIFIER2) != std::string::npos)
			{
				resource.type = RR_PRIVATECALL;
				m_privateCallResource = resource.id;
			} else 
				resource.type = RR_TALKGROUP;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Adding Talkgroup Resource: id = %u, alias = %s", resource.id, resource.alias.c_str());
			m_mccCollection.push_back(resource);

			RadioAgentMcc::CreateResCapabilityIndexQuery createResCapabilityIndexQueryCommand(m_IAPIRequest, hDb, tgUrid);
			createResCapabilityIndexQueryCommand.execute();
			createResCapabilityIndexQueryCommand.printStatus();
			if (createResCapabilityIndexQueryCommand.getStatus()) {
				RadioAgentMcc::MccDbException ex("createResCapabilityIndexQueryCommand error");
				TA_THROW(ex);
			}
			rcHq = createResCapabilityIndexQueryCommand.getQueryHdb();
			rcNumRec = createResCapabilityIndexQueryCommand.getNumRecs();

			if (rcNumRec > 0) {
				RadioAgentMcc::GetNextResCapabilityIndexRec getNextResCapabilityIndexRecCommand(m_IAPIRequest, rcHq);
				getNextResCapabilityIndexRecCommand.execute();
				getNextResCapabilityIndexRecCommand.printStatus();
				if (getNextResCapabilityIndexRecCommand.getStatus()) {
					RadioAgentMcc::MccDbException ex("getNextResCapabilityIndexRecCommand error");
					TA_THROW(ex);
				}
				rcLabel = getNextResCapabilityIndexRecCommand.getLabel();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: RC alias = %s", rcLabel.c_str());
			}

			RadioAgentMcc::DestroyQuery rcDestroyQueryCommand(m_IAPIRequest, rcHq);
			rcDestroyQueryCommand.execute();
			rcDestroyQueryCommand.printStatus();
			if (rcDestroyQueryCommand.getStatus()) {
				RadioAgentMcc::MccDbException ex("rcDestroyQueryCommand error");
				TA_THROW(ex);
			}
		}

		RadioAgentMcc::DestroyQuery tgDestroyQueryCommand(m_IAPIRequest, tgHq);
		tgDestroyQueryCommand.execute();
		tgDestroyQueryCommand.printStatus();
		if (tgDestroyQueryCommand.getStatus()) {
			RadioAgentMcc::MccDbException ex("tgDestroyQueryCommand error");
			TA_THROW(ex);
		}
		RadioAgentMcc::CloseDatabase closeDatabaseCommand(m_IAPIRequest, hDb);
		closeDatabaseCommand.execute();
		closeDatabaseCommand.printStatus();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Restarting timer");
		m_timeoutThread->start(RadioMFTResponse::KEEPALIVE_TIMEOUT);
	}
	else if (RR_RADIO == type)
	{	
		unsigned int hDb = 0;

		RadioAgentMcc::OpenDatabase openDatabaseCommand(m_IAPIRequest);
		openDatabaseCommand.execute();
		openDatabaseCommand.printStatus();
		if (openDatabaseCommand.getStatus()) {
			RadioAgentMcc::MccDbException ex("openDatabaseCommand error");
			TA_THROW(ex);
		}
		hDb = openDatabaseCommand.getHandle();

		if (0 == m_privateCallResource) {
			RadioAgentMcc::MccDbException ex("Private call resource not found");
			TA_THROW(ex);
		} 
		unsigned int radioNumRec = 0;
		unsigned int radioHq = 0;
		RadioAgentMcc::CreateUnitAliasListQuery createUnitAliasListQueryCommand(m_IAPIRequest, hDb, m_privateCallResource);
		createUnitAliasListQueryCommand.execute();
		createUnitAliasListQueryCommand.printStatus();
		if (createUnitAliasListQueryCommand.getStatus()) {
			RadioAgentMcc::MccDbException ex("createUnitAliasListQueryCommand error");
			TA_THROW(ex);
		}
		radioNumRec = createUnitAliasListQueryCommand.getNumRecs();
		radioHq = createUnitAliasListQueryCommand.getQueryHdb();
		for (; radioNumRec > 0; radioNumRec--)
		{
			RadioAgentMcc::GetNextUnitAlias getNextUnitAliasCommand(m_IAPIRequest, radioHq);
			getNextUnitAliasCommand.execute();
			getNextUnitAliasCommand.printStatus();
			if (getNextUnitAliasCommand.getStatus()) {
				RadioAgentMcc::MccDbException ex("getNextUnitAliasCommand error");
				TA_THROW(ex);
			}
			RadioResource resource;
			resource.ssi = getNextUnitAliasCommand.getUnitId();
			resource.id = resource.ssi;
			resource.alias = getNextUnitAliasCommand.getUnitAlias();
			resource.type = RR_RADIO;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Adding Radio Resource: ssi = %u, alias = %s", resource.ssi, resource.alias.c_str());
			m_mccCollection.push_back(resource);
		}
		RadioAgentMcc::DestroyQuery radioDestroyQueryCommand(m_IAPIRequest, radioHq);
		radioDestroyQueryCommand.execute();
		radioDestroyQueryCommand.printStatus();
		if (radioDestroyQueryCommand.getStatus()) {
			RadioAgentMcc::MccDbException ex("radioDestroyQueryCommand error");
			TA_THROW(ex);
		}

		RadioAgentMcc::CloseDatabase closeDatabaseCommand(m_IAPIRequest, hDb);
		closeDatabaseCommand.execute();
		closeDatabaseCommand.printStatus();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: Restarting timer");
		m_timeoutThread->start(RadioMFTResponse::KEEPALIVE_TIMEOUT);
	}

	FUNCTION_EXIT;
}

void RadioMFTHandler::getCurIscsDatabase(RadioResourceType type)
{
	TA_Base_Core::ThreadGuard guard(m_db);
	FUNCTION_ENTRY("getCurIscsDatabase");
	m_iscsCollection.clear();
	std::vector<TA_IRS_Core::IRadioSubscriber*> raSubscribers;
	std::vector<TA_IRS_Core::IRadioSubscriber*> pcSub;
	try {
		 raSubscribers = TA_IRS_Core::RadioSubscriberFactory::getInstance().getRadioSubscribersByResourseType(type, false);
		 if (RR_TALKGROUP == type) {
			pcSub = TA_IRS_Core::RadioSubscriberFactory::getInstance().getRadioSubscribersByResourseType(RR_PRIVATECALL, false);
			raSubscribers.push_back(*(pcSub.begin()));
			pcSub.clear();
		 }
	}
	catch (TA_Base_Core::TransactiveException& ex) {
		raSubscribers.clear();
		RadioAgentMcc::MccDbException e(ex.what());
		TA_THROW(e);
	}
	std::vector<TA_IRS_Core::IRadioSubscriber*>::iterator iter = raSubscribers.begin();

	for (; iter != raSubscribers.end(); iter++) {
		RadioResource resource;
		resource.type = (RadioResourceType)(*iter)->getResourceType();
		resource.id = (*iter)->getResourceId();
		resource.alias = (*iter)->getAlias();
		resource.ssi = (*iter)->getSSI();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC: Obtain resource, id = %u, alias = %s, type = %c", resource.id, resource.alias.c_str(), resource.type);
		m_iscsCollection.push_back(resource);
		delete (*iter);
	}
	raSubscribers.clear();
	FUNCTION_EXIT;
}

void RadioMFTHandler::notifyMftAgents(RadioResourceType type)
{
	m_mccCollection.clear();
	FUNCTION_ENTRY("notifyMftAgents");
	FUNCTION_EXIT;
}