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

#include "bus/ats/ats_agent/CommonATS/src/PlatformDoorDataProcessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/pointer_cast.hpp"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"

namespace TA_IRS_Bus
{
	PlatformDoorDataProcessor::PlatformDoorDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: DataProcessorThread(EntityData)
	{
		FUNCTION_ENTRY("PlatformDoorDataProcessor");

		start();

		FUNCTION_EXIT;
	}

	PlatformDoorDataProcessor::~PlatformDoorDataProcessor()
	{
		FUNCTION_ENTRY("~PlatformDoorDataProcessor");
		FUNCTION_EXIT;
	}

	void PlatformDoorDataProcessor::processAtsData(DataNotificationPtr atsDataNotif)
	{
		FUNCTION_ENTRY("processAtsData");

		if (NULL != boost::dynamic_pointer_cast<PDDataNotification>(atsDataNotif))
		{

			addItem(atsDataNotif,true);
		}

		FUNCTION_EXIT;
	}

	void PlatformDoorDataProcessor::processItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processItem");

		TA_ASSERT(item.get() ,"item is NULL");

		try
		{
			//This will process PlatformDoor Data Item from the Queue.
			processPlatformDoorDataItem(item);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Exception occur in Platform Data Processing.");
		}

		FUNCTION_EXIT;
	}

	void PlatformDoorDataProcessor::processPlatformDoorData(PDDataNotificationPtr platformDoorDataPtr)
	{
		FUNCTION_ENTRY("processPlatformDoorData");

		platformDoorDataPtr->updateAtsData();

		FUNCTION_EXIT;
	}

	void PlatformDoorDataProcessor::processPlatformDoorDataItem(DataNotificationPtr item)
	{
		FUNCTION_ENTRY("processPlatformDoorDataItem");

		PDDataNotificationPtr
			platformDoorDataNotif(boost::dynamic_pointer_cast<PDDataNotification>(item));

		//Process the Data if the Data Block Quality is GOOD
		if ( AtsHelper::isGoodDatapointQuality(item->getDataQuality()) )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Process PlatformDoor Data..");
			//Process PlatformDoor Data
			processPlatformDoorData(platformDoorDataNotif);
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Set All PlatformDoor Datapoint Quality to Bad");
			m_previousDataQuality=item->getDataQuality();
			platformDoorDataNotif->updatePDDatapointQuality(item->getDataQuality());
		}

		FUNCTION_EXIT;
	}
}