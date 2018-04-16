/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\IAtsDataNotification.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This interface will be used to notify Ats Data event.
  */

#ifndef _I_ATS_DATA_NOTIFICATION_H_
#define _I_ATS_DATA_NOTIFICATION_H_

#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"

namespace TA_IRS_Bus
{
	class IAtsDataNotification
	{
	public:
		/**
		 * notifyAtsDataEvent
		 *
		 * Method used to notify Ats Data Event
		 * 
		 * @param event : DataNotificaiton object
		 */
		virtual void notifyAtsDataEvent(DataNotificationPtr event) = 0;
	};
}
#endif