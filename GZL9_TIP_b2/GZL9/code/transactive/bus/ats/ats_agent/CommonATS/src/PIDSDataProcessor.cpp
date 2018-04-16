/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\PIDSDataProcessor.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling PIDS data processing
  */
#include "bus/ats/ats_agent/CommonATS/src/PIDSDataProcessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/PIDSDataNotification.h"
#include "boost/pointer_cast.hpp"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"

namespace TA_IRS_Bus
{
	PIDSDataProcessor::PIDSDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: DataProcessorThread(EntityData)
	{
		FUNCTION_ENTRY("PIDSDataProcessor");

		start();

		FUNCTION_EXIT;
	}

	PIDSDataProcessor::~PIDSDataProcessor()
	{
		FUNCTION_ENTRY("~PIDSDataProcessor");
		FUNCTION_EXIT;
	}

	void PIDSDataProcessor::processAtsData(DataNotificationPtr atsDataNotif)
	{
		FUNCTION_ENTRY("receiveAtsData");

		if (NULL != boost::dynamic_pointer_cast<PIDSDataNotification>(atsDataNotif))
		{
            //only need to process the latest one			
			addItem(atsDataNotif,true);
		}

		FUNCTION_EXIT;
	}

	void PIDSDataProcessor::processItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processItem");

		TA_ASSERT(item.get() ,"item is NULL");

		try
		{
			//This will process the PIDS Data Item from the Queue.
			processPIDSDataItem(item);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Exception occur in PIDS Data Processing.");
		}

		FUNCTION_EXIT;
	}

	void PIDSDataProcessor::processPIDSData(PIDSDataNotificationPtr paDataPtr)
	{
		FUNCTION_ENTRY("processPAData");

		paDataPtr->updateAtsData();
		paDataPtr->processAtsData();

		FUNCTION_EXIT;
	}

	void PIDSDataProcessor::processPIDSDataItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processPIDSDataItem");

		//Process the Data if the Data Block Quality is GOOD
		if ( AtsHelper::isGoodDatapointQuality(item->getDataQuality()) )
		{
			PIDSDataNotificationPtr
				pidsDataNotif(boost::dynamic_pointer_cast<PIDSDataNotification>(item));

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Process PIDS Data..");

			//Process PIDS Data
			processPIDSData(pidsDataNotif);
		}

		FUNCTION_EXIT;
	}
}