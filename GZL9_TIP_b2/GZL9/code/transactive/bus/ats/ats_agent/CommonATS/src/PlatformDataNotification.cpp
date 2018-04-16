/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\TrainPISDataNotification.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling Train PIS Information which will be used for data processing.
  */

#include "bus/ats/ats_agent/CommonATS/src/PlatformDataNotification.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"

namespace TA_IRS_Bus
{
	PlatformDataNotification::PlatformDataNotification(void* paData)
		: DataNotification(paData)
	{
		FUNCTION_ENTRY("PlatformDataNotification");

		PlatformInfoVec* platformInfoVec = static_cast<PlatformInfoVec*>(getData());
		PlatformInfoVecIt iterPlatform = platformInfoVec->begin();
		for (iterPlatform; iterPlatform != platformInfoVec->end(); iterPlatform++)
		{
			PlatformInfo platformInfo = (*iterPlatform);
			m_platformInfoData.push_back(platformInfo);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"PlatformID :%s, hold status %d, skip status %d",
				platformInfo.PlatformId.c_str(),platformInfo.hold, platformInfo.Skip);		
		}

		FUNCTION_EXIT;
	}

	PlatformDataNotification::~PlatformDataNotification()
	{
		FUNCTION_ENTRY("PlatformDataNotification");
		FUNCTION_EXIT;
	}

	PlatformInfoVec& PlatformDataNotification::getTrainPlatformInfo()
	{
		FUNCTION_ENTRY("getTrainPlatformInfo");
		FUNCTION_EXIT;
		return m_platformInfoData;
	}

	void PlatformDataNotification::updateAtsData()
	{
		FUNCTION_ENTRY("updateAtsData");

		//Update PIS Data Cache
		DataCache::getInstance()->updatePlatformCache(getTrainPlatformInfo());

		FUNCTION_EXIT;
	}

	void PlatformDataNotification::updatePlatformDatapointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updatePlatformDatapointQuality");

		//Update Platform Data Datapoint Quality
		TA_IRS_Bus::DataCache::getInstance()->updatePlatformInfoDatapointQuality(quality);

		FUNCTION_EXIT;
	}
	
}