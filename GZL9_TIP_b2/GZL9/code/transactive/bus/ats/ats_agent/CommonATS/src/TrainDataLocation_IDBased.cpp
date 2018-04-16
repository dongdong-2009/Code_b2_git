
#include "bus/ats/ats_agent/CommonATS/src/TrainDataLocation_IDBased.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "core/exceptions/src/CtaScadaExceptions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include <algorithm>

namespace TA_IRS_Bus
{
	TrainDataLocation_IDBased::TrainDataLocation_IDBased(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: TrainDataLocation(itScada, EntityData),
		m_TrainDriveModeAddress(""),
		m_TrainPosLostAddress(""),
		m_blnSyncBegin(false)
	{
		FUNCTION_ENTRY("TrainDataLocation_IDBased");

		m_activeTrainID.clear();
		m_oldTrainInfoCache.clear();
		m_tempTrainInfoCache.clear();
		m_activeTrainInfoVec.clear();

		//Get Train DataPoint Address
		m_TrainServAddress = EntityData->getTrainServiceNumberAddress();
		m_TrainDirAddress = EntityData->getTrainDirectionAddress();
		m_TrainCongestionAddress = EntityData->getTrainTimeOutDpAddress();
		m_TrainDriveModeAddress = EntityData->getTrainDriveModeDpAddress();
		m_TrainPosLostAddress = EntityData->getTrainPosLostDpAddress();

		FUNCTION_EXIT;
	}

	TrainDataLocation_IDBased::~TrainDataLocation_IDBased()
	{
		FUNCTION_ENTRY("~TrainDataLocation_IDBased");
		FUNCTION_EXIT;
	}

	void TrainDataLocation_IDBased::updateTrainlocation(TrainInfo& trainInfo, const TrainInfoVec& trainInfoVec)
	{
		FUNCTION_ENTRY("updateTrainlocation");

		//Check if the Track name exist in database.		

		TrainInfo* newTrainInfo = TrainDataLocation::createTrainInfoCopy(trainInfo);

		TA_Base_Core::ThreadGuard guard(m_lockCache);
		//Update active Train ID
		updateActiveTrainID(newTrainInfo->TrainID);

		if (false == TrainDataLocation::isTrackNameExist(trainInfo))
		{				
			removeTrainInfoCache(newTrainInfo->TrainID);
			insertTrainInfoCache(newTrainInfo);
			return;
		}

		//Check if the Train Service number exist in the old cache
		if (false == isTrainIDExist(newTrainInfo))
		{
			//Update datapoint for new Train Information;
			TrainInfo* oldTrainInfo = NULL;
			updateDataPoint(oldTrainInfo, newTrainInfo, trainInfoVec);
			//Put the new Train Information in the Cache
			insertTrainInfoCache(newTrainInfo);			
		}
		else
		{
			//Get TrainInfo from old Cache record
			TrainInfo oldTrainInfo = getTrainOldRecordCache(newTrainInfo->TrainID);
			//Construct empty value to update datapoint, only keep the train track location value
			TrainDataLocation::constructEmptyValue(&oldTrainInfo);
			//Update datapoint for old and new train info
			updateDataPoint(&oldTrainInfo, newTrainInfo, trainInfoVec);
			//Need to remove the old Train Info from the Cache to replace for a new Train Info
			removeTrainInfoCache(newTrainInfo->TrainID);
			//Replace with new Train Info
			
			insertTrainInfoCache(newTrainInfo);
		}
		//set the flag that the train info has been processed.
		trainInfo.shownTrainOnTrack = true;

		//Check if the Train Info is the last Train Record
		//This is for removing any inactive Train Service number from the Cache.
		//26/Feb/2016, Move it to  DataCache::updateTrainCache
		/*if (true == newTrainInfo->lastTrainRecord)
		{
			checkTrainInfoCache(trainInfoVec);
		}*/
		

		FUNCTION_EXIT;
	}

	void TrainDataLocation_IDBased::checkTrainInfoCache(const TrainInfoVec& trainInfoVec)
	{
		FUNCTION_ENTRY("checkTrainInfoCache");
        TA_Base_Core::ThreadGuard guard(m_lockCache);
		//Need to check the Active Train ID with the Train info Cache.
		//If the Train info Cache contains inactive Train ID,
		//Train ID must be removed from the cache and update the datapoint
		TA_IRS_Bus::ActiveTrainIDIt iterActive = m_activeTrainID.begin();
		for(iterActive; iterActive != m_activeTrainID.end(); iterActive++)
		{
			//Check the active Train Service number from the Train Info cache
			TrainInfoMapIt iterCache = m_oldTrainInfoCache.find((*iterActive));
			if (iterCache != m_oldTrainInfoCache.end())
			{
				//If found, need to place the Train info from the temp cache.
				m_tempTrainInfoCache.insert(std::pair<TA_IRS_Bus::TrainID,TrainInfo*>((*iterCache).second->TrainID,(*iterCache).second));
				//then remove the item from the old cache
				m_oldTrainInfoCache.erase(iterCache);
				
			}
		}

		//Handling of inactive remaining Train Service Number.
		//Need to remove any remaining Train info from the old Cache
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Remaining Train info Cache size : %d", m_oldTrainInfoCache.size());
		TrainInfoMapIt iterRemCache = m_oldTrainInfoCache.begin();
		for( iterRemCache; iterRemCache != m_oldTrainInfoCache.end(); iterRemCache++)
		{
			//Get the old Train Info
			TrainInfo* oldTrainInfo = (*iterRemCache).second;
			//Must set to empty value to set the datapoint
			TrainDataLocation::constructEmptyValue(oldTrainInfo);
			TrainInfo* newTrainInfo = NULL;
			//Update the Datapoint 
			updateDataPoint(oldTrainInfo,newTrainInfo,trainInfoVec);
			delete oldTrainInfo;
			oldTrainInfo=NULL;
			//Then remove the item in the cache
			//m_oldTrainInfoCache.erase(iterRemCache);
		}
		m_oldTrainInfoCache.clear();
		//Clear m_activeTrainID
		m_activeTrainID.clear();
		//Place back the active Train service number from temp cache to the original cache
		m_oldTrainInfoCache = m_tempTrainInfoCache;
		//Then clear the temp cache
		m_tempTrainInfoCache.clear();

		FUNCTION_EXIT;
	}


	TrainInfo TrainDataLocation_IDBased::getTrainOldRecordCache(const std::string& trainID)
	{
		FUNCTION_ENTRY("getTrainInfoRecord");
        TA_Base_Core::ThreadGuard guard(m_lockCache);  
		TrainInfo oldRecord;
		TrainDataLocation::constructEmptyValue(&oldRecord);
		TrainInfoMapIt iterRecord = m_oldTrainInfoCache.find(trainID);
		if( iterRecord != m_oldTrainInfoCache.end())
		{
			oldRecord = (*(*iterRecord).second);
		}

		FUNCTION_EXIT;
		return oldRecord;

	}

	void TrainDataLocation_IDBased::removeTrainInfoCache(const std::string& trainID)
	{
		FUNCTION_ENTRY("removeOldTrainRecordCache");
        TA_Base_Core::ThreadGuard guard(m_lockCache);
		TrainInfoMapIt iterRecord = m_oldTrainInfoCache.find(trainID);
		if( iterRecord != m_oldTrainInfoCache.end())
		{
			delete (*iterRecord).second;
			(*iterRecord).second = NULL;

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Train Info with Service Number : %s, will be removed from the Cache",
				trainID.c_str());
			m_oldTrainInfoCache.erase(iterRecord);
		}

		FUNCTION_EXIT;
	}


