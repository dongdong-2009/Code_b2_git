
#include "bus/ats/ats_agent/CommonATS/src/TrainDataLocation.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "core/exceptions/src/CtaScadaExceptions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include <algorithm>

namespace TA_IRS_Bus
{
	TrainDataLocation::TrainDataLocation(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: AtsSCADAData(itScada,"TrainLocationData"),
		  m_TrainServAddress(""),
		  m_TrainDirAddress(""),
		  m_TrainCongestionAddress("")
	{
		FUNCTION_ENTRY("TrainDataLocation");

		m_trackInfo              = TA_IRS_Bus::AtsHelper::getAllTrackInformation();
		m_TrainServAddress       = EntityData->getTrainServiceNumberAddress();
		m_TrainDirAddress        = EntityData->getTrainDirectionAddress();
		m_TrainCongestionAddress = EntityData->getStopFlagAddress();
		m_activeTrainSerNumber.clear();
		m_oldTrainInfoCache.clear();
		m_tempTrainInfoCache.clear();

		FUNCTION_EXIT;
	}

	TrainDataLocation::~TrainDataLocation()
	{
		FUNCTION_ENTRY("~TrainDataLocation");
		FUNCTION_EXIT;
	}

	void TrainDataLocation::updateTrainlocation(TrainInfo& trainInfo, const TrainInfoVec& trainInfoVec)
	{
		FUNCTION_ENTRY("updateTrainlocation");

		//Check if the Track name exist in database.
		//Data will not be processed if the Track Name does not exist.
		if (false == isTrackNameExist(trainInfo))
			return;

		TrainInfo* newTrainInfo = createTrainInfoCopy(trainInfo);
		
		//Update active Train Service Number
		updateActiveTrainServNum(newTrainInfo->TrainServiceNumber);

		//Check if the Train Service number exist in the old cache
		if (false == isTrainServNumExist(newTrainInfo))
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
			TrainInfo oldTrainInfo = getTrainOldRecordCache(newTrainInfo->TrainServiceNumber);
			//Construct empty value to update datapoint 
			constructEmptyValue(&oldTrainInfo);
			//Update datapoint for old and new train info
			updateDataPoint(&oldTrainInfo, newTrainInfo, trainInfoVec);
			//Need to remove the old Train Info from the Cache to replace for a new Train Info
			removeTrainInfoCache(newTrainInfo->TrainServiceNumber);
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

	void TrainDataLocation::checkTrainInfoCache(const TrainInfoVec& trainInfoVec)
	{
		FUNCTION_ENTRY("checkTrainInfoCache");

		//Need to check the Active Train Service number with the Train info Cache.
		//If the Train info Cache contains inactive Train Service number,
		//Train Service number must be removed from the cache and update the datapoint
		TA_IRS_Bus::ActiveTrainServNumIt iterActive = m_activeTrainSerNumber.begin();
		for(iterActive; iterActive != m_activeTrainSerNumber.end(); iterActive++)
		{
			//Check the active Train Service number from the Train Info cache
			TrainInfoMapIt iterCache = m_oldTrainInfoCache.find((*iterActive));
			if (iterCache != m_oldTrainInfoCache.end())
			{
				//If found, need to place the Train info from the temp cache.
				m_tempTrainInfoCache.insert(std::pair<TA_IRS_Bus::TrainServiceNumber,TrainInfo*>((*iterCache).second->TrainServiceNumber,(*iterCache).second));
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
			constructEmptyValue(oldTrainInfo);
			TrainInfo* newTrainInfo = NULL;
			//Update the Datapoint 
			updateDataPoint(oldTrainInfo,newTrainInfo,trainInfoVec);
			delete oldTrainInfo;
			oldTrainInfo=NULL;
			//Then remove the item in the cache
			//m_oldTrainInfoCache.erase(iterRemCache);
		}
		m_oldTrainInfoCache.clear();
		//Clear m_activeTrainSerNumber
		m_activeTrainSerNumber.clear();
		//Place back the active Train service number from temp cache to the original cache
		m_oldTrainInfoCache = m_tempTrainInfoCache;
		//Then clear the temp cache
		m_tempTrainInfoCache.clear();

		FUNCTION_EXIT;
	}

	bool TrainDataLocation::isTrackNameExist(const TrainInfo& trainInfo)
	{
		FUNCTION_ENTRY("isTrackNameExist");

		bool _ret = true;
		TA_IRS_Bus::TrackInfoIt iterTrack = std::find(m_trackInfo.begin(),m_trackInfo.end(),trainInfo.TrainTrackLocation);
		if ( iterTrack == m_trackInfo.end() )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Track Location : %s does not exist in the database. Data will not be processed.",trainInfo.TrainTrackLocation.c_str());
			_ret = false;
		}

		FUNCTION_EXIT;
		return _ret;
	}
	TrainInfo* TrainDataLocation::createTrainInfoCopy(const TrainInfo& trainInfo)
	{
		FUNCTION_ENTRY("createTrainInfoCopy");

		TrainInfo* newTrainInfo = new TrainInfo;
		newTrainInfo->TrainServiceNumber = trainInfo.TrainServiceNumber;
		newTrainInfo->TrainDirection     = trainInfo.TrainDirection;
		newTrainInfo->TrainTrackLocation = trainInfo.TrainTrackLocation;
		newTrainInfo->TrainCongestion    = trainInfo.TrainCongestion;
		newTrainInfo->lastTrainRecord    = trainInfo.lastTrainRecord;
        newTrainInfo->shownTrainOnTrack  = trainInfo.shownTrainOnTrack;
		newTrainInfo->TrainDriveMode	 = trainInfo.TrainDriveMode;
		newTrainInfo->genaTrainDirection = trainInfo.genaTrainDirection;
		newTrainInfo->TrainID = trainInfo.TrainID;
		newTrainInfo->isTrainPosLost=trainInfo.isTrainPosLost;
		//newTrainInfo->msgType				= trainInfo.msgType;
		FUNCTION_EXIT;
		return newTrainInfo;
	}

	TrainInfo TrainDataLocation::getTrainOldRecordCache(const std::string& trainServiceNumber)
	{
		FUNCTION_ENTRY("getTrainInfoRecord");

		TrainInfo oldRecord;
		TrainInfoMapIt iterRecord = m_oldTrainInfoCache.find(trainServiceNumber);
		if( iterRecord != m_oldTrainInfoCache.end())
		{
			oldRecord = (*(*iterRecord).second);
		}

		FUNCTION_EXIT;
		return oldRecord;

	}

	void TrainDataLocation::removeTrainInfoCache(const std::string& trainServiceNum)
	{
		FUNCTION_ENTRY("removeOldTrainRecordCache");

		TrainInfoMapIt iterRecord = m_oldTrainInfoCache.find(trainServiceNum);
		if( iterRecord != m_oldTrainInfoCache.end())
		{
			delete (*iterRecord).second;
			(*iterRecord).second = NULL;

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Train Info with Service Number : %s, will be removed from the Cache",
				trainServiceNum.c_str());
			m_oldTrainInfoCache.erase(iterRecord);
		}

		FUNCTION_EXIT;
	}

	void TrainDataLocation::constructEmptyValue(TrainInfo* trainInfo)
	{
		FUNCTION_ENTRY("constructEmptyValue");

		//initialize the value.
		trainInfo->TrainServiceNumber = " ";
		trainInfo->TrainDirection     = TA_IRS_Bus::EMPTY_VAL;
		trainInfo->TrainCongestion    = true;
		trainInfo->TrainID			  = TA_IRS_Bus::EMPTY_VAL;
		trainInfo->lastTrainRecord		= false;
		trainInfo->shownTrainOnTrack	= false;
		trainInfo->TrainDriveMode		= 9; // 9-unknown status
		trainInfo->genaTrainDirection	= 2; // 2-unknown status
		trainInfo->msgType				= TA_IRS_Bus::EMPTY_VAL;
		trainInfo->isTrainPosLost		= false;
		//cannot set track to null, due to  TrainDataLocation_IDBased::checkTrainInfoCache uses the track to judge if its need update old track or not.
		FUNCTION_EXIT;
	}

	void TrainDataLocation::insertTrainInfoCache(TrainInfo* newTrainInfo)
	{
		FUNCTION_ENTRY("updateOldTrainInfoCache");

		m_oldTrainInfoCache.insert(std::pair<TA_IRS_Bus::TrainServiceNumber,TrainInfo*>(newTrainInfo->TrainServiceNumber,newTrainInfo));

		FUNCTION_EXIT;
	}

	bool TrainDataLocation::isTrainServNumExist(TrainInfo* trainInfo)
	{
		FUNCTION_ENTRY("isTrainServNumExist");

		TrainInfoMapIt iterTrain = m_oldTrainInfoCache.find(trainInfo->TrainServiceNumber); 
		bool _ret = false;
		if (iterTrain != m_oldTrainInfoCache.end())
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Train Service Number : %s is exist in old cache",trainInfo->TrainServiceNumber.c_str());
			_ret = true;
		}

		FUNCTION_EXIT;
		return _ret;
	}

