#include "app/radio/RadioTetraAgent/src/RadioProfileHandler.h"

#include <iostream>


using namespace TA_IRS_App;


const short RadioProfileHandler::RADIO_PROFILE_UPDATE_STATUS_DELETE = 0;
const short RadioProfileHandler::RADIO_PROFILE_UPDATE_STATUS_INSERT = 1;
const short RadioProfileHandler::RADIO_PROFILE_UPDATE_STATUS_UPDATE = 2;
const short RadioProfileHandler::TALKGROUP_GROUP_SIZE = 12;
const short RadioProfileHandler::PROFILE_NAME_MAX_LENGTH = 12;

RadioProfileHandler::RadioProfileHandler(BaseTcpServer* sender, RadioResourceSync* sync) : m_tcpSender(sender), m_syncHandler(sync)
{
	FUNCTION_ENTRY("constructor");
	initialize();
	FUNCTION_EXIT;
}

RadioProfileHandler::~RadioProfileHandler()
{
	FUNCTION_ENTRY("destructor");
	FUNCTION_EXIT;
}

void RadioProfileHandler::initialize()
{
	FUNCTION_ENTRY("initialize");
	FUNCTION_EXIT;
}

void RadioProfileHandler::start()
{
	FUNCTION_ENTRY("start");
	m_terminate = false;
	activate();
	FUNCTION_EXIT;
}

void RadioProfileHandler::stop()
{
	FUNCTION_ENTRY("stop");
	m_terminate = true;
	wait();
	FUNCTION_EXIT;
}

int RadioProfileHandler::svc()
{
	initializeProfiles();

	while (!m_terminate) 
	{
		try
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Polling data from gateway server with interval 10 seconds");

			RadioMethodProfileUpdate method;
			boost::shared_ptr<RadioEvent> radioEvent = m_tcpSender->sendMethod(method);
			radioEvent->setEventLength(EVENT_LENGTH_BASE_PROFILE_UPDATE);
			RadioEventReader* eventReader = radioEvent->getEventReader();

			if(NULL == eventReader)
			{
				throw(TA_Base_Core::TransactiveException("failed to initialize event reader"));
			}

			RadioEventProfileUpdate* radioEventProfileUpdate = dynamic_cast<RadioEventProfileUpdate*>(eventReader);
			DataSet set = radioEventProfileUpdate->getDataSet();
			int numSet = radioEventProfileUpdate->getNumDataSet();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Total number of updates: %u", numSet);
			
			for (int i = 0; i < numSet; i++) 
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Radio Profile Update %u: user alias %s with status %u", i+1, set[i].first.c_str(), set[i].second);
				switch (set[i].second) 
				{
					case RadioProfileHandler::RADIO_PROFILE_UPDATE_STATUS_DELETE:						
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Radio Profile Update: Deleting username %s", set[i].first.c_str());
						deleteRadioProfile(set[i].first);
						break;
					case RadioProfileHandler::RADIO_PROFILE_UPDATE_STATUS_INSERT:
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Radio Profile Update: Inserting username %s", set[i].first.c_str());
						insertRadioProfile(set[i].first);
						break;
					case RadioProfileHandler::RADIO_PROFILE_UPDATE_STATUS_UPDATE:
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Radio Profile Update: Updating username %s", set[i].first.c_str());
						updateRadioProfile(set[i].first);
						break;
				}
			}

			ACE_OS::sleep(10);
		}
		catch (const TA_Base_Core::TransactiveException& e) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occur: %s", e.what());
		}
		catch (...) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred in call to updateOperatorProfile");
		}
		
		ACE_OS::sleep(5);
	}
	return 0;
}

