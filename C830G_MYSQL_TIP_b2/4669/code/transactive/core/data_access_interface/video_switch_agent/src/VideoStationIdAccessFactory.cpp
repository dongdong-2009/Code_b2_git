/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/video_switch_agent/src/VideoStationIdAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/12/27 17:54:14 $
  * Last modified by:  $Author: weikun.lin $
  *
  */
#pragma warning(disable:4786)


#include "core/data_access_interface/video_switch_agent/src/VideoStationIdAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/VideoStationId.h"
#include "core/data_access_interface/video_switch_agent/src/ConfigVideoStationId.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DataException;

namespace TA_IRS_Core
{
    static const std::string STATION_ID_TABLE = "VI_STATION_ID";
	static const std::string TRIGGERING_EVENTS_TABLE = "VI_TRIGGERING_EVENTS";


    static const std::string PKEY_COL = "VISTID_ID";
    static const std::string PKEY_SEQ = "VISTID_SEQ.NEXTVAL";
    static const std::string STATION_COL = "STATION_ID";
    static const std::string TA_LOCATION_COL = "TA_LOCATION";
    static const std::string DATECREATED_COL = "DATE_CREATED";
    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";
	
    static const std::string TRIGGERING_EVENTS_FK = "VISTID_ID";

    VideoStationIdAccessFactory* VideoStationIdAccessFactory::m_instance = 0;

