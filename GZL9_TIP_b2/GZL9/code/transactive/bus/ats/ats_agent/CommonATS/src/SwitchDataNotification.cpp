
#include "bus/ats/ats_agent/CommonATS/src/SwitchDataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	SwitchDataNotification::SwitchDataNotification(void* switchData)
		: DataNotification(switchData)
	{
		FUNCTION_ENTRY("SwitchDataNotification");

		SwtichInfoVec* swVec= static_cast<SwtichInfoVec*>(getData());

		SwtichInfoVecIt itr = swVec->begin();

		if(itr != swVec->end())
		{
			m_switchInfoData = (*itr);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"Switch data notification id %s Position :%d, Switch Failed Status :%d, Switch Res Status :%d",
				(m_switchInfoData.SwitchId.c_str()),(m_switchInfoData.Position),
				(m_switchInfoData.FailedStatus),(m_switchInfoData.ResStatus));

		}	
		FUNCTION_EXIT;
	}

	SwitchDataNotification::~SwitchDataNotification()
	{
		FUNCTION_ENTRY("~SwitchDataNotification");
		FUNCTION_EXIT;
	}

	SwtichInfo& SwitchDataNotification::getSwitchInfo()
	{
		FUNCTION_ENTRY("getSignalInfo");
		
		return m_switchInfoData;

		FUNCTION_EXIT;
	}

	void SwitchDataNotification::updateAtsData()
	{
		FUNCTION_ENTRY("updateAtsData");

		//Update PA Data Cache
		TA_IRS_Bus::DataCache::getInstance()->updateSwitchStatus(getSwitchInfo());

		FUNCTION_EXIT;
	}

	void SwitchDataNotification::updateSwitchDatapointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateSwitchDatapointQuality");

		//Update Train Data Datapoint Quality
		TA_IRS_Bus::DataCache::getInstance()->updateSwtichInfoDatapointQuality(quality);

		FUNCTION_EXIT;
	}

}