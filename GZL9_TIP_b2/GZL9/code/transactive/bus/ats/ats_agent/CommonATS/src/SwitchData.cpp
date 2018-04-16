
#include "bus/ats/ats_agent/CommonATS/src/SwitchData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "core/exceptions/src/CtaScadaExceptions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include <algorithm>

namespace TA_IRS_Bus
{
	SwitchData::SwitchData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: AtsSCADAData(itScada,"SwitchData"),
		  m_positionDpAddr(""),
		  m_failedStatusDpAddr(""),
		  m_resStatusDpAddr("")
	{
		FUNCTION_ENTRY("SwitchData");

		m_positionDpAddr = EntityData->getSwPositionDpAddress();
		m_failedStatusDpAddr = EntityData->getSwFailedStatDpAddress();
		m_resStatusDpAddr = EntityData->getSwResStatDpAddress();

		FUNCTION_EXIT;
	}

	SwitchData::~SwitchData()
	{
		FUNCTION_ENTRY("~SwitchData");
		FUNCTION_EXIT;
	}

	void SwitchData::updateSwitchStatus(SwtichInfo& switchInfo)
	{
		FUNCTION_ENTRY("updateTrainlocation");

		if(switchInfo.isNeedUpdatePos)
		{
			updatePositionDP(&switchInfo);
		}

		if(switchInfo.isNeedUpdateFailed)
		{
			updateFailedStatusDP(&switchInfo);
		}

		if(switchInfo.isNeedUpdateRes)
		{
			updateResStatusDP(&switchInfo);
		}

		FUNCTION_EXIT;
	}

	void SwitchData::updatePositionDP(SwtichInfo* switchInfo)
	{
		FUNCTION_ENTRY("updatePositionDP");

		std::string DPStatusAddress = m_positionDpAddr + switchInfo->SwitchId;
		unsigned long DPStatusValue = switchInfo->Position;
	
		setLocalDataPoint(DPStatusAddress,DPStatusValue);

		FUNCTION_EXIT;
	}

	void SwitchData::updateFailedStatusDP(SwtichInfo* switchInfo)
	{
		FUNCTION_ENTRY("updateFailedStatusDP");

		std::string DPStatusAddress = m_failedStatusDpAddr + switchInfo->SwitchId;
		bool DPStatusValue = static_cast<bool>(switchInfo->FailedStatus);

		if(BOOLEAN_TYPE_INVALID_VALUE > switchInfo->FailedStatus)
		{
			setLocalDataPoint(DPStatusAddress,DPStatusValue ,TA_Base_Core::ITA_QUALITY_GOOD);
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"Switch Object ID=%s,received an invalid FailedStat value, set DataPoint quality to bad!", switchInfo->SwitchId.c_str());
			//setLocalDataPoint(DPStatusAddress,DPStatusValue ,TA_Base_Core::ITA_QUALITY_BAD);
		}
			
		FUNCTION_EXIT;
	}

	void SwitchData::updateResStatusDP(SwtichInfo* switchInfo)
	{
		FUNCTION_ENTRY("updateResStatusDP");

		std::string DPStatusAddress = m_resStatusDpAddr + switchInfo->SwitchId;
		bool DPStatusValue = static_cast<bool>(switchInfo->ResStatus);

		if(BOOLEAN_TYPE_INVALID_VALUE > switchInfo->ResStatus)
		{
			setLocalDataPoint(DPStatusAddress,DPStatusValue ,TA_Base_Core::ITA_QUALITY_GOOD);
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"Switch Object ID=%s,received an invalid ResStat value, set DataPoint quality to bad!", switchInfo->SwitchId.c_str());
			//setLocalDataPoint(DPStatusAddress,DPStatusValue ,TA_Base_Core::ITA_QUALITY_BAD);
		}

		FUNCTION_EXIT;
	}

	void SwitchData::initializeDataPoint()
	{
		FUNCTION_ENTRY("initializeDatapoint");

		//For Switch Position Address
		setAllLocalDataPointStateMatchingAddress(m_positionDpAddr,EMPTY_VAL,
			TA_Base_Core::ITA_QUALITY_GOOD); 

		//For Switch Failed Status Address
		setAllLocalDataPointStateMatchingAddress(m_failedStatusDpAddr,EMPTY_VAL,
			TA_Base_Core::ITA_QUALITY_GOOD); 

		//For Switch Reserved Status Address
		setAllLocalDataPointStateMatchingAddress(m_resStatusDpAddr,EMPTY_VAL,
			TA_Base_Core::ITA_QUALITY_GOOD); 

		FUNCTION_EXIT;
	}

	void SwitchData::updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateAllDataPointQuality");

		//This will update the All Datapoint based on the Address
		//For Position Address
		setAllLocalDataPointQualitiesMatchingAddress(m_positionDpAddr,quality);

		//For Signal Status Mode Address
		setAllLocalDataPointQualitiesMatchingAddress(m_failedStatusDpAddr,quality);

		//For res Status Mode Address
		setAllLocalDataPointQualitiesMatchingAddress(m_resStatusDpAddr,quality);

		FUNCTION_EXIT;
	}

}