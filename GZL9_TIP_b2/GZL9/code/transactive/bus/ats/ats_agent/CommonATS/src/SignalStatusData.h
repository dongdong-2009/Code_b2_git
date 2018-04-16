
#ifndef _SIGNAL_STATUS_DATA_H_
#define _SIGNAL_STATUS_DATA_H_

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsSCADAData.h"
#include <string>

namespace TA_IRS_Bus
{
	class SignalStatusData : public AtsSCADAData
	{
	public:
		SignalStatusData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~SignalStatusData();

		virtual void updateSignalStatus(SignalInfo& signalInfo);
		void initializeDataPoint();
		void updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality);

	private:
		/**
		 * Disable default,copy and assignment constructor
		 */
		SignalStatusData();
		SignalStatusData(const SignalStatusData&);
		SignalStatusData& operator= (const SignalStatusData&);

		std::string m_sigStatusDpAddr;
		std::string m_sigStatusModeDpAddr;

		void updateSigStatusDP(SignalInfo* signalInfo);
		void updateSigStatusModeDP(SignalInfo* signalInfo);
	};
}

#endif