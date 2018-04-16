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
#include "bus/ats/ats_agent/CommonATS/src/TrainPisData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/CtaScadaExceptions.h"
#include <sstream>
#include <string>

namespace TA_IRS_Bus
{
	TrainPisData::TrainPisData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: AtsSCADAData(itScada,"TrainPisData")
	{
		FUNCTION_ENTRY("TrainPisData");

		m_1stArrTrainTimeAddress = EntityData->getPIDLocationArrTimeDpAddress();

		FUNCTION_EXIT;
	}

	TrainPisData::~TrainPisData()
	{
		FUNCTION_ENTRY("TrainPisData");

		FUNCTION_EXIT;
	}

	void TrainPisData::updateTrainPisData(const PISInfoVec& pisInfo)
	{
		FUNCTION_ENTRY("updateTrainPisData");
		
		for (PISInfoVec::const_iterator iterPis = pisInfo.begin(); iterPis != pisInfo.end(); iterPis++)
		{
			if(iterPis->isNeedUpdateTime)
			{
				updateSchedArrTimeDp((*iterPis));
			}	
		}

		FUNCTION_EXIT;
	}

	void TrainPisData::updateSchedArrTimeDp(const PISInfo& pisInfo)
	{
		FUNCTION_ENTRY("updateSchedArrTimeDp");
		
		std::string DPStatusAddress = m_1stArrTrainTimeAddress + pisInfo.PlatformId;
		std::string DPStatusValue = pisInfo.Train1st_ArrivalTime;

		//Update Datapoint for Train Arrival Time
		setLocalDataPoint(DPStatusAddress,DPStatusValue);

		FUNCTION_EXIT;
	}

	void TrainPisData::initializeDatapoint()
	{
		FUNCTION_ENTRY("initializeDatapoint");

		setAllLocalDataPointStateMatchingAddress(m_1stArrTrainTimeAddress,EMPTY_VAL,
			TA_Base_Core::ITA_QUALITY_GOOD); 

		FUNCTION_EXIT;
	}

	void TrainPisData::updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateAllDataPointQuality");

		//This will update the All Datapoint based on the Address
		//For first arrival train Address
		setAllLocalDataPointQualitiesMatchingAddress(m_1stArrTrainTimeAddress,quality);

		FUNCTION_EXIT;
	}

}