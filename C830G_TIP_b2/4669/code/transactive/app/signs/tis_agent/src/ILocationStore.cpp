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
        TA_Base_Core::ThreadGuard guard( m_ilocationMapAccessLock );
        if ( m_theClassInstance == NULL )
            m_theClassInstance = new ILocationStore();
        return m_theClassInstance;
    }

    ILocationStore::~ILocationStore()
    {
		TA_Base_Core::ThreadGuard guard( m_ilocationMapAccessLock );
        for ( std::map<TA_IRS_Bus::StationIdType, TA_Base_Core::ILocation*>::iterator ite = m_ilocationMap.begin();
                ite != m_ilocationMap.end();
                ++ite )
        {
            delete ite->second;
        }
    }

    TA_Base_Core::ILocation* ILocationStore::getLocationByKey( TA_IRS_Bus::StationIdType stationId )
    {
        TA_Base_Core::ThreadGuard guard( m_ilocationMapAccessLock );
        if ( m_ilocationMap.find( stationId ) == m_ilocationMap.end() )
            m_ilocationMap[stationId] = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey( stationId );
        return m_ilocationMap[stationId];
    }
}