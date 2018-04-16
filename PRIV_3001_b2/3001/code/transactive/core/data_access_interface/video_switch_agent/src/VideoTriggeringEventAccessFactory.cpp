/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#pragma warning(disable:4786)


#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEvent.h"
#include "core/data_access_interface/video_switch_agent/src/ConfigVideoTriggeringEvent.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h" // TD15848

using TA_Base_Core::DataException;

namespace TA_IRS_Core
{
    
    // triggering events table
    static const std::string TRIGGERING_EVENTS_TABLE = "VI_TRIGGERING_EVENTS";
    static const std::string PKEY_COL = "VITREV_ID";
    static const std::string STATION_ID_PKEY_COL = "VISTID_ID";
    static const std::string EVENT_ID_COL = "EVENT_ID";
	static const std::string LOCATION_COL = "LOCATION";
    static const std::string TRIGGERING_EVENT_COL = "CCTV_TRIGGERING";
    static const std::string MANUAL_CLOSING_COL = "MANUAL_CLOSING";
	// TD 15200
	static const std::string ENTITYKEY_COL = "ENTITYKEY";
	// TD 15200
    static const std::string DATE_CREATED_COL = "DATE_CREATED";
    static const std::string DATE_MODIFIED_COL = "DATE_MODIFIED";

    // station id table
    static const std::string STATION_ID_TABLE = "VI_STATION_ID";
    static const std::string STATION_ID_COL = "STATION_ID";
    static const std::string TA_LOCATION_COL = "TA_LOCATION";

	// TD 15200 entity table
	static const std::string ENTITY_TABLE = "ENTITY";
    static const std::string E_PKEY_COL = "PKEY";


	VideoTriggeringEventAccessFactory* VideoTriggeringEventAccessFactory::m_instance = 0;

