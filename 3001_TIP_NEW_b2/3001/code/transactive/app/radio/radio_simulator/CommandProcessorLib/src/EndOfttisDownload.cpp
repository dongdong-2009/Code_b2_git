/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\EndOfTtisDownload.cpp$
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/11/20 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This is class is used for sending the status of End of TTIS Library Download 
  */

#include "app/radio/radio_simulator/CommandProcessorLib/src/EndOfTtisDownload.h"

namespace TA_IRS_App
{
	const char EndOfttisDownload::COMMAND = 0x27;
	const char EndOfttisDownload::LENGTH  = 0x08;

	EndOfttisDownload::EndOfttisDownload()
	{

	}

	EndOfttisDownload::~EndOfttisDownload()
	{

	}

	int EndOfttisDownload::getCommandType() const
	{
		return static_cast<int>(COMMAND);
	}

	void EndOfttisDownload::constructData()
	{
		//make sure that the data is clear
		m_commandData.clear();

		m_commandData.push_back(FLAG);
		m_commandData.push_back(COMMAND);
		m_commandData.push_back(m_trainID);
		m_commandData.push_back(LENGTH);
		m_commandData.push_back(ORIGIN);
		m_commandData.push_back(m_status);
	}
	
	std::string EndOfttisDownload::getCommandLog() const
	{
		std::string _ret = "Sending End of TTIS Download...";
		return _ret;
	}
}