/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/PidsManager/src/PidStationIdAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#pragma warning(disable:4786)


#include "core/data_access_interface/PidsManager/src/PidStationIdAccessFactory.h"
#include "core/data_access_interface/PidsManager/src/PidStationId.h"
#include "core/data_access_interface/PidsManager/src/ConfigPidStationId.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DataException;

namespace TA_IRS_Core
{
    static const std::string STATION_ID_TABLE = "TI_STATION_ID";
    static const std::string PKEY_COL = "TISTID_ID";
    static const std::string PKEY_SEQ = "TISTID_SEQ.NEXTVAL";
    static const std::string STATION_COL = "STATION_ID";
    static const std::string TA_LOCATION_COL = "TA_LOCATION";
    static const std::string DATECREATED_COL = "DATE_CREATED";
    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";

    PidStationIdAccessFactory* PidStationIdAccessFactory::m_instance = 0;

    PidStationIdAccessFactory& PidStationIdAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PidStationIdAccessFactory();
        }
        return *m_instance;
    }


    void PidStationIdAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }
    
    
    IPidStationId* PidStationIdAccessFactory::getPidStationId(const unsigned long idKey,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, TI_STATION_ID_SELECT_95001, idKey);

        /*std::ostringstream sql;
		sql << "select " << PKEY_COL
            << " from " << STATION_ID_TABLE 
            << " where " << STATION_COL << " = " << idKey;*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "No data found for id = %lu",idKey);			
                TA_THROW(TA_Base_Core::DataException(reasonMessage,TA_Base_Core::DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "More than one entry found for id = %lu",idKey);			
                TA_THROW(TA_Base_Core::DataException(reasonMessage,TA_Base_Core::DataException::NOT_UNIQUE,""));
        }


        IPidStationId* thePidStationId;
        
        try
        {
            // decide which PidStationId object to return
            if (readWrite) // need a config PidStationId
            {
                thePidStationId = new ConfigPidStationId( data->getUnsignedLongData(0,PKEY_COL) );
            }
            else // need a standard PidStationId
            {
                thePidStationId = new PidStationId( data->getUnsignedLongData(0,PKEY_COL) );
            }
        }
        catch (...)
        {
            // guard against memory leaks
            delete data;
            data = NULL;
            throw;
        }

        delete data;
        data = NULL;

        // Return the PidStationId pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePidStationId;
    }
    
    IPidStationIds PidStationIdAccessFactory::getAllPidStationIds( const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, TI_STATION_ID_SELECT_95002);

       /* std::ostringstream sql;
		sql << "select "
            << PKEY_COL << ", "
            << STATION_COL << ", "
            << TA_LOCATION_COL << ", "
            << "TO_CHAR(" << DATECREATED_COL << ",'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(nvl(" << DATEMODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')"
            << " from " << STATION_ID_TABLE;*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);
        columnNames.push_back(STATION_COL);
        columnNames.push_back(TA_LOCATION_COL);
        columnNames.push_back(DATECREATED_COL);
        columnNames.push_back(DATEMODIFIED_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);
        
        // create the vector of pointers to IEntityData
        IPidStationIds PidStationIdPointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                try
                {
                    unsigned long id = data->getUnsignedLongData(i,STATION_COL);
                    if (id != 0)
                    {
                        if (readWrite)
                        {
                            PidStationIdPointerVector.push_back( new ConfigPidStationId(i, *data));
                        }
                        else
                        {
                            PidStationIdPointerVector.push_back( new PidStationId(i, *data));
                        }
                    }
                }
                catch (TA_Base_Core::DataException& de)
                {
                    // guard against memory leaks
                    // and prevent one load failure from stopping all items loading
                    LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", de.what() );
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return PidStationIdPointerVector;
    }
    
  
    IPidStationId* PidStationIdAccessFactory::getPidStationIdByLocation(unsigned long taLocation, const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, TI_STATION_ID_SELECT_95003, taLocation);


        // create the SQL string
        /*std::ostringstream sql;
		sql << "select " << PKEY_COL
            << " from " << STATION_ID_TABLE 
            << " where " << TA_LOCATION_COL << " = " << taLocation;*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);
        
        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "No data found for id = %lu",taLocation);			
                TA_THROW(TA_Base_Core::DataException(reasonMessage,TA_Base_Core::DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "More than one entry found for id = %lu",taLocation);			
                TA_THROW(TA_Base_Core::DataException(reasonMessage,TA_Base_Core::DataException::NOT_UNIQUE,""));
        }

        IPidStationId* thePidStationId;

        try
        {
            // decide which PidStationId object to return
            if (readWrite) // need a config PidStationId
            {
                thePidStationId = new ConfigPidStationId( data->getUnsignedLongData(0,PKEY_COL) );
            }
            else // need a standard PidStationId
            {
                thePidStationId = new PidStationId( data->getUnsignedLongData(0,PKEY_COL) );
            }
        }
        catch (...)
        {
            // guard against memory leaks
            delete data;
            data = NULL;
            throw;
        }

        delete data;
        data = NULL;

		return thePidStationId;
    }


    IConfigPidStationId* PidStationIdAccessFactory::createPidStationId()
    {            
        return new ConfigPidStationId();    
    }


    IConfigPidStationId* PidStationIdAccessFactory::copyPidStationId(const IConfigPidStationId* PidStationIdToCopy)
    {
        FUNCTION_ENTRY("copyPidStationId");

        TA_ASSERT(PidStationIdToCopy !=  NULL, "The PidStationId to copy was NULL");
        
        const ConfigPidStationId* cast = dynamic_cast<const ConfigPidStationId*>(PidStationIdToCopy);

        TA_ASSERT(cast != NULL, "PidStationId passed could not be converted into a ConfigPidStationId");

        FUNCTION_EXIT;
        return new ConfigPidStationId(*cast);

    }


    PidStationIdAccessFactory::StationIdToLocationKeyMap PidStationIdAccessFactory::getStationIdToLocationKeyMap()
    {
        StationIdToLocationKeyMap newMap;

        IPidStationIds stations = getAllPidStationIds();

        try
        {
            for (IPidStationIds::iterator iter = stations.begin();
            iter != stations.end(); iter++)
            {
                newMap[(*iter)->getStationId()] = (*iter)->getTaLocation();

                // clean up
                delete (*iter);
            }
        }
        catch (...)
        {
            // prevent leaks
            for (IPidStationIds::iterator iter = stations.begin();
            iter != stations.end(); iter++)
            {
                delete *iter;
            }

            throw;
        }

        return newMap;
    }


    PidStationIdAccessFactory::StationIdToLocationKeyMap PidStationIdAccessFactory::getStationIdToLocationKeyMap(const LocationKeyToStationIdMap& mapToReverse)
    {
        StationIdToLocationKeyMap newMap;

        for (LocationKeyToStationIdMap::const_iterator iter = mapToReverse.begin();
        iter != mapToReverse.end(); iter++)
        {
            newMap[iter->second] = iter->first;
        }

        return newMap;
    }

    PidStationIdAccessFactory::LocationKeyToStationIdMap PidStationIdAccessFactory::getLocationKeyToStationIdMap()
    {
        LocationKeyToStationIdMap newMap;

        IPidStationIds stations = getAllPidStationIds();

        try
        {
            for (IPidStationIds::iterator iter = stations.begin(); iter != stations.end(); iter++)
            {
                newMap[(*iter)->getTaLocation()] = (*iter)->getStationId();

                // clean up
                delete (*iter);
            }
        }
        catch (...)
        {
            // prevent leaks
            for (IPidStationIds::iterator iter = stations.begin();
            iter != stations.end(); iter++)
            {
                delete *iter;
            }

            throw;
        }

        return newMap;
    }


    PidStationIdAccessFactory::LocationKeyToStationIdMap PidStationIdAccessFactory::getLocationKeyToStationIdMap(const StationIdToLocationKeyMap& mapToReverse)
    {
        LocationKeyToStationIdMap newMap;

        for (StationIdToLocationKeyMap::const_iterator iter = mapToReverse.begin();
        iter != mapToReverse.end(); iter++)
        {
            newMap[iter->second] = iter->first;
        }

        return newMap;
    }


    bool PidStationIdAccessFactory::stationIdExists(unsigned long stationId, unsigned long pkey /* = -1 */)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, TI_STATION_ID_SELECT_95004, stationId, pkey);

        // create the SQL string to retrieve the guis
        /*std::ostringstream sql;
		sql << "select " << PKEY_COL
            << " from " << STATION_ID_TABLE
            << " where " << STATION_COL << " = " << stationId
            << " and " << PKEY_COL << " != " << pkey;*/
        
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);

        bool exists = (data->getNumRows() > 0);

        // clean up the pointer
        delete data;
        data = NULL;

        return exists;
    }


    bool PidStationIdAccessFactory::locationIdExists(unsigned long locationKey, unsigned long pkey /* = -1 */)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, TI_STATION_ID_SELECT_95005, locationKey, pkey);

        // create the SQL string to retrieve the guis
       /* std::ostringstream sql;
		sql << "select " << PKEY_COL
            << " from " << STATION_ID_TABLE
            << " where " << TA_LOCATION_COL << " = " << locationKey
            << " and " << PKEY_COL << " != " << pkey;*/
        
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);

        bool exists = (data->getNumRows() > 0);

        // clean up the pointer
        delete data;
        data = NULL;

        return exists;
    }


} // closes TA_IRS_Core


