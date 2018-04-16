/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\PADataNotification.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling PA Information which will be used for data processing.
  */

#ifndef _PA_DATA_NOTIFICATION_H_
#define _PA_DATA_NOTIFICATION_H_

#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include <vector>

namespace TA_IRS_Bus
{
	class PADataNotification : public DataNotification
	{
	public:
		/**
		 * Constructor
		 */
		PADataNotification(void* paData);

		/**
		 * Destructor
		 */
		virtual ~PADataNotification();
		
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
		PADataNotification();
		PADataNotification(const PADataNotification&);
		PADataNotification& operator= (const PADataNotification&);

		/**
		 * getPAStationInfo
		 *
		 * This method returns PA Station information
		 * 
		 * @param PAStationInfo&  : returns PAStationInfo
		 */
		PAStationInfoVec& getPAStationInfo();

		PAStationInfoVec m_paStationInfoData;
	};
	typedef boost::shared_ptr< PADataNotification > PADataNotificationPtr;
}
#endif