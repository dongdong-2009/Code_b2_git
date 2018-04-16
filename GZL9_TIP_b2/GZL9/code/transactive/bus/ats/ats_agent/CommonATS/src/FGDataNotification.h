

#ifndef _FLOODGATE_DATA_NOTIFICATION_H_
#define _FLOODGATE_DATA_NOTIFICATION_H_

#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include <vector>

namespace TA_IRS_Bus
{
	class FGDataNotification : public DataNotification
	{
	public:
		/**
		 * Constructor
		 */
		FGDataNotification(void* fgData);

		/**
		 * Destructor
		 */
		virtual ~FGDataNotification();
		
		/**
		 * updateAtsData
		 *
		 * Method that will update the Ats Data
		 * 
		 */
		 void updateAtsData();

		  /**
		 * updateFGDatapointQuality
		 *
		 * Method to use to update Flood Gate Datapoint quality
		 * 
		 * @param quality : Data Block Quality
		 */
		void updateFGDatapointQuality(TA_Base_Core::EDataPointQuality quality);

		 /**
		 * processAtsData
		 *
		 * Method that will update the Ats Data
		 * 
		 */
		 void processAtsData(){};


		 /**
		 * getFloodGateInfo
		 *
		 * This method returns Signal information
		 * 
		 * @param SignalInfo&  : returns SignalInfo
		 */
		 FloodGateInfo& getFloodGateInfo();

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		FGDataNotification();
		FGDataNotification(const FGDataNotification&);
		FGDataNotification& operator= (const FGDataNotification&);

		FloodGateInfo m_floodgateInfoData;
	};
	typedef boost::shared_ptr< FGDataNotification > FGDataNotificationPtr;
}
#endif