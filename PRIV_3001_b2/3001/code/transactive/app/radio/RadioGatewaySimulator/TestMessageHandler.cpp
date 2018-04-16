#include "StdAfx.h"
#include "TestMessageHandler.h"

TestMessageHandler::TestMessageHandler(HWND hWnd)
:m_hWnd(hWnd)
{
}

TestMessageHandler::~TestMessageHandler(void)
{
}

bool TestMessageHandler::DoAnalyze()
{
	setOutputData(m_inputData);
	return true;
}

void TestMessageHandler::NotifyUI(const string& caption,const string& msg)
{
	;
}