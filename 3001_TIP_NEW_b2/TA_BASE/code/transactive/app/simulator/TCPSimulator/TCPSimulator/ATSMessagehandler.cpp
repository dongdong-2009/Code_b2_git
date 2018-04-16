#include "StdAfx.h"
#include "ATSMessagehandler.h"
#include "SimulatorConfig.h"

ATSMessageHandler::ATSMessageHandler(HWND hWnd,unsigned long msgNotification)
:IMessageHandler(),m_hWnd(hWnd),m_msgNotification(msgNotification),m_dataPointTable("")
{
	
}

ATSMessageHandler::~ATSMessageHandler(void)
{
}

void ATSMessageHandler::setInputData(const char* data,unsigned int length)
{
	m_inputData.erase(m_inputData.begin(),m_inputData.end());
	memcpy((void*)m_inputData.c_str(),data,length);
}

void ATSMessageHandler::setOutputData(const char* data,unsigned int length)
{
	;
}

bool ATSMessageHandler::DoAnalyze()
{

	if(!IsInputAvailable())
	{
		return false;
	}
	m_funcCode = m_inputData[7];
	m_startAddr[0] = m_inputData[8];
	m_startAddr[1] = m_inputData[9];
	m_iStartAddr=(m_startAddr[0]<<8)+m_startAddr[1];
	switch(m_funcCode)
	{
	case CODE_CMD_READ:
		return handleReadPointTable();
		break;
	case CODE_CMD_WRITE:
		return handleWriteTrainPower();
		break;
	default:
		NotifyUI("Error","Known type of message");
		break;
	}
	return false;
}

bool ATSMessageHandler::handleReadPointTable()
{
	m_wordCount[0] = m_inputData[10];
	m_wordCount[1] = m_inputData[11];
	m_outputData.erase(m_outputData.begin(),m_outputData.end());
	m_iWordCount=(m_wordCount[0]<<8)+m_wordCount[1];
	if(0x0 == m_iStartAddr && 0x1 == m_iWordCount )
		return setReplySignal();
	else
		return setReplyPointTable();
}

bool ATSMessageHandler::setReplySignal()
{
	m_outputData.append(m_inputData,0,4);
	m_outputData.push_back((char)0x0);
	m_outputData.push_back((char)0x5);
	m_outputData.push_back((char)0xff);//LUCKY
	m_outputData.push_back((char)CODE_CMD_READ);
	m_outputData.push_back((char)0x0);
	//m_outputData.append(2,0xff);
	int atsStatus = SimulatorConfig::getInstance()->m_atsStatus;
	m_outputData.push_back((char)(atsStatus>>8));
	m_outputData.push_back((char)(atsStatus&0xff));
	return true;
}

bool ATSMessageHandler::setReplyPointTable()
{
	if(m_dataPointTable.empty())
	{
		NotifyUI("Error","Point Table is empty");
		return false;
	}
	if(m_iStartAddr*2 + m_iWordCount*2 > m_dataPointTable.length())
	{
		NotifyUI("Error","The requested start address and word-count extent PointTable data size limit");
		return false;
	}
	int telLength = m_iWordCount*2 + 3;
	m_outputData.append(m_inputData,0,4);
	m_outputData.push_back(HIBYTE(telLength));
	m_outputData.push_back(LOBYTE(telLength));
	m_outputData.push_back((char)0xff); //LUCKY
	m_outputData.push_back((char)CODE_CMD_READ);
	m_outputData.push_back((char)0x0);
	m_outputData.append(m_dataPointTable,m_iStartAddr*2,m_iWordCount*2);
	
	return true;
}

bool ATSMessageHandler::handleWriteTrainPower()
{
	if(m_inputData.length()<15)
	{
		NotifyUI("Error","Length of Set Train Power Command is too short");
		return false;
	}
	m_wordCount[0] = m_inputData[10];
	m_wordCount[1] = m_inputData[11];
	m_outputData.erase(m_outputData.begin(),m_outputData.end());
	m_iByteCount = static_cast<int>(m_inputData[12]);
	if(m_inputData.length()!= 13+ m_iByteCount )
	{
		NotifyUI("Warn","Byte count of Set Train Power Command does not match to the actual length of thsi command");
	}
	int telLength = 6;
	m_outputData.append(m_inputData,0,4);
	m_outputData.push_back(HIBYTE(telLength));
	m_outputData.push_back(LOBYTE(telLength));
	m_outputData.push_back((char)0x00);//LUCKY
	m_outputData.push_back((char)CODE_CMD_WRITE);
	m_outputData.push_back(m_startAddr[0]);
	m_outputData.push_back(m_startAddr[1]);
	m_outputData.push_back(m_wordCount[0]);
	m_outputData.push_back(m_wordCount[1]);
	return true;
}

bool ATSMessageHandler::IsInputAvailable()
{
	if(m_inputData.length()<12 )
	{
		NotifyUI("Error","Input data is too short");
		return false;
	}
	return true;
}

void ATSMessageHandler::NotifyUI(const string& caption,const string& msg)
{
	::SendMessage(m_hWnd,m_msgNotification,(WPARAM)&caption,(LPARAM)&msg);
}
