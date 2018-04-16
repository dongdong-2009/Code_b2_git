/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\TrainDataProcessor.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling Train data processing
  */

#include "bus/ats/ats_agent/CommonATS/src/TrainDataProcessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainDataNotification.h"
#include "boost/pointer_cast.hpp"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"

namespace TA_IRS_Bus
{
	TrainDataProcessor::TrainDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: DataProcessorThread(EntityData)
	{
		FUNCTION_ENTRY("TrainDataProcessor");
        m_previousDataQuality=TA_Base_Core::ITA_QUALITY_GOOD;
		start();

		FUNCTION_EXIT;
	}

	TrainDataProcessor::~TrainDataProcessor()
	{
		FUNCTION_ENTRY("~TrainDataProcessor");
		FUNCTION_EXIT;
	}

	void TrainDataProcessor::processAtsData(DataNotificationPtr atsDataNotif)
	{
		FUNCTION_ENTRY("receiveAtsData");

		if (NULL != boost::dynamic_pointer_cast<TrainDataNotification>(atsDataNotif))
		{
			addItem(atsDataNotif,true);
		}

		FUNCTION_EXIT;
	}

	void TrainDataProcessor::processItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processItem");
		TA_ASSERT(item.get() ,"item is NULL");

		try
		{	 
			//This will process the Train Data Item from the Queue and update its Datapoint
			processTrainDataItem(item);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Exception occur in Train Data Processing.");
		}

		FUNCTION_EXIT;
	}

	void TrainDataProcessor::processTrainData(TrainDataNotificationPtr paDataPtr)
	{
		FUNCTION_ENTRY("processPAData");

		paDataPtr->updateAtsData();
		paDataPtr->processAtsData();

		FUNCTION_EXIT;
	}

	void TrainDataProcessor::processTrainDataItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processTrainDataItem");

		TrainDataNotificationPtr
			trainDataNotif(boost::dynamic_pointer_cast<TrainDataNotification>(item));

		//If Datapoint quality is GOOD then Train data will be processed
		// and update its Datapoint values.
		if ( AtsHelper::isGoodDatapointQuality(item->getDataQuality()) )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Process Train Data..");
			//Process Train Data
			processTrainData(trainDataNotif);
		}
		//For BAD Datapoint Quality, just to update DP quality
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Set All Train Datapoint Quality to Bad");
			m_previousDataQuality=item->getDataQuality();
			trainDataNotif->updateTrainDatapointQuality(item->getDataQuality());
		}

		FUNCTION_EXIT;
	}
}