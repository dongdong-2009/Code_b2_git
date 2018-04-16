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
#include "bus/ats/ats_agent/CommonATS/src/PlatformDoorData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/CtaScadaExceptions.h"

namespace TA_IRS_Bus
{

	PlatformDoorData::PlatformDoorData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: m_itScada(itScada)
	{
		FUNCTION_ENTRY("PlatformDoorData");

		m_overrideStatusAddress = EntityData->getPlatformDoorOverrideDpAddress();

		FUNCTION_EXIT;
	}

	PlatformDoorData::~PlatformDoorData()
	{
		FUNCTION_ENTRY("~PlatformDoorData");

		FUNCTION_EXIT;
	}

	void PlatformDoorData::updatePlatformDoorData(const PlatformDoorInfoVec& platformDoorInfo)
	{
		FUNCTION_ENTRY("updatePlatformDoorData");

		
		for (PlatformDoorInfoVec::const_iterator itr = platformDoorInfo.begin(); itr != platformDoorInfo.end(); itr++)
		{
			if(itr->isNeedUpdateOverride)
			{
				updateOverrideStatusDp((*itr));
			}
		}

		FUNCTION_EXIT;
	}

	void PlatformDoorData::updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateAllDataPointQuality");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Update Platform Datapoint Quality :%d, with Address matching:%s",
			static_cast<int>(quality),m_overrideStatusAddress.c_str());

		//This will update the All Datapoint based on the Address
		//For Position Address
		m_itScada->setAllLocalDataPointQualitiesMatchingAddress(m_overrideStatusAddress,quality);

		FUNCTION_EXIT;
	}

	void PlatformDoorData::updateOverrideStatusDp(const PlatformDoorInfo& platformDoorInfo)
	{
		FUNCTION_ENTRY("updateOverrideStatusDp");
		
		std::string DPStatusAddress = m_overrideStatusAddress + platformDoorInfo.PlatformDoorId;
		unsigned long DPStatusValue = platformDoorInfo.OverrideStatus;

		//Update Datapoint for Train Arrival Time
		updateDataPoint(DPStatusValue,DPStatusAddress);

		FUNCTION_EXIT;
	}

	
	void PlatformDoorData::updateDataPoint(unsigned long DPStatusValue, const std::string& DPStatusAddress)
	{
		FUNCTION_ENTRY("updateDataPoint");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Update PlatformDoor datapoint, Address : %s, Value : %d",
			DPStatusAddress.c_str(), DPStatusValue );

		try
		{
			time_t timestamp;
			time(&timestamp);

			//Update the Datapoint
			m_itScada->setLocalDataPointStateByAddress(DPStatusAddress,DPStatusValue,
				TA_Base_Core::ITA_QUALITY_GOOD,timestamp); 
		}
		catch(TA_Base_Core::CtaScadaInvalidConfigurationException& mssge)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"%s", mssge.what());
		}

		catch(TA_Base_Core::CtaScadaWriteException& e)
		{
			std::ostringstream reason;
			reason << "Setting Local Data Point Error "
				<< DPStatusAddress.c_str()
				<< ":"
				<< e.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "CtaScadaWriteException", reason.str().c_str());
		}


		FUNCTION_EXIT;
	}

	void PlatformDoorData::initializeDatapoint()
	{
		FUNCTION_ENTRY("initializeDatapoint");

		try
		{
			time_t timestamp;
			time(&timestamp);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Update PlatformDoor Datapoint Quality with Matching address %s",
				m_overrideStatusAddress.c_str());

			//Update the Datapoint
			m_itScada->setAllLocalDataPointStateMatchingAddress(m_overrideStatusAddress,EMPTY_VAL,
				TA_Base_Core::ITA_QUALITY_GOOD,timestamp); 
		}
		catch(TA_Base_Core::CtaScadaInvalidConfigurationException& mssge)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"%s", mssge.what());
		}
		catch(TA_Base_Core::CtaScadaWriteException& e)
		{
			std::ostringstream reason;
			reason << "Setting Local Data Point Error "
				<< m_overrideStatusAddress.c_str()
				<< ":"
				<< e.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "CtaScadaWriteException", reason.str().c_str());
		}

		FUNCTION_EXIT;
	}

}