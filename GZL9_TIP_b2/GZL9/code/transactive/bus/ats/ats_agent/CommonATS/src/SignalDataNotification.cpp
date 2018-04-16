
#include "bus/ats/ats_agent/CommonATS/src/SignalDataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	SignalDataNotification::SignalDataNotification(void* sigData)
		: DataNotification(sigData)
	{
		FUNCTION_ENTRY("SignalDataNotification");

		SignalInfoVec* tmpSigVec = static_cast<SignalInfoVec*>(getData());

		SignalInfoVecIt itr = tmpSigVec->begin();

		if(itr != tmpSigVec->end())
		{
			m_signalInfoData = (*itr);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
 				"SignalID :%s, Signal Status :%d, Health Status :%d",(m_signalInfoData.SignalId.c_str()),
 				(m_signalInfoData.Status),(m_signalInfoData.healthStatus));
		}

		FUNCTION_EXIT;
	}

	SignalDataNotification::~SignalDataNotification()
	{
		FUNCTION_ENTRY("SignalDataNotification");
		FUNCTION_EXIT;
	}

	SignalInfo& SignalDataNotification::getSignalInfo()
	{
		FUNCTION_ENTRY("getSignalInfo");
		FUNCTION_EXIT;
		return (m_signalInfoData);
	}

	void SignalDataNotification::updateAtsData()
	{
		FUNCTION_ENTRY("updateAtsData");

		//Update PA Data Cache
		TA_IRS_Bus::DataCache::getInstance()->updateSignalStatus(getSignalInfo());

		FUNCTION_EXIT;
	}

	void SignalDataNotification::updateSignalDatapointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateTrainDatapointQuality");

		//Update Train Data Datapoint Quality
		TA_IRS_Bus::DataCache::getInstance()->updateSigInfoDatapointQuality(quality);

		FUNCTION_EXIT;
	}

}