// ATSAlarm.h: interface for the CATSAlarm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATSALARM_H__A8817BFB_5FB3_4EB5_B0CE_B256D962EA50__INCLUDED_)
#define AFX_ATSALARM_H__A8817BFB_5FB3_4EB5_B0CE_B256D962EA50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataStructInfo.h"

#ifdef ENABLE_LOG4CXX
	#include "log4cxx/logger.h"
#else
	#include "logger.h"
#endif

using namespace log4cxx;
//class ATSAlarmInfoManager;

class CATSAlarm : public IATSAlarm  
{
public:
	CATSAlarm();
	virtual ~CATSAlarm();

	void submitATSAlarm(const AlarmDetail& details, const ProfileInfo& profileInfo); //May catch ATS_ALARM_QUEUE_OVERFLOW exception
	void submitATSAlarmList(std::list<AlarmDetail*> details, const ProfileInfo& profileInfo); //May catch ATS_ALARM_QUEUE_OVERFLOW exception
	bool getProfileInfo(ProfileInfo& profileInfo);
	unsigned long retrieveATSAlarmAckUpdate(std::list<AlarmAckUpdate*>& updates);
	unsigned long retrieveATSMmsJobRequest(std::list<MmsJobRequest*>& updates);
	void setUpdateTime(time_t updateTime);
private:
	//ATSAlarmInfoManager m_ATSAlarmInfoManager;

	LoggerPtr  m_logger;

};

#endif // !defined(AFX_ATSALARM_H__A8817BFB_5FB3_4EB5_B0CE_B256D962EA50__INCLUDED_)
