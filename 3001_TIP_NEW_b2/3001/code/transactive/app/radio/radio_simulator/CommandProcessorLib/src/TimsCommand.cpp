/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\TimsCommand.cpp $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/11/20 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * Interface class for the Tims Command.
  */
#include "app/radio/radio_simulator/CommandProcessorLib/src/TimsCommand.h"

namespace TA_IRS_App
{
	const char TimsCommand::FLAG   = 0x07;
	const char TimsCommand::ORIGIN = 0x00;

	TimsCommand::TimsCommand()
	{

	}

	TimsCommand::~TimsCommand()
	{

	}

	int TimsCommand::getTrainID() const
	{
		return m_trainID;
	}

	int TimsCommand::getStatusID() const
	{
		return m_status;
	}

	void TimsCommand::setTrainID(int trainID)
	{
		m_trainID = trainID;
	}

	void TimsCommand::setStatusID(int statusID)
	{
		m_status = statusID;
	}

	std::vector<unsigned char> TimsCommand::getCommandData()
	{
		constructData();
		return m_commandData;
	}
}