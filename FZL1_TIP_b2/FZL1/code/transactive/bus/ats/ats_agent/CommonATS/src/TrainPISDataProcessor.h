/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\TrainPISDataProcessor.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling Train PIS data processing
  */
#ifndef _TRAIN_PIS_DATA_PROCESSOR_H_
#define _TRAIN_PIS_DATA_PROCESSOR_H_

#include "bus/ats/ats_agent/CommonATS/src/DataProcessorThread.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainPISDataNotification.h"

namespace TA_IRS_Bus
{
	class TrainPISDataProcessor : public DataProcessorThread
	{
	public:
		TrainPISDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~TrainPISDataProcessor();

		void processItem(DataNotificationPtr item);
		void processAtsData(DataNotificationPtr atsDataNotif);
		
	private:
		/**
		 * Disable copy and assignment constructor
		 */
		TrainPISDataProcessor(const TrainPISDataProcessor&);
		TrainPISDataProcessor& operator= (const TrainPISDataProcessor&);

		void processTrainPISData(TrainPISDataNotificationPtr trainDataPtr);
		/**
		 * processTrainPISDataItem
		 *
		 * This will process Train PIS Data item from the Queue
		 *
		 * @param item : Train Data Item to process
		 */
		void processTrainPISDataItem(DataNotificationPtr item);
	};
}

#endif