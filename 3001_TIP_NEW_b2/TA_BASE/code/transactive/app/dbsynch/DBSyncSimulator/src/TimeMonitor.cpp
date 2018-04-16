//////////////////////////////////////////////////////////////////////
///		@file		TimeMonitor.cpp
///		@author		zhilin,ouyang
///		@date		2012-09-19 14:44:41
///
///		@brief	    the timer tool to monitor the time interval for  
///                 specified timer ID.
///                 
//////////////////////////////////////////////////////////////////////
#include "TimeMonitor.h"
#include <time.h>
#include "core/utilities/src/DebugUtil.h"
#include "UtilityFun.h"

NS_BEGIN(TA_DB_Sync)


CTimeMonitor::CTimeMonitor()
{
	m_mapTimer.clear();
}

CTimeMonitor::~CTimeMonitor()
{
	_clearData();
}

uint32  CTimeMonitor::getTimeDiffByMilSecond(uint32 uTimerID)
{
	uint32  uiMilSeconds = 0;
	uint32  uiSeconds = 0;

	_UpdateTimer(uTimerID);
	mapTimerIterT iter = m_mapTimer.find(uTimerID);
	if ( iter != m_mapTimer.end() )
	{
		uiSeconds = iter->second->m_timeEnd.time - iter->second->m_timeBegin.time;
		uiMilSeconds = iter->second->m_timeEnd.millitm - iter->second->m_timeBegin.millitm;
		uiMilSeconds += uiSeconds * 1000;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Error message: %s", "The target timer does not exist." );
	}

	return uiMilSeconds;
}

uint32  CTimeMonitor::getTimeDiffBySecond(uint32 uTimerID)
{
	uint32 uiSeconds = 0;

	uiSeconds = getTimeDiffByMilSecond(uTimerID) / 1000;

	return uiSeconds;
}

uint32   CTimeMonitor::getTimeDiffByMinute(uint32 uTimerID)
{
	uint32  uiMinute = 0;

	uiMinute = getTimeDiffBySecond(uTimerID) / 60;

	return uiMinute;
}

uint32  CTimeMonitor::getTimeDiffByHour(uint32 uTimerID)
{
	uint32 uiHour = 0;

	uiHour = getTimeDiffByMinute(uTimerID) / 60;

	return uiHour;
}

void  CTimeMonitor::reset(uint32 uTimerID)
{
	mapTimerIterT iter = m_mapTimer.find(uTimerID);
	if ( iter != m_mapTimer.end() )
	{
		DEF_DELETE(iter->second);

		m_mapTimer.erase( iter );
	}	
}

void  CTimeMonitor::_UpdateTimer(uint32 uTimerID)
{
	mapTimerIterT iter = m_mapTimer.find(uTimerID);
	if (iter != m_mapTimer.end())
	{
		ftime(&(iter->second->m_timeEnd));
	}
	else
	{
		PSTTIMEB pTimer = new STTIMEB();
		ftime(&(pTimer->m_timeBegin));
		_CopyTimer(&(pTimer->m_timeBegin), &(pTimer->m_timeEnd));
		m_mapTimer.insert(mapTimerValueT(uTimerID, pTimer));
	}
}

void  CTimeMonitor::_CopyTimer(struct timeb* pSrc, struct timeb* pDes)
{
	pDes->dstflag  = pSrc->dstflag;
	pDes->millitm  = pSrc->millitm;
	pDes->time     = pSrc->time;
	pDes->timezone = pSrc->timezone;
}

void  CTimeMonitor::_clearData()
{
	mapTimerIterT iter = m_mapTimer.begin();
	while (iter != m_mapTimer.end())
	{
		DEF_DELETE(iter->second);

		iter++;
	}

	m_mapTimer.clear();
}




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



std::string CTimeUtility::getCurrentTime(struct timeb* pfbtime)
{
	struct tm*			pTm = NULL;
	std::string			strCurrentTime;
	char				szCurTime[256] = {0};

	if (NULL != pfbtime)
	{
		ftime(pfbtime);	
		pTm = localtime(&(pfbtime->time));
		if ( NULL != pTm && NULL != pfbtime)
		{
			sprintf(szCurTime, "[%02d/%02d/%02d %02d:%02d:%02d:%03d]",
				pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday,
				pTm->tm_hour, pTm->tm_min, pTm->tm_sec, pfbtime->millitm); 

			strCurrentTime = szCurTime;     
		}	  

	}
	TA_DB_Sync::trim(strCurrentTime);
	return strCurrentTime;	 
}


std::string CTimeUtility::getDiffTime(struct timeb* pfbtimeBegin, struct timeb* pfbtimeEnd)
{  
	std::string			strDiffTime;
	char				szCurTime[256] = {0};
	time_t				diffSeconds;
	double				diffAll = 0;	

	if (NULL != pfbtimeBegin && NULL != pfbtimeEnd)
	{
		diffSeconds = pfbtimeEnd->time - pfbtimeBegin->time;
		diffAll = (diffSeconds*1000.0 + pfbtimeEnd->millitm - pfbtimeBegin->millitm);
		sprintf(szCurTime, "%10.3f ms", diffAll); 
		strDiffTime =	szCurTime;
	}
	TA_DB_Sync::trim(strDiffTime);
	return strDiffTime;	 
}

int CTimeUtility::getDiffTimeRes(struct timeb* pfbtimeBegin, struct timeb* pfbtimeEnd)
{  
	int                 nDiffTimeRes = 0;
	time_t				diffSeconds;

	if (NULL != pfbtimeBegin && NULL != pfbtimeEnd)
	{
		diffSeconds = pfbtimeEnd->time - pfbtimeBegin->time;
		nDiffTimeRes = (diffSeconds*1000.0 + pfbtimeEnd->millitm - pfbtimeBegin->millitm);
	}

	return nDiffTimeRes;	 
}

CTimeUtility::CTimeUtility()
{

}

CTimeUtility::~CTimeUtility()
{

}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

NS_END(TA_DB_Sync)