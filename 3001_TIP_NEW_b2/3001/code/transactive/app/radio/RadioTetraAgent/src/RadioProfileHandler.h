/**
 *
 * To access gateway to retrieve data and synchronize with ISCS database 
 */
#ifndef RADIO_PROFILE_HANDLER_H_INCLUDED
#define RADIO_PROFILE_HANDLER_H_INCLUDED
#include "app/radio/RadioTetraAgent/RadioTcpCommunication/src/BaseTcpServer.h"
#include "app/radio/RadioTetraAgent/src/RadioResourceSync.h"

#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodProfileUpdate.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventProfileUpdate.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodProfileRequest.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventProfileRequest.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethod.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

#include "core/data_access_interface/radio/src/RadioBtnCfgFactory.h"
#include "core/data_access_interface/radio/src/RadioSubscriberFactory.h"
#include "core/data_access_interface/radio/src/IRadioSubscriber.h"
#include "core/data_access_interface/radio/src/RadioBtnCfgFactory.h"
#include "core/data_access_interface/radio/src/IRadioBtnCfg.h"
#include "bus/radio/radio_bus/src/radiotypes.h"

#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"

#include "core/utilities/src/DebugUtil.h"
#include "ace/Time_Value.h"
#include "ace/OS_NS_unistd.h"
#include "ace/Task.h"
#include <vector>

namespace TA_IRS_App 
{
	class BaseTcpServer;

	typedef std::vector<std::pair<std::string, short> > DataSet;
	typedef std::vector<std::string> PredefinedTg;

	class RadioProfileHandler : public virtual ACE_Task_Base {
	public:
		RadioProfileHandler(BaseTcpServer* sender, RadioResourceSync* sync);
		~RadioProfileHandler();

		void initialize();
		void start();
		void stop();
		virtual int svc();

		// initialize profiles by requesting all talkgroups for all profiles from SE_PROFILE
		void initializeProfiles();

		// request radio profile (pre-defined talkgroups) from gateway
		PredefinedTg requestRadioProfile(std::string userAlias);

		// Delete this profile from database
		bool deleteRadioProfile(std::string userAlias);

		// Request radio profile from gateway and insert into database, delete existing one if any
		bool insertRadioProfile(std::string userAlias);

		// Request radio profile from gateway and update the database
		bool updateRadioProfile(std::string userAlias);
		
		PredefinedTg getTgNames(std::vector<TA_IRS_Core::IRadioBtnCfg*> btnCfg);
		//unsigned long getTgId(std::string name);
        bool isEmptyTg(const PredefinedTg& tgs);
		bool insertTgIntoDb(PredefinedTg& tgs, unsigned long profileKey);
		bool updateTgInDb(PredefinedTg& tgs, unsigned long profileKey);
		unsigned long getProfileKey(std::string name);

		void refreshProfiles();
		void refreshTgResources();
	private:
		bool m_terminate;
		BaseTcpServer* m_tcpSender;
		std::vector<TA_Base_Core::IProfile*> m_profiles;
		std::vector<TA_IRS_Core::IRadioSubscriber*> m_tgSubscribers;
		RadioResourceSync* m_syncHandler;

		static const short RADIO_PROFILE_UPDATE_STATUS_DELETE;
		static const short RADIO_PROFILE_UPDATE_STATUS_INSERT;
		static const short RADIO_PROFILE_UPDATE_STATUS_UPDATE;
		static const short TALKGROUP_GROUP_SIZE;
		static const short PROFILE_NAME_MAX_LENGTH;
		
		// function helper that maintain 12 TG items on each operator
		void validateTalkGroupNumberPerProfile (const long & lProfileKey);

	};
}

#endif