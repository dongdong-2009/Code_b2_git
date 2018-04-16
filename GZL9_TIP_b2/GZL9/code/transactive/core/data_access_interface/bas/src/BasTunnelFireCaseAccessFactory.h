#if !defined(Transactive_GF_3002_Core_Bas_TunnelModeManager_BasTunnelFireCaseAccessFactory)
#define Transactive_GF_3002_Core_Bas_TunnelModeManager_BasTunnelFireCaseAccessFactory
#pragma once  

#include "core/data_access_interface/bas/src/CommonDefine.h"
namespace TA_IRS_Core
{
	typedef struct  
	{
		unsigned long pkey;
		std::string name;
		std::string description;
	}TunnelFireCaseInfo;
	typedef std::vector<TunnelFireCaseInfo> TunnelFireCaseInfoVct;
	typedef std::vector<TunnelFireCaseInfo>::iterator TunnelFireCaseInfoVctIter;

	class BasTunnelFireCaseAccessFactory
	{
	public:
		BasTunnelFireCaseAccessFactory(void);
		~BasTunnelFireCaseAccessFactory(void);
	public:
		void getAllFireCaseInfo(TunnelFireCaseInfoVct& fireCaseVct);
	private:
		const static std::string TableName;
		const static std::string ColumnName_Pkey;
		const static std::string ColumnName_Name;
		const static std::string ColumnName_Description;
	};
	typedef ACE_Singleton<BasTunnelFireCaseAccessFactory,ACE_Recursive_Thread_Mutex> BasTunnelFireCaseAccessFactoryInstance;
}

#endif //Transactive_GF_3002_Core_Bas_TunnelModeManager_BasTunnelFireCaseAccessFactory