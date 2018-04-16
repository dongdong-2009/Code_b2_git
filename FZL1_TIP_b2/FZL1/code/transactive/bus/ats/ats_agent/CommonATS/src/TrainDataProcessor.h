/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\TrainDataProcessor.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling Train data processing
  */
#ifndef _TRAIN_DATA_PROCESSOR_H_
#define _TRAIN_DATA_PROCESSOR_H_

#include "bus/ats/ats_agent/CommonATS/src/DataProcessorThread.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainDataNotification.h"

namespace TA_IRS_Bus
{
	class TrainDataProcessor : public DataProcessorThread
	{
	public:
		TrainDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~TrainDataProcessor();

		void processItem(DataNotificationPtr item);
		void processAtsData(DataNotificationPtr atsDataNotif);
		
	private:
		/**
		 * Disable copy and assignment constructor
		 */
		TrainDataProcessor(const TrainDataProcessor&);
		TrainDataProcessor& operator= (const TrainDataProcessor&);

		void processTrainData(TrainDataNotificationPtr trainDataPtr);
		/**
		 * processTrainDataItem
		 *
		 * This will process Train data item from the Queue
		 *
		 * @param item : Train Data Item to process
		 */
		void processTrainDataItem(DataNotificationPtr item);
   
        //Keep last dataQuality, in order to handle from bad to good.
		TA_Base_Core::EDataPointQuality m_previousDataQuality;  
	};
}
#endif