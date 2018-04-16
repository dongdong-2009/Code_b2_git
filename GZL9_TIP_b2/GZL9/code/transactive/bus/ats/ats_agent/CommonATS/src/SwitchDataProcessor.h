
#ifndef _SWITCH_DATA_PROCESSOR_H_
#define _SWITCH_DATA_PROCESSOR_H_

#include "bus/ats/ats_agent/CommonATS/src/DataProcessorThread.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/SwitchDataNotification.h"

namespace TA_IRS_Bus
{
	class SwitchDataProcessor : public DataProcessorThread
	{
	public:
		SwitchDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~SwitchDataProcessor();

		void processItem(DataNotificationPtr item);
		void processAtsData(DataNotificationPtr atsDataNotif);
		
	private:
		/**
		 * Disable copy and assignment constructor
		 */
		SwitchDataProcessor(const SwitchDataProcessor&);
		SwitchDataProcessor& operator= (const SwitchDataProcessor&);

		void processSwitchData(SwitchDataNotificationPtr switchDataPtr);
		/**
		 * processPADataItem
		 *
		 * This will process Train PIS Data item from the Queue
		 *
		 * @param item : Train Data Item to process
		 */
		void processSwitchDataItem(DataNotificationPtr item);

		//Keep last dataQuality, in order to handle from bad to good.
		TA_Base_Core::EDataPointQuality m_previousDataQuality;  
	};
}
#endif