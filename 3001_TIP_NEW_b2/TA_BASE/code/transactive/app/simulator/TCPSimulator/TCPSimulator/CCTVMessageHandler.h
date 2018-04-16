#pragma once
#include "MessageHandler.h"
#include <map>
#include <list>
class CCTVMessageHandler :
	public IMessageHandler
{
public:
	CCTVMessageHandler(HWND hWnd,unsigned long msgNotification);
	virtual ~CCTVMessageHandler(void);
	virtual void SetExtConfig();
	virtual bool DoAnalyze();
	virtual void NotifyUI(const string& caption,const string& msg);

private:
	bool CheckInputData();
	bool AnalyseRequestCmd(unsigned char cmdType);
	bool AnalyseSAS();
	bool AnalyseCTE();
private:
	HWND m_hWnd;
	unsigned long m_msgNotification;
	unsigned char m_msgType;
	typedef std::pair<unsigned char,unsigned char> stuAlarm;
	std::list<stuAlarm> m_alarms;
	short m_CCTVResult;
	short m_03aStatus;
};
