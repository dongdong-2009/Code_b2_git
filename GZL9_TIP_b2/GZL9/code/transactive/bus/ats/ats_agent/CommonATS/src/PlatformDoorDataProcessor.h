/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\TrainPISDataProcessor.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling Train PIS data processing
  */
#ifndef _PLATFORM_DOOR_DATA_PROCESSOR_H_
#define _PLATFORM_DOOR_DATA_PROCESSOR_H_

#include "bus/ats/ats_agent/CommonATS/src/DataProcessorThread.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/PDDataNotification.h"

namespace TA_IRS_Bus
{
	class PlatformDoorDataProcessor : public DataProcessorThread
	{
	public:
		PlatformDoorDataProcessor(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~PlatformDoorDataProcessor();

		void processItem(DataNotificationPtr item);
		void processAtsData(DataNotificationPtr atsDataNotif);
		
	private:
		/**
		 * Disable copy and assignment constructor
		 */
		PlatformDoorDataProcessor(const PlatformDoorDataProcessor&);
		PlatformDoorDataProcessor& operator= (const PlatformDoorDataProcessor&);

		void processPlatformDoorData(PDDataNotificationPtr platformDoorDataPtr);
		/**
		 * processPlatformDoorDataItem
		 *
		 * This will process PlatformDoor Data item from the Queue
		 *
		 * @param item : Train Data Item to process
		 */
		void processPlatformDoorDataItem(DataNotificationPtr item);

		//Keep last dataQuality, in order to handle from bad to good.
		TA_Base_Core::EDataPointQuality m_previousDataQuality; 
	};
}

#endif