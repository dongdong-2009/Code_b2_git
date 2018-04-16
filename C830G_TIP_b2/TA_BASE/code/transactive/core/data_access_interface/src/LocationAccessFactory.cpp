/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/LocationAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/26 18:32:31 $
  * Last modified by:  $Author: builder $
  * 
  * LocationAccessFactory is a singleton that is used to retrieve Location objects either from the
  * database or newly created. All Location objects returned will adhear to the ILocation interface.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/ConfigLocation.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    // These need to be set to be the same as the names for the
    // OCC and depot in the database.
    const std::string LocationAccessFactory::OCC_LOCATION_NAME       = "OCC";
	const std::string LocationAccessFactory::OCC_COM_LOCATION_NAME   = "OCC_COM";
	const std::string LocationAccessFactory::OCC_INT_LOCATION_NAME   = "OCC_INT";
	const std::string LocationAccessFactory::OCC_SCADA_LOCATION_NAME = "OCC_SCADA";

	const unsigned int ALL_LOCATION = 0;

    // Jade: 12 Mar 04 - The depot is not DPT, but KCD (Defect #1951)
    const std::string LocationAccessFactory::DEPOT_LOCATION_NAME = "KCD";
	//TODO : Need to consider not hardcoding this. Find a way to group both occ and depot locations
	//hongran++ TD 15473
	const std::string LocationAccessFactory::DEPOT_ND_LOCATION_NAME = "ND";
	const std::string LocationAccessFactory::DEPOT_SD_LOCATION_NAME = "SD";
	const std::string LocationAccessFactory::DEPOT_MD_LOCATION_NAME = "MD";
	//++hongran TD 15473
	
	LocationAccessFactory* LocationAccessFactory::m_instance = 0;

    LocationAccessFactory& LocationAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new LocationAccessFactory();
        }
        return *m_instance;
    }


    void LocationAccessFactory::getLocationBySql(const std::string sql, std::vector< ILocation* >& locations, const bool readWrite)
    {
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("DESCRIPTION");
        columnNames.push_back("ORDER_ID");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql,columnNames);
		
        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    if (readWrite)
                    {
                        if (0 == data->getUnsignedLongData( i, columnNames[0] ))
                        {
                            // if we have the location where the key is 0 then we are not interested in it
                            // The user cannot configure this particular location
                            continue;
                        }

                        locations.push_back( new ConfigLocation(i, *data));
                    }
                    else
                    {
						locations.push_back( new Location(i, *data));
                    }

                }
                catch (TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }
			delete data;
			data = NULL;
        }
        while ( databaseConnection->moreData(data) );
    }


    std::vector<ILocation*> LocationAccessFactory::getAllLocations(const bool readWrite)
    {
		FUNCTION_ENTRY("getAllLocations");

		// create the SQL string to retrieve all the locations of the alarm
        std::ostringstream sql;
        sql << " select PKEY, NAME, DESCRIPTION, ORDER_ID,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM LOCATION order by ORDER_ID asc";

		std::vector< ILocation* > locations;
		getLocationBySql(sql.str(), locations, readWrite);

        FUNCTION_EXIT;
		return locations;
    }

	//xinsong++, exclude location=0
    std::vector<ILocation*> LocationAccessFactory::getAllAgentLocations(const bool readWrite)
    {
		FUNCTION_ENTRY("getAllLocations");
		
		// create the SQL string to retrieve all the locations of the alarm
        std::ostringstream sql;
        sql << " select PKEY, NAME, DESCRIPTION, ORDER_ID,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM LOCATION where PKEY <> " << ALL_LOCATION
			<< " order by ORDER_ID asc";
		
		std::vector< ILocation* > locations;
		getLocationBySql(sql.str(), locations, readWrite);
		
        FUNCTION_EXIT;
		return locations;
    }


	
	ILocation* LocationAccessFactory::getLocationByKey( const unsigned long locationKey, const bool readWrite )
	{
		FUNCTION_ENTRY("getLocationByKey");

		// create the SQL string to retrieve all the locations of the alarm
        std::ostringstream sql;
        sql << " select PKEY, NAME, DESCRIPTION, ORDER_ID,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM LOCATION WHERE PKEY = " << locationKey;

		std::vector< ILocation* > locations;
		getLocationBySql(sql.str(), locations, readWrite);

        if ( 0 == locations.size())
        {
            std::ostringstream message;
			message << "No data found for Location with key " << locationKey;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == locations.size(), "Location key unique constraint violated");

        FUNCTION_EXIT;
		return locations[0];
	}


    ILocation* LocationAccessFactory::getLocationByName( const std::string& locationName, const bool readWrite )
	{
		FUNCTION_ENTRY("getLocationByName");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		// create the SQL string to retrieve all the locations of the alarm
        std::ostringstream sql;
        sql << " select PKEY, NAME, DESCRIPTION, ORDER_ID,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM LOCATION WHERE NAME = '"
			<< databaseConnection->escapeQueryString(locationName) << "'";

		std::vector< ILocation* > locations;
		getLocationBySql(sql.str(), locations, readWrite);

        if ( 0 == locations.size())
        {
            std::ostringstream message;
			message << "No data found for Location with name " << locationName;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == locations.size(), "Location key unique constraint violated");

        FUNCTION_EXIT;
		return locations[0];
	}

    std::vector<ILocation*> LocationAccessFactory::getLocationsByProfile(unsigned long profileKey, const bool readWrite)
    {
        FUNCTION_ENTRY("getLocationsByProfile");

		// create the SQL string to retrieve all the locations of the alarm
        std::ostringstream sql;
        sql << " select a.PKEY, a.NAME, a.DESCRIPTION, a.ORDER_ID,"
			<< " TO_CHAR(a.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(a.DATE_CREATED,'YYYYMMDDHH24MISS') FROM LOCATION a, SE_PROFILE_LOCATION b"
			<< " where a.PKEY = b.LOCATIONKEY and b.SEPROF_ID = " << profileKey
			<< " order by a.ORDER_ID asc";

		std::vector< ILocation* > locations;
		getLocationBySql(sql.str(), locations, readWrite);

        // Return the vector of locations
        FUNCTION_EXIT;
        return locations;
    }

