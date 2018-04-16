#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerDataHelper)
#define Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerDataHelper
#if _MSC_VER > 1000
#pragma once
#endif

#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/bas/src/ModeAccessFactory.h"
#include "ace/Singleton.h"
#include "boost/smart_ptr.hpp"
#include "core/data_access_interface/bas/src/BASTimeTableMappingAccessFactory.h"
#include "core/data_access_interface/bas/src/SubsystemAccessFactory.h"

namespace TA_IRS_App
{
	using namespace TA_IRS_Core;
	typedef boost::shared_ptr<TA_Base_Core::ILocation> ILocation_SmartPtr;
	typedef std::vector<Mode_SmartPtr> locationModes;
	typedef std::vector<Mode_SmartPtr>::iterator locationModesIter;
	typedef std::map<unsigned long, locationModes> ModesInfo;
	typedef std::map<unsigned long, locationModes>::iterator ModesInfoIter;

	typedef enum 
	{
		TTT_Unknown = -1, 
        TTT_Workdays, 
        TTT_Friday, 
        TTT_Saturday, 
        TTT_Sunday, 
        TTT_Holiday, 
        TTT_Special1, 
        TTT_Special2, 
        TTT_Special3, 
        TTT_Special4, 
        TTT_Special5,
        TTT_ServerToday,
        TTT_ServerTomorrow
	} TimeTableType;

	typedef struct _TimeTableName_
	{
		_TimeTableName_()
			:strEnglishName(""), strChineseName(""){}
		_TimeTableName_(std::string englishName, const std::string& chineseName)
			:strEnglishName(englishName), strChineseName(chineseName){}
		std::string strEnglishName;
		std::string strChineseName;
	}TimeTableName;
	typedef std::map<TimeTableType, TimeTableName> TimeTableNameMap;

	typedef struct _ScheduleInfo_
	{
		unsigned long modeKey;
		//std::string modeName; //not used now
		std::string modeTime;
	}ScheduleInfo;
	typedef std::vector<ScheduleInfo> ScheduleInfoVct;

	class TimeTableManagerDataHelper
	{
	public:
		friend ACE_Singleton<TimeTableManagerDataHelper,ACE_Recursive_Thread_Mutex>;
		~TimeTableManagerDataHelper(void);
	public:
		// Get local location info
		ILocation_SmartPtr getLocalLocation();
		// Get all location info
		std::vector<ILocation_SmartPtr> getAllLocations();
		// Get the location by pkey
		ILocation_SmartPtr getLocationByKey(unsigned long locationKey);
		// Get time table schedule info by location and type
		void getTimeTableScheduleInfoByLocationAndType(unsigned long locationKey, 
			const std::string& strType,	TimeTableScheduleInfoVct& timeTableInfo);
		void deleteTimeTableScheduleInfoByLocationAndType(unsigned long locationKey, const std::string& strType);
		void addTimeTableScheduleInfoByLocationAndType(unsigned long locationKey, const std::string& strType, const ScheduleInfoVct& scheduleInfo);
		void getModesByLocation(unsigned long locationKey, locationModes& modes);
		unsigned long getModeKeyByName(unsigned long locationKey, const std::string& strModeName);
		Mode_SmartPtr getModeByModeValue(unsigned long locationKey, unsigned long modeValue);
		
		void getTimeTableNames(TimeTableNameMap& timeTableNames);
        unsigned long getSubsystemOrderIDByKey(Mode_SmartPtr modePtr);
        void loadModesByLocation(unsigned long locationKey);

	private:
		TimeTableManagerDataHelper(void);
	private:
		std::vector<ILocation_SmartPtr> m_locations;		// The locations
        ILocation_SmartPtr m_ptrLocalStation;
		TA_Base_Core::NonReEntrantThreadLockable m_LocationLock;	// The location Lock

		TimeTableScheduleInfoVct m_timeTableSchedules;		// The cache of all time table schedules

		ModesInfo m_modes;

		TimeTableNameMap m_timeTableNameMaps;

		TA_IRS_Core::AllLocationSubsystems m_allSubSystems;
	};
	typedef ACE_Singleton<TimeTableManagerDataHelper,ACE_Recursive_Thread_Mutex> TimeTableManagerDataHelperInstance;
}


#endif //Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerDataHelper
