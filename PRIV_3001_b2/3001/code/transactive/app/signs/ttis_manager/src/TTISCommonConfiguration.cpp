/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/ttis_manager/src/TTISCommonConfiguration.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This is used to load the current TTIS configuration.
  * Stores configuration items that may be needed in multiple places
  * in this GUI.
  */

#include "TTISCommonConfiguration.h"

#include "core/synchronisation/src/ThreadGuard.h"

#include <sstream>

namespace TA_IRS_App
{

    TTISCommonConfiguration* TTISCommonConfiguration::m_theClassInstance;
    TA_Base_Core::ReEntrantThreadLockable TTISCommonConfiguration::m_singletonLock;


    TTISCommonConfiguration& TTISCommonConfiguration::getInstance()
    {
        FUNCTION_ENTRY( "getInstance" );

        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        // If guard acquired then make sure the singleton is still NULL.

        if ( m_theClassInstance == NULL )
        {
            // Create the one & only object
            m_theClassInstance = new TTISCommonConfiguration();
        }

        FUNCTION_EXIT;
        return *m_theClassInstance;
    }


    TTISCommonConfiguration::TTISCommonConfiguration()
        : m_locationKey(),
          m_locationInfo()
    {
        FUNCTION_ENTRY( "TTISCommonConfiguration" );
        FUNCTION_EXIT;
    }


    std::string TTISCommonConfiguration::getSessionId()
    {
        FUNCTION_ENTRY( "getSessionId" );

        TA_ASSERT(RunParams::getInstance().isSet(RPARAM_SESSIONID), "Session ID is not set");

        FUNCTION_EXIT;
        return RunParams::getInstance().get(RPARAM_SESSIONID);
    }


     /**
      * getLocationKey
      *
      * Retrieves and returns the location key.
      *
      * @pre The LocationKey runparam is set, this should be the case
      *      TA_ASSERTS if not set.
      *
      * @return The location key of this console
      */
    unsigned long TTISCommonConfiguration::getLocationKey()
    {
        FUNCTION_ENTRY( "getLocationKey" );

        // LOG_FUNCTION_ENTRY( SourceInfo, "TTISCommonConfiguration::getLocationKey" );

        // lock
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        // if the value hasnt been retrieved yet
        if ( !m_locationKey.hasBeenSet() )
        {
            // get the location key and convert it to a number
            TA_ASSERT( TA_Base_Core::RunParams::getInstance().isSet(RPARAM_LOCATIONKEY),
                       "Location Key is not set");

            // extract the location key
            std::stringstream locationKeyStream;
            locationKeyStream << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
            unsigned long locationKey = 0;
            locationKeyStream >> locationKey;

            TA_ASSERT( locationKey != 0,
                       "Location Key could not be converted to a number, or it was 0");

            // set the member variable
            m_locationKey.setValue(locationKey);
        }

        // LOG_FUNCTION_EXIT( SourceInfo, "TTISCommonConfiguration::getLocationKey" );
        FUNCTION_EXIT;
        return m_locationKey.getValue();
    }


    /**
      * getLocationType
      *
      * return current entity Location Type, OCC, STATION, DEPOT.
      *
      * @return The type of the location
      *
      * @exception   DatabaseException
      *              Thrown if:
      *              1. There is some error with the database connection
      *              2. An SQL statement failed to execute for some reason.
      * @exception   DataException
      *              Thrown if the data cannot be converted to the correct format.
      *              Or the data doesnt exist.
      */
    TA_Base_Core::ILocation::ELocationType TTISCommonConfiguration::getLocationType()
    {
        FUNCTION_ENTRY( "getLocationType" );
        FUNCTION_EXIT;
        return getLocationTypeByKey(getLocationKey());
    }


    /**
      * getOccLocationKey
      *
      * Retrieves and returns the Occ location key.
      *
      * @return The location key of the OCC
      *
      * @exception   DatabaseException
      *              Thrown if:
      *              1. There is some error with the database connection
      *              2. An SQL statement failed to execute for some reason.
      * @exception   DataException
      *              Thrown if the data cannot be converted to the correct format.
      *              Or the data doesnt exist.
      */
    unsigned long TTISCommonConfiguration::getOccLocationKey()
    {
        FUNCTION_ENTRY( "getOccLocationKey" );

        // lock
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        // if the value hasnt been retrieved yet
        if ( !m_occLocationKey.hasBeenSet() )
        {
            // set the member variable
            m_occLocationKey.setValue(LocationAccessFactory::getInstance().getOccLocationKey());
        }

        FUNCTION_EXIT;
        return m_occLocationKey.getValue();
    }