#ifdef PROJECT_4669

    ILocation::ELocationType LocationAccessFactory::getLocationType(
        const std::string& locationName) const
    {
        if (0 == locationName.compare(OCC_LOCATION_NAME))
        {
            return ILocation::OCC;
        }
        else if (0 == locationName.compare(OCC_COM_LOCATION_NAME))
        {
            return ILocation::OCC;
        }
        else if (0 == locationName.compare(OCC_INT_LOCATION_NAME))
        {
            return ILocation::OCC;
        }
        else if (0 == locationName.compare(OCC_SCADA_LOCATION_NAME))
        {
            return ILocation::OCC;
        }
		//hongran++ TD 15473
		else if (0 == locationName.compare(DEPOT_SD_LOCATION_NAME))
        {
            return ILocation::DPT;
        }
		else if (0 == locationName.compare(DEPOT_ND_LOCATION_NAME))
        {
            return ILocation::DPT;
        }
		else if (0 == locationName.compare(DEPOT_MD_LOCATION_NAME))
        {
            return ILocation::DPT;
        }
		//++hongran TD 15473
        else if (0 == locationName.compare(DEPOT_LOCATION_NAME))
        {
            return ILocation::DPT;
        }
        else
        {
            return ILocation::STATION;
        }
    }


    unsigned long LocationAccessFactory::getOccLocationKey()
    {
        return getLocationKeyFromName(OCC_LOCATION_NAME);
    }

    unsigned long LocationAccessFactory::getOccComLocationKey()
    {
        return getLocationKeyFromName(OCC_COM_LOCATION_NAME);
    }

    unsigned long LocationAccessFactory::getOccIntLocationKey()
    {
        return getLocationKeyFromName(OCC_INT_LOCATION_NAME);
    }

    unsigned long LocationAccessFactory::getOccScadaLocationKey()
    {
        return getLocationKeyFromName(OCC_SCADA_LOCATION_NAME);
    }


    unsigned long LocationAccessFactory::getDepotLocationKey()
    {
        return getLocationKeyFromName(DEPOT_LOCATION_NAME);
    }

	//hongran++ TD 15473
	unsigned long LocationAccessFactory::getDepotSDLocationKey()
    {
        return getLocationKeyFromName(DEPOT_SD_LOCATION_NAME);
    }

	unsigned long LocationAccessFactory::getDepotNDLocationKey()
    {
        return getLocationKeyFromName(DEPOT_ND_LOCATION_NAME);
    }

	unsigned long LocationAccessFactory::getDepotMDLocationKey()
    {
        return getLocationKeyFromName(DEPOT_MD_LOCATION_NAME);
    }
	//++hongran TD 15473

	std::string LocationAccessFactory::getOccComLocationName()
	{
		return OCC_COM_LOCATION_NAME;
	}

