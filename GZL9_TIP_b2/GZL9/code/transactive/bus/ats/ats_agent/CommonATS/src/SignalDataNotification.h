

#ifndef _SIG_DATA_NOTIFICATION_H_
#define _SIG_DATA_NOTIFICATION_H_

#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include <vector>

namespace TA_IRS_Bus
{
	class SignalDataNotification : public DataNotification
	{
	public:
		/**
		 * Constructor
		 */
		SignalDataNotification(void* sigData);

		/**
		 * Destructor
		 */
		virtual ~SignalDataNotification();
		
		/**
		 * updateAtsData
		 *
		 * Method that will update the Ats Data
		 * 
		 */
		 void updateAtsData();

		  /**
		 * updateSignalDatapointQuality
		 *
		 * Method to use to update Train Datapoint quality
		 * 
		 * @param quality : Data Block Quality
		 */
		void updateSignalDatapointQuality(TA_Base_Core::EDataPointQuality quality);

		 /**
		 * processAtsData
		 *
		 * Method that will update the Ats Data
		 * 
		 */
		 void processAtsData(){};


		 /**
		 * getSignalInfo
		 *
		 * This method returns Signal information
		 * 
		 * @param SignalInfo&  : returns SignalInfo
		 */
		 SignalInfo& getSignalInfo();

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		SignalDataNotification();
		SignalDataNotification(const SignalDataNotification&);
		SignalDataNotification& operator= (const SignalDataNotification&);

		SignalInfo m_signalInfoData;
	};
	typedef boost::shared_ptr< SignalDataNotification > SignalDataNotificationPtr;
}
#endif