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
#ifndef _GENA_PLATFORM_DATA_H_
#define _GENA_PLATFORM_DATA_H_

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsSCADAData.h"
#include <string>

namespace TA_IRS_Bus
{
	class PlatformData : public AtsSCADAData
	{
	public:
		PlatformData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~PlatformData();

		void updatePlatformData(const PlatformInfoVec& platformInfo);
		void initializeDatapoint();
		void updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality);

	private:
		/**
		 * Disable default,copy and assignment constructor
		 */
		PlatformData();
		PlatformData(const PlatformData&);
		PlatformData& operator= (const PlatformData&);

		void updateSkipStatusDp(const PlatformInfo& platformInfo);
		void updateHoldStatusDp(const PlatformInfo& platformInfo);
		
		std::string m_skipStatusDpAddr;
		std::string m_holdStatusDpAddr;

	};
}

#endif