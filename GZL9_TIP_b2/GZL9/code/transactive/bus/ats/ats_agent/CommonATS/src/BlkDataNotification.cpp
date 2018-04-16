
#include "bus/ats/ats_agent/CommonATS/src/BlkDataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	BlkDataNotification::BlkDataNotification(void* blkData)
		: DataNotification(blkData)
	{
		FUNCTION_ENTRY("BlkDataNotification");

		 
		BlkInfoVec* blkVec= static_cast<BlkInfoVec*>(getData());

		BlkInfoVecIt itr = blkVec->begin();

		if(itr != blkVec->end())
		{
			m_blkInfoData = (*itr);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"Block data notification Id:%s Status:%d",m_blkInfoData.BlockId.c_str(), m_blkInfoData.BlkStatus);
		}
		
		FUNCTION_EXIT;
	}

	BlkDataNotification::~BlkDataNotification()
	{
		FUNCTION_ENTRY("~BlkDataNotification");
		FUNCTION_EXIT;
	}

	BlkInfo& BlkDataNotification::getBlockInfo()
	{
		FUNCTION_ENTRY("getBlockInfo");
		FUNCTION_EXIT;
		return m_blkInfoData;
	}

	void BlkDataNotification::updateAtsData()
	{
		FUNCTION_ENTRY("updateAtsData");

		//Update Block Data Cache
		TA_IRS_Bus::DataCache::getInstance()->updateBlockStatus(getBlockInfo());

		FUNCTION_EXIT;
	}

	void BlkDataNotification::updateBlkDatapointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateBlkDatapointQuality");

		//Update Block Data Datapoint Quality
		TA_IRS_Bus::DataCache::getInstance()->updateBlkInfoDatapointQuality(quality);

		FUNCTION_EXIT;
	}

}