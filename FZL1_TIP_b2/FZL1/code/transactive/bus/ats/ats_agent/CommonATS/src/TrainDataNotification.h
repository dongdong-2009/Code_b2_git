/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\TrainDataNotification.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling Train Data Information which will be used for data processing.
  */

#ifndef _TRAIN_DATA_NOTIFICATION_H_
#define	_TRAIN_DATA_NOTIFICATION_H_

#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"

namespace TA_IRS_Bus
{
	class TrainDataNotification : public DataNotification
	{
	public:
		/**
		 * Constructor
		 */
		TrainDataNotification(void* trainData);
		/**
		 * Destructor
		 */
		virtual ~TrainDataNotification();

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

		 /**
		 * updateTrainDatapointQuality
		 *
		 * Method to use to update Train Datapoint quality
		 * 
		 * @param quality : Data Block Quality
		 */
		void updateTrainDatapointQuality(TA_Base_Core::EDataPointQuality quality);

	
	private:
		/**
		 * Disable copy and assignment constructor
		 */
		TrainDataNotification();
		TrainDataNotification(const TrainDataNotification&);
		TrainDataNotification& operator= (const TrainDataNotification&);

		TrainInfoVec  m_trainInfoVec;

		/**
		 * getTrainInfo
		 *
		 * This method returns Train Information
		 * 
		 * @param TrainInfo&  : Train information object
		 */
		TrainInfoVec& getTrainInfo();
	};

	typedef boost::shared_ptr< TrainDataNotification > TrainDataNotificationPtr;
}
#endif