#endif // #ifdef PROJECT_4669


    unsigned long LocationAccessFactory::getLocationKeyFromName(
        const std::string& name)
    {
        ILocation* location = getLocationByName(name);

        unsigned long locationKey = 0;
        try
        {
            locationKey = location->getKey();
			delete location;
			location = NULL;
        }
        catch(...)
        {
            delete location;
            location = NULL;
            throw;
        }

        return locationKey;
    }

	std::map<std::string, unsigned char> LocationAccessFactory::getTIMSOriginValues()
	{
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		// create the SQL string to retrieve all the locations of the alarm
        std::ostringstream sql;
        sql << "SELECT pkey, 40 + dense_rank() OVER (ORDER BY pkey ASC) -1 AS origin, name "
			<< " FROM LOCATION WHERE pkey <> 0 AND name NOT IN ('OCC', 'TDS', 'KCD') "
			<< " UNION SELECT pkey, 10, name FROM LOCATION WHERE name = 'OCC' "
			<< " UNION SELECT pkey, 31, name FROM LOCATION WHERE name = 'KCD' "
			<< " UNION SELECT pkey, 35, name FROM LOCATION WHERE name = 'TDS' "
			<< std::ends;

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn( "PKEY" );
		std::string originColumn( "ORIGIN" );
		std::string nameColumn( "NAME" );
		std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
        columnNames.push_back(originColumn);
        columnNames.push_back(nameColumn);

		std::map<std::string, unsigned char> originValues;

		IData* data = NULL;
		try
		{
			data = databaseConnection->executeQuery(sql.str(),columnNames);
		}
		catch( ... )
		{
			delete data;
			data = NULL;
			throw;
		}

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified alarm_id
        {
			delete data;
            return originValues;
        }

        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                try
                {
					unsigned char origin = static_cast<unsigned char>(data->getUnsignedLongData(i, originColumn));
					std::string name = data->getStringData(i, nameColumn);

                    originValues.insert( std::make_pair(name, origin) );
                }
                catch (...)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }
			delete data;
			data = NULL;
        }
        while ( databaseConnection->moreData(data) );

		return originValues;

	}

    IConfigLocation* LocationAccessFactory::createLocation()
    {
        return new ConfigLocation();
    }


    IConfigLocation* LocationAccessFactory::copyLocation(const IConfigLocation* locationToCopy)
    {
        FUNCTION_ENTRY("copyLocation");

        TA_ASSERT(locationToCopy !=  NULL, "The location to copy was NULL");
        
        const ConfigLocation* cast = dynamic_cast<const ConfigLocation*>(locationToCopy);

        TA_ASSERT(cast != NULL, "Location passed could not be converted into a ConfigLocation");

        FUNCTION_EXIT;
        return new ConfigLocation(*cast);
    }


} // closes TA_Base_Core


