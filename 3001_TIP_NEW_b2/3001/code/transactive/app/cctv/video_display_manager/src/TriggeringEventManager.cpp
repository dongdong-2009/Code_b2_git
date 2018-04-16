#include "core/data_access_interface/video_switch_agent/src/IVideoTriggeringEvent.h"
#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.cpp"
#include "core/data_access_interface/video_switch_agent/src/VideoStationIdAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "TriggeringEventManager.h"

namespace TA_IRS_App
{
	TA_Base_Core::ReEntrantThreadLockable TriggeringEventManager::s_lock;
	TriggeringEventManager* TriggeringEventManager::s_instance = NULL;

	TriggeringEventManager::TriggeringEventManager()
	{
	}

	TriggeringEventManager& TriggeringEventManager::instance()
	{
		if(NULL == s_instance)
		{
			TA_Base_Core::ThreadGuard guard(s_lock);
			if(NULL == s_instance)
			{
				s_instance = new TriggeringEventManager();
			}
		}
		return *s_instance;
	}

	unsigned long TriggeringEventManager::getLocationKeyByStationId(unsigned long stationId)
	{
		TA_Base_Core::ThreadGuard guard(s_lock);
		unsigned long key = -1;
		if(m_stationIdToLocationKeyMap.empty())
		{
	        try
	        {
				m_stationIdToLocationKeyMap = TA_IRS_Core::VideoStationIdAccessFactory::getInstance().getStationIdToLocationKeyMap();
	        }
	        catch (...)
	        {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Load stationid to locationkey map failed");
	        }
		}
		if(m_stationIdToLocationKeyMap.find(stationId) != m_stationIdToLocationKeyMap.end())
		{
			key = m_stationIdToLocationKeyMap[stationId];
		}
		return key;
	}

	std::string TriggeringEventManager::getLocationNameByStationId(unsigned long stationId)
	{
		TA_Base_Core::ThreadGuard guard(s_lock);
		std::string location = "";
		try
		{
			location = TA_IRS_Bus::LocationCache::getInstance().getLocationByKey(getLocationKeyByStationId(stationId));
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can not get location name for stationid:%lu", stationId);
		}
		return location;
	}

	TA_IRS_Core::IVideoTriggeringEvent* TriggeringEventManager::getVideoTriggeringEvent(unsigned long staitonId, unsigned long eventId)
	{
		TA_Base_Core::ThreadGuard guard(s_lock);
		EventPair pair(staitonId, eventId);
		TA_IRS_Core::IVideoTriggeringEvent* event = NULL;
		if(m_eventsMap.find(pair) == m_eventsMap.end())
		{
			try
			{
				event = TA_IRS_Core::VideoTriggeringEventAccessFactory::getInstance().getVideoTriggeringEvent(pair.first, pair.second);
				m_eventsMap.insert(std::make_pair(pair,event));
				event = m_eventsMap[pair];
			}
			catch (...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Load event stationid:%lu, eventid:%lu failed", pair.first, pair.second);
			}
		}
		else
		{
			event = m_eventsMap[pair];
		}
		return event;
	}
}