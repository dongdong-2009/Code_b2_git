/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\TimsCommand.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/11/20 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * Interface class for the Tims Command.
  */

#ifndef _TIMS_COMMAND_H_
#define _TIMS_COMMAND_H_
#include <vector>
#include <string>
#include <sstream>

namespace TA_IRS_App
{
	class TimsCommand
	{
	public:
		TimsCommand();
		virtual ~TimsCommand();

		virtual int getCommandType() const = 0;
		virtual void constructData() = 0;
		virtual std::string getCommandLog() const = 0;

		int getTrainID() const ;
		int getStatusID() const;
		void setTrainID(int trainID);
		void setStatusID(int statusID);
		std::vector<unsigned char> getCommandData();
		

	protected:
		int m_trainID;
		int m_status;
		std::vector<unsigned char > m_commandData;

		static const char FLAG;
		static const char ORIGIN;
	};
}
#endif