#pragma once
#include "MessageHandler.h"
#include "HexStringHelper.h"
#define CODE_CMD_READ	0x04
#define CODE_CMD_WRITE	0x10
class ATSC95MessageHandler : public IMessageHandler
{
public:
	ATSC95MessageHandler(HWND hWnd,unsigned long msgNotification,int baseAddr);
public:
	~ATSC95MessageHandler(void);
	
	void setInputData(const char* data,unsigned int length) ;
	void setOutputData(const char* data,unsigned int length);
	bool DoAnalyze();
	void NotifyUI(const string& caption,const string& msg);
	inline void setDataPointTable(const string& strData) { m_dataPointTable = strData;};
private:
	bool IsInputAvailable();
	bool handleReadPointTable();
	bool setWatchDogData();
	bool setReplyPointTable();
	bool handleATSReply();
private:
	HWND m_hWnd;
	int m_baseAddr;
	unsigned long m_msgNotification;
	unsigned char m_funcCode,m_startAddr[2],m_byteCount,m_wordCount[2];
	int m_iStartAddr,m_iByteCount,m_iWordCount;
	int m_atsStatuas;
	unsigned int m_watchDogCount; 
	string m_dataPointTable;
	HexStringHelper m_hexStringHelper;
};