    VideoStationIdAccessFactory& VideoStationIdAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new VideoStationIdAccessFactory();
        }
        return *m_instance;
    }


    void VideoStationIdAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }
    
    
    IVideoStationId* VideoStationIdAccessFactory::getVideoStationId(const unsigned long idKey,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

       /* std::ostringstream sql;
		sql << "select " << PKEY_COL
            << " from " << STATION_ID_TABLE 
            << " where " << STATION_COL << " = " << idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_STATION_ID_STD_SELECT_62001, idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_STATION_ID_SELECT_62001, idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_SELECT_62001, idKey);

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


        IVideoStationId* theVideoStationId;
        
        try
        {
            // decide which VideoStationId object to return
            if (readWrite) // need a config VideoStationId
            {
                theVideoStationId = new ConfigVideoStationId( data->getUnsignedLongData(0,PKEY_COL) );
            }
            else // need a standard VideoStationId
            {
                theVideoStationId = new VideoStationId( data->getUnsignedLongData(0,PKEY_COL) );
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

        // Return the VideoStationId pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theVideoStationId;
    }
    
    IVideoStationIds VideoStationIdAccessFactory::getAllVideoStationIds( const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

       /* std::ostringstream sql;
		sql << "select "
            << PKEY_COL << ", "
            << STATION_COL << ", "
            << TA_LOCATION_COL << ", "
            << "TO_CHAR(" << DATECREATED_COL << ",'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(nvl(" << DATEMODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')"
            << " from " << STATION_ID_TABLE;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_STATION_ID_Oracle_SELECT_62051);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_STATION_ID_SELECT_62051);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_SELECT_62051);

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
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IVideoStationIds VideoStationIdPointerVector;

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
                            VideoStationIdPointerVector.push_back( new ConfigVideoStationId(i, *data));
                        }
                        else
                        {
                            VideoStationIdPointerVector.push_back( new VideoStationId(i, *data));
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

        return VideoStationIdPointerVector;
    }
    
  
    IVideoStationId* VideoStationIdAccessFactory::getVideoStationIdByLocation(unsigned long taLocation, const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

        // create the SQL string
       /* std::ostringstream sql;
		sql << "select " << PKEY_COL
            << " from " << STATION_ID_TABLE 
            << " where " << TA_LOCATION_COL << " = " << taLocation;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_STATION_ID_STD_SELECT_62002, taLocation);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_STATION_ID_SELECT_62002, taLocation);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_SELECT_62002, taLocation);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);

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

        IVideoStationId* theVideoStationId;

        try
        {
            // decide which VideoStationId object to return
            if (readWrite) // need a config VideoStationId
            {
                theVideoStationId = new ConfigVideoStationId( data->getUnsignedLongData(0,PKEY_COL) );
            }
            else // need a standard VideoStationId
            {
                theVideoStationId = new VideoStationId( data->getUnsignedLongData(0,PKEY_COL) );
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

		return theVideoStationId;
    }


    IConfigVideoStationId* VideoStationIdAccessFactory::createVideoStationId()
    {            
        return new ConfigVideoStationId();    
    }


    IConfigVideoStationId* VideoStationIdAccessFactory::copyVideoStationId(const IConfigVideoStationId* VideoStationIdToCopy)
    {
        FUNCTION_ENTRY("copyVideoStationId");

        TA_ASSERT(VideoStationIdToCopy !=  NULL, "The VideoStationId to copy was NULL");
        
        const ConfigVideoStationId* cast = dynamic_cast<const ConfigVideoStationId*>(VideoStationIdToCopy);

        TA_ASSERT(cast != NULL, "VideoStationId passed could not be converted into a ConfigVideoStationId");

        FUNCTION_EXIT;
        return new ConfigVideoStationId(*cast);

    }


    VideoStationIdAccessFactory::StationIdToLocationKeyMap VideoStationIdAccessFactory::getStationIdToLocationKeyMap()
    {
        StationIdToLocationKeyMap newMap;

        IVideoStationIds stations = getAllVideoStationIds();

        try
        {
            for (IVideoStationIds::iterator iter = stations.begin();
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
            for (IVideoStationIds::iterator iter = stations.begin();
            iter != stations.end(); iter++)
            {
                delete *iter;
            }

            throw;
        }

        return newMap;
    }


    VideoStationIdAccessFactory::StationIdToLocationKeyMap VideoStationIdAccessFactory::getStationIdToLocationKeyMap(const LocationKeyToStationIdMap& mapToReverse)
    {
        StationIdToLocationKeyMap newMap;

        for (LocationKeyToStationIdMap::const_iterator iter = mapToReverse.begin();
        iter != mapToReverse.end(); iter++)
        {
            newMap[iter->second] = iter->first;
        }

        return newMap;
    }


    VideoStationIdAccessFactory::LocationKeyToStationIdMap VideoStationIdAccessFactory::getLocationKeyToStationIdMap()
    {
        LocationKeyToStationIdMap newMap;

        IVideoStationIds stations = getAllVideoStationIds();

        try
        {
            for (IVideoStationIds::iterator iter = stations.begin();
            iter != stations.end(); iter++)
            {
                newMap[(*iter)->getTaLocation()] = (*iter)->getStationId();

                // clean up
                delete (*iter);
            }
        }
        catch (...)
        {
            // prevent leaks
            for (IVideoStationIds::iterator iter = stations.begin();
            iter != stations.end(); iter++)
            {
                delete *iter;
            }

            throw;
        }

        return newMap;
    }


    VideoStationIdAccessFactory::LocationKeyToStationIdMap VideoStationIdAccessFactory::getLocationKeyToStationIdMap(const StationIdToLocationKeyMap& mapToReverse)
    {
        LocationKeyToStationIdMap newMap;

        for (StationIdToLocationKeyMap::const_iterator iter = mapToReverse.begin();
        iter != mapToReverse.end(); iter++)
        {
            newMap[iter->second] = iter->first;
        }

        return newMap;
    }


    bool VideoStationIdAccessFactory::stationIdExists(unsigned long stationId, unsigned long pkey /* = -1 */)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the guis
       /* std::ostringstream sql;
		sql << "select " << PKEY_COL
            << " from " << STATION_ID_TABLE
            << " where " << STATION_COL << " = " << stationId
            << " and " << PKEY_COL << " != " << pkey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_STATION_ID_STD_SELECT_62003, stationId, pkey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_STATION_ID_SELECT_62003, stationId, pkey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_SELECT_62003, stationId, pkey);
        
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


    bool VideoStationIdAccessFactory::locationIdExists(unsigned long locationKey, unsigned long pkey /* = -1 */)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the guis
      /*  std::ostringstream sql;
		sql << "select " << PKEY_COL
            << " from " << STATION_ID_TABLE
            << " where " << TA_LOCATION_COL << " = " << locationKey
            << " and " << PKEY_COL << " != " << pkey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_STATION_ID_STD_SELECT_62004, locationKey, pkey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_STATION_ID_SELECT_62004, locationKey, pkey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_SELECT_62004, locationKey, pkey);
        
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


