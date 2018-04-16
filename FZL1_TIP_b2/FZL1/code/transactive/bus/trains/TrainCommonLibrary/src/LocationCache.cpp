/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"


namespace TA_IRS_Bus
{

    using TA_Base_Core::RunParams;
    using TA_Base_Core::ILocation;
    using TA_Base_Core::LocationAccessFactory;
    using TA_Base_Core::DebugUtil;
    using TA_Base_Core::DataException;

    LocationCache::LocationCache()
    {
        initialise();
    }


    LocationCache::~LocationCache()
    {
    }


    LocationCache& LocationCache::getInstance()
    {
        return *( LocationCacheSingleton::instance() );
    }


    unsigned long LocationCache::getLocationKey()
    {
        TA_THREADGUARD( m_dataLock );
    
        // if the value hasnt been retrieved yet
        if ( false == m_locationKey.hasBeenSet() )
        {
            // get the location key and convert it to a number
            TA_ASSERT( RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ),
                       "Location Key is not set");
    
            // extract the location key
            std::stringstream locationKeyStream;
            locationKeyStream << RunParams::getInstance().get( RPARAM_LOCATIONKEY );
            unsigned long locationKey = 0;
            locationKeyStream >> locationKey;
    
            TA_ASSERT( locationKey != 0,
                       "Location Key could not be converted to a number, or it was 0");
    
            // set the member variable
            m_locationKey.setValue( locationKey );
        }
    
        return m_locationKey.getValue();
    }


    unsigned long LocationCache::getOccLocationKey()
    {
        TA_THREADGUARD( m_dataLock );
    
        // if the value hasnt been retrieved yet
        if ( false == m_occLocationKey.hasBeenSet() )
        {
            // set the member variable
            m_occLocationKey.setValue( LocationAccessFactory::getInstance().getOccLocationKey() );
        }
    
        return m_occLocationKey.getValue();
    }


    unsigned long LocationCache::getDepotLocationKey()
    {
        TA_THREADGUARD( m_dataLock );
    
        // if the value hasnt been retrieved yet
        if ( false == m_depotLocationKey.hasBeenSet() )
        {
            // set the member variable
            m_depotLocationKey.setValue( LocationAccessFactory::getInstance().getDepotLocationKey() );
        }
    
        return m_depotLocationKey.getValue();
    }


    LocationCache::LocationInfoMap LocationCache::getAllLocations()
    {
        TA_THREADGUARD( m_dataLock );

        return m_locationInfo;
    }


    LocationCache::LocationInfo LocationCache::getLocationInfoByKey( unsigned long key )
    {
        TA_THREADGUARD( m_dataLock );

        // try and find the value in the map
        std::map<unsigned long, LocationInfo>::iterator iter = m_locationInfo.find( key );
    
        // if it's found, return it
        if ( iter != m_locationInfo.end() )
        {
            return iter->second;
        }
        else
        {
            std::ostringstream keyStream;
            keyStream << key;
            TA_THROW( DataException( "No location found for key",
                                     DataException::NO_VALUE,
                                     keyStream.str() ) );
        }
    }


    std::string LocationCache::getLocationByKey( unsigned long key )
    {
        TA_THREADGUARD( m_dataLock );

        return getLocationInfoByKey( key ).name;
    }


    TA_Base_Core::ILocation::ELocationType LocationCache::getLocationTypeByKey( unsigned long key )
    {
        TA_THREADGUARD( m_dataLock );

        return getLocationInfoByKey(key).type;
    }


    unsigned long LocationCache::getOrderIdByLocation( unsigned long locationKey )
    {
        TA_THREADGUARD( m_dataLock );

        return getLocationInfoByKey( locationKey ).orderId;
    }


    LocationCache::EStationType LocationCache::getAppropriateStationMode()
    {
        // get the appropriate mode based on location
        LocationCache::EStationType stationType;
    
        try
        {
            // get the location type
            TA_Base_Core::ILocation::ELocationType location = getLocationTypeByKey( getLocationKey() );
    
            switch (location)
            {
            case TA_Base_Core::ILocation::STATION:
                stationType = LocationCache::Station;
                break;
            case TA_Base_Core::ILocation::OCC:
                stationType = LocationCache::InService;
                break;
            case TA_Base_Core::ILocation::DPT:
                stationType = LocationCache::Depot;
                break;
            }
        }
        catch (const TA_Base_Core::TransactiveException& te)
        {
            std::stringstream msg;
            msg << "Error retrieving location type " << te.what();
    
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TransactiveException", msg.str().c_str());
    
            // failed to get the location type.
            // set it to in service
            stationType = LocationCache::InService;
        }
    
        return stationType;
    }


    bool LocationCache::isInControlOfLocation(unsigned long locationKey)
    {
        // first, just in case the given location is 0, then any operator anywhere should be able to attempt communication for safety reasons
        // if this is the case, the result is true
        //
        // second check if the location is within the controlled locations
        // if it is, then the result is true


        if ( 0 == locationKey )
        {
            return true;
        }
    
        TA_THREADGUARD( m_dataLock );
    
        std::set< unsigned long >::iterator findIter = m_controlledLocations.find( locationKey );
    
        return ( findIter != m_controlledLocations.end() );
    }


    void LocationCache::cacheLocationControl( const TrainInformationTypes::AgentOperationMode& operationMode )
    {
        // Just store the location control map locally

        TA_THREADGUARD( m_dataLock );
    
        m_controlledLocations = operationMode.controlledLocations;
    }

    void LocationCache::initialise()
    {
        // load all locations from the database using LocationAccessFactory and populate m_locationInfo
        TA_THREADGUARD( m_dataLock );

        std::vector<TA_Base_Core::ILocation*> locations = LocationAccessFactory::getInstance().getAllLocations();

        try
        {
            for ( std::vector<ILocation*>::iterator iter = locations.begin();
                  iter != locations.end(); ++iter )
            {
                // insert it into the map
                LocationInfo info;
                info.key = (*iter)->getKey();
                info.name = (*iter)->getName();
                info.type = (*iter)->getLocationType();
                info.orderId = (*iter)->getOrderId();

                m_locationInfo[ info.key ] = info;
                      
                delete *iter;
                *iter = NULL;
            }

            locations.clear();
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            for ( std::vector<ILocation*>::iterator iter = locations.begin();
                  iter != locations.end(); ++iter )
            {
                delete *iter;
                *iter = NULL;
            }

            locations.clear();

            throw;
        }
    }
}
