
#ifndef _TRAIN_DATA_LOCATION_H_
#define _TRAIN_DATA_LOCATION_H_

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsSCADAData.h"
#include <string>

namespace TA_IRS_Bus
{
	class TrainDataLocation : public AtsSCADAData
	{
	public:
		TrainDataLocation(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~TrainDataLocation();

		virtual void updateTrainlocation(TrainInfo& trainInfo, const TrainInfoVec& trainInfoVec);
        virtual void checkTrainInfoCache(const TrainInfoVec& trainInfoVec);
		virtual TrainInfo getTrainOldRecordCache(const std::string& trainServiceNum);

		void initializeDataPoint();
		void updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality);

		/**
		 * syncTrainDataBegin
		 *
		 * Method used to set Train Data to begin sync status
		 *  
		 */
		virtual void syncTrainDataBegin(){/*For Gena Protocol use, need to implement*/};
		/**
		 * syncTrainDataEnd
		 *
		 * Method used to set Train Data to end sync status
		 * and update train track
		 *
		 */
		virtual void syncTrainDataEnd(){/*For Gena Protocol use, need to implement*/};
		/**
		 * updateActiveTrainList
		 *
		 * Method to use to add train to active train list
		 * 
		 * @param TrainInfo : Train Info Data
		 */
		virtual void updateActiveTrainList(const TrainInfo& trainInfo){/*For Gena Protocol use, need to implement*/};

		std::vector<TrainInfo*> getTrainAtsInfoCacheList();

	private:
		/**
		 * Disable default,copy and assignment constructor
		 */
		TrainDataLocation();
		TrainDataLocation(const TrainDataLocation&);
		TrainDataLocation& operator= (const TrainDataLocation&);

		
		TA_IRS_Bus::ActiveTrainServNum m_activeTrainSerNumber;
		TrainInfoMap m_tempTrainInfoCache;
		TrainInfoMap m_oldTrainInfoCache;


		void updateActiveTrainServNum(const std::string& trainServNum);
		void removeTrainInfoCache(const std::string& trainServiceNum);
		void insertTrainInfoCache(TrainInfo* newTrainInfo);

		bool isTrainServNumExist(TrainInfo* trainInfo);
		void updateTrainServNumDatapoint(TrainInfo* trainInfo);
		void updateTrainDirDatapoint(TrainInfo* trainInfo);
		void updateTrainCongestionDatapoint(TrainInfo* trainInfo);
		

	protected:
		bool isTrackNameExist(const TrainInfo& trainInfo);
		TrainInfo* createTrainInfoCopy(const TrainInfo& trainInfo);
		void constructEmptyValue(TrainInfo* trainInfo);

		bool needUpdateOldTrack(TrainInfo* oldTrainInfo, TrainInfo* newTrainInfo, const TrainInfoVec& newTrainInfoVec);
		void updateDataPoint(TrainInfo* oldTrainInfo, TrainInfo* newTrainInfo, const TrainInfoVec& newTrainInfoVec);

		std::string m_TrainServAddress;
		std::string m_TrainDirAddress;
		std::string m_TrainCongestionAddress;
		TA_IRS_Bus::TrackInfo m_trackInfo;
	};
}
#endif