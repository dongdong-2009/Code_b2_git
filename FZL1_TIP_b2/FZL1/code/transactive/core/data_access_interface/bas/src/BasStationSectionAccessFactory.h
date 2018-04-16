#if !defined(Transactive_GF_3002_Core_Bas_TunnelModeManager_BasStationSectionAccessFactory)
#define Transactive_GF_3002_Core_Bas_TunnelModeManager_BasStationSectionAccessFactory
#pragma once  

#include "core/data_access_interface/bas/src/CommonDefine.h"

namespace TA_IRS_Core
{
	typedef struct  
	{
		unsigned long pkey;
		std::string strName;
		//bool isHaveShaft;
	} StationSectionInfo;
	class BasStationSectionAccessFactory
	{
	public:
		BasStationSectionAccessFactory(void);
		~BasStationSectionAccessFactory(void);
	public:
		void getStationSectionInfo(std::vector<StationSectionInfo>& StationSectionInfos);

		StationSectionInfo getStationSectionInfoByKey(unsigned long sectionKey);

	private:
		const static std::string StationSectionTableName;
		const static std::string ColumnName_Pkey;
		const static std::string ColumnName_Name;
	};

	typedef ACE_Singleton<BasStationSectionAccessFactory,ACE_Recursive_Thread_Mutex> BasStationSectionAccessFactoryInstance;
}

#endif //Transactive_GF_3002_Core_Bas_TunnelModeManager_BasStationSectionAccessFactory