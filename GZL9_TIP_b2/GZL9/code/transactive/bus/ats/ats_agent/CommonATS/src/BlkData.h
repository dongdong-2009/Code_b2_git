
#ifndef _BLOCK_DATA_H_
#define _BLOCK_DATA_H_

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsSCADAData.h"
#include <string>

namespace TA_IRS_Bus
{
	class BlkData: public AtsSCADAData
	{
	public:
		BlkData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~BlkData();

		virtual void updateBlkStatus(BlkInfo& blkInfo);
		void initializeDataPoint();
		void updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality);

	private:
		/**
		 * Disable default,copy and assignment constructor
		 */
		BlkData();
		BlkData(const BlkData&);
		BlkData& operator= (const BlkData&);

		void updateBlkStatusDP(BlkInfo* blkInfo);

	private:
		std::string m_blkStatusDpAddr;

	};
}
#endif