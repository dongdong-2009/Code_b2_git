
#include "bus/ats/ats_agent/CommonATS/src/FGDataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	FGDataNotification::FGDataNotification(void* fgData)
		: DataNotification(fgData)
	{
		FUNCTION_ENTRY("FGDataNotification");

		FloodGateInfoVec* fgVec= static_cast<FloodGateInfoVec*>(getData());

		FloodGateInfoVecIt itr = fgVec->begin();

		if(itr != fgVec->end())
		{
			m_floodgateInfoData = (*itr);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"Flood Gate Id %s Permission Close Status:%d, Request Close Status:%d", (m_floodgateInfoData.FloodGateId.c_str()),
				(m_floodgateInfoData.PermissionClose), (m_floodgateInfoData.RequestClose));
		}

		FUNCTION_EXIT;
	}

	FGDataNotification::~FGDataNotification()
	{
		FUNCTION_ENTRY("~FGDataNotification");
		FUNCTION_EXIT;
	}

	FloodGateInfo& FGDataNotification::getFloodGateInfo()
	{
		FUNCTION_ENTRY("getFloodGateInfo");
		FUNCTION_EXIT;
		return m_floodgateInfoData;
	}

	void FGDataNotification::updateAtsData()
	{
		FUNCTION_ENTRY("updateAtsData");

		//Update PA Data Cache
		TA_IRS_Bus::DataCache::getInstance()->updateFloodGateStatus(getFloodGateInfo());

		FUNCTION_EXIT;
	}

	void FGDataNotification::updateFGDatapointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateFGDatapointQuality");

		//Update Train Data Datapoint Quality
		TA_IRS_Bus::DataCache::getInstance()->updateFGInfoDatapointQuality(quality);

		FUNCTION_EXIT;
	}

}