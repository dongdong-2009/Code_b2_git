#include "WFErrorManager.h"
#include <sstream>
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
	// Static variables
	WfErrorManager* WfErrorManager::m_pInstance = NULL;
	TA_Base_Core::NonReEntrantThreadLockable WfErrorManager::m_singletonLock;
	
	WfErrorManager& WfErrorManager::getInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
		if (NULL == m_pInstance)
		{
			m_pInstance = new WfErrorManager;
		}
		return *m_pInstance;
	}

	void WfErrorManager::freeInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}
	
	WfErrorManager::WfErrorManager()
	{
		FUNCTION_ENTRY("WfErrorManager()");
		databaseErrorMsg[1]  = "|1|子站WF通道中断，校核失败！";
		databaseErrorMsg[10] = "KG|0|10|开关两侧刀闸状态不一致，不能操作！";
		databaseErrorMsg[11] = "KG|0|11|不能带地线拉开关！";
		databaseErrorMsg[12] = "KG|0|12|变低侧开关在合位，不能分变高开关！";
		databaseErrorMsg[13] = "KG|0|13|中性点未接地，不能分开关！";
		databaseErrorMsg[14] = "KG|0|14|站内主变中性点未接地，不能分开关！";
		databaseErrorMsg[15] = "KG|0|15|该操作导致负荷失电！";
		databaseErrorMsg[16] = "KG|0|16|该操作导致解环/解列！";
		databaseErrorMsg[17] = "KG|0|17|该操作导致母线失压！";
		
		databaseErrorMsg[30] = "KG|1|30|操作范围内有接地，不能操作！";
		databaseErrorMsg[31] = "KG|1|31|开关两侧刀闸状态不一致，不能操作！";
		databaseErrorMsg[32] = "KG|1|32|操作范围内有接地，不能合开关！";
		databaseErrorMsg[33] = "KG|1|33|变高开关在分位，不能合变低开关！";
		databaseErrorMsg[34] = "KG|1|34|中性点未接地，不能合开关！";
		databaseErrorMsg[35] = "KG|1|35|该操作导致合环/并列！";

		databaseErrorMsg[50] = "DZ|0|50|开关或线刀在合位，不能分母刀！";
		databaseErrorMsg[51] = "DZ|0|51|开关或线刀在合位，不能分母刀！";
		databaseErrorMsg[52] = "DZ|0|52|站内主变中性点未接地，不能拉刀闸！";
		databaseErrorMsg[53] = "DZ|0|53|该操作导致负荷或保电用户失电！";

		databaseErrorMsg[70] = "DZ|1|70|操作范围内有接地，不能合刀闸！";
		databaseErrorMsg[71] = "DZ|1|71|操作范围内有地线，不能合刀闸！";
		databaseErrorMsg[72] = "DZ|1|72|开关在合位或母刀在分位，不能合线刀！";
		databaseErrorMsg[73] = "DZ|1|73|开关在合位或线刀在分位，不能合母刀！";
		databaseErrorMsg[74] = "DZ|1|74|旁母在代路状态，不能合旁刀！";
		databaseErrorMsg[75] = "DZ|1|75|不能带负荷合刀闸！";
		
		databaseErrorMsg[90] = "XC|0|90|开关或线刀在合位，不能分小车！";
		databaseErrorMsg[91] = "XC|0|91|该操作导致负荷或保电用户失电！";
		
		databaseErrorMsg[110] = "XC|2|110|开关或线刀在合位，不能分小车！";
		databaseErrorMsg[111] = "XC|2|111|该操作导致负荷或保电用户失电！";

		databaseErrorMsg[130] = "XC|1|130|该操作范围内有接地，不能合小车！";
		databaseErrorMsg[131] = "XC|1|131|开关在合位或母刀在分位，不能合小车！";
		databaseErrorMsg[132] = "XC|1|132|不能带负荷合小车！";

		databaseErrorMsg[150] = "DD|0|150|有检修牌，不能拆地线！";
		databaseErrorMsg[151] = "DD|0|151|站内主变中性点未接地，不能拆地线！";

		databaseErrorMsg[170] = "DD|1|170|操作范围内有电，不能挂地线！";
		databaseErrorMsg[171] = "DD|0|171|先关门再拆地线！";		

		FUNCTION_EXIT;
	}

	WfErrorManager::~WfErrorManager()
	{
		FUNCTION_ENTRY("~WfErrorManager()");
		FUNCTION_EXIT;
	}
	
	std::string WfErrorManager::getErrorMsg(unsigned long errorCode)
	{
		std::map<unsigned long, std::string>::iterator itErrorMsg;
		itErrorMsg = databaseErrorMsg.find(errorCode);
		
		if (itErrorMsg != databaseErrorMsg.end())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "check errorCode [%d] [%s] ",errorCode, itErrorMsg->second.c_str());
			return itErrorMsg->second;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "errorCode %d is invalid", errorCode );
		
		std::stringstream ss;
		ss <<  "无效错误码" << errorCode;
		return ss.str();
	}
}


