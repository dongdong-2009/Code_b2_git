
#include "bus/ats/ats_agent/CommonATS/src/EsbDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/EsbDataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/pointer_cast.hpp"


namespace TA_IRS_Bus
{
	EsbDataProcessor::EsbDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: DataProcessorThread(EntityData)
	{
		FUNCTION_ENTRY("EsbDataProcessor");

		start();

		FUNCTION_EXIT;
	}

	EsbDataProcessor::~EsbDataProcessor()
	{
		FUNCTION_ENTRY("~EsbDataProcessor");
		FUNCTION_EXIT;
	}

	void EsbDataProcessor::processAtsData(DataNotificationPtr atsDataNotif)
	{
		FUNCTION_ENTRY("receiveAtsData");

		if (NULL != boost::dynamic_pointer_cast<EsbDataNotification>(atsDataNotif))
		{
			addItem(atsDataNotif,true);
		}
		
		FUNCTION_EXIT;
	}

	void EsbDataProcessor::processItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processItem");

		TA_ASSERT(item.get() ,"item is NULL");

		try
		{
			//This will process the Emergency Stop Button Data Item from the Queue.
			processEsbDataItem(item);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Exception occur in Emergency Stop Button Data Processing.");
		}

		FUNCTION_EXIT;
	}

	void EsbDataProcessor::processEsbData(EsbDataNotificationPtr esbDataPtr)
	{
		FUNCTION_ENTRY("processEsbData");

		esbDataPtr->updateAtsData();
		esbDataPtr->processAtsData();

		FUNCTION_EXIT;
	}

	void EsbDataProcessor::processEsbDataItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processPADataItem");

		EsbDataNotificationPtr
			esbDataNotif(boost::dynamic_pointer_cast<EsbDataNotification>(item));

		//If Datapoint quality is GOOD then Train data will be processed
		// and update its Datapoint values.
		if ( AtsHelper::isGoodDatapointQuality(item->getDataQuality()) )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Process Train Data..");		
			//Process Signal Data
			processEsbData(esbDataNotif);
		}
		//For BAD Datapoint Quality
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Set All Esb Datapoint Quality to Bad");
			m_previousDataQuality=item->getDataQuality();
			esbDataNotif->updateEsbDatapointQuality(item->getDataQuality());
		}

		FUNCTION_EXIT;
	}


}