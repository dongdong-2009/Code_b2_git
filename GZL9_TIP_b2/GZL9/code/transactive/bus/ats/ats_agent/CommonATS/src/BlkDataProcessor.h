
#ifndef _BLK_DATA_PROCESSOR_H_
#define _BLK_DATA_PROCESSOR_H_

#include "bus/ats/ats_agent/CommonATS/src/DataProcessorThread.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/BlkDataNotification.h"

namespace TA_IRS_Bus
{
	class BlkDataProcessor : public DataProcessorThread
	{
	public:
		BlkDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~BlkDataProcessor();

		void processItem(DataNotificationPtr item);
		void processAtsData(DataNotificationPtr atsDataNotif);
		
	private:
		/**
		 * Disable copy and assignment constructor
		 */
		BlkDataProcessor(const BlkDataProcessor&);
		BlkDataProcessor& operator= (const BlkDataProcessor&);

		void processBlkData(BlkDataNotificationPtr blkDataPtr);
		/**
		 * processPADataItem
		 *
		 * This will process Train PIS Data item from the Queue
		 *
		 * @param item : Train Data Item to process
		 */
		void processBlkDataItem(DataNotificationPtr item);

		//Keep last dataQuality, in order to handle from bad to good.
		TA_Base_Core::EDataPointQuality m_previousDataQuality;  
	};
}
#endif