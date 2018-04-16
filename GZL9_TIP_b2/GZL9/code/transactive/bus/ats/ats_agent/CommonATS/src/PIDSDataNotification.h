/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\PIDSDataNotification.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling PIDS Information which will be used for data processing.
  */

#ifndef _PIDS_DATA_NOTIFICATION_H_
#define _PIDS_DATA_NOTIFICATION_H_

#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"

namespace TA_IRS_Bus
{
	class PIDSDataNotification : public DataNotification
	{
	public:
		/**
		 * Constructor
		 */
		PIDSDataNotification(const RawTable& rawTable);
		/**
		 * Destructor
		 */
		virtual ~PIDSDataNotification();

		/**
		 * updateAtsData
		 *
		 * Method that will update the Ats Data
		 * 
		 */
		 void updateAtsData();

		 /**
		 * processAtsData
		 *
		 * Method that will update the Ats Data
		 * 
		 */
		 void processAtsData();

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		PIDSDataNotification();
		PIDSDataNotification(const PIDSDataNotification&);
		PIDSDataNotification& operator= (const PIDSDataNotification&);

		/**
		 * getRawTable
		 *
		 * This method returns raw table information
		 * 
		 * @param RawTable&  : RawTable object
		 */
		RawTable& getRawTable();

		RawTable m_rawTable;
	};
	typedef boost::shared_ptr< PIDSDataNotification > PIDSDataNotificationPtr;
}
#endif