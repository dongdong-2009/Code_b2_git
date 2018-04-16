/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TTISCommonConfiguration.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is used to load the current TTIS configuration.
  * Stores configuration items that may be needed in multiple places
  * in this GUI.
  */

#if !defined(TTIS_COMMON_CONFIGURATION_H)
#define TTIS_COMMON_CONFIGURATION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning ( disable : 4786 )

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include <string>
#include <map>

using TA_Base_Core::RunParams;
using TA_Base_Core::ILocation;
using TA_Base_Core::LocationAccessFactory;
using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    class TTISCommonConfiguration
    {

    public:

        /**
         * ~TTISCommonConfiguration
         *
         * Standard destructor.
         */

        virtual ~TTISCommonConfiguration() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      TTISCommonConfiguration&
         *              A reference to an instance of a TTISCommonConfiguration object.
         *
         *
         */
        static TTISCommonConfiguration& getInstance();


        /**
          * getSessionId
          *
          * a wrapper for the runparams call to get the session id.
          * Asserts if not set.
          *
          * @return the current session id
          */
        std::string getSessionId();


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
        unsigned long getLocationKey();


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
        TA_Base_Core::ILocation::ELocationType getLocationType();


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
        unsigned long getOccLocationKey();

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
        unsigned long getDepotLocationKey();


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
        std::string getLocationByKey(unsigned long key);

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
        TA_Base_Core::ILocation::ELocationType getLocationTypeByKey(unsigned long key);


    private:

        struct LocationInfo
        {
            std::string name;
            TA_Base_Core::ILocation::ELocationType type;

        };


        /**
         * TTISCommonConfiguration
         *
         * Private constructors.
         */
        TTISCommonConfiguration();
        TTISCommonConfiguration& operator=(const TTISCommonConfiguration &) {};
        TTISCommonConfiguration( const TTISCommonConfiguration& ) {};


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
        LocationInfo getLocationInfoByKey(unsigned long key);


        // class instance
        static TTISCommonConfiguration* m_theClassInstance;

        // Thread lock to protect singleton creation.
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        // thread lock to guard data
        TA_Base_Core::ReEntrantThreadLockable m_dataLock;

        // 
        // data
        // 

        // location key
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_locationKey;

        // OCC location key
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_occLocationKey;

        // Depot location key
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_depotLocationKey;

        // map of location keys to name/type structures
        std::map<unsigned long, LocationInfo> m_locationInfo;
    };

} // end namespace TA_IRS_App

#endif
