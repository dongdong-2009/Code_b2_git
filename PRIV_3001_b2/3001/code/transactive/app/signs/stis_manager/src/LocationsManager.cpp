#include "LocationsManager.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

TA_Base_Core::ReEntrantThreadLockable LocationsManager::m_lock;
LocationsManager* LocationsManager::m_instance = 0;

LocationsManager& LocationsManager::instance()
{
	if(0 == m_instance)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		if(0 == m_instance)
		{
			m_instance = new LocationsManager();
		}
	}
	return *m_instance;
}

LocationsManager::LocationsManager()
{
	
	try
	{
		m_allLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
		for(std::vector<TA_Base_Core::ILocation*>::iterator it = m_allLocations.begin();
			it != m_allLocations.end();
			++it)
		{
			TA_Base_Core::ILocation* location = *it;
			m_locationKeyMap.insert(std::make_pair(location->getKey(), location));
			m_locationNameMap.insert(std::make_pair(location->getName(), location));
		}
	}
	catch (...)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed get location information");
	}
}

TA_Base_Core::ILocation* LocationsManager::getLocationByName(const std::string& locationName)
{
	TA_Base_Core::ILocation* location = NULL;
	if(m_locationNameMap.find(locationName) != m_locationNameMap.end())
	{
		location = m_locationNameMap[locationName];
	}
	return location;
}

TA_Base_Core::ILocation* LocationsManager::getLocationByKey(unsigned long locationKey)
{
	TA_Base_Core::ILocation* location = NULL;
	if(m_locationKeyMap.find(locationKey) != m_locationKeyMap.end())
	{
		location = m_locationKeyMap[locationKey];
	}
	return location;
}

std::vector<TA_Base_Core::ILocation*> LocationsManager::getAllLocations()
{
	return m_allLocations;
}