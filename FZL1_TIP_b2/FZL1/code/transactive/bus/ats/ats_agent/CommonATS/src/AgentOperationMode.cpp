
#include "bus/ats/ats_agent/CommonATS/src/AgentOperationMode.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_IRS_Bus
{			   
	TA_Base_Core::ReEntrantThreadLockable AgentOperationMode::m_singletonLock;
	AgentOperationMode* AgentOperationMode::m_theClassInstance = NULL;

	AgentOperationMode::AgentOperationMode()
		: m_isInControlMode(false)
	{
		FUNCTION_ENTRY("AgentOperationMode");
		FUNCTION_EXIT;
	}
	AgentOperationMode& AgentOperationMode::getInstance()
	{
		TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( NULL == m_theClassInstance )
		{
			// Create the one & only object.

			m_theClassInstance = new AgentOperationMode();
		}

		return *m_theClassInstance;
	}

	bool AgentOperationMode::isInControlMode()
	{
		TA_Base_Core::ThreadGuard guard( m_modeLock );
		return m_isInControlMode;
	}

	void AgentOperationMode::setToMonitorMode()
	{
		TA_Base_Core::ThreadGuard guard( m_modeLock );
		m_isInControlMode = false;
	}

	void AgentOperationMode::setToControlMode()
	{
		TA_Base_Core::ThreadGuard guard( m_modeLock );
		m_isInControlMode = true;
	}
}