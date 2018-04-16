

#ifndef _BLK_DATA_NOTIFICATION_H_
#define _BLK_DATA_NOTIFICATION_H_

#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include <vector>

namespace TA_IRS_Bus
{
	class BlkDataNotification : public DataNotification
	{
	public:
		/**
		 * Constructor
		 */
		BlkDataNotification(void* blkData);

		/**
		 * Destructor
		 */
		virtual ~BlkDataNotification();
		
		/**
		 * updateAtsData
		 *
		 * Method that will update the Ats Data
		 * 
		 */
		 void updateAtsData();

		 /**
		 * updateBlkDatapointQuality
		 *
		 * Method to use to update Block Datapoint quality
		 * 
		 * @param quality : Data Block Quality
		 */
		void updateBlkDatapointQuality(TA_Base_Core::EDataPointQuality quality);

		 /**
		 * processAtsData
		 *
		 * Method that will update the Ats Data
		 * 
		 */
		 void processAtsData(){};


		 /**
		 * getBlockInfo
		 *
		 * This method returns Block information
		 * 
		 * @param SignalInfo&  : returns BlockInfo
		 */
		 BlkInfo& getBlockInfo();

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		BlkDataNotification();
		BlkDataNotification(const BlkDataNotification&);
		BlkDataNotification& operator= (const BlkDataNotification&);

	private:
		BlkInfo m_blkInfoData;
	};
	typedef boost::shared_ptr< BlkDataNotification > BlkDataNotificationPtr;
}
#endif