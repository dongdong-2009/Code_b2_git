#pragma once
#include "MessageHandler.h"

class RadioMessageHandler :
	public IMessageHandler
{
public:
	RadioMessageHandler(HWND hWnd,unsigned long msgNotification);
	virtual ~RadioMessageHandler(void);
	bool DoAnalyze();
	void NotifyUI(const string& caption,const string& msg);

private:
	bool CheckInputData();
	bool AnalyseSubcriberMonitor();
	bool AnalyseDynamicGroup();
	bool AnalyseForwardRequest();
	bool AnalyseCancelForward();
	bool AnalyseQueueCallForward();
	bool AnalyseProfileRequest();
private:
	HWND m_hWnd;
	unsigned long m_msgNotification;
	char m_msgType[4];
};
