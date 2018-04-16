#include "StdAfx.h"
#include "RadioMessageHandler.h"
#include <fstream>


RadioMessageHandler::RadioMessageHandler(HWND hWnd,unsigned long msgNotification)
:m_hWnd(hWnd),m_msgNotification(msgNotification)
{
	m_radioGatewayObj = new CRadioGatewayConfigHandler ();
}

RadioMessageHandler::~RadioMessageHandler(void)
{
}

bool RadioMessageHandler::DoAnalyze()
{
	if(m_inputData.length()<7)
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
	else
	if(memcmp(m_msgType,"M42",3)==0)
		AnalyseProfileUpdate();
	else
	if (memcmp(m_msgType,"M37",3)==0)
		AnalyzeSystemStatus();

	else if (memcmp(m_msgType,"M38",3)==0)
		AnalyzeSystemStatus(FALSE);

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
	if (length==7 && m_inputData.compare(3,3,"M37",3)==0)
	{
		strcpy(m_msgType, "M37");
		return true;
	}
	
	length =m_inputData.length();
	if (length==7 && m_inputData.compare(3,3,"M38",3)==0)
	{
		strcpy(m_msgType, "M38");
		return true;
	}

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
	else if(length == 21 && m_inputData.compare(17,3,"M42",3)==0)
	{
		strcpy(m_msgType,"M42");
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
	char status = trainid;


	char * pszdataTemp = new char [1024];

	m_inputData.copy(pszdataTemp,12,3);

	unsigned long lResourceId = getRadioTsi(pszdataTemp);

	RadioSubscriberData subscriberData = m_radioGatewayObj->getRadioSubscriberData(lResourceId);
	
	delete [] pszdataTemp;
	
	std::string strTG,strBaseStation;
	
	strTG = subscriberData.strAffiliatedTG;
	strBaseStation = subscriberData.strBaseStation;

	m_outputData.append(m_inputData,0,15);
	m_outputData.append(1,status);
	m_outputData.append(strTG);
	m_outputData.append(strBaseStation);
	m_outputData.append(timestamp,0,14);
	m_outputData.append("A40");
	m_outputData.append(1,(char)0x3);

	return true;
}

bool RadioMessageHandler::AnalyseDynamicGroup()
{
	std::string timestamp;
	std::string issi,talkgroup;
	std::string request;

	issi=m_inputData.substr(3,12);
	talkgroup=m_inputData.substr(16,12);
	timestamp=m_inputData.substr(28,14);
	request = m_inputData.substr(15,1);


	
	m_outputData.append(m_inputData,0,15);
	m_outputData.append(talkgroup,0,12);
	
	unsigned long lStatus = m_radioGatewayObj->getDGNARequestStatus();
	if (lStatus == 0)
	{
		m_outputData.append(1,'0');
	}
	else if (lStatus == 1)
	{
		m_outputData.append(1,'1');
	}
	
	
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
	std::string forwordedISSI  = "000000010011";
	
	RadioCallForwardingQueryData callForwardData = m_radioGatewayObj->getQueryCallForwardingData();

	int  trainid =0;
	memcpy((void*)&trainid,m_inputData.c_str()+1,2);
	char status = trainid;//%3;
	m_outputData.append(m_inputData,0,17);
	m_outputData.append("A34");
	m_outputData.append(1,callForwardData.lCondition);
	m_outputData.append(callForwardData.strForwardingTsi);
	m_outputData.append(callForwardData.strDestinationTsi);
	m_outputData.append(1,(char)0x3);
	return true;
}

bool RadioMessageHandler::AnalyseProfileRequest()
{
	m_outputData.append(m_inputData,0,17);

	int nDataIndex = 20;
	int nDataLen = 12;
	std::string strProfileData = m_inputData.substr(nDataIndex,nDataLen);

	m_outputData.append("A39");

	ProfileTalkGroupListType tgList = m_radioGatewayObj->getTalkGroupsFromProfile(strProfileData);

	for(int i=0;i<nDataLen;i++) 
	{
		m_outputData.append(tgList.at (i));
		m_outputData.append(12-tgList.at(i).size(),(char)0x00);
	}
	m_outputData.append(1,(char)0x3);
	return true;
}

bool RadioMessageHandler::AnalyseProfileUpdate()
{
	
	m_outputData.append(m_inputData,0,17);
	m_outputData.append("A42");

	ifstream input_config("profile.cfg");
	std::string username, num, status;
	int numI = 0;
	int stat = 0;
	
	ProfileDBUpdateCollectionType profileDbUpdates = m_radioGatewayObj->getProfileDBUpdate();
	
	int nSize = profileDbUpdates.size();
	
	m_outputData.append(1,(char)0x00);
	m_outputData.append(1,(char)nSize);

	for (int nCounter = 0; nCounter < nSize; nCounter++)
	{
		m_outputData.append(profileDbUpdates.at (nCounter).strProfile);
		m_outputData.append(12-profileDbUpdates.at (nCounter).strProfile.size(), (char)0x00);
		m_outputData.append(1,(char)profileDbUpdates.at (nCounter).ulOperation);
	}


	m_outputData.append(1,(char)0x03);
	return true;
}

bool RadioMessageHandler::AnalyzeSystemStatus(bool bOcc)
{
	
	m_outputData.append(m_inputData,0,3);

	ifstream input_config("status.cfg");
	std::string status;
	int stat = 0;

	unsigned long lStatus = m_radioGatewayObj->getRadioSystemStatus();

	if (lStatus == 0)
	{
		m_outputData.append(1, 0x30);
	}
	else if (lStatus == 1)
	{
		m_outputData.append(1, 0x31);
	}
	
	if (bOcc)
	{
		m_outputData.append("A37");
	}
	else
	{
		m_outputData.append("A38");
	}


	m_outputData.append(1,(char)0x03);
	return true;
}


void RadioMessageHandler::NotifyUI(const string& caption,const string& msg)
{
	::SendMessage(m_hWnd,m_msgNotification,(WPARAM)&caption,(LPARAM)&msg);
}

unsigned long RadioMessageHandler::getRadioTsi (std::string strData)
{
	unsigned long lReturnData = 0;

	if (strData.empty())
		return lReturnData;

	lReturnData = atol (strData.c_str());

	return lReturnData;
}