void RadioProfileHandler::initializeProfiles()
{
	
	std::cout << "Initializing Profiles" << std::endl;

	try
	{
		m_profiles.clear();
		m_profiles = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles();
		std::vector<TA_Base_Core::IProfile*>::iterator iter = m_profiles.begin();

		m_tgSubscribers.clear();
		m_tgSubscribers = TA_IRS_Core::RadioSubscriberFactory::getInstance().getRadioSubscribersByResourseType(RR_TALKGROUP);		
		std::vector<TA_IRS_Core::IRadioBtnCfg*> dbData;

		PredefinedTg gwTg, dbTg;
		bool isGwTgEmpty = false; 
		bool isDbTgEmpty = false;

		for(; iter!=m_profiles.end(); iter++) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Debugging, key=%u, name=%s", (*iter)->getKey(), (*iter)->getName().c_str());
			// get TG set from ISCS
			std::cout << "Initializing Talkgroup for Profile :" <<(*iter)->getKey()<< std::endl;
			validateTalkGroupNumberPerProfile ((*iter)->getKey());

			dbData = TA_IRS_Core::RadioBtnCfgFactory::getInstance().getAllRadioBtnCfgByProfile((*iter)->getKey());

			if (dbData.size() != TALKGROUP_GROUP_SIZE) 
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Profile %s, number of talkgroup on RA_BTNCFG not equal to 12(%d). will only look at first 12", (*iter)->getName().c_str(), dbData.size());
			}

			dbTg = getTgNames(dbData);
			isDbTgEmpty = isEmptyTg(dbTg);

			// get TG set from gateway
			gwTg = requestRadioProfile((*iter)->getName());
			isGwTgEmpty = isEmptyTg(gwTg);

			if (isDbTgEmpty && isGwTgEmpty) 
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Profile %s not existing for both ISCS database and gateway.", (*iter)->getName().c_str());
				continue;
			} 
			else if(isDbTgEmpty && !isGwTgEmpty) 
			{
				// insert
				// if want to notify MFT Agents
				m_syncHandler->onProfileDatabaseChange((*iter)->getKey(), gwTg);
				// commit to database
				insertTgIntoDb(gwTg, (*iter)->getKey());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Profile %s not existing on ISCS, adding it to ISCS", (*iter)->getName().c_str());

			} 

			else if(!isDbTgEmpty && isGwTgEmpty) 
			{
				// delete
				// if want to notify MFT Agents
				m_syncHandler->onProfileDatabaseChange((*iter)->getKey(), dbTg);
				// commit to database
				deleteRadioProfile((*iter)->getName());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Profile %s not exit on Gateway, delete it from ISCS", (*iter)->getName().c_str());
			} 
			else 
			{
				// check if it is updated
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Profile %s is being updated on ISCS", (*iter)->getName().c_str());
				bool needUpdate = false;
				for (unsigned index = 0; index < TALKGROUP_GROUP_SIZE; index++) 
				{
					if (dbTg[index].compare(gwTg[index]) != 0)
					{
						// update
						needUpdate = true;
						break;
					}
				}
				if (needUpdate) 
				{
					// if want to notify MFT Agents
					m_syncHandler->onProfileDatabaseChange((*iter)->getKey(), gwTg);
					// commit to database
					updateTgInDb(gwTg, (*iter)->getKey());
				}
			}
		}
	} 
	catch (const TA_Base_Core::TransactiveException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occur: %s", e.what());
	}
}

PredefinedTg RadioProfileHandler::requestRadioProfile(std::string userAlias)
{
	FUNCTION_ENTRY("requestRadioProfile");
	
	PredefinedTg tgs;
	if (userAlias.length() > PROFILE_NAME_MAX_LENGTH) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Profile %s exceeds maximum length (12) of message. Trim to %s",userAlias.c_str(), userAlias.substr(0,PROFILE_NAME_MAX_LENGTH).c_str());
		userAlias = userAlias.substr(0,PROFILE_NAME_MAX_LENGTH);
	}
	try
	{
		RadioMethodProfileRequest method(userAlias);

		boost::shared_ptr<RadioEvent> radioEvent = m_tcpSender->sendMethod(method);
		radioEvent->setEventLength( EVENT_LENGTH_PROFILE_REQUEST);
		RadioEventReader* eventReader = radioEvent->getEventReader();
		if(NULL == eventReader)
		{
			throw(TA_Base_Core::TransactiveException("failed to initialize event reader"));
		}
		RadioEventProfileRequest* radioEventProfile = dynamic_cast<RadioEventProfileRequest*>(eventReader);
		tgs = radioEventProfile->getTalkgroup();
	}
	catch (const TA_Base_Core::TransactiveException& e) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occur: %s", e.what());
	}
	catch (...) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred in call to requestRadioProfile");
	}
	FUNCTION_EXIT;

	return tgs;
}

bool RadioProfileHandler::deleteRadioProfile(std::string userAlias)
{
	FUNCTION_ENTRY("deleteRadioProfile");
	
	unsigned long profileKey = getProfileKey(userAlias);
	
	if (profileKey == 0) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Profile key not found for profile %s", userAlias.c_str());
		return false;
	}
	try 
	{
		
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Notifying Subscriber With ProfileDBChange From GW: Delete profile key : %d",profileKey);
		
		std::vector <std::string> vctrTgEmpty (TALKGROUP_GROUP_SIZE,"");
		m_syncHandler->onProfileDatabaseChange(profileKey, vctrTgEmpty);

		TA_IRS_Core::RadioBtnCfgFactory::getInstance().deletePredefinedTG(profileKey);
	}
	catch (const TA_Base_Core::TransactiveException& e) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occur: %s", e.what());
	}
	catch (...) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred in call to deleteRadioProfile");
	}

	FUNCTION_EXIT;
	return true;
}

