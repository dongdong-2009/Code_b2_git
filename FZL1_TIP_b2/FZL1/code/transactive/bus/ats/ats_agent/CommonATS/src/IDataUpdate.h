/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\IDataUpdate.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This interface will be used by the Data Processor.
  * It will notify the Data processor the process the data
  */

#ifndef _IDATA_UPDATE_H_
#define _IDATA_UPDATE_H_ 

#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_Bus
{
	class IDataUpdate
	{
	public:
		/**
		 * processDataDispatcher
		 *
		 * Interface used to notify Data Dispatchers to process the data
		 * 
		 @ param type : ATS Message type
		 */
		virtual void processDataDispatcher(AtsMessageType type) = 0;

		/**
		 * processDataSync
		 *
		 * Interface used to notify Data Synchronization
		 * 
		 @ param type : ATS Message type
		 */
		virtual void processDataSync(AtsMessageType type) = 0;	

		virtual void processDataNotification(DataNotificationPtr& newEvent) = 0;
	};
}
#endif