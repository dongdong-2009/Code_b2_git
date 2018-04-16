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
#ifndef _TRAIN_PIS_DATANOTIFICATION_H_
#define _TRAIN_PIS_DATANOTIFICATION_H_

#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include <vector>

namespace TA_IRS_Bus
{
	class TrainPISDataNotification : public DataNotification
	{
	public:
		/**
		 * Constructor
		 */
		TrainPISDataNotification(void* paData);

		/**
		 * Destructor
		 */
		virtual ~TrainPISDataNotification();
		
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

		 void updatePISDatapointQuality(TA_Base_Core::EDataPointQuality quality);

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		TrainPISDataNotification();
		TrainPISDataNotification(const TrainPISDataNotification&);
		TrainPISDataNotification& operator= (const TrainPISDataNotification&);

		/**
		 * getTrainPISInfo
		 *
		 * This method returns Train PIS information
		 * 
		 * @param PISInfoVec&  : returns PISInfoVec
		 */
		PISInfoVec& getTrainPISInfo();

		PISInfoVec m_pisInfoData;
	};

	typedef boost::shared_ptr< TrainPISDataNotification > TrainPISDataNotificationPtr;
}
#endif