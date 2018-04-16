#include "StdAfx.h"
#include "TimeTableDatapointPairManager.h"


namespace TA_IRS_App
{
	TimeTableDatapointPairManager::TimeTableDatapointPairManager(void)
		:m_listener(NULL)
	{
	}

	void TimeTableDatapointPairManager::uninit()
	{
		TA_THREADGUARD( m_lckDatapointsMap );

		for (ModeAndTimeDatapointPairMap::iterator it = m_datapointsPairMap.begin(); it != m_datapointsPairMap.end(); ++it)
        {
            for(ModeAndTimeDatapointPairVct::iterator itPair = it->second.m_currentDay.begin(); itPair != it->second.m_currentDay.end(); itPair++)
            {
                if ((*itPair) != NULL)
                {
                    (*itPair)->cancelWaitInitialization();
                }
            }

            for(ModeAndTimeDatapointPairVct::iterator itPair = it->second.m_nextDay.begin(); itPair != it->second.m_nextDay.end(); itPair++)
            {
                if ((*itPair) != NULL)
                {
                    (*itPair)->cancelWaitInitialization();
                }
            }

			for(ModeAndTimeDatapointPairVct::iterator itPair = it->second.m_currentDay.begin(); itPair != it->second.m_currentDay.end(); itPair++)
			{
				if ((*itPair) != NULL)
				{
					(*itPair)->uninit();
				}
            }

            it->second.m_currentDay.clear();

			for(ModeAndTimeDatapointPairVct::iterator itPair = it->second.m_nextDay.begin(); itPair != it->second.m_nextDay.end(); itPair++)
			{
				if ((*itPair) != NULL)
				{
					(*itPair)->uninit();
				}
            }

            it->second.m_nextDay.clear();
		}

        m_datapointsPairMap.clear();
	}

	TimeTableDatapointPairManager::~TimeTableDatapointPairManager(void)
	{
	}

	void TimeTableDatapointPairManager::getTimeTablePairsByLocation(unsigned long locationKey, bool isCurrentDay, ModeAndTimeDatapointPairVct& datapointsVct)
	{
        FUNCTION_ENTRY("getTimeTablePairsByLocation");

        TA_THREADGUARD( m_lckDatapointsMap );

        ModeAndTimeDatapointPairMap::iterator itExist = m_datapointsPairMap.find(locationKey);

        if (itExist == m_datapointsPairMap.end())
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot find data point pair for this station. locationKey=%d",locationKey);
            return;
        }

		datapointsVct = isCurrentDay? itExist->second.m_currentDay : itExist->second.m_nextDay;
		