	void TrainDataLocation::updateDataPoint(TrainInfo* oldTrainInfo, TrainInfo* newTrainInfo, const TrainInfoVec& newTrainInfoVec)
	{
		FUNCTION_ENTRY("updateDataPoint");

		//For Old Train Information
		//set old data point to empty when newTrainInfo is null, 
		if (needUpdateOldTrack(oldTrainInfo, newTrainInfo, newTrainInfoVec)==true)
		{
			//Update Train Service Number Datapoint
			updateTrainServNumDatapoint(oldTrainInfo);
			//Update Train Direction Datapoint
			updateTrainDirDatapoint(oldTrainInfo);
			//Update Train Congestion Datapoint
			updateTrainCongestionDatapoint(oldTrainInfo);
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Updated old track ServiceNum=%s,track=%s", oldTrainInfo->TrainServiceNumber.c_str(), oldTrainInfo->TrainTrackLocation.c_str());
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
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Updated new track ServiceNum=%s,track=%s", newTrainInfo->TrainServiceNumber.c_str(), newTrainInfo->TrainTrackLocation.c_str());
		}
		

		FUNCTION_EXIT;
	}

	void TrainDataLocation::updateActiveTrainServNum(const std::string& trainServNum)
	{
		FUNCTION_ENTRY("updateActiveTrainServNum");

		TA_IRS_Bus::ActiveTrainServNumIt iter = std::find(m_activeTrainSerNumber.begin(),m_activeTrainSerNumber.end(),trainServNum);
		if (iter == m_activeTrainSerNumber.end())
		{
			m_activeTrainSerNumber.push_back(trainServNum);
		}

		FUNCTION_EXIT
	}

