#if !defined(Transactive_GF_3002_Core_Bas_TunnelModeManager_BasTunnelCongestionModeAccessFactory)
#define Transactive_GF_3002_Core_Bas_TunnelModeManager_BasTunnelCongestionModeAccessFactory
#pragma once  

#include "core/data_access_interface/bas/src/CommonDefine.h"

namespace TA_IRS_Core
{
	typedef struct 
	{
		unsigned long pkey;
		unsigned long trackNumber;
		unsigned long frontStation_modeKey;
		std::string frontStation_modeType;
		std::string frontStation_modeName;
		unsigned long frontStation_modeValue;
		std::string frontStation_modeDesc;
		unsigned long backStation_modeKey;
		std::string backStation_modeType;
		std::string backStation_modeName;
		unsigned long backStation_modeValue;
		std::string backStation_modeDesc;
	} CongestionModeInfo;
	typedef boost::shared_ptr<CongestionModeInfo> CongestionModeInfo_SmartPtr;
	typedef std::vector<CongestionModeInfo_SmartPtr> CongestionModeInfoVct;

	class BasTunnelCongestionModeAccessFactory
	{
	public:
		BasTunnelCongestionModeAccessFactory(void);
		~BasTunnelCongestionModeAccessFactory(void);

	public:
		void getFrontAndBackStationByKey(unsigned long pkey, unsigned long& frontStationKey, unsigned long& backStationKey);
		void getCongestionModes(CongestionModeInfoVct& modeInfos);
		void getCongestionModes(unsigned long trackNumber, CongestionModeInfoVct& modeInfos);

		void getModesByWhereSql(const std::string& strSqlWhere, CongestionModeInfoVct& modeInfos);

	private:
		const static std::string CongestionModeTableName;
		const static std::string CongestionModeTableName_V;
		const static std::string ColumnName_Pkey;
		const static std::string ColumnName_TrackNumber;
		const static std::string ColumnName_FrontStationKey;
		const static std::string ColumnName_FrontStation_ModeKey;
		const static std::string ColumnName_FrontStation_ModeType;
		const static std::string ColumnName_FrontStation_ModeName;
		const static std::string ColumnName_FrontStation_ModeValue;
		const static std::string ColumnName_FrontStation_ModeDescription;
		const static std::string ColumnName_BackStationKey;
		const static std::string ColumnName_BackStation_ModeKey;
		const static std::string ColumnName_BackStation_ModeType;
		const static std::string ColumnName_BackStation_ModeName;
		const static std::string ColumnName_BackStation_ModeValue;
		const static std::string ColumnName_BackStation_ModeDescription;
	};

	typedef ACE_Singleton<BasTunnelCongestionModeAccessFactory,ACE_Recursive_Thread_Mutex> BasTunnelCongestionModeAccessFactoryInstance;
}

#endif //Transactive_GF_3002_Core_Bas_TunnelModeManager_BasTunnelCongestionModeAccessFactory