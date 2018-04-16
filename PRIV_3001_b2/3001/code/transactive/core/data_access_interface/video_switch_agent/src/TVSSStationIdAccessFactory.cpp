/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/core/data_access_interface/video_switch_agent/src/TVSSStationIdAccessFactory.cpp $
  * @author:  Zhang hongzhi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#pragma warning(disable:4786)


#include "core/data_access_interface/video_switch_agent/src/TVSSStationIdAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/TVSSStationId.h"
#include "core/data_access_interface/video_switch_agent/src/ConfigTVSSStationId.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DataException;

namespace TA_IRS_Core
{
    static const std::string STATION_ID_TABLE = "TVSS_STATION_ID";
    static const std::string PKEY_COL = "TVSSSTID_ID";
    static const std::string PKEY_SEQ = "TVSSSTID_SEQ.NEXTVAL";
    static const std::string STATION_COL = "STATION_ID";
    static const std::string TA_LOCATION_COL = "TA_LOCATION";
    static const std::string DATECREATED_COL = "DATE_CREATED";
    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";

    TVSSStationIdAccessFactory* TVSSStationIdAccessFactory::m_instance = 0;

    TVSSStationIdAccessFactory& TVSSStationIdAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new TVSSStationIdAccessFactory();
        }
        return *m_instance;
    }


    void TVSSStationIdAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }
    
    
    ITVSSStationId* TVSSStationIdAccessFactory::getTVSSStationId(const unsigned long idKey,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        /*std::ostringstream sql;
		sql << "select " << PKEY_COL
            << " from " << STATION_ID_TABLE 
            << " where " << STATION_COL << " = " << idKey;*/

		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TVSS_STATION_ID_SELECT_86001, idKey);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

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


        ITVSSStationId* theTVSSStationId;
        
        try
        {
            // decide which TVSSStationId object to return
            if (readWrite) // need a config TVSSStationId
            {
                theTVSSStationId = new ConfigTVSSStationId( data->getUnsignedLongData(0,PKEY_COL) );
            }
            else // need a standard TVSSStationId
            {
                theTVSSStationId = new TVSSStationId( data->getUnsignedLongData(0,PKEY_COL) );
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

        // Return the TVSSStationId pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theTVSSStationId;
    }
    
    ITVSSStationIds TVSSStationIdAccessFactory::getAllTVSSStationIds( const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        /*std::ostringstream sql;
		sql << "select "
            << PKEY_COL << ", "
            << STATION_COL << ", "
            << TA_LOCATION_COL << ", "
            << "TO_CHAR(" << DATECREATED_COL << ",'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(nvl(" << DATEMODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')"
            << " from " << STATION_ID_TABLE;*/
		
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TVSS_STATION_ID_SELECT_86002);

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
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // create the vector of pointers to IEntityData
        ITVSSStationIds TVSSStationIdPointerVector;

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
                            TVSSStationIdPointerVector.push_back( new ConfigTVSSStationId(i, *data));
                        }
                        else
                        {
                            TVSSStationIdPointerVector.push_back( new TVSSStationId(i, *data));
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

        return TVSSStationIdPointerVector;
    }
    
  
    ITVSSStationId* TVSSStationIdAccessFactory::getTVSSStationIdByLocation(unsigned long taLocation, const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        // create the SQL string
        /*std::ostringstream sql;
		sql << "select " << PKEY_COL
            << " from " << STATION_ID_TABLE 
            << " where " << TA_LOCATION_COL << " = " << taLocation;*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);

		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TVSS_STATION_ID_SELECT_86003, taLocation);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
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

        ITVSSStationId* theTVSSStationId;

        try
        {
            // decide which TVSSStationId object to return
            if (readWrite) // need a config TVSSStationId
            {
                theTVSSStationId = new ConfigTVSSStationId( data->getUnsignedLongData(0,PKEY_COL) );
            }
            else // need a standard TVSSStationId
            {
                theTVSSStationId = new TVSSStationId( data->getUnsignedLongData(0,PKEY_COL) );
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

		return theTVSSStationId;
    }


    IConfigTVSSStationId* TVSSStationIdAccessFactory::createTVSSStationId()
    {            
        return new ConfigTVSSStationId();    
    }


    IConfigTVSSStationId* TVSSStationIdAccessFactory::copyTVSSStationId(const IConfigTVSSStationId* TVSSStationIdToCopy)
    {
        FUNCTION_ENTRY("copyTVSSStationId");

        TA_ASSERT(TVSSStationIdToCopy !=  NULL, "The TVSSStationId to copy was NULL");
        
        const ConfigTVSSStationId* cast = dynamic_cast<const ConfigTVSSStationId*>(TVSSStationIdToCopy);

        TA_ASSERT(cast != NULL, "TVSSStationId passed could not be converted into a ConfigTVSSStationId");

        FUNCTION_EXIT;
        return new ConfigTVSSStationId(*cast);

    }


    TVSSStationIdAccessFactory::StationIdToLocationKeyMap TVSSStationIdAccessFactory::getStationIdToLocationKeyMap()
    {
        StationIdToLocationKeyMap newMap;

        ITVSSStationIds stations = getAllTVSSStationIds();

        try
        {
            for (ITVSSStationIds::iterator iter = stations.begin();
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
            for (ITVSSStationIds::iterator iter = stations.begin();
            iter != stations.end(); iter++)
            {
                delete *iter;
            }

            throw;
        }

        return newMap;
    }


    TVSSStationIdAccessFactory::StationIdToLocationKeyMap TVSSStationIdAccessFactory::getStationIdToLocationKeyMap(const LocationKeyToStationIdMap& mapToReverse)
    {
        StationIdToLocationKeyMap newMap;

        for (LocationKeyToStationIdMap::const_iterator iter = mapToReverse.begin();
        iter != mapToReverse.end(); iter++)
        {
            newMap[iter->second] = iter->first;
        }

        return newMap;
    }

    TVSSStationIdAccessFactory::LocationKeyToStationIdMap TVSSStationIdAccessFactory::getLocationKeyToStationIdMap()
    {
        LocationKeyToStationIdMap newMap;

        ITVSSStationIds stations = getAllTVSSStationIds();

        try
        {
            for (ITVSSStationIds::iterator iter = stations.begin(); iter != stations.end(); iter++)
            {
                newMap[(*iter)->getTaLocation()] = (*iter)->getStationId();

                // clean up
                delete (*iter);
            }
        }
        catch (...)
        {
            // prevent leaks
            for (ITVSSStationIds::iterator iter = stations.begin();
            iter != stations.end(); iter++)
            {
                delete *iter;
            }

            throw;
        }

        return newMap;
    }


    TVSSStationIdAccessFactory::LocationKeyToStationIdMap TVSSStationIdAccessFactory::getLocationKeyToStationIdMap(const StationIdToLocationKeyMap& mapToReverse)
    {
        LocationKeyToStationIdMap newMap;

        for (StationIdToLocationKeyMap::const_iterator iter = mapToReverse.begin();
        iter != mapToReverse.end(); iter++)
        {
            newMap[iter->second] = iter->first;
        }

        return newMap;
    }


    bool TVSSStationIdAccessFactory::stationIdExists(unsigned long stationId, unsigned long pkey /* = -1 */)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the guis
        /*std::ostringstream sql;
		sql << "select " << PKEY_COL
            << " from " << STATION_ID_TABLE
            << " where " << STATION_COL << " = " << stationId
            << " and " << PKEY_COL << " != " << pkey;*/
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TVSS_STATION_ID_SELECT_86004, stationId, pkey);

        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        bool exists = (data->getNumRows() > 0);

        // clean up the pointer
        delete data;
        data = NULL;

        return exists;
    }


    bool TVSSStationIdAccessFactory::locationIdExists(unsigned long locationKey, unsigned long pkey /* = -1 */)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the guis
        /*std::ostringstream sql;
		sql << "select " << PKEY_COL
            << " from " << STATION_ID_TABLE
            << " where " << TA_LOCATION_COL << " = " << locationKey
            << " and " << PKEY_COL << " != " << pkey;*/
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TVSS_STATION_ID_SELECT_86005, locationKey, pkey);

        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        bool exists = (data->getNumRows() > 0);

        // clean up the pointer
        delete data;
        data = NULL;

        return exists;
    }


} // closes TA_IRS_Core


