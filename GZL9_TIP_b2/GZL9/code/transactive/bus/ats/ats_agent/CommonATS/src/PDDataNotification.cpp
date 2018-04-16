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

#include "bus/ats/ats_agent/CommonATS/src/PDDataNotification.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"

namespace TA_IRS_Bus
{
	PDDataNotification::PDDataNotification(void* paData)
		: DataNotification(paData)
	{
		FUNCTION_ENTRY("PlatformDoorDataNotification");

		PlatformDoorInfoVec* platformDoorInfoVec = static_cast<PlatformDoorInfoVec*>(getData());
		PlatformDoorInfoVecIt itrPlatformDoor = platformDoorInfoVec->begin();
		for (itrPlatformDoor; itrPlatformDoor != platformDoorInfoVec->end(); itrPlatformDoor++)
		{
			PlatformDoorInfo platformDoorInfo = (*itrPlatformDoor);
			m_platformDoorInfoData.push_back(platformDoorInfo);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"PlatformDoorID :%s, Override Status : %d, Need to be updated %d",
				platformDoorInfo.PlatformDoorId.c_str(), platformDoorInfo.OverrideStatus, platformDoorInfo.isNeedUpdateOverride);		
		}

		FUNCTION_EXIT;
	}

	PDDataNotification::~PDDataNotification()
	{
		FUNCTION_ENTRY("PlatformDoorDataNotification");
		FUNCTION_EXIT;
	}

	PlatformDoorInfoVec& PDDataNotification::getTrainPlatformDoorInfo()
	{
		FUNCTION_ENTRY("getTrainPlatformDoorInfo");
		FUNCTION_EXIT;
		return m_platformDoorInfoData;
	}

	void PDDataNotification::updateAtsData()
	{
		FUNCTION_ENTRY("updateAtsData");

		//Update PIS Data Cache
		DataCache::getInstance()->updatePlatformDoorCache(getTrainPlatformDoorInfo());

		FUNCTION_EXIT;
	}

	void PDDataNotification::updatePDDatapointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updatePlatformDoorDatapointQuality");

		//Update PlatformDoor Data Datapoint Quality
		TA_IRS_Bus::DataCache::getInstance()->updatePlatformDoorDatapointQuality(quality);

		FUNCTION_EXIT;
	}
	
}