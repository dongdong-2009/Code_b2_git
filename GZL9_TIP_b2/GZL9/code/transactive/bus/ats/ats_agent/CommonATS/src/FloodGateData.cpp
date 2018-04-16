
#include "bus/ats/ats_agent/CommonATS/src/FloodGateData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "core/exceptions/src/CtaScadaExceptions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include <algorithm>

namespace TA_IRS_Bus
{
	FloodGateData::FloodGateData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: AtsSCADAData(itScada,"FloodGateData"),
		  m_RequestCloseDpAddr(""),
		  m_PermissionCloseDpAddr("")

	{
		FUNCTION_ENTRY("FloodGateData");

		m_RequestCloseDpAddr = EntityData->getFloodgateReqDpAddress();
		m_PermissionCloseDpAddr = EntityData->getFloodgatePermissionDpAddress();

		FUNCTION_EXIT;
	}

	FloodGateData::~FloodGateData()
	{
		FUNCTION_ENTRY("~FloodGateData");
		FUNCTION_EXIT;
	}

	void FloodGateData::updateFloodGateStatus(FloodGateInfo& floodgateInfo)
	{
		FUNCTION_ENTRY("updateFloodGateStatus");

		if(floodgateInfo.isNeedUpdateReq)
		{
			updateRequestCloseDP(&floodgateInfo);
		}

		if(floodgateInfo.isNeedUpdatePermission)
		{
			updatePermissionCloseDP(&floodgateInfo);
		}

		FUNCTION_EXIT;
	}

	void FloodGateData::updateRequestCloseDP(FloodGateInfo* floodgateInfo)
	{
		FUNCTION_ENTRY("updateRequestCloseDP");

		std::string DPStatusAddress = m_RequestCloseDpAddr + floodgateInfo->FloodGateId;
		bool DPStatusValue = floodgateInfo->RequestClose;

		if(BOOLEAN_TYPE_INVALID_VALUE > floodgateInfo->RequestClose)
		{
			setLocalDataPoint(DPStatusAddress,DPStatusValue ,TA_Base_Core::ITA_QUALITY_GOOD);
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"Floodgate Object ID=%s,received an invalid ReqClose value, set DataPoint quality to bad!", floodgateInfo->FloodGateId.c_str());
			//setLocalDataPoint(DPStatusAddress,DPStatusValue ,TA_Base_Core::ITA_QUALITY_BAD);
		}

		FUNCTION_EXIT;
	}

	void FloodGateData::updatePermissionCloseDP(FloodGateInfo* floodgateInfo)
	{
		FUNCTION_ENTRY("updatePermissionCloseDP");

		std::string DPStatusAddress = m_PermissionCloseDpAddr + floodgateInfo->FloodGateId;
		bool DPStatusValue = floodgateInfo->PermissionClose;

		if(BOOLEAN_TYPE_INVALID_VALUE > floodgateInfo->PermissionClose)
		{
			setLocalDataPoint(DPStatusAddress,DPStatusValue ,TA_Base_Core::ITA_QUALITY_GOOD);
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"Floodgate Object ID=%s,received an invalid PermClose value, set DataPoint quality to bad!", floodgateInfo->FloodGateId.c_str());
			//setLocalDataPoint(DPStatusAddress,DPStatusValue ,TA_Base_Core::ITA_QUALITY_BAD);
		}
		FUNCTION_EXIT;
	}

	void FloodGateData::initializeDataPoint()
	{
		FUNCTION_ENTRY("initializeDatapoint");

		//For Signal Status Address
		setAllLocalDataPointStateMatchingAddress(m_RequestCloseDpAddr,EMPTY_VAL,
			TA_Base_Core::ITA_QUALITY_GOOD); 

		//For Signal Status Mode Address
		setAllLocalDataPointStateMatchingAddress(m_PermissionCloseDpAddr,EMPTY_VAL,
			TA_Base_Core::ITA_QUALITY_GOOD); 

		FUNCTION_EXIT;
	}

	void FloodGateData::updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateAllDataPointQuality");

		//This will update the All Datapoint based on the Address
		//For Request Close Status Address
		setAllLocalDataPointQualitiesMatchingAddress(m_RequestCloseDpAddr,quality);

		//For Permission Close Status Address
		setAllLocalDataPointQualitiesMatchingAddress(m_PermissionCloseDpAddr,quality);

		FUNCTION_EXIT;
	}

}