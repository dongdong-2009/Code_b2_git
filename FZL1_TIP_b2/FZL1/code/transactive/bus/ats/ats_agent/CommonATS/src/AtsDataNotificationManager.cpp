/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\AtsDataNotificationManager.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for receiving Data notification and process the data based on the event receive.
  */

#include "bus/ats/ats_agent/CommonATS/src/AtsDataNotificationManager.h"
#include "core/utilities/src/DebugUtil.h"
#include <boost/pointer_cast.hpp>
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"

namespace TA_IRS_Bus
{
	AtsDataNotificationManager::AtsDataNotificationManager(TA_IRS_Bus::IDataUpdate* dataUpdate)
		: m_dataUpdate(dataUpdate)
	{
		FUNCTION_ENTRY("AtsDataNotificationManager");

		//Start queue thread
		start();

		FUNCTION_EXIT;
	}

	AtsDataNotificationManager::~AtsDataNotificationManager()
	{
		FUNCTION_ENTRY("~AtsDataNotificationManager");

		terminateAndWait();

		FUNCTION_EXIT;
	}

	void AtsDataNotificationManager::notifyAtsDataEvent(TA_IRS_Bus::DataNotificationPtr event)
	{
		FUNCTION_ENTRY("notifyAtsDataEvent");

		//Insert Ats Data in a queue for data processing.
		insert(event);
	
		FUNCTION_EXIT;
	}

	void AtsDataNotificationManager::processEvent(TA_IRS_Bus::DataNotificationPtr event)
	{
		FUNCTION_ENTRY("processEvent");

		unsigned long queuSize = getQueueSize();
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Queue Size : %d", queuSize);

		m_dataUpdate->processDataNotification(event);

		FUNCTION_EXIT;
	}
}