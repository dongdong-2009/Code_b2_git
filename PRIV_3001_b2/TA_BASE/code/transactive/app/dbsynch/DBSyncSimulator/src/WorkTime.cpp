#include "WorkTime.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"



NS_BEGIN(TA_DB_Sync)


using  TA_Base_Core::DebugUtil;




CWorkTime::CWorkTime( Timer_ID_Code nWorkTimeCode )
{
	m_nWorkTimeCode = nWorkTimeCode;	
	_GetCurrentTime(&m_fTimeWorkBegin);
	_GetCurrentTime(&m_fTimeWorkEnd);
	m_nWorkTime = 0;	
	m_bSetWorkBegin = false;
	m_bSetWorkEnd = false;
	m_nNotWorkTime = 0;

}
CWorkTime::~CWorkTime(void)
{
}

//////////////////////////////////////////////////////////////////////////


 //////////////////////////////////////////////////////////////////////////
int CWorkTime::workBegin()
{
	int nFunRes = 0;
	TA_Base_Core::ThreadGuard guard(m_lockWorkTime);
	_GetCurrentTime(&m_fTimeWorkBegin);
	_GetCurrentTime(&m_fTimeWorkEnd);
	m_bSetWorkBegin = true;
	m_bSetWorkEnd = false;

	return nFunRes;
}

int CWorkTime::workEnd()
{
	int nFunRes = 0;
	TA_Base_Core::ThreadGuard guard(m_lockWorkTime);
	
	if (m_bSetWorkBegin)
	{ 	
		_GetCurrentTime(&m_fTimeWorkEnd);
		m_nWorkTime = _GetDiffTime(&m_fTimeWorkBegin, &m_fTimeWorkEnd);

		m_bSetWorkEnd = true;
		m_bSetWorkBegin = false;
	} 
	else
	{
		m_bSetWorkEnd = false;		
		m_nWorkTime = 0;
	}

	return nFunRes;
}

int CWorkTime::getNotworkTime()
{
	int nFunRes = 0;
	struct timeb m_fTimeNow;

	TA_Base_Core::ThreadGuard guard(m_lockWorkTime);
	
	if (true == m_bSetWorkBegin && false == m_bSetWorkEnd)
	{
		_GetCurrentTime(&m_fTimeNow);
		m_nNotWorkTime = _GetDiffTime(&m_fTimeWorkBegin, &m_fTimeNow);
	}
	else
	{
		m_nNotWorkTime = 0;
		return m_nNotWorkTime;
	}

	
	if (m_nNotWorkTime < 0)
	{
		m_nNotWorkTime = 0;
	}
	return m_nNotWorkTime;
}


int CWorkTime::getWorkTime()
{
	int nFunRes = 0;
	TA_Base_Core::ThreadGuard guard(m_lockWorkTime); 
	return m_nWorkTime;
}

//////////////////////////////////////////////////////////////////////////


std::string CWorkTime::_GetCurrentTime(struct timeb* pfbtime)
{
	struct tm*			pTm = NULL;
	std::string			strCurrentTime;

	if (NULL != pfbtime)
	{
		ftime(pfbtime);	
		
		//format to string
		/*
		char szCurTime[256] = {0};
		pTm = localtime(&(pfbtime->time));
		if ( NULL != pTm && NULL != pfbtime)
		{
			sprintf(szCurTime, "[%02d/%02d/%02d %02d:%02d:%02d:%03d]",
				pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday,
				pTm->tm_hour, pTm->tm_min, pTm->tm_sec, pfbtime->millitm); 

			strCurrentTime = szCurTime;     
		}
		TA_DB_Sync::trim(strCurrentTime);
		*/

	}
	return strCurrentTime;	 
}



int CWorkTime::_GetDiffTime(struct timeb* pfbtimeBegin, struct timeb* pfbtimeEnd)
{  
	int                 nDiffTimeRes = 0;
	time_t				diffSeconds;

	if (NULL != pfbtimeBegin && NULL != pfbtimeEnd)
	{
		diffSeconds = pfbtimeEnd->time - pfbtimeBegin->time;
		nDiffTimeRes = (diffSeconds*1000.0 + pfbtimeEnd->millitm - pfbtimeBegin->millitm);
	}
	
	//format to str		
	//char szCurTime[256] = {0};
	//sprintf(szCurTime, "%10.3f ms", diffAll); 
	//strDiffTime =	szCurTime;
	//TA_DB_Sync::trim(strDiffTime);


	return nDiffTimeRes;	 
}


NS_END(TA_DB_Sync)


