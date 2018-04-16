#pragma once
#include "MessageHandler.h"

class TestMessageHandler :
	public IMessageHandler
{
public:
	TestMessageHandler(HWND hWnd);
	~TestMessageHandler(void);
	bool DoAnalyze();
	void NotifyUI(const string& caption,const string& msg);
private:
	HWND m_hWnd;
};
