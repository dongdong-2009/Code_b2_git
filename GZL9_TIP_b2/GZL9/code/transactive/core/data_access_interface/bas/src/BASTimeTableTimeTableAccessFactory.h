#if !defined (Transactive_3002_GF_Core_Bas_TimeTable_BASTimeTableTimeTableAccessFactory)
#define Transactive_3002_GF_Core_Bas_TimeTable_BASTimeTableTimeTableAccessFactory
#if _MSC_VER > 1000
#pragma once
#endif

#include <sstream>
#include <string>
#include <map>
#include "ace/Singleton.h"

namespace TA_IRS_Core
{
	class LocationAndTimeTableType 
	{
	public:
		LocationAndTimeTableType()
		:pkey(0),locationkey(0),timetableTypeName(""), timetableDisplayName("")
		{
		}
        LocationAndTimeTableType(unsigned long key, unsigned long location, std::string typeName, std::string displayName)
			:pkey(key),locationkey(location),timetableTypeName(typeName), timetableDisplayName(displayName)
		{
		}
	public:
		unsigned long pkey;
		unsigned long locationkey;
		std::string timetableTypeName;
        std::string timetableDisplayName;
	};

	typedef std::map<unsigned long, LocationAndTimeTableType> LocationAndTimeTableTypeMap;
	typedef LocationAndTimeTableTypeMap::iterator LocationAndTimeTableTypeMapIterator;

	// normally this table is a configed table, and read only
	class BASTimeTableTimeTableAccessFactory
	{
	public:
		BASTimeTableTimeTableAccessFactory(void);
		~BASTimeTableTimeTableAccessFactory(void);
	public:
		void getAllLocationAndTimeTableType(LocationAndTimeTableTypeMap& locationAndTypeMap);

		void getLocationAndTimeTableTypeByLocation(LocationAndTimeTableTypeMap& locationAndTypeMap, unsigned long locationKey);

		unsigned long getTimeTableKeyByLocationkeyAndType(unsigned long locationKey, const std::string& strType);

		void getLocationKeyAndTypeByKey(unsigned long pkey, unsigned long& locationKey, std::string& strType);
	public:
		static const std::string TableName;
		static const std::string ColumnName_Pkey;
		static const std::string ColumnName_locationKey;
		static const std::string ColumnName_TypeName;
        static const std::string ColumnName_DisplayName;
	};
	typedef ACE_Singleton<BASTimeTableTimeTableAccessFactory,ACE_Recursive_Thread_Mutex> BASTimeTableTimeTableAccessFactoryInstance;
}

#endif //Transactive_3002_GF_Core_Bas_TimeTable_BASTimeTableTimeTableAccessFactory