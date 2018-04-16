/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\EndOfTtisDownload.h$
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/11/20 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is used for sending the status of End of TTIS Library Download 
  */

#ifndef _END_OF_TTIS_DOWNLOAD_H_
#define _END_OF_TTIS_DOWNLOAD_H_
#include "app/radio/radio_simulator/CommandProcessorLib/src/TimsCommand.h"

namespace TA_IRS_App
{
	class EndOfttisDownload : public TimsCommand
	{
	public:
		EndOfttisDownload();
		~EndOfttisDownload();

		//Interface for TimsCommand
		int getCommandType() const;
		std::vector<unsigned char> getCommandData();
		std::string getCommandLog() const;
		void constructData();

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		EndOfttisDownload (const EndOfttisDownload&){};
		EndOfttisDownload& operator= (const EndOfttisDownload& ){};

		static const char COMMAND;
		static const char LENGTH;
	};
}
#endif