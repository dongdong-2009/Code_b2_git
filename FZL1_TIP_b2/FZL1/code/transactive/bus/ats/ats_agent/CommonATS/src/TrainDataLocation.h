
#ifndef _TRAIN_DATA_LOCATION_H_
#define _TRAIN_DATA_LOCATION_H_

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include <string>

namespace TA_IRS_Bus
{
	class TrainDataLocation
	{
	public:
		TrainDataLocation(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~TrainDataLocation();

		void updateTrainlocation(TrainInfo& trainInfo, const TrainInfoVec& trainInfoVec);
		void initializeDataPoint();
		void updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality);
        void checkTrainInfoCache(const TrainInfoVec& trainInfoVec);
	private:
		/**
		 * Disable default,copy and assignment constructor
		 */
		TrainDataLocation();
		TrainDataLocation(const TrainDataLocation&);
		TrainDataLocation& operator= (const TrainDataLocation&);

		TA_IRS_Bus::TrackInfo m_trackInfo;
		TA_IRS_Bus::ActiveTrainServNum m_activeTrainSerNumber;
		TrainInfoMap m_tempTrainInfoCache;
		TrainInfoMap m_oldTrainInfoCache;

		TA_Base_Bus::ItaScada* m_itScada;

		
		
		std::string m_TrainServAddress;
		std::string m_TrainDirAddress;
		std::string m_TrainCongestionAddress;

		void updateActiveTrainServNum(const std::string& trainServNum);
		TrainInfo* createTrainInfoCopy(const TrainInfo& trainInfo);
		TrainInfo* getTrainOldRecordCache(const std::string& trainServiceNum);
		void removeTrainInfoCache(const std::string& trainServiceNum);
		void constructEmptyValue(TrainInfo* trainInfo);
		void insertTrainInfoCache(TrainInfo* newTrainInfo);

		bool isTrackNameExist(const TrainInfo& trainInfo);
		bool isTrainServNumExist(TrainInfo* trainInfo);
		void updateDataPoint(TrainInfo* oldTrainInfo, TrainInfo* newTrainInfo, const TrainInfoVec& newTrainInfoVec);
		void setLocalDataPoint(const std::string& dpAddress, const std::string& dpValue);
		void setLocalDataPoint(const std::string& dpAddress, bool dpValue);
		void updateTrainServNumDatapoint(TrainInfo* trainInfo);
		void updateTrainDirDatapoint(TrainInfo* trainInfo);
		void updateTrainCongestionDatapoint(TrainInfo* trainInfo);
		bool needUpdateOldTrack(TrainInfo* oldTrainInfo, TrainInfo* newTrainInfo, const TrainInfoVec& newTrainInfoVec);
	};
}
#endif