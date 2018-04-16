/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\PADataProcessor.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling PA data processing
  */

#include "bus/ats/ats_agent/CommonATS/src/PADataProcessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/PADataNotification.h"
#include "boost/pointer_cast.hpp"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"

namespace TA_IRS_Bus
{
	PADataProcessor::PADataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: DataProcessorThread(EntityData)
	{
		FUNCTION_ENTRY("PADataProcessor");

		start();

		FUNCTION_EXIT;
	}

	PADataProcessor::~PADataProcessor()
	{
		FUNCTION_ENTRY("~PADataProcessor");
		FUNCTION_EXIT;
	}

	void PADataProcessor::processAtsData(DataNotificationPtr atsDataNotif)
	{
		FUNCTION_ENTRY("receiveAtsData");

		if (NULL != boost::dynamic_pointer_cast<PADataNotification>(atsDataNotif))
		{
			addItem(atsDataNotif,true);
		}
		
		FUNCTION_EXIT;
	}

	void PADataProcessor::processItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processItem");

		TA_ASSERT(item.get() ,"item is NULL");

		try
		{
			//This will process the PA Data Item from the Queue.
			processPADataItem(item);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Exception occur in PA Data Processing.");
		}

		FUNCTION_EXIT;
	}

	void PADataProcessor::processPAData(PADataNotificationPtr paDataPtr)
	{
		FUNCTION_ENTRY("processPAData");

		paDataPtr->updateAtsData();
		paDataPtr->processAtsData();

		FUNCTION_EXIT;
	}

	void PADataProcessor::processPADataItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processPADataItem");

		//Process the Data if the Data Block Quality is GOOD
		if ( AtsHelper::isGoodDatapointQuality(item->getDataQuality()))
		{
			PADataNotificationPtr
				paDataNotif(boost::dynamic_pointer_cast<PADataNotification>(item));

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Process PA Data..");

			//Process PA Data
			processPAData(paDataNotif);
		}

		FUNCTION_EXIT;
	}
}