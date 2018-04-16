

#ifndef _SWITCH_DATA_NOTIFICATION_H_
#define _SWITCH_DATA_NOTIFICATION_H_

#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include <vector>

namespace TA_IRS_Bus
{
	class SwitchDataNotification : public DataNotification
	{
	public:
		/**
		 * Constructor
		 */
		SwitchDataNotification(void* switchData);

		/**
		 * Destructor
		 */
		virtual ~SwitchDataNotification();
		
		/**
		 * updateAtsData
		 *
		 * Method that will update the Ats Data
		 * 
		 */
		 void updateAtsData();

		  /**
		 * updateTrainDatapointQuality
		 *
		 * Method to use to update Train Datapoint quality
		 * 
		 * @param quality : Data Block Quality
		 */
		void updateSwitchDatapointQuality(TA_Base_Core::EDataPointQuality quality);

		 /**
		 * processAtsData
		 *
		 * Method that will update the Ats Data
		 * 
		 */
		 void processAtsData(){};


		 /**
		 * getSwitchInfo
		 *
		 * This method returns Switch information
		 * 
		 * @param SwtichInfo&  : returns SwitchInfo
		 */
		 SwtichInfo& getSwitchInfo();

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		SwitchDataNotification();
		SwitchDataNotification(const SwitchDataNotification&);
		SwitchDataNotification& operator= (const SwitchDataNotification&);

		SwtichInfo m_switchInfoData;
	};
	typedef boost::shared_ptr< SwitchDataNotification > SwitchDataNotificationPtr;
}
#endif