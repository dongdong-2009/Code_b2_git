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

#include "bus/ats/ats_agent/CommonATS/src/PlatformDataProcessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/pointer_cast.hpp"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"

namespace TA_IRS_Bus
{
	PlatformDataProcessor::PlatformDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: DataProcessorThread(EntityData)
	{
		FUNCTION_ENTRY("PlatformDataProcessor");

		start();

		FUNCTION_EXIT;
	}

	PlatformDataProcessor::~PlatformDataProcessor()
	{
		FUNCTION_ENTRY("~PlatformDataProcessor");
		FUNCTION_EXIT;
	}

	void PlatformDataProcessor::processAtsData(DataNotificationPtr atsDataNotif)
	{
		FUNCTION_ENTRY("processAtsData");

		if (NULL != boost::dynamic_pointer_cast<PlatformDataNotification>(atsDataNotif))
		{

			addItem(atsDataNotif,true);
		}

		FUNCTION_EXIT;
	}

	void PlatformDataProcessor::processItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processItem");

		TA_ASSERT(item.get() ,"item is NULL");

		try
		{
			//This will process Platform Data Item from the Queue.
			processPlatformDataItem(item);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Exception occur in Platform Data Processing.");
		}

		FUNCTION_EXIT;
	}

	void PlatformDataProcessor::processPlatformData(PlatformDataNotificationPtr platformDataPtr)
	{
		FUNCTION_ENTRY("processPlatformData");

		platformDataPtr->updateAtsData();

		FUNCTION_EXIT;
	}

	void PlatformDataProcessor::processPlatformDataItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processPlatformDataItem");

		PlatformDataNotificationPtr
			platformDataNotif(boost::dynamic_pointer_cast<PlatformDataNotification>(item));

		//Process the Data if the Data Block Quality is GOOD
		if ( AtsHelper::isGoodDatapointQuality(item->getDataQuality()) )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Process Platform Data..");	
			//Process platform Data
			processPlatformData(platformDataNotif);
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Set All Platform Datapoint Quality to Bad");
			m_previousDataQuality=item->getDataQuality();
			platformDataNotif->updatePlatformDatapointQuality(item->getDataQuality());
		}

		FUNCTION_EXIT;
	}
}