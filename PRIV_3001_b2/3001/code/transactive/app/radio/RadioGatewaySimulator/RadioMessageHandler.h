#pragma once
#include "MessageHandler.h"

#include "RadioGatewayConfigHandler.h"


class RadioMessageHandler :
	public IMessageHandler
{
public:
	RadioMessageHandler(HWND hWnd,unsigned long msgNotification);
	~RadioMessageHandler(void);
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
	bool AnalyseProfileUpdate();
	bool AnalyzeSystemStatus(bool bOcc = true);
	
private:
	HWND m_hWnd;
	unsigned long m_msgNotification;

	IRadioGatewayDataProvider * m_radioGatewayObj;

	char m_msgType[4];

	unsigned long getRadioTsi (std::string strData);
};
