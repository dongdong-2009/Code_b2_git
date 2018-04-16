#if !defined (Transactive_3002_GF_Core_Bas_TimeTable_BASTimeTableMappingAccessFactory)
#define Transactive_3002_GF_Core_Bas_TimeTable_BASTimeTableMappingAccessFactory
#if _MSC_VER > 1000
#pragma once  
#endif

#include "ace/Singleton.h"
#include "boost/smart_ptr.hpp"
#include <string>
#include <set>
#include "core/data_access_interface/bas/src/BASTimeTableModeTimeAccessFactory.h"
#include "core/data_access_interface/bas/src/BASTimeTableTimeTableAccessFactory.h"

namespace TA_IRS_Core
{
	class TimeTableMapping
	{
	public:
		TimeTableMapping(unsigned long pkey, unsigned long timeTableKey, unsigned long modeTimeKey)
			: m_pkey(pkey), m_timeTableKey(timeTableKey), m_modeTimekey(modeTimeKey)
		{
		}
		unsigned long m_pkey;
		unsigned long m_timeTableKey;
		unsigned long m_modeTimekey;
	};

	class TimeTableScheduleInfo
	{
	public:
		TimeTableScheduleInfo() :
         m_tmBegin( 0u )
		{
		}

        TimeTableScheduleInfo( const TimeTableScheduleInfo& refSrc ) :
        m_pkey(refSrc.m_pkey), m_locationKey(refSrc.m_locationKey), m_modeKey(refSrc.m_modeKey), 
        m_tmBegin(refSrc.m_tmBegin), m_locationName(refSrc.m_locationName), m_typeName(refSrc.m_typeName), 
        m_modeName(refSrc.m_modeName), m_modeValue(refSrc.m_modeValue), m_modeDescription(refSrc.m_modeDescription)
        {
        }

	public:
		unsigned long m_pkey;
        unsigned long m_locationKey;
        unsigned long m_modeKey;
        time_t m_tmBegin;
		std::string m_locationName;
		std::string m_typeName;
		std::string m_modeName;
		std::string m_modeValue;
		std::string m_modeDescription;
	};

	typedef boost::shared_ptr<TimeTableScheduleInfo> TimeTableScheduleInfo_SmartPtr;
	typedef std::vector<TimeTableScheduleInfo_SmartPtr> TimeTableScheduleInfoVct;


	class BASTimeTableMappingAccessFactory
	{
	public:
		BASTimeTableMappingAccessFactory(void);
		~BASTimeTableMappingAccessFactory(void);
	public:
        std::set<unsigned long> getAllValidLocationKeys();
		void addTimeTableMapping(unsigned long timeTableKey, unsigned long modeTimeKey);
		void deleteTimeTableMapping(unsigned long pKey);
		void deleteTimeTableMapping(const std::vector<unsigned long>& pKeyVct);
		void getAllTimeTableScheduleInfo(TimeTableScheduleInfoVct& timeTableScheduleVct);
		void getAllTimeTableScheduleInfo(TimeTableScheduleInfoVct& timeTableScheduleVct, const std::string& strSqlWherePart);
		void getAllTimeTableScheduleInfoByLocationAndType(unsigned long locationKey, const std::string& typeName, TimeTableScheduleInfoVct& timeTableScheduleVct);
		void getAllTimeTableScheduleInfoByTableTableKey(unsigned long timeTableKey, TimeTableScheduleInfoVct& timeTableScheduleVct);
	private:
		unsigned long getNextPkey();
        time_t transformTimeString( const std::string& strSrc );
		std::string getSqlInPart(std::vector<unsigned long> vctValue, bool withBrackets);
	private:
		static const std::string TableName;
		static const std::string ViewTableName;
		static const std::string ColumnName_Pkey;
		static const std::string ColumnName_LocationName_V;
		static const std::string ColumnName_Description_V;
		static const std::string ColumnName_ModeSubsystemKey_V;
		static const std::string ColumnName_ModeSubsystemName_V;
		static const std::string ColumnName_ModeName_V;
		static const std::string ColumnName_ModeValue_V;
		static const std::string ColumnName_TimeTableKey;
		static const std::string ColumnName_ModeTimeKey;
	};
	typedef ACE_Singleton<BASTimeTableMappingAccessFactory,ACE_Recursive_Thread_Mutex> BASTimeTableMappingAccessFactoryInstance;
}

#endif //Transactive_3002_GF_Core_Bas_TimeTable_BASTimeTableMappingAccessFactory