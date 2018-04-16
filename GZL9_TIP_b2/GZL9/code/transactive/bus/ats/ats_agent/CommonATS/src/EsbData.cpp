
#include "bus/ats/ats_agent/CommonATS/src/EsbData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "core/exceptions/src/CtaScadaExceptions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include <algorithm>

namespace TA_IRS_Bus
{
	EsbData::EsbData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: AtsSCADAData(itScada,"EsbData"),
		  m_esbStatusDpAddr("")
	{
		FUNCTION_ENTRY("EsbInfo");

		m_esbStatusDpAddr = EntityData->getEsbStatusDpAddress();

		FUNCTION_EXIT;
	}

	EsbData::~EsbData()
	{
		FUNCTION_ENTRY("~EsbData");
		FUNCTION_EXIT;
	}

	void EsbData::updateEsbStatus(EsbInfo& esbInfo)
	{
		FUNCTION_ENTRY("updateTrainlocation");

		if(esbInfo.isNeedUpdateEsbStatus)
		{
			updateEsbStatusDP(&esbInfo);
		}

		FUNCTION_EXIT;
	}

	void EsbData::updateEsbStatusDP(EsbInfo* esbInfo)
	{
		FUNCTION_ENTRY("updateSigStatusDataPoint");

		std::string DPAddress = m_esbStatusDpAddr + esbInfo->EsbId;
		unsigned long DPValue = esbInfo->Status;

		setLocalDataPoint(DPAddress,DPValue);

		FUNCTION_EXIT;
	}

	void EsbData::initializeDataPoint()
	{
		FUNCTION_ENTRY("initializeDatapoint");

		setAllLocalDataPointStateMatchingAddress(m_esbStatusDpAddr,EMPTY_VAL,
				TA_Base_Core::ITA_QUALITY_GOOD); 

		FUNCTION_EXIT;
	}

	void EsbData::updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateAllDataPointQuality");

		//This will update the All Datapoint based on the Address
		//For Emergency Stop Button Status Address
		setAllLocalDataPointQualitiesMatchingAddress(m_esbStatusDpAddr,quality);

		FUNCTION_EXIT;
	}

}