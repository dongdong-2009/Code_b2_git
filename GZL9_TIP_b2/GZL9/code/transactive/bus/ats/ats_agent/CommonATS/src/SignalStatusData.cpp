
#include "bus/ats/ats_agent/CommonATS/src/SignalStatusData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "core/exceptions/src/CtaScadaExceptions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include <algorithm>

namespace TA_IRS_Bus
{
	SignalStatusData::SignalStatusData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: AtsSCADAData(itScada,"SignalStatusData"),
		  m_sigStatusDpAddr(""),
		  m_sigStatusModeDpAddr("")

	{
		FUNCTION_ENTRY("SignalStatusData");

		m_sigStatusDpAddr = EntityData->getSigDisplayDpAddress();
		m_sigStatusModeDpAddr = EntityData->getSigHealthDpAddress();

		FUNCTION_EXIT;
	}

	SignalStatusData::~SignalStatusData()
	{
		FUNCTION_ENTRY("~SignalStatusData");
		FUNCTION_EXIT;
	}

	void SignalStatusData::updateSignalStatus(SignalInfo& signalInfo)
	{
		FUNCTION_ENTRY("updateTrainlocation");

		if(signalInfo.isNeedUpdateStatus)
		{
			updateSigStatusDP(&signalInfo);
		}

		if(signalInfo.isNeedUpdateMode)
		{
			updateSigStatusModeDP(&signalInfo);
		}

		FUNCTION_EXIT;
	}

	void SignalStatusData::updateSigStatusDP(SignalInfo* signalInfo)
	{
		FUNCTION_ENTRY("updateSigStatusDataPoint");

		std::string DPSigStatusAddress = m_sigStatusDpAddr + signalInfo->SignalId;
		unsigned long DPSigStatusValue = signalInfo->Status;

		setLocalDataPoint(DPSigStatusAddress,DPSigStatusValue);

		FUNCTION_EXIT;
	}

	void SignalStatusData::updateSigStatusModeDP(SignalInfo* signalInfo)
	{
		FUNCTION_ENTRY("updateSigStatusModeDataPoint");

		std::string DPSigStatusModeAddress = m_sigStatusModeDpAddr + signalInfo->SignalId;
		unsigned long DPSigStatusModeValue = signalInfo->healthStatus;

		setLocalDataPoint(DPSigStatusModeAddress,DPSigStatusModeValue);

		FUNCTION_EXIT;
	}

	void SignalStatusData::initializeDataPoint()
	{
		FUNCTION_ENTRY("initializeDatapoint");

		//For Signal Status Address
		setAllLocalDataPointStateMatchingAddress(m_sigStatusDpAddr,EMPTY_VAL,
			TA_Base_Core::ITA_QUALITY_GOOD); 

		//For Signal Status Mode Address
		setAllLocalDataPointStateMatchingAddress(m_sigStatusModeDpAddr,EMPTY_VAL,
			TA_Base_Core::ITA_QUALITY_GOOD); 
		FUNCTION_EXIT;
	}

	void SignalStatusData::updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateAllDataPointQuality");
		//This will update the All Datapoint based on the Address
		//For Signal Display Address
		setAllLocalDataPointQualitiesMatchingAddress(m_sigStatusDpAddr,quality);

		//For Signal Health Status Address
		setAllLocalDataPointQualitiesMatchingAddress(m_sigStatusModeDpAddr,quality);

		FUNCTION_EXIT;
	}

}