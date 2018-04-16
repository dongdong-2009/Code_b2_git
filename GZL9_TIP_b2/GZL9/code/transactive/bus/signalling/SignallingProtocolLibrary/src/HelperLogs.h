/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\GZL6_TIP\3002\transactive\app\ats\AtsAgent\SignallingProcessors\src\HelperLogs.h$
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/10/31 02:37:50 $
  * Last modified by:  $Author: builder $
  *
  * This is a helper class which used to print the logs.
  */

#ifndef HELPER_LOGS_H_
#define HELPER_LOGS_H_
#include <vector>
#include <string>

namespace TA_IRS_Bus
{
	class HelperLogs
	{
	public:
		/**
		 * printRawData
		 *
		 * This will print the raw data information in the logs
		 *
		 * @param : const std::vector<unsigned char>& rawdata  - Raw data information
		 * @param : const std::string& sourceMsg               - The source of the logs
		 *
		 */
		static void printRawData(const std::vector<unsigned char>& rawdata, const std::string& sourceMsg);

	private:
		/**
		 * Disable default, copy and assignment operator
		 */
		HelperLogs();
		HelperLogs( const HelperLogs& );
		HelperLogs& operator= (const HelperLogs& );
		
	};
}
#endif