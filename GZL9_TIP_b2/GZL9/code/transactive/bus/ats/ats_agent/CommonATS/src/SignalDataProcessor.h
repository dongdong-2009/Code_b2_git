
#ifndef _SIG_DATA_PROCESSOR_H_
#define _SIG_DATA_PROCESSOR_H_

#include "bus/ats/ats_agent/CommonATS/src/DataProcessorThread.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/SignalDataNotification.h"

namespace TA_IRS_Bus
{
	class SignalDataProcessor : public DataProcessorThread
	{
	public:
		SignalDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~SignalDataProcessor();

		void processItem(DataNotificationPtr item);
		void processAtsData(DataNotificationPtr atsDataNotif);
		
	private:
		/**
		 * Disable copy and assignment constructor
		 */
		SignalDataProcessor(const SignalDataProcessor&);
		SignalDataProcessor& operator= (const SignalDataProcessor&);

		void processSIGData(SignalDataNotificationPtr paDataPtr);
		/**
		 * processPADataItem
		 *
		 * This will process Train PIS Data item from the Queue
		 *
		 * @param item : Train Data Item to process
		 */
		void processSIGDataItem(DataNotificationPtr item);

		//Keep last dataQuality, in order to handle from bad to good.
		TA_Base_Core::EDataPointQuality m_previousDataQuality;  
	};
}
#endif