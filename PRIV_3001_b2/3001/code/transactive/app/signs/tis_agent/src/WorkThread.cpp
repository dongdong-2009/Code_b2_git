#include "WorkThread.h"

namespace TA_IRS_App
{
	WorkThread::WorkThread(const ThreadFunc& workFunc)
	    : m_workFunc(workFunc),
		  m_shouldRunning(false)
	{
	}

	void WorkThread::run()
	{
		try
		{
			m_workFunc();
		}
		catch (...)
		{
		}
	}

	void WorkThread::terminate()
	{
		m_shouldRunning = false;
	}

	void WorkThread::start()
	{
		if(TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != getCurrentState())
		{
			terminateAndWait();
		}
		m_shouldRunning = true;
		TA_Base_Core::Thread::start();
	}
}