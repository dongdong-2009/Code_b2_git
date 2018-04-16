
#include "app/radio/radio_simulator/CommandProcessorLib/src/RequestOccCall.h"

namespace TA_IRS_App
{
	const char RequestOccCall::COMMAND = 0x50;
	const char RequestOccCall::LENGTH  = 0x07;
	const char RequestOccCall::REQUEST_ORIGIN = 0x09;

	RequestOccCall::RequestOccCall()
	{

	}

	RequestOccCall::~RequestOccCall()
	{

	}

	int RequestOccCall::getCommandType() const
	{
		return static_cast<int>(COMMAND);
	}

	void RequestOccCall::constructData()
	{
		//make sure that the data is clear
		m_commandData.clear();

		m_commandData.push_back(FLAG);
		m_commandData.push_back(COMMAND);
		m_commandData.push_back(m_trainID);
		m_commandData.push_back(LENGTH);
		m_commandData.push_back(REQUEST_ORIGIN);
	
	}

	std::string RequestOccCall::getCommandLog() const
	{
		std::string _ret = "Sending RequestOccCall...";
		return _ret;
	}
}