bool RadioProfileHandler::insertRadioProfile(std::string userAlias)
{
	FUNCTION_ENTRY("insertRadioProfile");
	try 
	{	
		unsigned long profileKey = getProfileKey(userAlias);
		if (profileKey == 0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Profile key not found for profile %s", userAlias.c_str());
			return false;
		}
	
		TA_IRS_Core::RadioBtnCfgFactory::getInstance().deletePredefinedTG(profileKey);

		// Get the list of monitored talkgroups
		PredefinedTg tgs = requestRadioProfile(userAlias);

		// if want to notify Mft Agent
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					 "Notifying Subscriber With ProfileDBChange From GW: Insert");

		m_syncHandler->onProfileDatabaseChange(profileKey, tgs);
		

		FUNCTION_EXIT;
		return insertTgIntoDb(tgs,profileKey);
	}
	catch (const TA_Base_Core::TransactiveException& e) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occur: %s", e.what());
		FUNCTION_EXIT;
		return false;
	}
	catch (...) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred in call to insertRadioProfile");
		FUNCTION_EXIT;
		return false;
	}
}

bool RadioProfileHandler::updateRadioProfile(std::string userAlias)
{
	FUNCTION_ENTRY("updateRadioProfile");
	try {
		unsigned long profileKey = getProfileKey(userAlias);

		if (profileKey == 0) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Profile key not found for profile %s", userAlias.c_str());
			return false;
		}

		// Get the list of monitored talkgroups
		PredefinedTg tgs = requestRadioProfile(userAlias);
		
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Notifying Subscriber With ProfileDBChange From GW: Update");
				
		m_syncHandler->onProfileDatabaseChange(profileKey, tgs);
		
		FUNCTION_EXIT;
		return updateTgInDb(tgs, profileKey);
		
	}
	catch (const TA_Base_Core::TransactiveException& e) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occur: %s", e.what());
		FUNCTION_EXIT;
		return false;
	}
	catch (...) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred in call to updateRadioProfile");
		FUNCTION_EXIT;
		return false;
	}
}

bool RadioProfileHandler::insertTgIntoDb(PredefinedTg& tgs, unsigned long profileKey)
{
	FUNCTION_ENTRY("insertTgIntoDb");
	try {

		int btnRef = 1;
		RadioResource res;
		res.type = RR_TALKGROUP;
		PredefinedTg::iterator index = tgs.begin();
		
		for (; index!=tgs.end(); index++) 
		{
			std::vector<TA_IRS_Core::IRadioSubscriber*> tg = TA_IRS_Core::RadioSubscriberFactory::getInstance().getRadioSubscribersByResourceAlias(*index, false);
			if (tg.size() != 1) 
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Talkgroup Resource %s not found on the database", (*index).c_str());
				res.alias = "";
				res.id = 0;
				res.ssi = 0;
			} 
			else 
			{
				res.alias = *index;
				res.id = tg[0]->getResourceId();
				res.ssi = tg[0]->getSSI();
			}		

			TA_IRS_Core::RadioBtnCfgFactory::getInstance().insertPredefinedTG(profileKey, btnRef, res);
			btnRef++;
		}
	}
	catch (const TA_Base_Core::TransactiveException& e) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occur: %s", e.what());
		FUNCTION_EXIT;
		return false;
	}
	catch (...) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred in call to insertTgIntoDb");
		FUNCTION_EXIT;
		return false;
	}
	FUNCTION_EXIT;
	return true;
}

bool RadioProfileHandler::updateTgInDb(PredefinedTg& tgs, unsigned long profileKey)
{
	FUNCTION_ENTRY("updateTgInDb");
	try {
		int btnRef = 1;
		RadioResource res;
		res.type = RR_TALKGROUP;
		PredefinedTg::iterator index = tgs.begin();
		for (; index!=tgs.end(); index++) 
		{
			std::vector<TA_IRS_Core::IRadioSubscriber*> tg = TA_IRS_Core::RadioSubscriberFactory::getInstance().getRadioSubscribersByResourceAlias(*index, false);
			if (tg.size() != 1)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Talkgroup Resource %s not found on the database", (*index).c_str());
				res.alias = "";
				res.id = 0;
				res.ssi = 0;
			} 
			else 
			{
				res.alias = *index;
				res.id = tg[0]->getResourceId();
				res.ssi = tg[0]->getSSI();
			}	

			TA_IRS_Core::RadioBtnCfgFactory::getInstance().updatePredefinedTG(profileKey, btnRef, res);
			btnRef++;
		}
	}
	catch (const TA_Base_Core::TransactiveException& e) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occur: %s", e.what());
		FUNCTION_EXIT;
		return false;
	}
	catch (...) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred in call to updateRadioProfile");
		FUNCTION_EXIT;
		return false;
	}
	FUNCTION_EXIT;
	return true;
}

