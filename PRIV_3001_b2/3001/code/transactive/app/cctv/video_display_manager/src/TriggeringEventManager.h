#pragma once

#include <boost/noncopyable.hpp>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <map>

namespace TA_IRS_Core
{
    class IVideoTriggeringEvent;
}

namespace TA_IRS_App
{
	class TriggeringEventManager : boost::noncopyable
	{
	public:
		typedef std::pair<unsigned long, unsigned long> EventPair;
		static TriggeringEventManager& instance();
		TA_IRS_Core::IVideoTriggeringEvent* getVideoTriggeringEvent(unsigned long staitonId, unsigned long eventId);
		unsigned long getLocationKeyByStationId(unsigned long stationId); // return -1 as invalid
		std::string getLocationNameByStationId(unsigned long stationId); // return "" as invalid
	private:
		static TA_Base_Core::ReEntrantThreadLockable s_lock;
		static TriggeringEventManager* s_instance;
		TriggeringEventManager();
		std::map<EventPair, TA_IRS_Core::IVideoTriggeringEvent*> m_eventsMap;
		std::map<unsigned long, unsigned long> m_stationIdToLocationKeyMap;
	};
}