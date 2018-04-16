
#include "bus/ats/ats_agent/CommonATS/src/BlkData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "core/exceptions/src/CtaScadaExceptions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include <algorithm>

namespace TA_IRS_Bus
{
	BlkData::BlkData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: AtsSCADAData(itScada,"BlockData"),
		  m_blkStatusDpAddr("")

	{
		FUNCTION_ENTRY("BlkData");

		m_blkStatusDpAddr = EntityData->getBlockDpAddress();

		FUNCTION_EXIT;
	}

	BlkData::~BlkData()
	{
		FUNCTION_ENTRY("~BlkData");
		FUNCTION_EXIT;
	}

	void BlkData::updateBlkStatus(BlkInfo& blkInfo)
	{
		FUNCTION_ENTRY("updateBlock");

		if(blkInfo.isNeedUpdateBlkStatus)
		{
			updateBlkStatusDP(&blkInfo);
		}

		FUNCTION_EXIT;
	}


	void BlkData::updateBlkStatusDP(BlkInfo* blockInfo)
	{
		FUNCTION_ENTRY("updateSigStatusDataPoint");

		std::string DPAddress = m_blkStatusDpAddr + blockInfo->BlockId;
		unsigned long DPValue = blockInfo->BlkStatus;

		setLocalDataPoint(DPAddress,DPValue);

		FUNCTION_EXIT;
	}

	void BlkData::initializeDataPoint()
	{
		FUNCTION_ENTRY("initializeDatapoint");

		setAllLocalDataPointStateMatchingAddress(m_blkStatusDpAddr,EMPTY_VAL,
			TA_Base_Core::ITA_QUALITY_GOOD); 

		FUNCTION_EXIT;
	}

	void BlkData::updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateAllDataPointQuality");

		//This will update the All Datapoint based on the Address
		//For Block Status Address
		setAllLocalDataPointQualitiesMatchingAddress(m_blkStatusDpAddr,quality);

		FUNCTION_EXIT;
	}

}