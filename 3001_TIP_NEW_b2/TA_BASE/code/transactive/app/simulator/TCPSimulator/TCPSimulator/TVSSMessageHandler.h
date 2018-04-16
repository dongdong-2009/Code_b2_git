#pragma once
#include "MessageHandler.h"

class TVSSMessageHandler :
	public IMessageHandler
{
public:
	TVSSMessageHandler(HWND hWnd,unsigned long msgNotification);
	virtual ~TVSSMessageHandler(void);
	virtual void SetExtConfig();
	virtual bool DoAnalyze();
	virtual void NotifyUI(const string& caption,const string& msg);
private:
	bool AnalyseEquipmentAlarmQuery();
private:
	HWND m_hWnd;
	unsigned long m_msgNotification;
	char m_alarmGW,m_alarmAP,m_alarmSB,m_alarmLTM;
	short m_TVSSResult;
};
