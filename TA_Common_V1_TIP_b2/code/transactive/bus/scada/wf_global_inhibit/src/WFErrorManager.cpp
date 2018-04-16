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
		databaseErrorMsg[1]  = "|1|��վWFͨ���жϣ�У��ʧ�ܣ�";
		databaseErrorMsg[10] = "KG|0|10|�������൶բ״̬��һ�£����ܲ�����";
		databaseErrorMsg[11] = "KG|0|11|���ܴ����������أ�";
		databaseErrorMsg[12] = "KG|0|12|��Ͳ࿪���ں�λ�����ֱܷ�߿��أ�";
		databaseErrorMsg[13] = "KG|0|13|���Ե�δ�ӵأ����ֿܷ��أ�";
		databaseErrorMsg[14] = "KG|0|14|վ���������Ե�δ�ӵأ����ֿܷ��أ�";
		databaseErrorMsg[15] = "KG|0|15|�ò������¸���ʧ�磡";
		databaseErrorMsg[16] = "KG|0|16|�ò������½⻷/���У�";
		databaseErrorMsg[17] = "KG|0|17|�ò�������ĸ��ʧѹ��";
		
		databaseErrorMsg[30] = "KG|1|30|������Χ���нӵأ����ܲ�����";
		databaseErrorMsg[31] = "KG|1|31|�������൶բ״̬��һ�£����ܲ�����";
		databaseErrorMsg[32] = "KG|1|32|������Χ���нӵأ����ܺϿ��أ�";
		databaseErrorMsg[33] = "KG|1|33|��߿����ڷ�λ�����ܺϱ�Ϳ��أ�";
		databaseErrorMsg[34] = "KG|1|34|���Ե�δ�ӵأ����ܺϿ��أ�";
		databaseErrorMsg[35] = "KG|1|35|�ò������ºϻ�/���У�";

		databaseErrorMsg[50] = "DZ|0|50|���ػ��ߵ��ں�λ�����ܷ�ĸ����";
		databaseErrorMsg[51] = "DZ|0|51|���ػ��ߵ��ں�λ�����ܷ�ĸ����";
		databaseErrorMsg[52] = "DZ|0|52|վ���������Ե�δ�ӵأ���������բ��";
		databaseErrorMsg[53] = "DZ|0|53|�ò������¸��ɻ򱣵��û�ʧ�磡";

		databaseErrorMsg[70] = "DZ|1|70|������Χ���нӵأ����ܺϵ�բ��";
		databaseErrorMsg[71] = "DZ|1|71|������Χ���е��ߣ����ܺϵ�բ��";
		databaseErrorMsg[72] = "DZ|1|72|�����ں�λ��ĸ���ڷ�λ�����ܺ��ߵ���";
		databaseErrorMsg[73] = "DZ|1|73|�����ں�λ���ߵ��ڷ�λ�����ܺ�ĸ����";
		databaseErrorMsg[74] = "DZ|1|74|��ĸ�ڴ�·״̬�����ܺ��Ե���";
		databaseErrorMsg[75] = "DZ|1|75|���ܴ����ɺϵ�բ��";
		
		databaseErrorMsg[90] = "XC|0|90|���ػ��ߵ��ں�λ�����ܷ�С����";
		databaseErrorMsg[91] = "XC|0|91|�ò������¸��ɻ򱣵��û�ʧ�磡";
		
		databaseErrorMsg[110] = "XC|2|110|���ػ��ߵ��ں�λ�����ܷ�С����";
		databaseErrorMsg[111] = "XC|2|111|�ò������¸��ɻ򱣵��û�ʧ�磡";

		databaseErrorMsg[130] = "XC|1|130|�ò�����Χ���нӵأ����ܺ�С����";
		databaseErrorMsg[131] = "XC|1|131|�����ں�λ��ĸ���ڷ�λ�����ܺ�С����";
		databaseErrorMsg[132] = "XC|1|132|���ܴ����ɺ�С����";

		databaseErrorMsg[150] = "DD|0|150|�м����ƣ����ܲ���ߣ�";
		databaseErrorMsg[151] = "DD|0|151|վ���������Ե�δ�ӵأ����ܲ���ߣ�";

		databaseErrorMsg[170] = "DD|1|170|������Χ���е磬���ܹҵ��ߣ�";
		databaseErrorMsg[171] = "DD|0|171|�ȹ����ٲ���ߣ�";		

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
		ss <<  "��Ч������" << errorCode;
		return ss.str();
	}
}


