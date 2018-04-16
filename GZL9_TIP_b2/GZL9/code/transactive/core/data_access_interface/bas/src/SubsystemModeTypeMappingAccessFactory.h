#if !defined(TA_3002_transactive_core_data_access_interface_bas_SubsystemModeTypeMappingAccessFactory_H)
#define  TA_3002_transactive_core_data_access_interface_bas_SubsystemModeTypeMappingAccessFactory_H
#pragma once  

#include "ace/Singleton.h"
#include <map>
#include <string>
#include <vector>
#include <set>

namespace TA_IRS_Core
{
	typedef std::vector<unsigned long> modeTypes;
	typedef std::map<unsigned long, modeTypes> SubSystemModeTypeMap; //SubsystemKey, ModeTypeKey
	typedef std::map<unsigned long, SubSystemModeTypeMap> LocationSubSystemModeTypeMap;
	class SubsystemModeTypeMappingAccessFactory
	{
	public:
		SubsystemModeTypeMappingAccessFactory(void);
		~SubsystemModeTypeMappingAccessFactory(void);
    public:

        std::set<unsigned long> getAllValidLocationKeys();
		void getSubSystemModeTypeMapByLocation(unsigned long locationKey, SubSystemModeTypeMap& subSystemTypeMaps);
	private:
		const static std::string TableName;
		const static std::string ColumnName_Pkey;
		const static std::string ColumnName_LocationKey;
		const static std::string ColumnName_SubsystemKey;
		const static std::string ColumnName_TypeKey;
	};
	typedef ACE_Singleton<SubsystemModeTypeMappingAccessFactory,ACE_Recursive_Thread_Mutex> SubsystemModeTypeMappingAccessFactoryInstance;
}
#endif //TA_3002_transactive_core_data_access_interface_bas_SubsystemModeTypeMappingAccessFactory_H