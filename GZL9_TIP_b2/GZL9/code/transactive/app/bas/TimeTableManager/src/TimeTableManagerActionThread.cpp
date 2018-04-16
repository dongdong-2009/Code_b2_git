#include "StdAfx.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerActionThread.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
	TimeTableManagerActionThread::TimeTableManagerActionThread(ActionThreadCallbackInterface* callback)
		:m_callback(callback)
	{
		FUNCTION_ENTRY("TimeTableManagerActionThread");
		FUNCTION_EXIT;
	}

	TimeTableManagerActionThread::~TimeTableManagerActionThread(void)
	{
		FUNCTION_ENTRY("~TimeTableManagerActionThread");
		FUNCTION_EXIT;
	}

	void TimeTableManagerActionThread::setAction(TimeTableManagerAction action)
	{
		FUNCTION_ENTRY("setAction");
		m_action = action;
		FUNCTION_EXIT;
	}

	void TimeTableManagerActionThread::run()
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

	void TimeTableManagerActionThread::terminate()
	{
        FUNCTION_ENTRY("terminate");
		if (NULL != m_callback)
		{
			m_callback->CancelActionThreadFunction(m_action);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_callback is NULL.");
		}
		FUNCTION_EXIT;
	}
}
