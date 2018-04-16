
#ifndef _SWITCH_STATUS_DATA_H_
#define _SWITCH_STATUS_DATA_H_

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsSCADAData.h"
#include <string>

namespace TA_IRS_Bus
{
	class SwitchData : public AtsSCADAData
	{
	public:
		SwitchData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~SwitchData();

		virtual void updateSwitchStatus(SwtichInfo& switchInfo);
		void initializeDataPoint();
		void updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality);

	private:
		/**
		 * Disable default,copy and assignment constructor
		 */
		SwitchData();
		SwitchData(const SwitchData&);
		SwitchData& operator= (const SwitchData&);

		void updatePositionDP(SwtichInfo* switchInfo);
		void updateFailedStatusDP(SwtichInfo* switchInfo);
		void updateResStatusDP(SwtichInfo* switchInfo);

	private:
		std::string m_positionDpAddr;
		std::string m_failedStatusDpAddr;
		std::string m_resStatusDpAddr;

	};
}
#endif