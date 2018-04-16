#ifndef _MESSAGE_HANDLE_HEAD_FILE_
#define _MESSAGE_HANDLE_HEAD_FILE_
#include <string>
#include <iostream>
#include "SimulatorConfig.h"

using namespace std;
class IMessageHandler
{
public:
	IMessageHandler();
	virtual ~IMessageHandler();
	inline virtual string getInputData() {return m_inputData;};
	inline virtual string getOutputData() {return m_outputData;};
	inline virtual void setInputData(const string& data) 
	{ m_inputData.erase(m_inputData.begin(),m_inputData.end());
	m_inputData.append( data.c_str(),data.length());};
	inline virtual void setOutputData(const string& data) 
	{ m_outputData.erase(m_outputData.begin(),m_outputData.end());
	m_outputData.append( data.c_str(),data.length());};
	virtual bool DoAnalyze() =0;
	virtual void SetExtConfig() {};
	virtual void NotifyUI(const string& caption,const string& msg) = 0;

protected:
	string m_inputData,m_outputData;
	std::list<std::string> m_extInfo;
};
#endif