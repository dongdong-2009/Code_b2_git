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

#include "bus/ats/ats_agent/CommonATS/src/TrainPISDataNotification.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"

namespace TA_IRS_Bus
{
	TrainPISDataNotification::TrainPISDataNotification(void* paData)
		: DataNotification(paData)
	{
		FUNCTION_ENTRY("TrainPISDataNotification");

		PISInfoVec* pisInfoVec = static_cast<PISInfoVec*>(getData());
		PISInfoVecIt iterPis = pisInfoVec->begin();
		for (iterPis; iterPis != pisInfoVec->end(); iterPis++)
		{
			PISInfo trainPisInfo = (*iterPis);
			m_pisInfoData.push_back(trainPisInfo);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"PlatformID :%d, 1st Train Arrival Time : %lu",
				trainPisInfo.PlatformId, trainPisInfo.Train1st_ArrivalTime);
				
		}

		FUNCTION_EXIT;
	}

	TrainPISDataNotification::~TrainPISDataNotification()
	{
		FUNCTION_ENTRY("TrainPISDataNotification");
		FUNCTION_EXIT;
	}

	PISInfoVec& TrainPISDataNotification::getTrainPISInfo()
	{
		FUNCTION_ENTRY("getTrainPISInfo");
		FUNCTION_EXIT;
		return m_pisInfoData;
	}

	void TrainPISDataNotification::updateAtsData()
	{
		FUNCTION_ENTRY("updateAtsData");

		//Update PIS Data Cache
		DataCache::getInstance()->updateTrainPISCache(getTrainPISInfo());

		FUNCTION_EXIT;
	}
}