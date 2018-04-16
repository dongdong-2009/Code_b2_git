#include "bus/security/duty_agent/duty_agent_state/src/DutyAgentState.h"
#include "app/security/duty_agent/src/DutyCenter.h"

namespace TA_Base_App
{
	DutyCenter::DutyCenter(TA_Base_Bus::DutyPeerTaskManager& dutyPeerTaskManager)
	:m_dutyPeerTaskManager(dutyPeerTaskManager),
	m_commsSender(TA_Base_Core::MessagePublicationManager::getInstance().
			getCommsMessageSender(TA_Base_Core::DutyAgentBroadcastComms::Context))
	{
		DASType::LocationSet locations;
		DASType::LocationSet::const_iterator locIt;
		DutyDbCache::instance()->getAllLocationKeys( locations );

		for( locIt = locations.begin(); locIt != locations.end(); locIt++ )
		{
			if( *locIt != TA_Base_Bus::TransactiveAgentInfo::m_location )
			{
				m_stationStatusMap[*locIt] = OnLine;
			}
		}
	}

	DutyCenter::~DutyCenter()
	{

	}

	void	DutyCenter::stationOnLine(unsigned long locationKey)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"DutyCenter::stationOnLine %u",locationKey);
		
		std::map<unsigned long,StationStatus>::iterator foundItr = m_stationStatusMap.find(locationKey);
		if(foundItr != m_stationStatusMap.end())
		{
			if(foundItr->second == OffLine)
			{
				//from offline to online
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Begin update to remote peer");
				m_dutyPeerTaskManager.updateDutyPeer(locationKey, this->getRemoteDutyPeer(locationKey));
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"End update to remote peer");
				this->broadcastStationStatus(locationKey, OnLine);
			}
			foundItr->second = OnLine;
		}
	}


	void	DutyCenter::stationOffLine(unsigned long locationKey)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"DutyCenter::stationOffLine %u",locationKey);

		std::map<unsigned long,StationStatus>::iterator foundItr = m_stationStatusMap.find(locationKey);
		if(foundItr != m_stationStatusMap.end())
		{
			if(foundItr->second == OnLine)
			{
				//from online to offline
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Begin update to local peer");
				m_dutyPeerTaskManager.updateDutyPeer(locationKey, this->getLocalDutyPeer(locationKey));
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"End update to local peer");
				this->broadcastStationStatus(locationKey, OffLine);
			}
			foundItr->second = OffLine;
		}
	}


	bool	DutyCenter::isStationOnLine(unsigned long locationKey)
	{
		std::map<unsigned long,StationStatus>::iterator foundItr = m_stationStatusMap.find(locationKey);
		if(foundItr != m_stationStatusMap.end())
		{
			return foundItr->second == OnLine;
		}
	}
	

	TA_Base_Bus::DASType::LocationSet  DutyCenter::getOffLineStations()
	{
		TA_Base_Bus::DASType::LocationSet rtnLocationSet;
		std::map<unsigned long,StationStatus>::iterator  statusItr = m_stationStatusMap.begin();

		for(;statusItr != m_stationStatusMap.end(); ++statusItr)
		{
			if( OffLine == statusItr->second)
			{
				rtnLocationSet.insert(statusItr->first);
			}
		}

		return rtnLocationSet;
	}
	
	
	std::auto_ptr<IDutyPeer>	DutyCenter::getLocalDutyPeer(unsigned long locationKey)
	{
		return std::auto_ptr<IDutyPeer>(new LocalDutyPeer(locationKey));
	}


	std::auto_ptr<IDutyPeer>	DutyCenter::getRemoteDutyPeer(unsigned long locationKey)
	{
		return std::auto_ptr<IDutyPeer>(new RemoteDutyPeer(locationKey));
	}


	void DutyCenter::broadcastStationStatus(unsigned long locationKey, StationStatus status)
	{
		TA_Base_Bus::DutyAgentTypeCorbaDef::StationStatus stationStatus;
		stationStatus.regionKey = locationKey;
		stationStatus.isOnline  = status==OnLine?true:false;
		CORBA::Any commsData;
		commsData <<= stationStatus;
		m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::StationOnLineOffLineNotification, commsData, NULL );
	}
}