/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\TrainPisData.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * An object which is responsible for handling Train PIS Datapoint
  */
#ifndef _GENA_PLATFORM_DOOR_DATA_H_
#define _GENA_PLATFORM_DOOR_DATA_H_

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include <string>

namespace TA_IRS_Bus
{
	class PlatformDoorData
	{
	public:
		PlatformDoorData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~PlatformDoorData();

		void updatePlatformDoorData(const PlatformDoorInfoVec& platformInfo);
		void initializeDatapoint();
		void updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality);

	private:
		/**
		 * Disable default,copy and assignment constructor
		 */
		PlatformDoorData();
		PlatformDoorData(const PlatformDoorData&);
		PlatformDoorData& operator= (const PlatformDoorData&);
		TA_Base_Bus::ItaScada* m_itScada;


		void updateDataPoint(unsigned long overrideStatus, const std::string& trainPisAddress);

		void updateOverrideStatusDp(const PlatformDoorInfo& platformDoorInfo);
		
		std::string m_overrideStatusAddress;

	};
}

#endif