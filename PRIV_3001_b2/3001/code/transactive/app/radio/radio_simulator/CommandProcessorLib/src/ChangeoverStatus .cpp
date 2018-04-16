/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\ChangeoverStatus.h$
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/11/20 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This is class is used for sending the status of Changeover Status 
  */

#include "app/radio/radio_simulator/CommandProcessorLib/src/ChangeoverStatus.h"

#include "..\..\TrainSimulator\EtcTrainSimConfigHandler.h"
#include "..\..\TrainSimulator\AutoSendDataModel.h"

namespace TA_IRS_App
{
	const char ChangeoverStatus::COMMAND = 0x41;
	const char ChangeoverStatus::LENGTH  = 0x09;
	const char ChangeoverStatus::STATUS8 = 0;

	ChangeoverStatus::ChangeoverStatus()
	{

	}

	ChangeoverStatus::~ChangeoverStatus()
	{

	}

	int ChangeoverStatus::getCommandType() const
	{
		return static_cast<int>(COMMAND);
	}

	void ChangeoverStatus::constructData()
	{
		//make sure that the data is clear
		m_commandData.clear();

		m_commandData.push_back(FLAG);
		m_commandData.push_back(COMMAND);
		m_commandData.push_back(m_trainID);
		m_commandData.push_back(LENGTH);
		m_commandData.push_back(ORIGIN);

		IConfigDataItemPtrType autoSendSharedPtr = EtcTrainSimConfigHandler::getInstance()->getAutoSendingData(EAutoSendType::EChangeOver);
		ChangeOverConfig * autoSendPtr = (ChangeOverConfig *)autoSendSharedPtr.get();
		
		m_commandData.push_back(autoSendPtr->getStatusFailure());
		m_commandData.push_back(m_status);
	}

	std::string ChangeoverStatus::getCommandLog() const
	{
		std::string _ret = "Sending Changeover Status...";
		return _ret;
	}

}