    VideoTriggeringEventAccessFactory& VideoTriggeringEventAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new VideoTriggeringEventAccessFactory();
        }
        return *m_instance;
    }


    void VideoTriggeringEventAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }
    
    
    IVideoTriggeringEvent* VideoTriggeringEventAccessFactory::getVideoTriggeringEvent(const unsigned long key, const bool readWrite)
    {
        IVideoTriggeringEvent* theVideoTriggeringEvent;
        
        // decide which VideoTriggeringEvent object to return
        if (readWrite) // need a config VideoTriggeringEvent
        {
            theVideoTriggeringEvent = new ConfigVideoTriggeringEvent(key);
        }
        else // need a standard VideoTriggeringEvent
        {
            theVideoTriggeringEvent = new VideoTriggeringEvent(key);
        }

        // now force a reload to ensure that the given key was valid
        try
        {
            // an invalid key will fail on reload()
            theVideoTriggeringEvent->getLocation();
        }
        catch(...)
        {
            delete theVideoTriggeringEvent;
            theVideoTriggeringEvent = NULL;
            throw;
        }

        // Return the VideoTriggeringEvent pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theVideoTriggeringEvent;
    }

    IVideoTriggeringEvent* VideoTriggeringEventAccessFactory::getVideoTriggeringEvent(const unsigned long stationId, const unsigned long eventId, const bool readWrite)
	{
		        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

   //     std::ostringstream sql;
   //     sql << "select "
   //         << "e." << PKEY_COL << ", "
   //         << "s." << STATION_ID_COL << ", "
   //         << "e." << EVENT_ID_COL << ", "
   //         << "e." << LOCATION_COL << ", "
   //         << "e." << TRIGGERING_EVENT_COL << ", "
   //         << "e." << MANUAL_CLOSING_COL << ", "
   //         << "en." << E_PKEY_COL << ", "  // TD 15200
   //         << "TO_CHAR(e." << DATE_CREATED_COL << ",'YYYYMMDDHH24MISS'), "
   //         << "TO_CHAR(nvl(e." << DATE_MODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')"
   //         << " from " << TRIGGERING_EVENTS_TABLE << " e, " << STATION_ID_TABLE << " s, " << ENTITY_TABLE << " en" // TD 15200
   //         << " where e." << STATION_ID_PKEY_COL <<" = s." << STATION_ID_PKEY_COL
			//<< " and e." << EVENT_ID_COL << " = " << eventId
   //         << " and en." << E_PKEY_COL <<" = e." << ENTITYKEY_COL // TD 15200
			//<< " and s." << STATION_ID_COL << " = " << stationId;
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_STATION_ID_Oracle_SELECT_62054,eventId, stationId);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_STATION_ID_SELECT_62054,eventId, stationId);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_SELECT_62054,eventId, stationId);

        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);
		columnNames.push_back(STATION_ID_COL);
        columnNames.push_back(EVENT_ID_COL);
		columnNames.push_back(LOCATION_COL);
        columnNames.push_back(TRIGGERING_EVENT_COL);
		columnNames.push_back(MANUAL_CLOSING_COL);
		columnNames.push_back(ENTITYKEY_COL);  // TD 15200
        columnNames.push_back(DATE_CREATED_COL);
		columnNames.push_back(DATE_MODIFIED_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        IVideoTriggeringEvent* theVideoTriggeringEvent = NULL ;

        if(data->getNumRows() >= 1)
        {
			for (int i=0; i < data->getNumRows();i++)
			{
				unsigned long entityKey = data->getUnsignedLongData(i, ENTITYKEY_COL);
				TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
				if (entityData == NULL)
					continue;
				
				//if (entityData->getLocation() != 1) // disregard if not STATION
				{
					try
					{
						if (readWrite)
						{
							theVideoTriggeringEvent = new ConfigVideoTriggeringEvent(i, *data);
						}
						else
						{
							theVideoTriggeringEvent = new VideoTriggeringEvent(i, *data);
						}					
						delete entityData;
						entityData = NULL;
						break;
					}
					catch (TA_Base_Core::DataException& de)
					{
						// guard against memory leaks
						LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", de.what() );
						delete data;
						delete entityData;
						throw;
					}
				}
				delete entityData;
			}
        }
		else
		{
			delete data;
        	TA_THROW( TA_Base_Core::DataException("No triggering event found",DataException::NO_VALUE,"" ));
		}
		
        try
        {
            // an invalid key will fail on reload()
			if(theVideoTriggeringEvent!=NULL)
				theVideoTriggeringEvent->getLocation();
        }
        catch(...)
        {
			if(theVideoTriggeringEvent!=NULL)
			{
				delete theVideoTriggeringEvent;
				theVideoTriggeringEvent = NULL;
			}
            throw;
        }


        delete data;
        data = NULL;
        
        return theVideoTriggeringEvent;
	}


    IVideoTriggeringEvents VideoTriggeringEventAccessFactory::getAllVideoTriggeringEvents( const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

        //std::ostringstream sql;
        //sql << "select "
        //    << "e." << PKEY_COL << ", "
        //    << "s." << STATION_ID_COL << ", "
        //    << "e." << EVENT_ID_COL << ", "
        //    << "e." << LOCATION_COL << ", "
        //    << "e." << TRIGGERING_EVENT_COL << ", "
        //    << "e." << MANUAL_CLOSING_COL << ", "
        //    << "en." << E_PKEY_COL << ", "  // TD 15200
        //    << "TO_CHAR(e." << DATE_CREATED_COL << ",'YYYYMMDDHH24MISS'), "
        //    << "TO_CHAR(nvl(e." << DATE_MODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')"
        //    << " from " << TRIGGERING_EVENTS_TABLE << " e, " << STATION_ID_TABLE << " s, " << ENTITY_TABLE << " en"
        //    << " where e." << STATION_ID_PKEY_COL <<" = s." << STATION_ID_PKEY_COL
        //    << " and en." << E_PKEY_COL <<" = e." << ENTITYKEY_COL; // TD 15200
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_STATION_ID_Oracle_SELECT_62055);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_STATION_ID_SELECT_62055);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_SELECT_62055);
 
        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);
		columnNames.push_back(STATION_ID_COL);
        columnNames.push_back(EVENT_ID_COL);
		columnNames.push_back(LOCATION_COL);
        columnNames.push_back(TRIGGERING_EVENT_COL);
		columnNames.push_back(MANUAL_CLOSING_COL);
		columnNames.push_back(ENTITYKEY_COL);
        columnNames.push_back(DATE_CREATED_COL);
		columnNames.push_back(DATE_MODIFIED_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // create the vector of pointers to IEntityData
        IVideoTriggeringEvents VideoTriggeringEventPointerVector;

        do
        {   
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {
                try
                {
                    if (readWrite)
                    {
                        VideoTriggeringEventPointerVector.push_back( new ConfigVideoTriggeringEvent(i, *data));
                    }
                    else
                    {
                        VideoTriggeringEventPointerVector.push_back( new VideoTriggeringEvent(i, *data));
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

        return VideoTriggeringEventPointerVector;
    }


    IVideoTriggeringEvents VideoTriggeringEventAccessFactory::getVideoTriggeringEventsByLocation(unsigned long taLocation, const bool readWrite /* = false */)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

        //std::ostringstream sql;
        //sql << "select "
        //    << "e." << PKEY_COL << ", "
        //    << "s." << STATION_ID_COL << ", "
        //    << "e." << EVENT_ID_COL << ", "
        //    << "e." << LOCATION_COL << ", "
        //    << "e." << TRIGGERING_EVENT_COL << ", "
        //    << "e." << MANUAL_CLOSING_COL << ", "
        //    << "en." << E_PKEY_COL << ", "  // TD 15200
        //    << "TO_CHAR(e." << DATE_CREATED_COL << ",'YYYYMMDDHH24MISS'), "
        //    << "TO_CHAR(nvl(e." << DATE_MODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')"
        //    << " from " << TRIGGERING_EVENTS_TABLE << " e, " << STATION_ID_TABLE << " s, " << ENTITY_TABLE << " en"
        //    << " where e." << STATION_ID_PKEY_COL <<" = s." << STATION_ID_PKEY_COL
        //    << " and en." << E_PKEY_COL <<" = e." << ENTITYKEY_COL // TD 15200
        //    << " and s." << TA_LOCATION_COL << " = " << taLocation;
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_STATION_ID_Oracle_SELECT_62056, taLocation);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_STATION_ID_SELECT_62056, taLocation);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_SELECT_62056, taLocation);

 
        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);
		columnNames.push_back(STATION_ID_COL);
        columnNames.push_back(EVENT_ID_COL);
		columnNames.push_back(LOCATION_COL);
        columnNames.push_back(TRIGGERING_EVENT_COL);
		columnNames.push_back(MANUAL_CLOSING_COL);
		columnNames.push_back(ENTITYKEY_COL); // TD 15200
        columnNames.push_back(DATE_CREATED_COL);
		columnNames.push_back(DATE_MODIFIED_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // create the vector of pointers to IEntityData
        IVideoTriggeringEvents VideoTriggeringEventPointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {
                try
                {
                    if (readWrite)
                    {
                        VideoTriggeringEventPointerVector.push_back( new ConfigVideoTriggeringEvent(i, *data));
                    }
                    else
                    {
                        VideoTriggeringEventPointerVector.push_back( new VideoTriggeringEvent(i, *data));
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

        return VideoTriggeringEventPointerVector;
    }

 
    IConfigVideoTriggeringEvent* VideoTriggeringEventAccessFactory::createVideoTriggeringEvent()
    {            
        return new ConfigVideoTriggeringEvent();    
    }


    IConfigVideoTriggeringEvent* VideoTriggeringEventAccessFactory::copyVideoTriggeringEvent(const IConfigVideoTriggeringEvent* VideoTriggeringEventToCopy)
    {
        FUNCTION_ENTRY("copyVideoTriggeringEvent");

        TA_ASSERT(VideoTriggeringEventToCopy !=  NULL, "The VideoTriggeringEvent to copy was NULL");
        
        const ConfigVideoTriggeringEvent* cast = dynamic_cast<const ConfigVideoTriggeringEvent*>(VideoTriggeringEventToCopy);

        TA_ASSERT(cast != NULL, "VideoTriggeringEvent passed could not be converted into a ConfigVideoTriggeringEvent");

        FUNCTION_EXIT;
        return new ConfigVideoTriggeringEvent(*cast);

    }


    bool VideoTriggeringEventAccessFactory::pairExists( unsigned long stationId,
                                                        unsigned long eventId,
                                                        unsigned long pkey /* = -1 */)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

        // create the SQL string

      /*  std::ostringstream nestedSql;
        nestedSql << "select " << STATION_ID_PKEY_COL
            << " from " << STATION_ID_TABLE
            << " where " << STATION_ID_COL << " = " << stationId;

        std::ostringstream sql;
        sql << "select " << PKEY_COL
            << " from " << TRIGGERING_EVENTS_TABLE
            << " where " << EVENT_ID_COL << " = " << eventId
            << " and " << STATION_ID_PKEY_COL << " = (" << nestedSql.str() << ")"
            << " and " << PKEY_COL << " != " << pkey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_STATION_ID_STD_SELECT_62006, eventId, stationId, pkey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_STATION_ID_SELECT_62006, eventId, stationId, pkey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_SELECT_62006, eventId, stationId, pkey);

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


