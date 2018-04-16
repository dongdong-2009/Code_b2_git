

#ifndef _ESB_DATA_NOTIFICATION_H_
#define _ESB_DATA_NOTIFICATION_H_

#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include <vector>

namespace TA_IRS_Bus
{
	class EsbDataNotification : public DataNotification
	{
	public:
		/**
		 * Constructor
		 */
		EsbDataNotification(void* esbData);

		/**
		 * Destructor
		 */
		virtual ~EsbDataNotification();
		
		/**
		 * updateAtsData
		 *
		 * Method that will update the Ats Data
		 * 
		 */
		 void updateAtsData();

		  /**
		 * updateEsbDatapointQuality
		 *
		 * Method to use to update Emergency Stop Button Datapoint quality
		 * 
		 * @param quality : Data Emergency Stop Button Data Quality
		 */
		void updateEsbDatapointQuality(TA_Base_Core::EDataPointQuality quality);

		 /**
		 * processAtsData
		 *
		 * Method that will update the Ats Data
		 * 
		 */
		 void processAtsData(){};


		 /**
		 * getEsbInfo
		 *
		 * This method returns Emergency Stop Button information
		 * 
		 * @param SignalInfo&  : returns EsbInfo
		 */
		 EsbInfo& getEsbInfo();

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		EsbDataNotification();
		EsbDataNotification(const EsbDataNotification&);
		EsbDataNotification& operator= (const EsbDataNotification&);

		EsbInfo m_esbInfoData;
	};

	typedef boost::shared_ptr< EsbDataNotification > EsbDataNotificationPtr;
}
#endif