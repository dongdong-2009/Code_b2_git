
#ifndef _TRAIN_DATA_LOCATION_ID_H_
#define _TRAIN_DATA_LOCATION_ID_H_

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainDataLocation.h"
#include <string>

namespace TA_IRS_Bus
{
	class TrainDataLocation_IDBased : public TrainDataLocation
	{
	public:
		TrainDataLocation_IDBased(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~TrainDataLocation_IDBased();

		//These function need to re-write in derived class
		virtual void updateTrainlocation(TrainInfo& trainInfo, const TrainInfoVec& trainInfoVec);
		virtual void checkTrainInfoCache(const TrainInfoVec& trainInfoVec);
		virtual TrainInfo getTrainOldRecordCache(const std::string& trainID);
		/**
		 * syncTrainDataBegin
		 *
		 * Method used to set Train Data to begin sync status
		 *  
		 */
		virtual void syncTrainDataBegin();
		/**
		 * syncTrainDataEnd
		 *
		 * Method used to set Train Data to end sync status
		 * and update train track
		 *
		 */
		virtual void syncTrainDataEnd();

		/**
		 * updateActiveTrainList
		 *
		 * Method to use to add train to active train list
		 * 
		 * @param TrainInfo : Train Info Data
		 */
		virtual void updateActiveTrainList(TrainInfo& trainInfo);

		/**
		 * receivedTrainDeathMsg
		 *
		 * Method to use to handle train death message, removed the
		 * train from Datapoint and old train cache
		 *
		 * @param TrainInfo : Train Info Data
		 */
		void receivedTrainDeathMsg(const TrainInfo& trainInfo);

	private:
		/**
		 * Disable default,copy and assignment constructor
		 */
		TrainDataLocation_IDBased();
		TrainDataLocation_IDBased(const TrainDataLocation_IDBased&);
		TrainDataLocation_IDBased& operator= (const TrainDataLocation_IDBased&);

		void updateActiveTrainID(const std::string& trainID);
		void removeTrainInfoCache(const std::string& trainID);
		void insertTrainInfoCache(TrainInfo* newTrainInfo);

		bool isTrainIDExist(TrainInfo* trainInfo);

		void updateDataPoint(TrainInfo* oldTrainInfo, TrainInfo* newTrainInfo, const TrainInfoVec& newTrainInfoVec);

		void updateTrainServNumDatapoint(TrainInfo* trainInfo);
		void updateTrainDirDatapoint(TrainInfo* trainInfo);
		void updateTrainCongestionDatapoint(TrainInfo* trainInfo);
		void updateTrainDriveMode(TrainInfo* trainInfo);
		void updateTrainPosLostFlag(TrainInfo* trainInfo);

	private:
		TA_IRS_Bus::ActiveTrainID	m_activeTrainID;
		IDBaseTrainInfoMap			m_tempTrainInfoCache;
		IDBaseTrainInfoMap			m_oldTrainInfoCache;
		std::string					m_TrainDriveModeAddress;
		std::string					m_TrainPosLostAddress;
		TrainInfoVec				m_activeTrainInfoVec;

        //for udpating(birh,update) message, there is no possible to judge if a track has been occupied by other train. 
		//declare it for reduce memory allocation operation.
		TrainInfoVec				m_helpInfoVec; 

		bool m_blnSyncBegin;

		TA_Base_Core::ReEntrantThreadLockable m_lockCache;



	};
}
#endif //_TRAIN_DATA_LOCATION_ID_H_