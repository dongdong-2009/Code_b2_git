#include "StdAfx.h"
#include "TVSSMessageHandler.h"

TVSSMessageHandler::TVSSMessageHandler(HWND hWnd,unsigned long msgNotification)
:m_hWnd(hWnd),m_msgNotification(msgNotification),m_alarmGW(0),m_alarmAP(0),m_alarmSB(0),m_alarmLTM(0),m_TVSSResult(1)
{
	SetExtConfig();
}

TVSSMessageHandler::~TVSSMessageHandler(void)
{
}

void TVSSMessageHandler::SetExtConfig()
{
	m_extInfo = SimulatorConfig::getInstance()->m_extInfo;;
	if(m_extInfo.size()==0)
		return;
	std::list<std::string>::iterator it =m_extInfo.begin();
	int stationId,eventId;
	int rtn;
	for( ;it != m_extInfo.end();it++)
	{
		int alarmValue;
		int result;
		rtn = sscanf((*it).c_str(),"AlarmGW = %d",&alarmValue);
		if(0!=rtn && EOF != rtn)
		{
			m_alarmGW = alarmValue;
			continue;
		}
		rtn = sscanf((*it).c_str(),"AlarmAP = %d",&alarmValue);
		if(0!=rtn && EOF != rtn)
		{
			m_alarmAP = alarmValue;
			continue;
		}
		rtn = sscanf((*it).c_str(),"AlarmSB = %d",&alarmValue);
		if(0!=rtn && EOF != rtn)
		{
			m_alarmSB = alarmValue;
			continue;
		}
		rtn = sscanf((*it).c_str(),"AlarmLTM = %d",&alarmValue);
		if(0!=rtn && EOF != rtn)
		{
			m_alarmLTM = alarmValue;
			continue;
		}
		rtn = sscanf((*it).c_str(),"Result = %d",&result);
		if(0!=rtn && EOF != rtn)
		{
			m_TVSSResult = result;
			continue;
		}
	}
}

bool TVSSMessageHandler::DoAnalyze()
{
	if(m_inputData.length()<6)
	{
		NotifyUI("Error","Length of TVSS message is too short");
		return false;
	}
	SimulatorConfig::getInstance()->ReadExtConfig();
	SetExtConfig();
	m_outputData.erase(m_outputData.begin(),m_outputData.end());
	if(m_inputData[1] == 0x8)
	{
		return AnalyseEquipmentAlarmQuery();
	}
	else
	{
		m_outputData.append(m_inputData.c_str(),8);
		m_outputData[1]=0x6;
		m_outputData[5]=0x8;		//length
		m_outputData[7]=m_TVSSResult;			//0x1;	//must be 1
	}
	return true;
}

bool TVSSMessageHandler::AnalyseEquipmentAlarmQuery()
{
	SimulatorConfig::getInstance()->ReadExtConfig();
	SetExtConfig();
	m_outputData.append(m_inputData.c_str(),7);
	m_outputData[1]=0xA;
	m_outputData[5]=0xB;
	m_outputData.append(1,m_alarmGW);
	m_outputData.append(1,m_alarmAP);
	m_outputData.append(1,m_alarmSB);
	m_outputData.append(1,m_alarmLTM);
	return true;
}

void TVSSMessageHandler::NotifyUI(const string& caption,const string& msg)
{
	::SendMessage(m_hWnd,m_msgNotification,(WPARAM)&caption,(LPARAM)&msg);
}