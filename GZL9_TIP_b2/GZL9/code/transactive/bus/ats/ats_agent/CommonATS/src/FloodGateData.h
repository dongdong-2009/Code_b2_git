
#ifndef _FLOODGATE_STATUS_DATA_H_
#define _FLOODGATE_STATUS_DATA_H_

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsSCADAData.h"
#include <string>

namespace TA_IRS_Bus
{
	class FloodGateData : public AtsSCADAData
	{
	public:
		FloodGateData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~FloodGateData();

		virtual void updateFloodGateStatus(FloodGateInfo& floodgateInfo);
		void initializeDataPoint();
		void updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality);

	private:
		/**
		 * Disable default,copy and assignment constructor
		 */
		FloodGateData();
		FloodGateData(const FloodGateData&);
		FloodGateData& operator= (const FloodGateData&);
		void updateRequestCloseDP(FloodGateInfo* floodgateInfo);
		void updatePermissionCloseDP(FloodGateInfo* floodgateInfo);

	private:
		std::string m_RequestCloseDpAddr;
		std::string m_PermissionCloseDpAddr;

	};
}
#endif