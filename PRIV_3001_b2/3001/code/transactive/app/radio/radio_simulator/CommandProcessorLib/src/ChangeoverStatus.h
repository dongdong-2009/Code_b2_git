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

#pragma once
#ifndef _CHANGEOVER_STATUS_H_
#define _CHANGEOVER_STATUS_H_

#include "app/radio/radio_simulator/CommandProcessorLib/src/ICommandProcessor.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/TimsCommand.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/TrainCommandProcessorThread.h"

namespace TA_IRS_App
{
	class ChangeoverStatus : public TimsCommand
	{
	public:
		ChangeoverStatus();
		~ChangeoverStatus();
// 
//		Interface for TimsCommand
		int getCommandType() const;
		std::vector<unsigned char> getCommandData();
		std::string getCommandLog() const;
		void constructData();


	private:
		/**
		 * Disable copy and assignment constructor
		 */
		ChangeoverStatus (const ChangeoverStatus&){};
		ChangeoverStatus& operator= (const ChangeoverStatus& ){};

		static const char COMMAND;
		static const char LENGTH;
		static const char STATUS8;

	};
}
#endif