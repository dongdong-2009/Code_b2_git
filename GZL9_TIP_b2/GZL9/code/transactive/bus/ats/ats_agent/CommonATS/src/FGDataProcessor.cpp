
#include "bus/ats/ats_agent/CommonATS/src/FGDataProcessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/pointer_cast.hpp"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"

namespace TA_IRS_Bus
{
	FGDataProcessor::FGDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: DataProcessorThread(EntityData)
	{
		FUNCTION_ENTRY("FGDataProcessor");

		start();

		FUNCTION_EXIT;
	}

	FGDataProcessor::~FGDataProcessor()
	{
		FUNCTION_ENTRY("~FGDataProcessor");
		FUNCTION_EXIT;
	}

	void FGDataProcessor::processAtsData(DataNotificationPtr atsDataNotif)
	{
		FUNCTION_ENTRY("receiveAtsData");

		if (NULL != boost::dynamic_pointer_cast<FGDataNotification>(atsDataNotif))
		{
			addItem(atsDataNotif,true);
		}
		
		FUNCTION_EXIT;
	}

	void FGDataProcessor::processItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processItem");

		TA_ASSERT(item.get() ,"item is NULL");

		try
		{
			//This will process the Flood Gate Data Item from the Queue.
			processFGDataItem(item);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Exception occur in PA Data Processing.");
		}

		FUNCTION_EXIT;
	}

	void FGDataProcessor::processFGData(FGDataNotificationPtr fgDataPtr)
	{
		FUNCTION_ENTRY("processFGData");

		fgDataPtr->updateAtsData();
		fgDataPtr->processAtsData();

		FUNCTION_EXIT;
	}

	void FGDataProcessor::processFGDataItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processPADataItem");

		FGDataNotificationPtr
			fgDataNotif(boost::dynamic_pointer_cast<FGDataNotification>(item));

		//If Datapoint quality is GOOD then Train data will be processed
		// and update its Datapoint values.
		if ( AtsHelper::isGoodDatapointQuality(item->getDataQuality()) )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Process Flood Gate Data..");
			//Process Signal Data
			processFGData(fgDataNotif);
		}
		//For BAD Datapoint Quality
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Set All Flood Gate Datapoint Quality to Bad");
			m_previousDataQuality=item->getDataQuality();
			fgDataNotif->updateFGDatapointQuality(item->getDataQuality());
		}

		FUNCTION_EXIT;
	}


}