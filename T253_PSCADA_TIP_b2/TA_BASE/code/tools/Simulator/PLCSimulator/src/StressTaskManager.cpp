// RTUTaskManager.cpp: implementation of the RTUTaskManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StressTaskManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


StressTaskManager::StressTaskManager( RTUSimulator* sim ):
m_sim(sim)
{
	m_tasks.clear();
}

StressTaskManager::~StressTaskManager()
{
	RemoveTasks();	
}

void StressTaskManager::StartTasks()
{
	int nSize = m_tasks.size();
	for (int i = 0; i < nSize; ++i)
	{
		StressTask* pTask = m_tasks[i];
		int res = pTask->resume();
	}
}

void StressTaskManager::ResetCfg(std::vector<std::string>& cfgs)
{
	RemoveTasks();
	int nSize = cfgs.size();
	for (int i = 0; i < nSize; ++i)
	{
		StressTask* pTask = new StressTask(m_sim, cfgs[i]);
		ACE_hthread_t handles[2];
		pTask->activate(THR_NEW_LWP | THR_JOINABLE | THR_SUSPENDED,
			1,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
            handles);

		pTask->SetHandle(handles[0]);
		m_tasks.push_back(pTask);
	}
}

void StressTaskManager::RemoveTasks()
{
	int nSize = m_tasks.size();
	for (int i = 0; i < nSize; ++i)
	{
		ACE_hthread_t h = m_tasks[i]->GetHandle();
		//ACE_Thread_Manager::instance()->kill_task(m_tasks[i], 0);
		TRACE("remove %d\n",GetTickCount());
		m_tasks[i]->CloseTask();
		DWORD res = WaitForSingleObject(h, 1000);
		TRACE("remove2 %d\n",GetTickCount());
		if (WAIT_TIMEOUT == res)
		{
			TerminateThread(h, 0);
		}
		CloseHandle(h);
		delete m_tasks[i];
	}
	m_tasks.clear();
}