    /**
      * getDepotLocationKey
      *
      * Retrieves and returns the Depot location key.
      *
      * @return The location key of the Depot
      *
      * @exception   DatabaseException
      *              Thrown if:
      *              1. There is some error with the database connection
      *              2. An SQL statement failed to execute for some reason.
      * @exception   DataException
      *              Thrown if the data cannot be converted to the correct format.
      *              Or the data doesnt exist.
      */
    unsigned long TTISCommonConfiguration::getDepotLocationKey()
    {
        FUNCTION_ENTRY( "getDepotLocationKey" );

        // lock
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        // if the value hasnt been retrieved yet
        if ( !m_depotLocationKey.hasBeenSet() )
        {
            // set the member variable
            m_depotLocationKey.setValue(LocationAccessFactory::getInstance().getDepotLocationKey());
        }

        FUNCTION_EXIT;
        return m_depotLocationKey.getValue();
    }


    /**
      * getLocationByKey
      *
      * Given a location key, return it's textual name.
      *
      * @param key  The key of the location to look up
      *
      * @return The name of the location specified by key.
      *
      * @exception   DatabaseException
      *              Thrown if:
      *              1. There is some error with the database connection
      *              2. An SQL statement failed to execute for some reason.
      * @exception   DataException
      *              Thrown if the data cannot be converted to the correct format.
      *              Or the data doesnt exist.
      */
    std::string TTISCommonConfiguration::getLocationByKey(unsigned long key)
    {
        FUNCTION_ENTRY( "getLocationByKey" );

        // lock
        TA_Base_Core::ThreadGuard guard( m_dataLock );

        FUNCTION_EXIT;
        return getLocationInfoByKey(key).name;
    }


    /**
      * getLocationTypeByKey
      *
      * Given a location key, return it's type, OCC, STATION, DEPOT.
      *
      * @param key  The key of the location to look up
      *
      * @return The type of the location specified by key.
      *
      * @exception   DatabaseException
      *              Thrown if:
      *              1. There is some error with the database connection
      *              2. An SQL statement failed to execute for some reason.
      * @exception   DataException
      *              Thrown if the data cannot be converted to the correct format.
      *              Or the data doesnt exist.
      */
    TA_Base_Core::ILocation::ELocationType TTISCommonConfiguration::getLocationTypeByKey(unsigned long key)
    {
        FUNCTION_ENTRY( "getLocationTypeByKey" );

        // lock
        TA_Base_Core::ThreadGuard guard( m_dataLock );

        FUNCTION_EXIT;
        return getLocationInfoByKey(key).type;
    }


    /**
      * getLocationInfoByKey
      *
      * Given a location key, return it's info.
      * This is called by getLocationByKey and getLocationTypeByKey.
      *
      * @param key  The key of the location to look up
      *
      * @return The info of the location specified by key.
      *
      * @exception   DatabaseException
      *              Thrown if:
      *              1. There is some error with the database connection
      *              2. An SQL statement failed to execute for some reason.
      * @exception   DataException
      *              Thrown if the data cannot be converted to the correct format.
      *              Or the data doesnt exist.
      */
    TTISCommonConfiguration::LocationInfo TTISCommonConfiguration::getLocationInfoByKey(unsigned long key)
    {
        FUNCTION_ENTRY( "getLocationInfoByKey" );

        // LOG_FUNCTION_ENTRY( SourceInfo, "TTISCommonConfiguration::getLocationInfoByKey" );

        // try and find the value in the map
        std::map<unsigned long, LocationInfo>::iterator iter = m_locationInfo.find(key);

        // if it's found, return it
        if ( iter != m_locationInfo.end() )
        {
            // LOG_FUNCTION_EXIT( SourceInfo, "TTISCommonConfiguration::getLocationInfoByKey" );
            FUNCTION_EXIT;
            return iter->second;
        }
        else
        {

            // otherwise, load it from the database and add it to the map, then return it.

            // load from DB
            // this will throw the appropriate exception for us if it doesnt exist.
            ILocation* location = LocationAccessFactory::getInstance().getLocationByKey(key);

            // insert it into the map
            LocationInfo info;
            info.name = location->getName();
            info.type = location->getLocationType();

            m_locationInfo[key] = info;

            // clean up
            delete location;

            // LOG_FUNCTION_EXIT( SourceInfo, "TTISCommonConfiguration::getLocationInfoByKey" );
            FUNCTION_EXIT;
            return info;
        }
    }


} // end namespace TA_IRS_App
