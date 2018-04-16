#pragma once
#include "ace/Singleton.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "app/bas/BASModeManager/src/CommonDef.h"

namespace TA_IRS_App
{
	// the mode status config manager
	class ModeRunningStatusConfigManager
	{
	public:
		ModeRunningStatusConfigManager(void);
		~ModeRunningStatusConfigManager(void);
	public:
		// get the config mode status string by mode status type
		static std::string getModeRunningStatusByType(int modeStatus);
		static int getModeStatusByName(const std::string& modeStatus);
	private:
		static const std::string m_MRS_Unknown;
		static const std::string m_MRS_Running;
		static const std::string m_MRS_Success;
		static const std::string m_MRS_Fail;
	};

	//////////////////////////////////////////////////////////////////////////////
	// the mode status manager
	class ModeStatusManager
	{
	public:
		ModeStatusManager(void);
		~ModeStatusManager(void);
	public:
		// update the mode status
		void updateModeStatus( unsigned long modeKey, int status );
	    void updateModeStatusForClean( unsigned long locationKey, unsigned long subsytemKey );

		// get the mode current status 
		int getModeStatus(unsigned long modeKey);

		bool isModeExecuteSuccessInTime(unsigned long modeKey);

		std::vector<unsigned long> getRunningMode();

	private:
		void cleanLocationSubsystemModesStatus(unsigned long locationKey, unsigned long subsystemKey);

		TA_Base_Core::NonReEntrantThreadLockable 								m_modeStatusLock;
		typedef std::map<unsigned long /*modeKey*/,int> 						ModeStatus;
		typedef std::map<unsigned long /*subsystemKey*/, ModeStatus> 			SubsystemModeStatus;
		typedef std::map<unsigned long /*locationKey*/, SubsystemModeStatus> 	LocationModeStatus;
		typedef ModeStatus::iterator 											ModeStatusIterator;
		typedef SubsystemModeStatus::iterator 									SubsystemModeIterator;
		typedef LocationModeStatus::iterator 									LocationModeStatusIterator;
		
		LocationModeStatus 														m_modeStatus;

		TA_Base_Core::NonReEntrantThreadLockable 								m_RunningModeLock;
		std::map<unsigned long, time_t> 										m_modeExecuteTime;
	};
	
	typedef ACE_Singleton<ModeStatusManager,ACE_Recursive_Thread_Mutex> ModeStatusManagerInstance;
}

