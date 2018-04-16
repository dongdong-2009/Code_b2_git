/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\PIDSDispatcher.cpp $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This is object is responsible for handling PIDS Dispatching.
  */

#include "bus/ats/ats_agent/CommonATS/src/PIDSDispatcher.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/PIDSAgentInterface.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/AgentOperationMode.h"

namespace TA_IRS_Bus
{
	PIDSDispatcher::PIDSDispatcher()
		: m_pidsInterface(NULL)
	{
		FUNCTION_ENTRY("PIDSDispatcher");

		initialize();

		FUNCTION_EXIT;
	}

	PIDSDispatcher::~PIDSDispatcher()
	{
		FUNCTION_ENTRY("~PIDSDispatcher");

		deInitialize();

		FUNCTION_EXIT;
	}

	void PIDSDispatcher::sendDataUpdate()
	{
		FUNCTION_ENTRY("sendDataUpdate");

		if (false == AgentOperationMode::getInstance().isInControlMode())
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Agent is not running in Control mode.");
			return;
		}
			
		m_pidsInterface->processPIDSInformation(DataCache::getInstance()->getPIDSData());

		FUNCTION_EXIT;
	}

	void PIDSDispatcher::initialize()
	{
		FUNCTION_ENTRY("initialize");

		m_pidsInterface = new PIDSAgentInterface();

		FUNCTION_EXIT;
	}

	void PIDSDispatcher::deInitialize()
	{
		FUNCTION_ENTRY("deInitialize");

		if (NULL != m_pidsInterface)
		{
			delete m_pidsInterface;
			m_pidsInterface = NULL;
		}

		FUNCTION_EXIT;
	}
}