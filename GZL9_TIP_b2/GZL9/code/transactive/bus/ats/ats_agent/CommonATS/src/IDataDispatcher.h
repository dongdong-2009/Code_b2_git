/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\IDataDispatcher.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This interface will be used by the Data Dispatchers( E.g PA Dispatcher, Train Dispatcher or etc.)
  * It use to notify Data Dispatchers to send updates
  */

#ifndef _I_DATA_DISPATCHER_H_
#define _I_DATA_DISPATCHER_H_

namespace TA_IRS_Bus
{
	class IDataDispatcher
	{
	public:
		/**
		 * sendDataUpdate
		 *
		 * Interface used to send Ats data
		 * 
		 */
		virtual void sendDataUpdate() = 0;
	};
}
#endif