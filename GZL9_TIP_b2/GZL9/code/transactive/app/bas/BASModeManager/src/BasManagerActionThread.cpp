#include "StdAfx.h"
#include "resource.h"
#include "app/bas/BASModeManager/src/BasManagerActionThread.h"
#include "app/bas/BASModeManager/src/BASManagerDlg.h"

namespace TA_IRS_App
{
	BasManagerActionThread::BasManagerActionThread(IActionThreadFunctionInterface* callback)
	:m_callBackDlg(callback)
	{
		FUNCTION_ENTRY("BasManagerActionThread");
		FUNCTION_EXIT;
	}

	BasManagerActionThread::~BasManagerActionThread(void)
	{
		FUNCTION_ENTRY("~BasManagerActionThread");
		FUNCTION_EXIT;
	}

	void BasManagerActionThread::setAction(BasManagerAction action)
	{
		FUNCTION_ENTRY("setAction");
		m_action = action;
		FUNCTION_EXIT;
	}
	void BasManagerActionThread::run()
	{
		FUNCTION_ENTRY("run");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BasManagerActionThread action %d", static_cast<int>(m_action));
		if (NULL != m_callBackDlg)
		{
			m_callBackDlg->executeThreadFunction(m_action);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_callBackDlg pointer is NULL.");
		}

		FUNCTION_EXIT;
	}

	void BasManagerActionThread::terminate()
	{
		FUNCTION_ENTRY("terminate");
		FUNCTION_EXIT;
	}
}

