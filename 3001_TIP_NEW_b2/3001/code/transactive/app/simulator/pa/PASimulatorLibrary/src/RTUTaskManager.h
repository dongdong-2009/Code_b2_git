// RTUTaskManager.h: interface for the RTUTaskManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RTUTASKMANAGER_H__20419F50_87AF_4AD4_B1D6_C5B99A3E9EF8__INCLUDED_)
#define AFX_RTUTASKMANAGER_H__20419F50_87AF_4AD4_B1D6_C5B99A3E9EF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Run.h"
#include "RTUSimulator.h"

const unsigned long TASK_COUNT = 22;

class RTUTaskManager  
{
public:
	static Run& getTaskItem( unsigned long itemIndex )
	{
		if ( m_vttask.size() == 0 )
		{
			initTaskArray();
		}
		return *( m_vttask[itemIndex] );
	}

	virtual ~RTUTaskManager();

private:
	static void initTaskArray()
	{
		for( int i=0; i<TASK_COUNT; i++ )
		{
			Run * pRun = new Run( RTUSimulator::getInstance() );
			m_vttask.push_back( pRun );
		}
	}

	RTUTaskManager();

private:
	static std::vector< Run* > m_vttask;
};

#endif // !defined(AFX_RTUTASKMANAGER_H__20419F50_87AF_4AD4_B1D6_C5B99A3E9EF8__INCLUDED_)
