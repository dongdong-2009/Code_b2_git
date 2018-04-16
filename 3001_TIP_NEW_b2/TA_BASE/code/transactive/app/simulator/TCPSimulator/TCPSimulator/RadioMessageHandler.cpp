#include "StdAfx.h"
#include "RadioMessageHandler.h"

RadioMessageHandler::RadioMessageHandler(HWND hWnd,unsigned long msgNotification)
:m_hWnd(hWnd),m_msgNotification(msgNotification)
{
}

RadioMessageHandler::~RadioMessageHandler(void)
{
}

bool RadioMessageHandler::DoAnalyze()
{
	if(m_inputData.length()<33)
	{
		NotifyUI("Error","Length of Radio message is too short");
		return false;
	}
	if(!CheckInputData())
		return false;
	m_outputData.erase(m_outputData.begin(),m_outputData.end());
	if(memcmp(m_msgType,"M32",3)==0)
		AnalyseForwardRequest();
	else
	if(memcmp(m_msgType,"M33",3)==0)
		AnalyseCancelForward();
		
	else
	if(memcmp(m_msgType,"M34",3)==0)
		AnalyseQueueCallForward();
	else
	if(memcmp(m_msgType,"M39",3)==0)
		AnalyseProfileRequest();
	else
	if(memcmp(m_msgType,"M40",3)==0)
		AnalyseSubcriberMonitor();
	else
	if(memcmp(m_msgType,"M41",3)==0)
		AnalyseDynamicGroup();
	return true;
}

bool RadioMessageHandler::CheckInputData()
{
	if(m_inputData[0]!=0x2 || m_inputData[m_inputData.length()-1]!=0x3)
	{
		NotifyUI("Error","incorrect message format");
		return false;
	}
	int length =m_inputData.length();
	std::string str = m_inputData.substr(17,3);
	if(length==46 && m_inputData.compare(17,3,"M32",3)==0)
	{
		strcpy(m_msgType,"M32");
		return true;
	}
	else if(length==33 && m_inputData.compare(17,3,"M33",3)==0)
	{
		strcpy(m_msgType,"M33");
		return true;
	}
	else if(length==33 && m_inputData.compare(17,3,"M34",3)==0)
	{
		strcpy(m_msgType,"M34");
		return true;
	}
	else if(length==33 && m_inputData.compare(17,3,"M39",3)==0)
	{
		strcpy(m_msgType,"M39");
		return true;
	}
	else if(length==33 && m_inputData.compare(29,3,"M40",3)==0)
	{
		strcpy(m_msgType,"M40");
		return true;
	}
	else if(length==46 && m_inputData.compare(42,3,"M41",3)==0)
	{
		strcpy(m_msgType,"M41");
		return true;
	}
	NotifyUI("Error","unrecognized message type");
	return false;
}

bool RadioMessageHandler::AnalyseSubcriberMonitor()
{
	std::string timestamp;
	int  trainid =0;
	memcpy((void*)&trainid,m_inputData.c_str()+1,2);
	timestamp=m_inputData.substr(15,14);
	
	char status = trainid%3;
	std::string talkGroup[3] = {"Talk Group 1","Talk Group 2","Talk Group 3"};
	std::string baseStation[3] = {"BaseStation1","BaseStation2","BaseStation3"};
	m_outputData.append(m_inputData,0,15);
	m_outputData.append(1,status);
	m_outputData.append(talkGroup[status]);
	m_outputData.append(baseStation[status]);
	m_outputData.append(timestamp,0,14);
	m_outputData.append("A40");
	m_outputData.append(1,(char)0x3);
	return true;
}

bool RadioMessageHandler::AnalyseDynamicGroup()
{
	std::string timestamp;
	std::string issi,talkgroup;

	issi=m_inputData.substr(3,12);
	talkgroup=m_inputData.substr(16,12);
	timestamp=m_inputData.substr(28,14);

	m_outputData.append(m_inputData,0,15);
	m_outputData.append(talkgroup,0,12);
	m_outputData.append(1,'0');
	m_outputData.append(timestamp,0,14);
	m_outputData.append("A41");
	m_outputData.append(1,(char)0x3);
	return true;
}

bool RadioMessageHandler::AnalyseForwardRequest()
{

	std::string timestamp;
	timestamp = m_inputData.substr(3,14);
	m_outputData.append(m_inputData,0,3);
	m_outputData.append(1,(char)0x1);
	m_outputData.append(timestamp,0,14);
	m_outputData.append("A32");
	m_outputData.append(1,(char)0x3);
	return true;
}

bool RadioMessageHandler::AnalyseCancelForward()
{
	std::string timestamp;
	timestamp = m_inputData.substr(3,14);
	m_outputData.append(m_inputData,0,3);
	m_outputData.append(1,(char)0x1);
	m_outputData.append(timestamp,0,14);
	m_outputData.append("A33");
	m_outputData.append(1,(char)0x3);
	return true;
}

bool RadioMessageHandler::AnalyseQueueCallForward()
{
	std::string timestamp;
	std::string forwordingISSI = "000001000106";
	std::string forwordedISSI  = "000000091103";
	
	int  trainid =0;
	memcpy((void*)&trainid,m_inputData.c_str()+1,2);
	char status = trainid%3;
	m_outputData.append(m_inputData,0,17);
	m_outputData.append("A34");
	m_outputData.append(1,status==2?9:status);
	m_outputData.append(forwordingISSI);
	m_outputData.append(forwordedISSI);
	m_outputData.append(1,(char)0x3);
	return true;
}

bool RadioMessageHandler::AnalyseProfileRequest()
{
	std::string talkgroups[12] ={"Talk Group01","Talk Group02","Talk Group03","Talk Group04","Talk Group05","Talk Group06",
								 "Talk Group07","Talk Group08","Talk Group09","Talk Group10","Talk Group11","Talk Group12"};
	m_outputData.append(m_inputData,0,17);
	m_outputData.append("A39");
	for(int i=0;i<12;i++)
		m_outputData.append(talkgroups[i]);
	m_outputData.append(1,(char)0x3);
	return true;
}


void RadioMessageHandler::NotifyUI(const string& caption,const string& msg)
{
	::SendMessage(m_hWnd,m_msgNotification,(WPARAM)&caption,(LPARAM)&msg);
}