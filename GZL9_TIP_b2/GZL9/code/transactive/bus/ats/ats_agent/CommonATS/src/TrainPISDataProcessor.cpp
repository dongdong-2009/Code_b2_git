/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\TrainPISDataProcessor.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling Train PIS data processing
  */

#include "bus/ats/ats_agent/CommonATS/src/TrainPISDataProcessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/pointer_cast.hpp"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"

namespace TA_IRS_Bus
{
	TrainPISDataProcessor::TrainPISDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: DataProcessorThread(EntityData)
	{
		FUNCTION_ENTRY("TrainPISDataProcessor");

		start();

		FUNCTION_EXIT;
	}

	TrainPISDataProcessor::~TrainPISDataProcessor()
	{
		FUNCTION_ENTRY("~TrainPISDataProcessor");
		FUNCTION_EXIT;
	}

	void TrainPISDataProcessor::processAtsData(DataNotificationPtr atsDataNotif)
	{
		FUNCTION_ENTRY("receiveAtsData");

		if (NULL != boost::dynamic_pointer_cast<TrainPISDataNotification>(atsDataNotif))
		{

			addItem(atsDataNotif,true);
		}

		FUNCTION_EXIT;
	}

	void TrainPISDataProcessor::processItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processItem");

		TA_ASSERT(item.get() ,"item is NULL");

		try
		{
			//This will process the Train PIS Data Item from the Queue.
			processTrainPISDataItem(item);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Exception occur in PA Data Processing.");
		}

		FUNCTION_EXIT;
	}

	void TrainPISDataProcessor::processTrainPISData(TrainPISDataNotificationPtr pisDataPtr)
	{
		FUNCTION_ENTRY("processPAData");

		pisDataPtr->updateAtsData();

		FUNCTION_EXIT;
	}

	void TrainPISDataProcessor::processTrainPISDataItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processTrainPISItem");

		TrainPISDataNotificationPtr
			trainPisDataNotif(boost::dynamic_pointer_cast<TrainPISDataNotification>(item));

		//Process the Data if the Data Block Quality is GOOD
		if ( AtsHelper::isGoodDatapointQuality(item->getDataQuality()) )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Process TrainPIS Data..");
			//Process Train PIS Data
			processTrainPISData(trainPisDataNotif);
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Set All TrainPIS Datapoint Quality to Bad");
			m_previousDataQuality=item->getDataQuality();
			trainPisDataNotif->updatePISDatapointQuality(item->getDataQuality());
		}

		FUNCTION_EXIT;
	}
}