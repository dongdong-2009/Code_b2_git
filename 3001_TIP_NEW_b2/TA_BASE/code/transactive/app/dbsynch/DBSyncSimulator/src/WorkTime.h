/**
*
* Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/dbsynch/SynchEngine/src/WorkTime.h $
* @author:  Shenglong.lin
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2013/02/19 09:15:14 $
* Last modified by:  $Author: Shenglong.lin $
* 
* This class is used to get Thread Work time 
*/


#if !defined(__C_WORKTIME_HH__)
#define __C_WORKTIME_HH__

#include <sys/timeb.h>
#include <iostream>
#include <fstream>
#include <list> 
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "CommonDef.h"
#include "CommonData.h"
#include "TimeMonitor.h"


NS_BEGIN(TA_DB_Sync)

using namespace TA_Base_Core;

class CWorkTime
{
public:
	CWorkTime(TA_DB_Sync::Timer_ID_Code nWorkTimeCode);
	~CWorkTime(void); 
public:	
	//set begin work 
	int workBegin(); 
	//set begin work
	int workEnd();
	//WorkTime valuse is end work time - begin work time
	int getWorkTime();   //for log time 
	//NotWorkTime valuse is now time - begin work time
	int getNotworkTime(); //for monitor
	
private:
	std::string _GetCurrentTime(struct timeb* pfbtime);
	int _GetDiffTime(struct timeb* pfbtimeBegin, struct timeb* pfbtimeEnd);
	
private:
	ReEntrantThreadLockable	m_lockWorkTime; 

	TA_DB_Sync::Timer_ID_Code  m_nWorkTimeCode;
	//begin work time
	struct timeb m_fTimeWorkBegin;	
	//end work time
	struct timeb m_fTimeWorkEnd;		
	//WorkTime valuse is end work time - begin work time
	int			 m_nWorkTime;//millsecs	 
	//NotWorkTime valuse is now time - begin work time
	int          m_nNotWorkTime;
	//set begin work 
	bool         m_bSetWorkBegin;
	//set begin work 
	bool         m_bSetWorkEnd;


};

NS_END(TA_DB_Sync)




#endif//__C_WORKTIME_HH__



