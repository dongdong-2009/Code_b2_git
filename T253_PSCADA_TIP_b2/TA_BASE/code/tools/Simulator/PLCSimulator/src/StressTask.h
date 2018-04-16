// Run.h: interface for the Run class.
//
//////////////////////////////////////

#pragma once

#include "RTUSimulator.h"
#include "ace/Task.h"
#include "ace/Synch.h"

class StressTask : public ACE_Task_Base
{
public:
	StressTask( RTUSimulator* sim, std::string & cfg );
	~StressTask( void );

	virtual int svc( void );

	void Stress();
	void GetStressConfig();
	void SetHandle(ACE_hthread_t h);
	ACE_hthread_t GetHandle();
	void CloseTask();

private:
	bool			m_bRunning;
	std::string		m_StressCfgFile;
	RTUSimulator*	m_sim;
	smap_type		m_mulmapStress;
	ACE_hthread_t	m_handle;
	HANDLE			m_eventHandle;
};
