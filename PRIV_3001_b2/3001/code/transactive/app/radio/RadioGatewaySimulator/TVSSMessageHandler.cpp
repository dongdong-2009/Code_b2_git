#include "StdAfx.h"
#include "TVSSMessageHandler.h"

TVSSMessageHandler::TVSSMessageHandler(HWND hWnd,unsigned long msgNotification)
:m_hWnd(hWnd),m_msgNotification(msgNotification)
{
}

TVSSMessageHandler::~TVSSMessageHandler(void)
{
}

bool TVSSMessageHandler::DoAnalyze()
{
	if(m_inputData.length()<6)
	{
		NotifyUI("Error","Length of TVSS message is too short");
		return false;
	}
	m_outputData.erase(m_outputData.begin(),m_outputData.end());
	m_outputData.append(m_inputData.c_str(),8);
	m_outputData[1]=0x6;
	m_outputData[5]=0x8;		//length
	m_outputData[7]=0x1;	//must be 1
	return true;
}

void TVSSMessageHandler::NotifyUI(const string& caption,const string& msg)
{
	::SendMessage(m_hWnd,m_msgNotification,(WPARAM)&caption,(LPARAM)&msg);
}