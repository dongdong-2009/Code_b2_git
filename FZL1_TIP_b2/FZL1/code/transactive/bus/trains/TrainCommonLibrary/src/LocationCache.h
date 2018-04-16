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
#if !defined(LocationCache_H)
#define LocationCache_H

#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"

#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"

#include <ace/Singleton.h>

namespace TA_IRS_Bus
{
    /**
     * @version 1.0
     * @created 18-Mar-2008 3:19:10 PM
     */
    class LocationCache
    {

        friend class ACE_Singleton< LocationCache, ACE_Recursive_Thread_Mutex >;

    public:

        /**
         * The modes that the GUI may be in.
         * @version 1.0
         * @created 18-Mar-2008 3:19:11 PM
         */
        typedef enum EStationType
        {
            /**
             * OCC mode, showing all trains that are in service
             */
            InService,
            /**
             * OCC mode, showing all trains that are in the depot
             */
            Depot,
            /**
             * Station mode - shows all trains that are at the station
             */
            Station,
            /**
             * Show all trains (InService plus Depot)
             */
            All
        };

        /**
         * @version 1.0
         * @created 18-Mar-2008 3:19:11 PM
         */
        struct LocationInfo
        {

        public:

            unsigned long key;

            std::string name;

            TA_Base_Core::ILocation::ELocationType type;

            unsigned long orderId;
        };

        /**
         * A map of location keys to location info
         * @author adamr
         * @version 1.0
         * @created 18-Mar-2008 3:19:11 PM
         */
        typedef std::map< unsigned long, LocationInfo > LocationInfoMap;


        /**
         * Creates and returns an instance of this object.
         * 
         * @return A reference to an instance of a LocationCache object.
         */
        static LocationCache& getInstance();


        /**
         * Retrieves and returns the location key.
         * 
         * @pre The LocationKey runparam is set, this should be the case. TA_ASSERTS if not set.
         * 
         * @return The location key of this console
         */
        unsigned long getLocationKey();


        /**
         * Retrieves and returns the Occ location key.
         * 
         * @return The location key of the OCC
         * 
         * @exception   DatabaseException
         *               Thrown if:
         *               1. There is some error with the database connection
         *               2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *               Thrown if the data cannot be converted to the correct format.
         *               Or the data doesnt exist.
         */
        unsigned long getOccLocationKey();


        /**
         * Retrieves and returns the Depot location key.
         * 
         * @return The location key of the Depot
         * 
         * @exception   DatabaseException
         *               Thrown if:
         *               1. There is some error with the database connection
         *               2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *               Thrown if the data cannot be converted to the correct format.
         *               Or the data doesnt exist.
         */
        unsigned long getDepotLocationKey();


        /**
         * Gets all locations
         * 
         * @return all locations in the system, mapped key->info
         */
        LocationInfoMap getAllLocations();


        /**
         * Given a location key, return it's information
         * 
         * @return The location specified by key.
         * 
         * @exception   DatabaseException
         *               Thrown if:
         *               1. There is some error with the database connection
         *               2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *               Thrown if the data cannot be converted to the correct format.
         *               Or the data doesnt exist.
         * 
         * @param key
         */
        LocationInfo getLocationInfoByKey( unsigned long key );


        /**
         * Given a location key, return it's textual name.
         * 
         * @return The name of the location specified by key.
         * 
         * @exception   DatabaseException
         *               Thrown if:
         *               1. There is some error with the database connection
         *               2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *               Thrown if the data cannot be converted to the correct format.
         *               Or the data doesnt exist.
         * 
         * @param key    The key of the location to look up
         */
        std::string getLocationByKey( unsigned long key );


        /**
         * Given a location key, return it's type, OCC, STATION, DEPOT.
         * 
         * @return The type of the location specified by key.
         * 
         * @exception   DatabaseException
         *               Thrown if:
         *               1. There is some error with the database connection
         *               2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *               Thrown if the data cannot be converted to the correct format.
         *               Or the data doesnt exist.
         * 
         * @param key    The key of the location to look up
         */
        TA_Base_Core::ILocation::ELocationType getLocationTypeByKey( unsigned long key );


        /**
         * Given a location key, return it's ORDER_ID.
         * 
         * @return The order of the location specified by key.
         * 
         * @exception   DatabaseException
         *               Thrown if:
         *               1. There is some error with the database connection
         *               2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *               Thrown if the data cannot be converted to the correct format.
         *               Or the data doesnt exist.
         * 
         * @param locationKey    The key of the location to look up
         */
        unsigned long getOrderIdByLocation( unsigned long locationKey );


        /**
         * Used when setting up the train selection component.
         * Is this InService or Station?
         * 
         * @return InService or Station.
         */
        EStationType getAppropriateStationMode();


        /**
         * Used to check if the location's train agent is in control of the location
         * 
         * @return true if agent is in control of location
         *         false if agent is not in control of location
         * 
         * @param locationkey    key of location that we are querying
         */
        bool isInControlOfLocation( unsigned long locationKey );


        /**
         * Stores the control of the location
         * If current Component's location is a Stn/Depot, isInControl will be set to m_isStationInControl.
         * Otherwise, the control state will be stored into the LocationControlMap
         * 
         * @param operationMode    The information about location control
         */
        void cacheLocationControl( const TrainInformationTypes::AgentOperationMode& operationMode );

    protected:

        LocationCache();
        LocationCache(const LocationCache& prm1);
        LocationCache operator=(const LocationCache& prm1);

    private:

        /**
         * Standard destructor.
         */
        virtual ~LocationCache();


        /**
         * This will load all locations from the database and populate m_locationInfo
         */
        void initialise();


        /**
         * Thread guard to lock m_locationKey, m_occLocationKey, m_locationControlMap, m_isStationInControl
         */
        TA_Base_Core::ReEntrantThreadLockable m_dataLock;


        /**
         * location key
         */
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_locationKey;


        /**
         * OCC location key
         */
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_occLocationKey;


        /**
         * OCC location key
         */
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_depotLocationKey;


        /**
         * The list of locations controlled by this local train agent
         */
        std::set< unsigned long > m_controlledLocations;


        /**
         * map of location keys to name/type structures
         */
        LocationInfoMap m_locationInfo;

    };

    /**
     * @author adamr
     * @version 1.0
     * @created 18-Mar-2008 3:19:11 PM
     */
    typedef ACE_Singleton< LocationCache, ACE_Recursive_Thread_Mutex > LocationCacheSingleton;

}
#endif // !defined(LocationCache_H)
