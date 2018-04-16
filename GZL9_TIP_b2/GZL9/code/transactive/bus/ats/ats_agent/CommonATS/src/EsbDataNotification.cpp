
#include "bus/ats/ats_agent/CommonATS/src/EsbDataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	EsbDataNotification::EsbDataNotification(void* esbData)
		: DataNotification(esbData)
	{
		FUNCTION_ENTRY("EsbDataNotification");

		EsbInfoVec* esbVec= static_cast<EsbInfoVec*>(getData());

		EsbInfoVecIt itr = esbVec->begin();

		if(itr != esbVec->end())
		{
			m_esbInfoData = (*itr);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"Emergency Stop Button id %s Status : %d",(m_esbInfoData.EsbId.c_str()),(m_esbInfoData.Status));
		}
		FUNCTION_EXIT;
	}

	EsbDataNotification::~EsbDataNotification()
	{
		FUNCTION_ENTRY("~EsbDataNotification");
		FUNCTION_EXIT;
	}

	EsbInfo& EsbDataNotification::getEsbInfo()
	{
		FUNCTION_ENTRY("getEsbInfo");
		FUNCTION_EXIT;
		return m_esbInfoData;
	}

	void EsbDataNotification::updateAtsData()
	{
		FUNCTION_ENTRY("updateAtsData");

		//Update Emergency Stop Button Data Cache
		TA_IRS_Bus::DataCache::getInstance()->updateEsbStatus(getEsbInfo());

		FUNCTION_EXIT;
	}

	void EsbDataNotification::updateEsbDatapointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateEsbDatapointQuality");

		//Update Emergency Stop Button Data Datapoint Quality
		TA_IRS_Bus::DataCache::getInstance()->updateEsbInfoDatapointQuality(quality);

		FUNCTION_EXIT;
	}

}