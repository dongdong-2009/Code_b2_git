#if !defined(AFX_MAPPEDECSDATACACHE_H__94A21303_E03F_484D_B2DE_E8AE19CF41AC__INCLUDED_)
#define AFX_MAPPEDECSDATACACHE_H__94A21303_E03F_484D_B2DE_E8AE19CF41AC__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/MappedECSDataCache.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Utility to retrieve and cache ECS Station Mode and Master Mode related data in a suitable format
  * for the ECSManager application. The cached data is mapped so as to optimise retrieval for client
  * code.
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <map>

#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"

namespace TA_IRS_App
{
    class MappedECSDataCache  
    {

    public:

        /** 
          * Constructor
          */
        MappedECSDataCache();


        /** 
          * Destructor
          */
        virtual ~MappedECSDataCache();


        /** 
          * refreshOrderedLocationData
          *
          * Initialise (or refresh) all locations from database and verify which are Stations
          * through the presense if related Station Modes. The Station Locations
          * are subsequently cached in "In Bound" order.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException
          */
        void refreshOrderedLocationData();


        /** 
          * refreshStationModeData
          *
          * Initialise (or refresh) the cached data for ECSStationModes.
          * (client code can optionally specify one particular location (by key) or default to all (key = 0),
          * see param.)
          *
          * @param unsigned long locationKey:   Key of Location to cache StationModeData for. The default
          *                                     value of "0" causes caching of StationModeData for all
          *                                     locations.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException
          */
        void refreshStationModeData( unsigned long locationKey = 0 );

        
        /** 
          * refreshNormalMasterModeData
          *
          * Initialise (or refresh) the cached data for Normal ECS Master Modes.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException
          */
        void refreshNormalMasterModeData();


        /** 
          * refreshMasterModeData
          *
          * Initialise (or refresh) the cached data for particular ECS Master Mode.
          *
          * @param masterMode     - Master Mode (by mode) to be refreshed.
          * @param doForceRefresh - true if should refresh from database if already cached,
          *                         false if shouldn't refresh if already cached.
          *                         (Defaults to false)
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException
          */
        void refreshMasterModeData( unsigned long masterMode, bool doForceRefresh = false );


        /** 
          * refreshEmergencyMasterModeData
          *
          * Initialise (or refresh) the cached data for Emergency ECS Master Modes
          * (client code can optionally specify one particular ECSZone (by zoneID) or default to all (zoneId = -1),
          * see param.)
          *
          * @param int zoneID:  ID of ECSZone to cache MasterModeData for. The default
          *                     value of "-1" causes caching of MasterModeData for all
          *                     ECSZones.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException
          */
        void refreshEmergencyMasterModeData( int zoneID = -1 );


        /** 
          * refreshZoneDirectionKeys
          *
          * Initialise (or refresh) cached container of ECS Zone Direction Keys.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException
          */
          // No longer relevant , now using zone equipment name to get the zone id
          //void refreshZoneDirectionKeys( bool doForceRefresh = false );

        /** 
          * refreshZoneEquipmentData
          *
          * Initialise (or refresh) cached container of ECS Zone EquipmentName to ZoneId data.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException
          */
          void refreshZoneEquipmentData( bool doForceRefresh = false );

        /** 
          * getOrderedLocations
          *
          * Get cached container of Station Location Names and Keys, ordered by "In Bound".
          *
          * @return Vector of OrderedLocations.
          */
        OrderedLocations getOrderedLocations() const; 



        /** 
          * getZoneDirectionKeys
          *
          * Get cached container of all Zone Direction Keys.
          *
          * @return Vector of all cached ECS Zone Direction keys.
          */
        // No longer relevant
        //ZoneDirectionKeys getZoneDirectionKeys() const;


        /** 
          * getZoneIdFromZoneDirectionName
          *
          * Get Cached Zone ID from name of Zone Direction DataPoint. Note, this method
          * will call refreshZoneDirectionKeys if not already initialised.
          *
          * @param zoneDirectionName - Name of Zone Direction DataPoint.
          *
          * @return unsigned long ECS Zone ID (-1 if name passed does not map.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException thrown if name is not valid database entity.
          */
        // No longer relevant , now using zone equipment name to get the zone id 
        //unsigned long getZoneIdFromZoneDirectionName( const std::string& zoneDirectionName );

