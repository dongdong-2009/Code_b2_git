#include "StdAfx.h"
#include "ATSC95Messagehandler.h"
#include "SimulatorConfig.h"

ATSC95MessageHandler::ATSC95MessageHandler(HWND hWnd,unsigned long msgNotification,int baseAddr)
:IMessageHandler(),m_hWnd(hWnd),m_msgNotification(msgNotification),m_dataPointTable(""),
m_watchDogCount(0),m_baseAddr(baseAddr)
{
	
}

ATSC95MessageHandler::~ATSC95MessageHandler(void)
{
}

void ATSC95MessageHandler::setInputData(const char* data,unsigned int length)
{
	m_inputData.erase(m_inputData.begin(),m_inputData.end());
	memcpy((void*)m_inputData.c_str(),data,length);
}

void ATSC95MessageHandler::setOutputData(const char* data,unsigned int length)
{
	;
}

bool ATSC95MessageHandler::DoAnalyze()
{

	if(!IsInputAvailable())
	{
		return false;
	}
	m_funcCode = m_inputData[1];
	m_startAddr[0] = m_inputData[2];
	m_startAddr[1] = m_inputData[3];
	m_iStartAddr=(m_startAddr[0]<<8)+m_startAddr[1];
	switch(m_funcCode)
	{
	case CODE_CMD_READ:
		return handleReadPointTable();
		break;
	case CODE_CMD_WRITE:
		return handleATSReply();
		break;
	default:
		NotifyUI("Error","Known type of message");
		break;
	}
	return false;
}

bool ATSC95MessageHandler::handleReadPointTable()
{
	m_wordCount[0] = m_inputData[4];
	m_wordCount[1] = m_inputData[5];
	m_outputData.erase(m_outputData.begin(),m_outputData.end());
	m_iWordCount=(m_wordCount[0]<<8)+m_wordCount[1];
	if(m_iStartAddr==m_baseAddr && m_iWordCount==1)
		return setWatchDogData(); 
	else
		return setReplyPointTable();
}

bool ATSC95MessageHandler::setWatchDogData()
{
	m_outputData.push_back((char)0x00);//LUCKY
	m_outputData.push_back((char)CODE_CMD_READ);
	m_outputData.push_back(m_iWordCount*2);
	m_watchDogCount+=1;
	if(m_watchDogCount ==0xffff)
		m_watchDogCount=1;
	m_outputData.push_back(HIBYTE(m_watchDogCount));
	m_outputData.push_back(LOBYTE(m_watchDogCount));
	m_outputData.push_back(0);
	m_outputData.push_back(0);
	return true;
}

bool ATSC95MessageHandler::setReplyPointTable()
{
	if(m_dataPointTable.empty())
	{
		NotifyUI("Error","Point Table is empty");
		return false;
	}
	if(m_iStartAddr < m_baseAddr)
	{
		NotifyUI("Error","The requested start address is smaller than BaseATSAddress");
		return false;
	}
	if((m_iStartAddr-m_baseAddr)*2 + m_iWordCount*2 > m_dataPointTable.length())
	{
		NotifyUI("Error","The requested start address and word-count extent PointTable data size limit");
		return false;
	}
	
	m_outputData.push_back((char)0x00); //bihui 14/02/2011
	m_outputData.push_back((char)CODE_CMD_READ);

//	m_outputData.push_back(HIBYTE(m_iWordCount*2));
//	m_outputData.push_back(LOBYTE(m_iWordCount*2));
	m_outputData.push_back(0);
	m_outputData.append(m_dataPointTable,(m_iStartAddr-m_baseAddr)*2,m_iWordCount*2);
	m_outputData.push_back(0);
	m_outputData.push_back(0);
	return true;
}

bool ATSC95MessageHandler::handleATSReply()
{
	m_outputData.erase(m_outputData.begin(),m_outputData.end());
	for(int i=0;i<6;i++)
	{
		m_outputData.push_back(m_inputData[i]);
	}
	m_outputData.push_back(0);
	m_outputData.push_back(0);
	return true;
}

bool ATSC95MessageHandler::IsInputAvailable()
{
	if(m_inputData.length()<8 )
	{
		NotifyUI("Error","Input data is too short");
		return false;
	}
	return true;
}

void ATSC95MessageHandler::NotifyUI(const string& caption,const string& msg)
{
	::SendMessage(m_hWnd,m_msgNotification,(WPARAM)&caption,(LPARAM)&msg);
}