		FUNCTION_EXIT;
	}

	void TimeTableDatapointPairManager::getChildDataPointData(const std::string& dataNodeName, std::vector<TA_Base_Core::DataPointEntityDataPtr>& retValue)
	{
		FUNCTION_ENTRY("getChildDataPointData");
		try
		{
			TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(dataNodeName);
			TA_Base_Core::DataNodeEntityData* dnEntityData = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(entityData);

            if (NULL != dnEntityData)
			{
				TA_Base_Core::SharedIEntityDataList myDatapoints = dnEntityData->getChildDataPoints();
				TA_Base_Core::SharedIEntityDataList::iterator it;
				for (it = myDatapoints.begin(); it != myDatapoints.end(); it++)
				{
					TA_Base_Core::DataPointEntityData* dpEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData*>((*it).get());
					//TA_ASSERT(dpEntityData != NULL, "Equipment is not defined as datapoint entity");
					if (NULL == dpEntityData)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Equipment is not defined as datapoint entity,EquipmentName=%s", dataNodeName.c_str());
						continue;
					}

					TA_Base_Core::DataPointEntityDataPtr dp(dpEntityData);
					retValue.push_back(dp);
				}
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Equipment is not defined as datanode entity,EquipmentName=%s", dataNodeName.c_str());
			}
		}
		catch ( const std::exception& expWhat )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), expWhat.what());
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Caught unknown exception when getChildDataPointData");
		}
		
		FUNCTION_EXIT;
	}

	void TimeTableDatapointPairManager::registerModeBeginTimeUpdateListener(IModeBeginTimeUpdateInterface* listener)
	{
		if (NULL != listener)
		{
			m_listener = listener;
		}
	}

	void TimeTableDatapointPairManager::updateModeBeginTime(const unsigned long locationKey, const unsigned long entityKey)
	{
		if (NULL != m_listener)
		{
			m_listener->updateModeBeginTime(locationKey, entityKey);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Mode beginTime listener invalid, can not update the mode begintime");
		}
	}

    void TimeTableDatapointPairManager::initializeTimeTablePairsByLocation( unsigned long locationKey )
    {
        FUNCTION_ENTRY("initializeTimeTablePairsByLocation");

        ModeAndTimeDatapointPairVctPair datapointsVctPair;

        {
            TA_THREADGUARD( m_lckDatapointsMap );

            if (m_datapointsPairMap.find(locationKey) == m_datapointsPairMap.end())
            {
                TimeTableDatapointNameInfos timetableDatapointNameInfo;
                AgentAccessFactory::getInstance().getTimeTableDatapointNamesByLocation(locationKey, timetableDatapointNameInfo);
                
				if (!timetableDatapointNameInfo.empty())
				{
					std::string strDataNodeName = timetableDatapointNameInfo[0].todayTimeTableModeCodeFeedback.substr(0, timetableDatapointNameInfo[0].todayTimeTableModeCodeFeedback.find_last_of('.'));
					std::vector<TA_Base_Core::DataPointEntityDataPtr> myDatapointsToday;
					getChildDataPointData(strDataNodeName, myDatapointsToday);
					strDataNodeName = timetableDatapointNameInfo[0].tomorrowTimeTableModeCodeFeedback.substr(0, timetableDatapointNameInfo[0].tomorrowTimeTableModeCodeFeedback.find_last_of('.'));
					std::vector<TA_Base_Core::DataPointEntityDataPtr> myDatapointsNextDay;
					getChildDataPointData(strDataNodeName, myDatapointsNextDay);

					for (TimeTableDatapointNameInfos::iterator it = timetableDatapointNameInfo.begin(); it != timetableDatapointNameInfo.end(); it++)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Initialize Table Table Mode [%s][%s][%s][%s]",
							(*it).todayTimeTableBeginTimeFeedback.c_str(),(*it).todayTimeTableModeCodeFeedback.c_str(),
							(*it).tomorrowTimeTableBeginTimeFeedback.c_str(),(*it).tomorrowTimeTableModeCodeFeedback.c_str());

						ModeAndTimeDatapointPair_SmartPtr modeTimeTodayDp = ModeAndTimeDatapointPair_SmartPtr(new ModeAndTimeDatapointPair(locationKey, (*it).todayTimeTableModeCodeFeedback, (*it).todayTimeTableBeginTimeFeedback));
						modeTimeTodayDp->initializeDatapoint(myDatapointsToday);

						ModeAndTimeDatapointPair_SmartPtr modeTimeNextDayDp = ModeAndTimeDatapointPair_SmartPtr(new ModeAndTimeDatapointPair(locationKey, (*it).tomorrowTimeTableModeCodeFeedback, (*it).tomorrowTimeTableBeginTimeFeedback));
						modeTimeNextDayDp->initializeDatapoint(myDatapointsNextDay);

						datapointsVctPair.m_currentDay.push_back(modeTimeTodayDp);
						datapointsVctPair.m_nextDay.push_back(modeTimeNextDayDp);
					}

					m_datapointsPairMap.insert( ModeAndTimeDatapointPairMap::value_type(locationKey, datapointsVctPair) );
				}
            }
        }

        for (ModeAndTimeDatapointPairVct::iterator itPair = datapointsVctPair.m_currentDay.begin(); itPair != datapointsVctPair.m_currentDay.end(); itPair++)
        {
            (*itPair)->waitInitialization();
        }

        for (ModeAndTimeDatapointPairVct::iterator itPair = datapointsVctPair.m_nextDay.begin(); itPair != datapointsVctPair.m_nextDay.end(); itPair++)
        {
            (*itPair)->waitInitialization();
        }

        FUNCTION_EXIT;
    }

    ModeAndTimeDatapointPair_SmartPtr TimeTableDatapointPairManager::getTimeTablePairsByEntityKey( const unsigned long locationKey, const unsigned long entityKey, bool& bIsCurrentDay, bool& bIsNextDay )
    {
        FUNCTION_ENTRY("getTimeTablePairsByEntityKey");

        bIsCurrentDay = false;
        bIsNextDay = false;

        TA_THREADGUARD( m_lckDatapointsMap );

        ModeAndTimeDatapointPairMap::iterator itExist = m_datapointsPairMap.find(locationKey);

        if (itExist != m_datapointsPairMap.end())
        {
            for (ModeAndTimeDatapointPairVct::iterator itPair = itExist->second.m_currentDay.begin(); itPair != itExist->second.m_currentDay.end(); itPair++)
            {
                if ( (*itPair)->getDpKey() == entityKey )
                {
                    bIsCurrentDay = true;

                    FUNCTION_EXIT;
                    return *itPair;
                }
            }

            for (ModeAndTimeDatapointPairVct::iterator itPair = itExist->second.m_nextDay.begin(); itPair != itExist->second.m_nextDay.end(); itPair++)
            {
                if ( (*itPair)->getDpKey() == entityKey )
                {
                    bIsNextDay = true;

                    FUNCTION_EXIT;
                    return *itPair;
                }
            }
        }

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot find mode and time datapoint. locationKey=%d, entityKey=%d", locationKey, entityKey);
        FUNCTION_EXIT;
        return ModeAndTimeDatapointPair_SmartPtr( static_cast<ModeAndTimeDatapointPair*>(NULL) );
    }
}