        /** 
          * getZoneIdFromZoneEquipmentName
          *
          * Get Cached Zone ID from name of ZoneEquipment entity. Note, this method
          * will call refreshZoneEquipmentData if not already initialised.
          *
          * @param zoneEquipmentName - Name of Zone Equipment.
          *
          * @return unsigned long ECS Zone ID (-1 if name passed does not map).
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException thrown if name is not valid database entity.
          */
        unsigned long getZoneIdFromZoneEquipmentName( const std::string& zoneEquipmentName );
        
        /** 
          * isValidCachedOrderedLocationIndex
          *
          * Indicates whether integer value is valid index for currently cached ordered 
          * Station location.
          *
          * @param orderedLocationIndex - integer index to validate
          *
          * @return true if valid orderedLocationIndex, false otherwise.
          */
        bool isValidCachedOrderedLocationIndex( int orderedLocationIndex ) const;


        
        /** 
          * getOrderedLocationFromIndex
          *
          * Get currently cached OrderedLocation from index (index relative to OrderedLocations
          * available from getOrderedLocations).
          *
          * PreCondition: isValidOrderedLocationIndex( orderedLocationIndex )
          *
          * @param orderedLocationIndex - index of OrderedLocation to retrieve.
          *
          * @return The OrderedLocation at provided index.
          */
        OrderedLocation getOrderedLocationFromIndex( int orderedLocationIndex ) const;

        
        /** 
          * getOrderedLocationIndexFromKey
          *
          * Get the index of cached OrderedLocation corresponding to location key.
          *
          * @param locationKey - Key of locaiton to retrieve OrderLocation index for.
          *
          * @return Integer index of corresponding cached OrderedLocation (-1 if no
          *         corresponding OrderedLocation in cache).
          */
        int getOrderedLocationIndexFromKey( unsigned long locationKey ) const;


        /** 
          * getStationModeKeysFromLocationKey
          *
          * Get container of cached Station Mode keys for particular location.
          *
          * @param locationKey - Key of Location to retrieve Station MOde Keys for.
          *
          * @return Vector of all cached Station Mode keys for location.
          */
        StationModeKeys getStationModeKeysFromLocationKey( unsigned long locationKey ) const;

        
        /** 
          * getStationModeKeyFromLocationKeyAndMode
          *
          * Get cached Station Mode Key corresponding to location key and mode value.
          *
          * @param locationKey - Location key of Station Mode key to retrieve.
          * @param mode        - Station Mode value of Station Mode key to retrieve.
          *
          * @return unsigned long key of related cached Station Mode (0 if no correponding
          *         Station Mode is currently cached).
          */
        unsigned long getStationModeKeyFromLocationKeyAndMode( unsigned long locationKey, unsigned long mode ) const;


        /** 
          * isValidCachedStationModeKey
          *
          * Indicate whether Station Mode key corresponds to currently cached Station mode.
          *
          * @param stationModeKey - Station Mode key to validate.
          *
          * @return true if valid key for currently cached Station Mode, false otherwise.
          */
        bool isValidCachedStationModeKey( unsigned long stationModeKey ) const;

        
        /** 
          * getStationModeDetailFromKey
          *
          * Get StationModeDetail struct from Station Mode key.
          *
          * PreCondition: isValidCachedStationModeKey( stationModeKey )
          *
          * @param stationModeKey - Key to retrieve StationModeDetail for.
          *
          * @return StationModeDetail for given key.
          */
        StationModeDetail getStationModeDetailFromKey( unsigned long stationModeKey ) const;


        /** 
          * getEmergencyMasterModeKeys
          *
          * Get container of currently cached Emergency Master Mode keys.
          *
          * @return Vector of currently cached Master Mode keys.
          */
        MasterModeKeys getEmergencyMasterModeKeys() const;


        /** 
          * getNormalMasterModeKeys
          *
          * Get container of currently cached Normal Master Mode keys.
          *
          * @return Vector of currently cached Master Mode keys.
          */
        MasterModeKeys getNormalMasterModeKeys() const;


        /** 
          * getMasterModesKeysFromZoneID
          *
          * Get container of all currently cached Master Mode keys for particular
          * ECS Zone.
          *
          * @param zoneID - Id of Zone to retrieve Master Mode keys for.
          *
          * @return Vector of all currently cached Master Mode keys for given Zone.
          */
        MasterModeKeys getMasterModesKeysFromZoneID( unsigned long zoneID ) const;


