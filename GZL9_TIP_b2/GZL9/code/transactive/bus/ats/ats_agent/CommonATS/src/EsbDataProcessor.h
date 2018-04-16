
#ifndef _ESB_DATA_PROCESSOR_H_
#define _ESB_DATA_PROCESSOR_H_

#include "bus/ats/ats_agent/CommonATS/src/DataProcessorThread.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/EsbDataNotification.h"

namespace TA_IRS_Bus
{
	class EsbDataProcessor : public DataProcessorThread
	{
	public:
		EsbDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~EsbDataProcessor();

		void processItem(DataNotificationPtr item);
		void processAtsData(DataNotificationPtr atsDataNotif);
		
	private:
		/**
		 * Disable copy and assignment constructor
		 */
		EsbDataProcessor(const EsbDataProcessor&);
		EsbDataProcessor& operator= (const EsbDataProcessor&);

		void processEsbData(EsbDataNotificationPtr esbDataPtr);
		/**
		 * processPADataItem
		 *
		 * This will process Train PIS Data item from the Queue
		 *
		 * @param item : Train Data Item to process
		 */
		void processEsbDataItem(DataNotificationPtr item);

		//Keep last dataQuality, in order to handle from bad to good.
		TA_Base_Core::EDataPointQuality m_previousDataQuality;  
	};
}
#endif