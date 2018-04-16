/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\TrainDataNotification.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling Train Data Information which will be used for data processing.
  */

#include "bus/ats/ats_agent/CommonATS/src/TrainDataNotification.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include "boost/algorithm/string.hpp"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"

namespace TA_IRS_Bus
{
	TrainDataNotification::TrainDataNotification(void* trainData)
		: DataNotification(trainData)
	{
		FUNCTION_ENTRY("TrainDataNotification");

		TrainInfoVec* trainDataVec = static_cast<TrainInfoVec*>(getData());
		TrainInfoVecIt iterTrain = trainDataVec->begin();
		for (iterTrain; iterTrain != trainDataVec->end(); iterTrain++)
		{
			TrainInfo trainInfo = (*iterTrain);
			m_trainInfoVec.push_back(trainInfo);
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"Train Congestion : %s, Train Service Number : %s, Train Direction :%s, Train Track Location :%s, last Train Record :%s",
				(trainInfo.TrainCongestion)?"TRUE":"FALSE",trainInfo.TrainServiceNumber.c_str(),
				trainInfo.TrainDirection.c_str(),trainInfo.TrainTrackLocation.c_str(),(trainInfo.lastTrainRecord)?"TRUE":"FASE");

		}
		
		FUNCTION_EXIT;
	}

	TrainDataNotification::~TrainDataNotification()
	{
		FUNCTION_ENTRY("TrainDataNotification");
		FUNCTION_EXIT;
	}

	TrainInfoVec& TrainDataNotification::getTrainInfo()
	{
		FUNCTION_ENTRY("getTrainInfo");
		FUNCTION_EXIT;
		return m_trainInfoVec;
	}

	void TrainDataNotification::updateAtsData()
	{
		FUNCTION_ENTRY("updateAtsData");

		//Update Train Data Cache
		TA_IRS_Bus::DataCache::getInstance()->updateTrainCache(getTrainInfo());

		FUNCTION_EXIT;
	}

	void TrainDataNotification::updateTrainDatapointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateTrainDatapointQuality");

		//Update Train Data Datapoint Quality
		TA_IRS_Bus::DataCache::getInstance()->updateTrainInfoDatapointQuality(quality);

		FUNCTION_EXIT;
	}
}