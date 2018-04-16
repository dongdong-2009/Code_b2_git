#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManagerActionThread.h"

namespace TA_IRS_App
{
	TunnelModeManagerActionThread::TunnelModeManagerActionThread(ActionThreadCallbackInterface* callback)
		:m_callback(callback)
	{
		FUNCTION_ENTRY("TunnelModeManagerActionThread");
		FUNCTION_EXIT;
	}

	TunnelModeManagerActionThread::~TunnelModeManagerActionThread(void)
	{
		FUNCTION_ENTRY("~TunnelModeManagerActionThread");
		FUNCTION_EXIT;
	}

	void TunnelModeManagerActionThread::setAction(TunnelModeManagerAction action)
	{
		FUNCTION_ENTRY("setAction");
		m_action = action;
		FUNCTION_EXIT;
	}

	void TunnelModeManagerActionThread::run()
	{
		FUNCTION_ENTRY("run");
		if (NULL != m_callback)
		{
			m_callback->ActionThreadFunction(m_action);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_callback is NULL.");
		}
		FUNCTION_EXIT;
	}

	void TunnelModeManagerActionThread::terminate()
	{
		FUNCTION_ENTRY("terminate");
		FUNCTION_EXIT;
	}
}
