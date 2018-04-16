
#include "bus/ats/ats_agent/CommonATS/src/BlkDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/BlkDataNotification.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/pointer_cast.hpp"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"

namespace TA_IRS_Bus
{
	BlkDataProcessor::BlkDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: DataProcessorThread(EntityData)
	{
		FUNCTION_ENTRY("BlkDataProcessor");

		start();

		FUNCTION_EXIT;
	}

	BlkDataProcessor::~BlkDataProcessor()
	{
		FUNCTION_ENTRY("~BlkDataProcessor");
		FUNCTION_EXIT;
	}

	void BlkDataProcessor::processAtsData(DataNotificationPtr atsDataNotif)
	{
		FUNCTION_ENTRY("processAtsData");

		if (NULL != boost::dynamic_pointer_cast<BlkDataNotification>(atsDataNotif))
		{
			addItem(atsDataNotif,true);
		}
		
		FUNCTION_EXIT;
	}

	void BlkDataProcessor::processItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processItem");

		TA_ASSERT(item.get() ,"item is NULL");

		try
		{
			//This will process the Block Data Item from the Queue.
			processBlkDataItem(item);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Exception occur in Block Data Processing.");
		}

		FUNCTION_EXIT;
	}

	void BlkDataProcessor::processBlkData(BlkDataNotificationPtr blkDataPtr)
	{
		FUNCTION_ENTRY("processBlkData");

		blkDataPtr->updateAtsData();
		blkDataPtr->processAtsData();

		FUNCTION_EXIT;
	}

	void BlkDataProcessor::processBlkDataItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processBlkDataItem");

		BlkDataNotificationPtr
			blkDataNotif(boost::dynamic_pointer_cast<BlkDataNotification>(item));

		//If Datapoint quality is GOOD then Train data will be processed
		// and update its Datapoint values.
		if ( AtsHelper::isGoodDatapointQuality(item->getDataQuality()) )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Process Block Data..");
			//Process Signal Data
			processBlkData(blkDataNotif);
		}
		//For BAD Datapoint Quality
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Set All Block Datapoint Quality to Bad");
			m_previousDataQuality=item->getDataQuality();
			blkDataNotif->updateBlkDatapointQuality(item->getDataQuality());
		}

		FUNCTION_EXIT;
	}


}