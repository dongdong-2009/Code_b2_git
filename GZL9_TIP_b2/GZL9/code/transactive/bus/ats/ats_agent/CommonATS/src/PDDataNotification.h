/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\TrainPISDataNotification.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling Train PIS Information which will be used for data processing.
  */
#ifndef _PLATFORM_DOOR_DATANOTIFICATION_H_
#define _PLATFORM_DOOR_DATANOTIFICATION_H_

#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include <vector>

namespace TA_IRS_Bus
{
	class PDDataNotification : public DataNotification
	{
	public:
		/**
		 * Constructor
		 */
		PDDataNotification(void* paData);

		/**
		 * Destructor
		 */
		virtual ~PDDataNotification();
		
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
		 void processAtsData(){};

		 void updatePDDatapointQuality(TA_Base_Core::EDataPointQuality quality);

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		PDDataNotification();
		PDDataNotification(const PDDataNotification&);
		PDDataNotification& operator= (const PDDataNotification&);

		/**
		 * getTrainPlatformInfo
		 *
		 * This method returns PlatformDoor information
		 * 
		 * @param PlatformInfoVec&  : returns PlatformInfoVec
		 */
		PlatformDoorInfoVec& getTrainPlatformDoorInfo();

		PlatformDoorInfoVec m_platformDoorInfoData;
	};

	typedef boost::shared_ptr< PDDataNotification > PDDataNotificationPtr;
}
#endif