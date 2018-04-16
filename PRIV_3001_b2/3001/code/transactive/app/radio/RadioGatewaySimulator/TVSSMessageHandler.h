#pragma once
#include "MessageHandler.h"

class TVSSMessageHandler :
	public IMessageHandler
{
public:
	TVSSMessageHandler(HWND hWnd,unsigned long msgNotification);
	~TVSSMessageHandler(void);
	bool DoAnalyze();
	void NotifyUI(const string& caption,const string& msg);
private:
	HWND m_hWnd;
	unsigned long m_msgNotification;
};
