/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/ECSStationColourAccessFactory.cpp $
 * @author:  Andy Siow
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * ECSStationColourAccessFactory is a singleton that is used to retrieve ECSStationColourData objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/data_access_interface/src/ECSStationColourAccessFactory.h"
#include "core/data_access_interface/src/ConfigECSStationColourData.h"
#include "core/data_access_interface/src/ECSStationColourData.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

    ECSStationColourAccessFactory* ECSStationColourAccessFactory::m_instance = 0;

    ECSStationColourAccessFactory& ECSStationColourAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new ECSStationColourAccessFactory();
        }
        return *m_instance;
    }
	
    void ECSStationColourAccessFactory::getECSStationColourBySql(const std::string& sql, std::vector<IECSStationColourData*>& ecsStationColours, const bool readWrite)
    {
		// get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		columnNames.push_back("ECSCOL_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("CURRENT_ECS_STATION_COLOUR");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery( sql, columnNames );

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
						ecsStationColours.push_back( new ConfigECSStationColourData(i, *data) );
					}
					else
					{
						ecsStationColours.push_back( new ECSStationColourData(i, *data) );
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


    IECSStationColourData* ECSStationColourAccessFactory::getStationByKey(const unsigned long key,const bool readWrite)
    {
		FUNCTION_ENTRY("getStationByKey");

		// create the SQL string to retrieve all the severities of the alarm
        std::ostringstream sql;
        sql << "select ECSCOL_ID, NAME, CURRENT_ECS_STATION_COLOUR,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from EC_STATION_COLOUR where ECSCOL_ID = " << key;
		
		std::vector<IECSStationColourData*> ecsStationColours;
		getECSStationColourBySql(sql.str(), ecsStationColours, readWrite);

		if ( 0 == ecsStationColours.size())
        {
            std::ostringstream message;
			message << "No data found for EcsStationColour with key " << key;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == ecsStationColours.size(), "EcsStationColours key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return ecsStationColours[0];
    }


    std::vector<IECSStationColourData*> ECSStationColourAccessFactory::getAllECSStationColours(const bool readWrite)
    {
		FUNCTION_ENTRY("getAllECSStationColours");

		// create the SQL string to retrieve all the severities of the alarm
        std::ostringstream sql;
        sql << "select ECSCOL_ID, NAME, CURRENT_ECS_STATION_COLOUR,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from EC_STATION_COLOUR order by ECSCOL_ID";

		std::vector<IECSStationColourData*> ecsStationColours;
		getECSStationColourBySql(sql.str(), ecsStationColours, readWrite);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return ecsStationColours;
    }

} //closes TA_Base_Core


