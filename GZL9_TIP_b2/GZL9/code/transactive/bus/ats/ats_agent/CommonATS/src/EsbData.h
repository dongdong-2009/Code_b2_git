
#ifndef _ESB_STATUS_DATA_H_
#define _ESB_STATUS_DATA_H_

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsSCADAData.h"
#include <string>

namespace TA_IRS_Bus
{
	class EsbData : public AtsSCADAData
	{
	public:
		EsbData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~EsbData();
		virtual void updateEsbStatus(EsbInfo& esbInfo);
		void initializeDataPoint();
		void updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality);

	private:
		/**
		 * Disable default,copy and assignment constructor
		 */
		EsbData();
		EsbData(const EsbData&);
		EsbData& operator= (const EsbData&);
		void updateEsbStatusDP(EsbInfo* esbInfo);

	private:
		std::string m_esbStatusDpAddr;
	};
}
#endif