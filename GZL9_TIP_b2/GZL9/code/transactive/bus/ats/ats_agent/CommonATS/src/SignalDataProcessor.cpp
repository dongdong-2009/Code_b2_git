
#include "bus/ats/ats_agent/CommonATS/src/SignalDataProcessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/SignalDataNotification.h"
#include "boost/pointer_cast.hpp"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"

namespace TA_IRS_Bus
{
	SignalDataProcessor::SignalDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: DataProcessorThread(EntityData)
	{
		FUNCTION_ENTRY("SignalDataProcessor");

		start();

		FUNCTION_EXIT;
	}

	SignalDataProcessor::~SignalDataProcessor()
	{
		FUNCTION_ENTRY("~SignalDataProcessor");
		FUNCTION_EXIT;
	}

	void SignalDataProcessor::processAtsData(DataNotificationPtr atsDataNotif)
	{
		FUNCTION_ENTRY("receiveAtsData");

		if (NULL != boost::dynamic_pointer_cast<SignalDataNotification>(atsDataNotif))
		{
			addItem(atsDataNotif,true);
		}
		
		FUNCTION_EXIT;
	}

	void SignalDataProcessor::processItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processItem");

		TA_ASSERT(item.get() ,"item is NULL");

		try
		{
			//This will process the Signal Data Item from the Queue.
			processSIGDataItem(item);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Exception occur in Signal Data Processing.");
		}

		FUNCTION_EXIT;
	}

	void SignalDataProcessor::processSIGData(SignalDataNotificationPtr sigDataPtr)
	{
		FUNCTION_ENTRY("processPAData");

		sigDataPtr->updateAtsData();
		sigDataPtr->processAtsData();

		FUNCTION_EXIT;
	}

	void SignalDataProcessor::processSIGDataItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processPADataItem");

		SignalDataNotificationPtr
			sigDataNotif(boost::dynamic_pointer_cast<SignalDataNotification>(item));

		//If Datapoint quality is GOOD then Signal data will be processed
		// and update its Datapoint values.
		if ( AtsHelper::isGoodDatapointQuality(item->getDataQuality()) )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Process Signal Data..");
			//Process Signal Data
			processSIGData(sigDataNotif);
		}
		//For BAD Datapoint Quality
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Set All Signal Datapoint Quality to Bad");
			m_previousDataQuality=item->getDataQuality();
			sigDataNotif->updateSignalDatapointQuality(item->getDataQuality());
		}

		FUNCTION_EXIT;
	}


}