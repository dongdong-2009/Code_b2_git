#include "StdAfx.h"
#include "CCTVMessageHandler.h"

CCTVMessageHandler::CCTVMessageHandler(HWND hWnd,unsigned long msgNotification)
:m_hWnd(hWnd),m_msgNotification(msgNotification),m_CCTVResult(0)
{
	
	SetExtConfig();
}

CCTVMessageHandler::~CCTVMessageHandler(void)
{

}

void CCTVMessageHandler::SetExtConfig()
{
	m_extInfo = SimulatorConfig::getInstance()->m_extInfo;;
	if(m_extInfo.size()==0)
		return;
	m_alarms.clear();
	std::list<std::string>::iterator it =m_extInfo.begin();
	int stationId,eventId;
	int result;
	int rtn;
	for( ;it != m_extInfo.end();it++)
	{
		rtn = sscanf((*it).c_str(),"Alarm = %d:%d",&stationId,&eventId);
		if(0!=rtn && EOF != rtn)
		{
			m_alarms.push_back(stuAlarm(stationId,eventId));
			continue;
		}
		rtn = sscanf((*it).c_str(),"Result = %d",&result);
		if(0!=rtn && EOF != rtn)
		{
			m_CCTVResult = result;
			continue;
		}
		rtn = sscanf((*it).c_str(),"03a status = %x",&result);
		if(0!=rtn && EOF != rtn)
		{
			m_03aStatus = result;
			continue;
		}
	}
}

bool CCTVMessageHandler::DoAnalyze()
{
	
	if(!CheckInputData())
		return false;
	m_outputData.erase(m_outputData.begin(),m_outputData.end());
	switch(m_msgType)
	{
	case 0x06:
	case 0x17:
	case 0x18:
	case 0x21:
	case 0x22:
	case 0x25:
	case 0x3C:
	case 0x3D:
		return AnalyseRequestCmd(m_msgType);
	case 0x3A:
		return AnalyseSAS();
	case 0x3B:
		return AnalyseCTE();
	default:
		NotifyUI("Error","unrecognized message type");
		return false;
	}
	
}

bool CCTVMessageHandler::CheckInputData()
{
	if(m_inputData.length()<4)
	{
		NotifyUI("Error","Length of CCTV message is too short");
		return false;
	}
	int actualLength =m_inputData.length();
	int msgLength = m_inputData[0]+(m_inputData[1]<<8);
	if(actualLength!=msgLength+2)
	{
		NotifyUI("Error","Message length of CCTV message is not equal to the actual length");
		return false;
	}
	m_msgType = m_inputData[3];
	return true;
}

bool CCTVMessageHandler::AnalyseRequestCmd(unsigned char cmdType)
{
	SimulatorConfig::getInstance()->ReadExtConfig();
	SetExtConfig();
	m_outputData.append(1,0x2);
	m_outputData.append(1,0x0);
	//m_outputData.append(1,0x0);
	m_outputData.append(1,m_CCTVResult);
	m_outputData.append(1,cmdType);
	Sleep(4000);
	return true;
}

bool CCTVMessageHandler::AnalyseSAS()
{
	SimulatorConfig::getInstance()->ReadExtConfig();
	SetExtConfig();
	m_outputData.append(1,0x3);
	m_outputData.append(1,0x0);
	m_outputData.append(1,0x0);
	m_outputData.append(1,0x3A);
	m_outputData.append(1,m_03aStatus);
	return true;
}

bool CCTVMessageHandler::AnalyseCTE()
{
	SimulatorConfig::getInstance()->ReadExtConfig();
	SetExtConfig();
	m_outputData.append(1,0x4);
	m_outputData.append(1,0x0);
	m_outputData.append(1,0x0);
	m_outputData.append(1,0x3B);
	
	if(m_alarms.empty())
	{	
		m_outputData.append(1,0x0);
		m_outputData.append(1,0x0);

	}
	else
	{
		int iAlarms = m_alarms.size();
		m_outputData[0] = 4+ iAlarms*2;
		m_outputData.append(1,LOBYTE(iAlarms));
		m_outputData.append(1,HIBYTE(iAlarms));
		
		std::list <stuAlarm>::iterator it = m_alarms.begin();
		for( ;it!=m_alarms.end();it++)
		{
			m_outputData.append(1,(*it).first);
			m_outputData.append(1,(*it).second);
		}
	}
	return true;
}

void CCTVMessageHandler::NotifyUI(const string& caption,const string& msg)
{
	::SendMessage(m_hWnd,m_msgNotification,(WPARAM)&caption,(LPARAM)&msg);
}