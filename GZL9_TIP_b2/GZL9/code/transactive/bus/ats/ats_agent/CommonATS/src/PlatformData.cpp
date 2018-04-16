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
#include "bus/ats/ats_agent/CommonATS/src/PlatformData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/CtaScadaExceptions.h"

namespace TA_IRS_Bus
{

	PlatformData::PlatformData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: AtsSCADAData(itScada,"EsbData")
	{
		FUNCTION_ENTRY("PlatformData");

		m_skipStatusDpAddr = EntityData->getPlatformJumpDpAddress();
		m_holdStatusDpAddr = EntityData->getPlatformHoldDpAddress();

		FUNCTION_EXIT;
	}

	PlatformData::~PlatformData()
	{
		FUNCTION_ENTRY("~PlatformData");

		FUNCTION_EXIT;
	}

	void PlatformData::updatePlatformData(const PlatformInfoVec& platformInfo)
	{
		FUNCTION_ENTRY("updatePlatformData");

		
		for (PlatformInfoVec::const_iterator itr = platformInfo.begin(); itr != platformInfo.end(); itr++)
		{
			if(itr->isNeedUpdateSkip)
			{
				updateSkipStatusDp((*itr));
			}
			if (itr->isNeedUpdateHold)
			{
				updateHoldStatusDp((*itr));
			}
		}

		FUNCTION_EXIT;
	}

	void PlatformData::updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateAllDataPointQuality");

		//This will update the All Datapoint based on the Address
		//For Signal Status Mode Address
		setAllLocalDataPointQualitiesMatchingAddress(m_holdStatusDpAddr,quality);
		//For res Status Mode Address
		setAllLocalDataPointQualitiesMatchingAddress(m_skipStatusDpAddr,quality);

		FUNCTION_EXIT;
	}

	void PlatformData::updateSkipStatusDp(const PlatformInfo& platformInfo)
	{
		FUNCTION_ENTRY("updateSkipStatusDp");

		std::string DPStatusAddress = m_skipStatusDpAddr + platformInfo.PlatformId;
		bool DPStatusValue = static_cast<bool>(platformInfo.Skip);

		if(BOOLEAN_TYPE_INVALID_VALUE > platformInfo.hold)
		{
			//Update Datapoint for Train Skip Status
			setLocalDataPoint(DPStatusAddress,DPStatusValue, TA_Base_Core::ITA_QUALITY_GOOD);
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"Platform Object ID=%s,received an invalid SkipStat value, set DataPoint quality to bad!", platformInfo.PlatformId.c_str());
			//updateDataPoint(DPStatusValue,DPStatusAddress, TA_Base_Core::ITA_QUALITY_BAD);
		}

		FUNCTION_EXIT;
	}

	void PlatformData::updateHoldStatusDp(const PlatformInfo& platformInfo)
	{
		FUNCTION_ENTRY("updateSkipStatusDp");

		std::string DPStatusAddress = m_holdStatusDpAddr + platformInfo.PlatformId;
		bool DPStatusValue = static_cast<bool>(platformInfo.hold);
		
		if(BOOLEAN_TYPE_INVALID_VALUE > platformInfo.hold)
		{
			//Update Datapoint for Train On Hold Status
			setLocalDataPoint(DPStatusAddress,DPStatusValue, TA_Base_Core::ITA_QUALITY_GOOD);
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"Platform Object ID=%s,received an invalid holdStat value, set DataPoint quality to bad!", platformInfo.PlatformId.c_str());
			//updateDataPoint(DPStatusValue,DPStatusAddress, TA_Base_Core::ITA_QUALITY_BAD);
		}

		FUNCTION_EXIT;
	}

	void PlatformData::initializeDatapoint()
	{
		FUNCTION_ENTRY("initializeDatapoint");

		//Update the Datapoint
		setAllLocalDataPointStateMatchingAddress(m_skipStatusDpAddr,EMPTY_VAL,
			TA_Base_Core::ITA_QUALITY_GOOD); 

		setAllLocalDataPointStateMatchingAddress(m_holdStatusDpAddr,EMPTY_VAL,
			TA_Base_Core::ITA_QUALITY_GOOD); 
	
		FUNCTION_EXIT;
	}

}