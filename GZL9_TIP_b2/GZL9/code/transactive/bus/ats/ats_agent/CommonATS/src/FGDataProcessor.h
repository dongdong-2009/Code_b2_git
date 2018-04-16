
#ifndef _FG_DATA_PROCESSOR_H_
#define _FG_DATA_PROCESSOR_H_

#include "bus/ats/ats_agent/CommonATS/src/DataProcessorThread.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/FGDataNotification.h"

namespace TA_IRS_Bus
{
	class FGDataProcessor : public DataProcessorThread
	{
	public:
		FGDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~FGDataProcessor();

		void processItem(DataNotificationPtr item);
		void processAtsData(DataNotificationPtr atsDataNotif);
		
	private:
		/**
		 * Disable copy and assignment constructor
		 */
		FGDataProcessor(const FGDataProcessor&);
		FGDataProcessor& operator= (const FGDataProcessor&);

		void processFGData(FGDataNotificationPtr fgDataPtr);
		/**
		 * processFGDataItem
		 *
		 * This will process Flood Gate Data item from the Queue
		 *
		 * @param item : Train Data Item to process
		 */
		void processFGDataItem(DataNotificationPtr item);

		//Keep last dataQuality, in order to handle from bad to good.
		TA_Base_Core::EDataPointQuality m_previousDataQuality;  
	};
}
#endif