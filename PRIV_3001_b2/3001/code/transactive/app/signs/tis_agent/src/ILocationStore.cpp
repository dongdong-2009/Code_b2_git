#include "ILocationStore.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

namespace TA_IRS_App
{
    ILocationStore* ILocationStore::m_theClassInstance = NULL;
    TA_Base_Core::ReEntrantThreadLockable ILocationStore::m_ilocationMapAccessLock;
    ILocationStore::ILocationStore()
    {
    }

    ILocationStore* ILocationStore::getInstance()
    {
        TA_Base_Core::ThreadGuard guard(m_ilocationMapAccessLock);
        if(m_theClassInstance == NULL)
        {
            m_theClassInstance = new ILocationStore();
        }
        return m_theClassInstance;
    }

    ILocationStore::~ILocationStore()
    {
        TA_Base_Core::ThreadGuard guard(m_ilocationMapAccessLock);
        for(std::map<unsigned long, TA_Base_Core::ILocation*>::iterator ite = m_locationKeyMap.begin();
                ite != m_locationKeyMap.end();
                ++ite)
        {
            delete ite->second;
        }
    }

    TA_Base_Core::ILocation* ILocationStore::getLocationByKey(unsigned long locationKey)
    {
        TA_Base_Core::ThreadGuard guard(m_ilocationMapAccessLock);
        if(m_locationKeyMap.find(locationKey) == m_locationKeyMap.end())
        {
            TA_Base_Core::ILocation* location =
                TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationKey);
            m_locationKeyMap[location->getKey()] = location;
            m_locationNameMap[location->getName()] = location;
        }
        return m_locationKeyMap[locationKey];
    }

    TA_Base_Core::ILocation* ILocationStore::getLocationByName(const std::string& locationName)
    {
        TA_Base_Core::ThreadGuard guard(m_ilocationMapAccessLock);
        if(m_locationNameMap.find(locationName) == m_locationNameMap.end())
        {
            TA_Base_Core::ILocation* location =
                TA_Base_Core::LocationAccessFactory::getInstance().getLocationByName(locationName);
            m_locationNameMap[location->getName()] = location;
            m_locationKeyMap[location->getKey()] = location;
        }
        return m_locationNameMap[locationName];
    }

}