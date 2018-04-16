// StressTaskManager.h: interface for the StressTaskManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RTUTASKMANAGER_H__20419F50_87AF_4AD4_B1D6_C5B99A3E9EF8__INCLUDED_)
#define AFX_RTUTASKMANAGER_H__20419F50_87AF_4AD4_B1D6_C5B99A3E9EF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StressTask.h"
#include "RTUSimulator.h"

const unsigned long TASK_COUNT = 22;

class StressTaskManager
{
public:
	StressTaskManager(RTUSimulator* sim);
	virtual ~StressTaskManager();

	void StartTasks();
	void RemoveTasks();
	void ResetCfg(std::vector<std::string>& cfgs);

private:
	std::vector< StressTask* > m_tasks;
	RTUSimulator* m_sim;
};

 #endif// !defined(AFX_RTUTASKMANAGER_H__20419F50_87AF_4AD4_B1D6_C5B99A3E9EF8__INCLUDED_)
