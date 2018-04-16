
#include "bus/ats/ats_agent/CommonATS/src/SwitchDataProcessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/pointer_cast.hpp"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"

namespace TA_IRS_Bus
{
	SwitchDataProcessor::SwitchDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: DataProcessorThread(EntityData)
	{
		FUNCTION_ENTRY("SwitchDataProcessor");

		start();

		FUNCTION_EXIT;
	}

	SwitchDataProcessor::~SwitchDataProcessor()
	{
		FUNCTION_ENTRY("~SwitchDataProcessor");
		FUNCTION_EXIT;
	}

	void SwitchDataProcessor::processAtsData(DataNotificationPtr atsDataNotif)
	{
		FUNCTION_ENTRY("processAtsData");

		if (NULL != boost::dynamic_pointer_cast<SwitchDataNotification>(atsDataNotif))
		{
			addItem(atsDataNotif,true);
		}
		
		FUNCTION_EXIT;
	}

	void SwitchDataProcessor::processItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processItem");

		TA_ASSERT(item.get() ,"item is NULL");

		try
		{
			//This will process the Switch Data Item from the Queue.
			processSwitchDataItem(item);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Exception occur in Switch Data Processing.");
		}

		FUNCTION_EXIT;
	}

	void SwitchDataProcessor::processSwitchData(SwitchDataNotificationPtr switchDataPtr)
	{
		FUNCTION_ENTRY("processSwitchData");

		switchDataPtr->updateAtsData();
		switchDataPtr->processAtsData();

		FUNCTION_EXIT;
	}

	void SwitchDataProcessor::processSwitchDataItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processSwitchDataItem");

		SwitchDataNotificationPtr
			switchDataNotif(boost::dynamic_pointer_cast<SwitchDataNotification>(item));

		//If Datapoint quality is GOOD then Train data will be processed
		// and update its Datapoint values.
		if ( AtsHelper::isGoodDatapointQuality(item->getDataQuality()) )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Process Switch Data..");	
			//Process Signal Data
			processSwitchData(switchDataNotif);
		}
		//For BAD Datapoint Quality
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Set All Switch Datapoint Quality to Bad");
			m_previousDataQuality=item->getDataQuality();
			switchDataNotif->updateSwitchDatapointQuality(item->getDataQuality());
		}

		FUNCTION_EXIT;
	}


}