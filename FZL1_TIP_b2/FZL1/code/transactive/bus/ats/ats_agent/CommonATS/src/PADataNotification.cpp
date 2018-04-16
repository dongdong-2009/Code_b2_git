/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\PADataNotification.cpp $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling PA Information which will be used for data processing.
  */

#include "bus/ats/ats_agent/CommonATS/src/PADataNotification.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"

namespace TA_IRS_Bus
{
	PADataNotification::PADataNotification(void* paData)
		: DataNotification(paData)
	{
		FUNCTION_ENTRY("PADataNotification");

		PAStationInfoVec* stationInfo = static_cast<PAStationInfoVec*>(getData());
		PAStationInfoVecIt iterPA = stationInfo->begin();
		for (iterPA; iterPA != stationInfo->end(); iterPA++)
		{
			PAStationInfo paStationInfo = (*iterPA);
			m_paStationInfoData.push_back(paStationInfo);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"PlatformID :%d, DestinationStationID :%d, Skip :%d, InDepot  :%d, Arrival :%d, PreArrival :%d",
				static_cast<int>(paStationInfo.PlatformId),static_cast<int>(paStationInfo.DestinationStationId),
				paStationInfo.Skip,paStationInfo.InDepot,paStationInfo.Arrival,paStationInfo.PreArrival);
		}

		FUNCTION_EXIT;
	}

	PADataNotification::~PADataNotification()
	{
		FUNCTION_ENTRY("PADataNotification");
		FUNCTION_EXIT;
	}

	PAStationInfoVec& PADataNotification::getPAStationInfo()
	{
		FUNCTION_ENTRY("getPAStationInfo");
		FUNCTION_EXIT;
		return m_paStationInfoData;
	}

	void PADataNotification::updateAtsData()
	{
		FUNCTION_ENTRY("updateAtsData");

		//Update PA Data Cache
		TA_IRS_Bus::DataCache::getInstance()->updatePACache(getPAStationInfo());

		FUNCTION_EXIT;
	}
	
	void PADataNotification::processAtsData()
	{
		FUNCTION_ENTRY("processAtsData");

		//Process Data Dispatch
		m_dataUpdater->processDataDispatcher(ePA);

		//Process Data PA Syn
		m_dataUpdater->processDataSync(ePA);

		FUNCTION_EXIT;
	}

}