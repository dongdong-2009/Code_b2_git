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
#ifndef _TRAIN_PIS_DATA_H_
#define _TRAIN_PIS_DATA_H_

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "core/data_access_interface/ats/src/IAtsPlatform.h"
#include <string>

namespace TA_IRS_Bus
{
	class TrainPisData
	{
	public:
		TrainPisData(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~TrainPisData();

		void updateTrainPisData(const PISInfoVec& pisInfo);
		void initializeDatapoint();

	private:
		/**
		 * Disable default,copy and assignment constructor
		 */
		TrainPisData();
		TrainPisData(const TrainPisData&);
		TrainPisData& operator= (const TrainPisData&);
		TA_Base_Bus::ItaScada* m_itScada;

		static const std::string ZERO_PADDING;
		static const std::string STATION_ST;
		static const std::string TIS_PLATFORM_SEPARATOR;

		void initialize();
		void deInitialize();
		std::string getStationId(unsigned long platformId);
		std::string getTISPlatformID(unsigned long platformID);
		std::string getAddressByPlatformID(unsigned long platformID);
		void updateDataPoint(const std::string& trainArrivalTime, const std::string& trainPisAddress);

		TA_IRS_Core::IAtsPlatforms m_platforms;
		void updateSchedArrTimeDp(const PISInfo& pisInfo);
		std::string m_trainPisAddress;
	};
}

#endif