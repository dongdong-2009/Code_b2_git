//////////////////////////////////////////////////////////////////////
///		@file		TimeMonitor.h
///		@author		zhilin,ouyang
///		@date		2012-09-19 14:44:41
///
///		@brief	    the timer tool to monitor the time interval for  
///                 specified timer ID.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  TIMEMONITOR_H
#define  TIMEMONITOR_H
#include "CommonDef.h"
#include <map>
#include <sys/timeb.h>
#include <iostream>
#include <fstream>
#include <list>

NS_BEGIN(TA_DB_Sync)

class CTimeMonitor
{
private:
	typedef struct st_timb_tag
	{
		struct timeb  m_timeBegin;
		struct timeb  m_timeEnd;
	}STTIMEB, *PSTTIMEB;

private:
	typedef std::map<uint32, PSTTIMEB>               mapTimerT;
	typedef std::map<uint32, PSTTIMEB>::iterator     mapTimerIterT;
	typedef std::map<uint32, PSTTIMEB>::value_type   mapTimerValueT;

public:
	CTimeMonitor();
	~CTimeMonitor();

public:
	uint32      getTimeDiffByMilSecond(uint32 uTimerID);
	uint32		getTimeDiffBySecond(uint32 uTimerID);
	uint32      getTimeDiffByMinute(uint32 uTimerID);
	uint32      getTimeDiffByHour(uint32 uTimerID);

	void        reset(uint32 uTimerID);

private:
	void        _UpdateTimer(uint32 uTimerID);
	void        _CopyTimer(struct timeb* pSrc, struct timeb* pDes);
	void        _clearData();

private:
	mapTimerT    m_mapTimer;

};

class CTimeUtility
{
public:
	CTimeUtility();
	~CTimeUtility();
public:
	std::string getCurrentTime(struct timeb* pfbtime);
	std::string getDiffTime(struct timeb* pfbtimeBegin, struct timeb* pfbtimeEnd);
	int getDiffTimeRes(struct timeb* pfbtimeBegin, struct timeb* pfbtimeEnd);
};


NS_END(TA_DB_Sync)

#endif // TIMEMONITOR_H