PredefinedTg RadioProfileHandler::getTgNames(std::vector<TA_IRS_Core::IRadioBtnCfg*> btnCfg)
{
	PredefinedTg tg;

	if (btnCfg.empty())
		return tg;

	std::vector<TA_IRS_Core::IRadioBtnCfg*>::iterator iterBtn = btnCfg.begin();
	std::vector<TA_IRS_Core::IRadioSubscriber*>::iterator iterRes;
	
	for (; iterBtn!=btnCfg.end(); iterBtn++) 
	{
		iterRes = m_tgSubscribers.begin();
		if (!(*iterBtn)->getResourceId()) 
		{
			tg.push_back("");
		} 
		else 
		{
			for (; iterRes!=m_tgSubscribers.end(); iterRes++) 
			{
				if ((*iterRes)->getResourceId() == (*iterBtn)->getResourceId()) 
				{
					tg.push_back((*iterRes)->getName());
					break;
				}
			}
			if (iterRes == m_tgSubscribers.end()) 
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Predefined talkgroup id %u from RA_BTNCFG not found on RA_SUBSCRIBER", (*iterBtn)->getResourceId());
			}
			//tg.push_back("");
		}
	}
	return tg;
}

//unsigned long RadioProfileHandler::getTgId(std::string name)
//{
//	std::vector<TA_IRS_Core::IRadioSubscriber*>::iterator iterRes = m_tgSubscribers.begin();		
//	for (; iterRes!=m_tgSubscribers.end(); iterRes++) {
//		if ((*iterRes)->getAlias().compare(name) == 0) {
//			return (*iterRes)->getResourceId();
//		}
//	}
//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Talkgroup with resource id %s not found in RA_SUBSCRIBER table", name.c_str());
//	return 0;
//}

bool RadioProfileHandler::isEmptyTg(const PredefinedTg& tgs)
{
	if (tgs.empty())
	{
		return true;
	}

	bool isEmpty = true;
	
	int nSize = TALKGROUP_GROUP_SIZE;

	if (tgs.size() < TALKGROUP_GROUP_SIZE)
	{
		nSize = tgs.size();
	}
	
	for(unsigned int index = 0; index < nSize; index++) 
	{
		if (!tgs[index].empty() || tgs[index].compare("") != 0) 
		{
			isEmpty = false;
		}
	}
	return isEmpty;
}

unsigned long RadioProfileHandler::getProfileKey(std::string name)
{
	if (m_profiles.empty()) return 0;
	std::vector<TA_Base_Core::IProfile*>::iterator iter = m_profiles.begin();
	for (; iter != m_profiles.end(); iter++)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Debugging, name = %s", (*iter)->getName().c_str());
		if ((*iter)->getName().compare(name) == 0) 
			return (*iter)->getKey();
	}
	return 0;
}

void RadioProfileHandler::refreshProfiles()
{
	m_profiles.clear();
	m_profiles = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles();
}

void RadioProfileHandler::refreshTgResources()
{
	m_tgSubscribers.clear();
	m_tgSubscribers = TA_IRS_Core::RadioSubscriberFactory::getInstance().getRadioSubscribersByResourseType(RR_TALKGROUP);		
}


void RadioProfileHandler::validateTalkGroupNumberPerProfile (const long & lProfileKey)
{
	FUNCTION_ENTRY ("validateTalkGroupNumberPerProfile");

	std::vector <TA_IRS_Core::IRadioBtnCfg *> vctrTGCollection;

	vctrTGCollection = TA_IRS_Core::RadioBtnCfgFactory::getInstance().getAllRadioBtnCfgByProfile(lProfileKey);
	
	int nCollectionSize = vctrTGCollection.size();

	if (TALKGROUP_GROUP_SIZE != nCollectionSize && 0 != nCollectionSize)
	{
		// delete the tg records
		// on iscs db.
		LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"TG number for operator : %d has a tg of %d, deleting all TGrecord and inserting all Gateway updates"
					,lProfileKey,nCollectionSize);
		
		std::cout << "Deleting TG(s) for operator :" << lProfileKey << std::endl;

		TA_IRS_Core::RadioBtnCfgFactory::getInstance().deletePredefinedTG(lProfileKey);
		
		std::cout << "Deleted " << std::endl;
	}

	FUNCTION_EXIT;
}