	void TrainDataLocation::updateTrainServNumDatapoint(TrainInfo* trainInfo)
	{
		FUNCTION_ENTRY("updateTrainServNumDatapoint");

		std::string DPTrainServAddress = m_TrainServAddress + trainInfo->TrainTrackLocation;
		std::string DPTrainServValue = trainInfo->TrainServiceNumber;

		setLocalDataPoint(DPTrainServAddress,DPTrainServValue);

		FUNCTION_EXIT;
	}

	void TrainDataLocation::updateTrainDirDatapoint(TrainInfo* trainInfo)
	{
		FUNCTION_ENTRY("updateTrainDirDatapoint");

		std::string DPTrainDirAddress = m_TrainDirAddress + trainInfo->TrainTrackLocation;
		std::string DPTrainDirValue = trainInfo->TrainDirection;

		setLocalDataPoint(DPTrainDirAddress,DPTrainDirValue);

		FUNCTION_EXIT;
	}

	void TrainDataLocation::updateTrainCongestionDatapoint(TrainInfo* trainInfo)
	{
		FUNCTION_ENTRY("updateTrainCongestionDatapoint");

		std::string DPTrainCongestionAddress = m_TrainCongestionAddress + trainInfo->TrainTrackLocation;
		bool DPTrainCongestionValue = static_cast<int>(trainInfo->TrainCongestion);

		setLocalDataPoint(DPTrainCongestionAddress,DPTrainCongestionValue);

		FUNCTION_EXIT;
	}

