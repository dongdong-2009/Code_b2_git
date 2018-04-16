#if !defined(Transactive_GF_3002_Core_Bas_TunnelModeManager_BasTunnelFireModeAccessFactory)
#define Transactive_GF_3002_Core_Bas_TunnelModeManager_BasTunnelFireModeAccessFactory
#pragma once  

#include "core/data_access_interface/bas/src/CommonDefine.h"

namespace TA_IRS_Core
{

	typedef struct  
	{
		unsigned long Pkey;
		unsigned long TrackNumber;
		unsigned long FireCaseKey;
		std::string FireCaseName;
		unsigned long FrontStation_ModeKey;
		std::string FrontStation_ModeName;
		std::string FrontStation_ModeType;
		unsigned long FrontStation_ModeValue;
		std::string FrontStation_ModeDescription;
		unsigned long BackStation_ModeKey;
		std::string BackStation_ModeName;
		std::string BackStation_ModeType;
		unsigned long BackStation_ModeValue;
		std::string BackStation_ModeDescription;
	} TunnelFireModeInfo;
	typedef boost::shared_ptr<TunnelFireModeInfo> TunnelFireModeInfo_SmartPtr;

	typedef std::vector<TunnelFireModeInfo_SmartPtr> TunnelFireModeInfoVct;

	class BasTunnelFireModeAccessFactory
	{
	public:
		BasTunnelFireModeAccessFactory(void);
		~BasTunnelFireModeAccessFactory(void);
	public:
		void getFrontAndBackStationByKey(unsigned long pkey, unsigned long& frontStationKey, unsigned long& backStationKey);

		void getFireModes(TunnelFireModeInfoVct& fireModeInfos);
		void getFireModes(unsigned long trackNumber, TunnelFireModeInfoVct& fireModeInfos);
		void getFireModes(unsigned long trackNumber, unsigned long firecaseKey, TunnelFireModeInfoVct& fireModeInfos);
		void getFireModesByWhereSql(const std::string& strSql, TunnelFireModeInfoVct& fireModes);

	private:
		const static std::string FireModeTableName;
		const static std::string FireModeTableName_V;
		const static std::string ColumnName_Pkey;
		const static std::string ColumnName_TrackNumber;
		const static std::string ColumnName_FireCaseKey;
		const static std::string ColumnName_FireCaseName;
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

	typedef ACE_Singleton<BasTunnelFireModeAccessFactory,ACE_Recursive_Thread_Mutex> BasTunnelFireModeAccessFactoryInstance;
}

#endif //Transactive_GF_3002_Core_Bas_TunnelModeManager_BasTunnelFireModeAccessFactory