	void TrainDataLocation_IDBased::insertTrainInfoCache(TrainInfo* newTrainInfo)
	{
		FUNCTION_ENTRY("updateOldTrainInfoCache");
        TA_Base_Core::ThreadGuard guard(m_lockCache);
		m_oldTrainInfoCache.insert(std::pair<TA_IRS_Bus::TrainID,TrainInfo*>(newTrainInfo->TrainID,newTrainInfo));

		FUNCTION_EXIT;
	}

	bool TrainDataLocation_IDBased::isTrainIDExist(TrainInfo* trainInfo)
	{
		FUNCTION_ENTRY("isTrainIDExist");
         TA_Base_Core::ThreadGuard guard(m_lockCache);
		TrainInfoMapIt iterTrain = m_oldTrainInfoCache.find(trainInfo->TrainID); 
		bool _ret = false;
		if (iterTrain != m_oldTrainInfoCache.end())
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Train ID : %s is exist in old cache",trainInfo->TrainID.c_str());
			_ret = true;
		}

		FUNCTION_EXIT;
		return _ret;
	}

	void TrainDataLocation_IDBased::updateActiveTrainID(const std::string& trainID)
	{
		FUNCTION_ENTRY("updateActiveTrainID");
        TA_Base_Core::ThreadGuard guard(m_lockCache);
		TA_IRS_Bus::ActiveTrainServNumIt iter = std::find(m_activeTrainID.begin(),m_activeTrainID.end(),trainID);
		if (iter == m_activeTrainID.end())
		{
			m_activeTrainID.push_back(trainID);
		}

		FUNCTION_EXIT;
	}

	void TrainDataLocation_IDBased::updateDataPoint(TrainInfo* oldTrainInfo, TrainInfo* newTrainInfo, const TrainInfoVec& newTrainInfoVec)
	{
		FUNCTION_ENTRY("updateDataPoint");

		//For Old Train Information
		//set old data point to empty when newTrainInfo is null, 
		if (TrainDataLocation::needUpdateOldTrack(oldTrainInfo, newTrainInfo, newTrainInfoVec)==true)
		{
			//Update Train Service Number Datapoint
			updateTrainServNumDatapoint(oldTrainInfo);
			//Update Train Direction Datapoint
			updateTrainDirDatapoint(oldTrainInfo);
			//Update Train Congestion Datapoint
			updateTrainCongestionDatapoint(oldTrainInfo);
			//Update Train Drive Mode
			updateTrainDriveMode(oldTrainInfo);
			//Update Train Position Lost Flag
			updateTrainPosLostFlag(oldTrainInfo);
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Updated old track Train Object ID=%s,track=%s", oldTrainInfo->TrainID.c_str(), oldTrainInfo->TrainTrackLocation.c_str());
		}

		//For New Train Information
		if (NULL != newTrainInfo)
		{
			//Update Train Service Number Datapoint
			updateTrainServNumDatapoint(newTrainInfo);
			//Update Train Direction Datapoint
			updateTrainDirDatapoint(newTrainInfo);
			//Update Train Congestion Datapoint
			updateTrainCongestionDatapoint(newTrainInfo);
			//Update Train Drive Mode
			updateTrainDriveMode(newTrainInfo);
			//Update Train Position Lost Flag
			updateTrainPosLostFlag(newTrainInfo);
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Updated new track Train Object ID=%s,track=%s", newTrainInfo->TrainID.c_str(), newTrainInfo->TrainTrackLocation.c_str());
		}


		FUNCTION_EXIT;
	}

	void TrainDataLocation_IDBased::updateTrainServNumDatapoint(TrainInfo* trainInfo)
	{
		FUNCTION_ENTRY("updateTrainServNumDatapoint");

		std::string DPAddress = m_TrainServAddress + trainInfo->TrainTrackLocation;
		std::string DPValue = trainInfo->TrainServiceNumber;

		setLocalDataPoint(DPAddress,DPValue);

		FUNCTION_EXIT;
	}

	void TrainDataLocation_IDBased::updateTrainDirDatapoint(TrainInfo* trainInfo)
	{
		FUNCTION_ENTRY("updateTrainDirDatapoint");

		std::string DPAddress = m_TrainDirAddress + trainInfo->TrainTrackLocation;
		unsigned long DPValue = trainInfo->genaTrainDirection;

		setLocalDataPoint(DPAddress,DPValue);

		FUNCTION_EXIT;
	}

	void TrainDataLocation_IDBased::updateTrainCongestionDatapoint(TrainInfo* trainInfo)
	{
		FUNCTION_ENTRY("updateTrainCongestionDatapoint");

		std::string DPAddress = m_TrainCongestionAddress + trainInfo->TrainTrackLocation;
		bool DPValue = static_cast<int>(trainInfo->TrainCongestion);

		if(BOOLEAN_TYPE_INVALID_VALUE > trainInfo->TrainCongestion)
		{
			setLocalDataPoint(DPAddress,DPValue,TA_Base_Core::ITA_QUALITY_GOOD);
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
				"Train Object ID=%s,received an invalid congestion value, set DataPoint quality to bad!", trainInfo->TrainID.c_str());
			//setLocalDataPoint(DPAddress,DPValue,TA_Base_Core::ITA_QUALITY_BAD);
		}
		FUNCTION_EXIT;
	}

	void TrainDataLocation_IDBased::updateTrainDriveMode(TrainInfo* trainInfo)
	{
		FUNCTION_ENTRY("updateTrainDriveMode");

		std::string DPAddress = m_TrainDriveModeAddress + trainInfo->TrainTrackLocation;
		unsigned long DPValue = static_cast<unsigned long>(trainInfo->TrainDriveMode);

		setLocalDataPoint(DPAddress,DPValue);

		FUNCTION_EXIT;
	}

	void TrainDataLocation_IDBased::updateTrainPosLostFlag(TrainInfo* trainInfo)
	{
		FUNCTION_ENTRY("updateTrainPosLostFlag");

		std::string DPAddress = m_TrainPosLostAddress + trainInfo->TrainTrackLocation;
		bool DPValue = trainInfo->isTrainPosLost;

		setLocalDataPoint(DPAddress,DPValue,TA_Base_Core::ITA_QUALITY_GOOD);

		FUNCTION_EXIT;
	}

	void TrainDataLocation_IDBased::syncTrainDataBegin()
	{
		FUNCTION_ENTRY("syncTrainDataBegin");

		TA_Base_Core::ThreadGuard guard(m_lockCache);
		//Clear active train data cache
		m_activeTrainInfoVec.clear();
		m_blnSyncBegin=true;

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Train Data Sync Begin ...");
		FUNCTION_EXIT;
	}

	void TrainDataLocation_IDBased::syncTrainDataEnd()
	{
		FUNCTION_ENTRY("syncTrainDataEnd");
		//not need to update, due to new trains have been updated in updateActiveTrainList method after receiving train data.
		/*TrainInfoVec::iterator iterTrain = m_activeTrainInfoVec.begin();
		for (iterTrain; iterTrain != m_activeTrainInfoVec.end(); iterTrain++ )
		{
			updateTrainlocation((*iterTrain),m_activeTrainInfoVec);
		}*/
		//remove non-existing record and clear data point based on oldtraininfo.

		TA_Base_Core::ThreadGuard guard(m_lockCache);
		checkTrainInfoCache(m_activeTrainInfoVec);
		m_activeTrainInfoVec.clear();
		m_blnSyncBegin=false;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Train Data Sync End ...");
		FUNCTION_EXIT;
	}

	void TrainDataLocation_IDBased::updateActiveTrainList( TrainInfo& trainInfo)
	{
		FUNCTION_ENTRY("updateActiveTrainList");
		if (m_blnSyncBegin)  
		{
			//Process syncBegin and SyncEnd;
			//update train location immediately 
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Sync Begin Update train %s data, object ID %s", trainInfo.msgType.c_str(), trainInfo.TrainID.c_str());

				TA_Base_Core::ThreadGuard guard(m_lockCache);
				updateTrainlocation(trainInfo,m_activeTrainInfoVec);
				//push train into new train list
				m_activeTrainInfoVec.push_back(trainInfo);

			}
		}
		else if("gena:death" == trainInfo.msgType) // Received death message
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Process train message type %s", trainInfo.msgType.c_str());
			receivedTrainDeathMsg(trainInfo);
		}
		else if (("gena:birth" == trainInfo.msgType) ||
				("gena:update" == trainInfo.msgType))//birth, update
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Process train message type %s", trainInfo.msgType.c_str());
			//m_helpInfoVec send an empty vector to just add train Info
			m_helpInfoVec.clear();
			updateTrainlocation(trainInfo,m_helpInfoVec);
		}

		FUNCTION_EXIT;
	}

	void TrainDataLocation_IDBased::receivedTrainDeathMsg(const TrainInfo& trainInfo)
	{
		FUNCTION_ENTRY("receivedTrainDeathMsg");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Received Train Death Message, TrainID %s", trainInfo.TrainID.c_str());

		TrainInfoMapIt iterCache = m_oldTrainInfoCache.find(trainInfo.TrainID);
		if (iterCache != m_oldTrainInfoCache.end())
		{	
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Removed TrainID %s from data cache", trainInfo.TrainID.c_str());
			//Get the old Train Info
			TrainInfo* oldTrainInfo = (*iterCache).second;
			//Must set to empty value to set the datapoint
			TrainDataLocation::constructEmptyValue(oldTrainInfo);
			//Update Train Service Number Datapoint
			updateTrainServNumDatapoint(oldTrainInfo);
			//Update Train Direction Datapoint
			updateTrainDirDatapoint(oldTrainInfo);
			//Update Train Congestion Datapoint
			updateTrainCongestionDatapoint(oldTrainInfo);
			//Update Train Drive Mode
			updateTrainDriveMode(oldTrainInfo);
			//Update Train Position Lost Flag
			updateTrainPosLostFlag(oldTrainInfo);
			delete oldTrainInfo;
			oldTrainInfo=NULL;

			//then remove the item from the old cache
			m_oldTrainInfoCache.erase(iterCache);
		}
		FUNCTION_EXIT;
	}
}