        /** 
          * getMasterModeKeyFromMode
          *
          * Get cached Master Mode Key from Mode value.
          *
          * @param masterMode - Master Mode value.
          *
          * @return unsigned long key of Master Mode for given mode value (0 if
          *         there is no corresponding currently cached Master Mode).
          */
        unsigned long getMasterModeKeyFromMode( unsigned long masterMode ) const;
        
    
        /** 
          * isValidCachedMasterModeKey
          *
          * Indicate whether Master Mode key corresponds to currently cached Master Mode.
          *
          * @param masterModeKey - Master Mode key to validate.
          *
          * @return true if valid key for currently cached Master Mode, false otherwise.
          */
        bool isValidCachedMasterModeKey( unsigned long masterModeKey ) const;
        

        /** 
          * getMasterModeDetailFromKey
          *
          * Get MasterModeDetail struct from Master Mode key.
          *
          * PreCondition: isValidCachedMasterModeKey( masterModeKey )
          *
          * @param masterModeKey - Key to retrieve MasterModeDetail for.
          *
          * @return MasterModeDetail for given key.
          */
        MasterModeDetail getMasterModeDetailFromKey( unsigned long masterModeKey ) const;

    private:

        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        MappedECSDataCache( const MappedECSDataCache& );


        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        MappedECSDataCache& operator=( const MappedECSDataCache& );


        /** 
          * populateCachedMasterModeDataFromKeys
          *
          * Helper to populate cached Master Mode details from container of keys.
          *
          * @param masterModeKeys - Vector of Master Mode keys to cache Master Mode details for.
          */
        void populateCachedMasterModeDataFromKeys( const MasterModeKeys& masterModeKeys );

		//zn++ - Launching manager optimise
		void populateAllStationModeData();
		//++zn

    private:

        //
        // Typedefs to be used by this class only.
        //
        typedef std::map< unsigned long, unsigned long >                                LocationKeyToOrderIndex;
        typedef std::map< unsigned long, StationModeDetail >   StationModeKeyToDetail;
        typedef std::map< unsigned long, unsigned long >                                StationModeToKey;
        typedef std::multimap< unsigned long, unsigned long >                           LocationKeyToStationModeKeys;
        typedef std::map< unsigned long, StationModeToKey >                             LocationAndModeToStationModeKey;
        typedef std::map< unsigned long, MasterModeDetail >    MasterModeKeyToDetail;
        typedef std::map< unsigned long, unsigned long >                                MasterModeToKey;
        typedef std::multimap< unsigned long, unsigned long >                           ZoneIDToMasterModeKeys;
        //typedef std::map< unsigned long, unsigned long >                                ZoneDirectionKeyToZoneID;
        typedef std::map< unsigned long, unsigned long >                                ZoneEquipmentKeyToZoneID;
        

        //
        // Member containers to cache data.
        //
        OrderedLocations                            m_orderedLocations;
        LocationKeyToOrderIndex                     m_locationKeyToOrderIndex;
        StationModeKeyToDetail                      m_stationModeKeyToDetail;
        LocationKeyToStationModeKeys                m_locationKeyToStationModeKeys;
        LocationAndModeToStationModeKey             m_locationAndModeToStationModeKey;
        MasterModeKeys                              m_normalMasterModeKeys;
        MasterModeKeys                              m_emergencyMasterModeKeys;
        MasterModeKeyToDetail                       m_masterModeKeyToDetail;
        MasterModeToKey                             m_masterModeToKey;
        ZoneIDToMasterModeKeys                      m_zoneIDToMasterModeKeys;
        //No longer relevant now using equipment name
        //ZoneDirectionKeyToZoneID                    m_zoneDirectionKeyToZoneID;
        //ZoneDirectionKeys  m_zoneDirectionKeys;
        ZoneEquipmentKeyToZoneID                    m_zoneEquipmentKeyToZoneID;

        //
        // Member flags to indicate whether cached data has been initialised 
        // (refreshed at least once)
        //
        bool    m_isStationModeDataForAllLocationsInitialised;
        bool    m_isNormalMasterModeDataInitialised;
        bool    m_isEmergencyMasterModeDataInitialised;
        bool    m_isOrderedLocationDataInitialised;
        //bool  m_isZoneDirectionDataInitialised; // No longer relvant
        bool    m_isZoneEquipmentNameDataInitialised; 
    };
} // namespace TA_IRS_App

#endif // !defined(AFX_MAPPEDECSDATACACHE_H__94A21303_E03F_484D_B2DE_E8AE19CF41AC__INCLUDED_)
