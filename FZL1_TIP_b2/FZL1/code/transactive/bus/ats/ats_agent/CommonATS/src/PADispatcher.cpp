
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\PADispatcher.cpp $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This is object is responsible for handling PA Dispatching.
  */

#include "bus/ats/ats_agent/CommonATS/src/PADispatcher.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/ats/ats_agent/CommonATS/src/AgentOperationMode.h"

namespace TA_IRS_Bus
{
	PADispatcher::PADispatcher()
	{
		FUNCTION_ENTRY("PADispatcher");
		FUNCTION_EXIT;
	}

	PADispatcher::~PADispatcher()
	{
		FUNCTION_ENTRY("~PADispatcher");
		FUNCTION_EXIT;
	}

	void PADispatcher::sendDataUpdate()
	{
		FUNCTION_ENTRY("processDataUpdate");

		if (false == AgentOperationMode::getInstance().isInControlMode())
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Agent is not running in Control mode.");
			return;
		}
		PasDataMap& pasDataMap = DataCache::getInstance()->getPasDataMap();
		PasDataMapIt pasIt = pasDataMap.begin();
		for (pasIt; pasIt != pasDataMap.end(); pasIt++)
		{
			if ((*pasIt).second != NULL)
			{
				TA_Base_Core::ThreadGuard guard((*(*pasIt).second));
				try
				{
					//Send PA updates to PA Agent
					(*pasIt).second->processUpdates();
				}
				catch (...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
						"run() - Exception caught while processing PA updates");
				}
			}
		}

		FUNCTION_EXIT;
	}

}