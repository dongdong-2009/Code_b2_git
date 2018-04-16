#if !defined(TA_3002_transactive_core_data_access_interface_bas_ModeDeviceAccessFactory_H) 
#define TA_3002_transactive_core_data_access_interface_bas_ModeDeviceAccessFactory_H
#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include "boost/smart_ptr.hpp"

namespace TA_IRS_Core
{
	typedef struct _ModeDeviceInfo_
	{
		unsigned long pkey;
		unsigned long locationKey;
		unsigned long modeKey;
		std::string deviceName;
		std::string datapointTag_Status;
		std::string datapointTag_Auto;
		std::string description;
		std::string expectStatus;
	}ModeDeviceInfo;
	typedef std::vector<ModeDeviceInfo> ModeDevices;
	typedef ModeDevices::iterator ModeDevicesIter;
	typedef std::map<unsigned long /*modeKey*/,ModeDevices> ModeToDevicesMap;
	typedef std::map<unsigned long /*locationKey*/,ModeToDevicesMap> LocationModeDevices;

	class ModeDeviceAccessFactory
	{
	public:
		ModeDeviceAccessFactory(void);
		~ModeDeviceAccessFactory(void);
	public:
		static ModeDeviceAccessFactory& getInstance();

		void getAllModeDevices(ModeDevices& modeDevices);
		void getModeDevicesByModeKey(unsigned long locationKey,unsigned long modeKey, ModeDevices& modeDevices);
		void getModeDevicesByModeName(unsigned long locationKey, const std::string& modeName, ModeDevices& modeDevices);
		void getModeDevicesBySqlWhere(const std::string& strSqlWhere, ModeDevices& modeDevices);
		void getModeDevicesBySqlWhereEx(const std::string& strSqlWhere, ModeDevices& modeDevices);
	private:
		static ModeDeviceAccessFactory* m_instance;

		static const std::string TableName_V;
		static const std::string TableName;
		static const std::string ColumnName_Pkey;
		static const std::string ColumnName_LocationKey;
		static const std::string ColumnName_ModeKey;
		static const std::string ColumnName_ModeName;
		static const std::string ColumnName_DeviceName;
		static const std::string ColumnName_DatapointTag_Status;
		static const std::string ColumnName_DatapointTag_Auto;
		static const std::string ColumnName_Description;
		static const std::string ColumnName_ExpectStatus;
	};
}

#endif //TA_3002_transactive_core_data_access_interface_bas_ModeDeviceAccessFactory_H