	void TrainDataLocation::initializeDataPoint()
	{
		FUNCTION_ENTRY("initializeDatapoint");

		//For Train Service Address
		setAllLocalDataPointStateMatchingAddress(m_TrainServAddress,EMPTY_VAL,
			TA_Base_Core::ITA_QUALITY_GOOD); 

		//For Train Direction Address
		setAllLocalDataPointStateMatchingAddress(m_TrainDirAddress,EMPTY_VAL,
			TA_Base_Core::ITA_QUALITY_GOOD); 

		//For Train Congestion Address
		setAllLocalDataPointStateMatchingAddress(m_TrainCongestionAddress,EMPTY_VAL,
			TA_Base_Core::ITA_QUALITY_GOOD); 
	
		FUNCTION_EXIT;
	}

	void TrainDataLocation::updateAllDataPointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateAllDataPointQuality");

		//This will update the All Datapoint based on the Address
		//For Train Service Address
		setAllLocalDataPointQualitiesMatchingAddress(m_TrainServAddress,quality);

		//For Train Direction Address
		setAllLocalDataPointQualitiesMatchingAddress(m_TrainDirAddress,quality);

		//For Train Congestion Address
		setAllLocalDataPointQualitiesMatchingAddress(m_TrainCongestionAddress,quality);

		FUNCTION_EXIT;
	}

	bool TrainDataLocation::needUpdateOldTrack(TrainInfo* oldTrainInfo, TrainInfo* newTrainInfo, const TrainInfoVec& newTrainInfoVec)
	{
		FUNCTION_ENTRY("needUpdateOldTrack");

		//Role of update track location of train (25/Feb/2016)
		// 1.  old track location of the train  = new track location of the train, only need to update the track based on new train info.
		// 2.  old track location of the train != new track location of the train.
		//2.1 if the old track location of the train has been occupied and updated by another train, only need to update the new track based on the new train info.
		//2.2 if the old track location of the train has not been occupied and updated by another new train, we need update old track first, then update the new track.
		// 3.  disappeared trains,  update the old track if these is no new train occupied the track.

		if (NULL == oldTrainInfo)
		{
			return false;
		}
		if (NULL==newTrainInfo) //there is no train in the track, removing disappeared train
		{			 
			//Check if another new train has occupied the old track.
			TrainInfoVec::const_iterator iterTrain = newTrainInfoVec.begin();
			for (iterTrain; iterTrain != newTrainInfoVec.end(); iterTrain++ )
			{
				if (iterTrain->TrainTrackLocation == oldTrainInfo->TrainTrackLocation)
				{
					return !iterTrain->shownTrainOnTrack;  //if another train has occupied the old track, there is no need to update the old track.
				}
			}
			return true;
		}
		//train is in the same track, only need to update new
		else if (oldTrainInfo->TrainTrackLocation == newTrainInfo->TrainTrackLocation  
			     /*&& oldTrainInfo->TrainCongestion == newTrainInfo->TrainCongestion  &&
				 oldTrainInfo->TrainServiceNumber == newTrainInfo->TrainServiceNumber &&
				 oldTrainInfo->TrainDriveMode == newTrainInfo->TrainDriveMode &&
				 oldTrainInfo->TrainDirection == newTrainInfo->TrainDirection*/)
		{
			return false;
		}
		else
		{
			//Check if another new train has occupied the old track.
			TrainInfoVec::const_iterator iterTrain = newTrainInfoVec.begin();
			for (iterTrain; iterTrain != newTrainInfoVec.end(); iterTrain++ )
			{
				if (iterTrain->TrainTrackLocation == oldTrainInfo->TrainTrackLocation &&
					iterTrain->TrainServiceNumber != newTrainInfo->TrainServiceNumber)
				{
					return !iterTrain->shownTrainOnTrack;  //if another train has occupied the old track, there is no need to update the old track.
				}
			}
		}
		return true;

		FUNCTION_EXIT;
	}

	std::vector<TrainInfo*> TrainDataLocation::getTrainAtsInfoCacheList()
	{
		FUNCTION_ENTRY("getTrainAtsInfoCacheList");
		std::vector<TrainInfo*> retVal;
		IDBaseTrainInfoMap::iterator itr = m_oldTrainInfoCache.begin();
		for (itr; itr!=m_oldTrainInfoCache.end(); itr++)
		{
			TrainInfo* pNewData = createTrainInfoCopy(*(itr->second));
			retVal.push_back(pNewData);
		}

		FUNCTION_EXIT;
		return retVal;
	}
}
