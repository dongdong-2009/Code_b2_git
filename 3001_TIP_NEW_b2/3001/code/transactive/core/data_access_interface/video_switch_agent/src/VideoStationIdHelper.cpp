/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/video_switch_agent/src/VideoStationIdHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/12/27 17:54:14 $
  * Last modified by:  $Author: weikun.lin $
  *
  * VideoStationIdHelper is an object that is held by VideoStationId and ConfigVideoStationId objects. 
  * It contains all data used by VideoStationIds, and manipulation
  * methods for the data. It helps avoid re-writing code for both VideoStationId and ConfigVideoStationId.
  */


#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/video_switch_agent/src/VideoStationIdHelper.h"
#include "core/data_access_interface/video_switch_agent/src/VideoStationIdAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

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

    VideoStationIdHelper::VideoStationIdHelper( const VideoStationIdHelper& theVideoStationIdHelper)
		: m_pkey(-1),
          m_stationId(theVideoStationIdHelper.m_stationId),
		  m_taLocation(theVideoStationIdHelper.m_taLocation),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
	}


    VideoStationIdHelper::VideoStationIdHelper(const unsigned long pKey)
        : m_pkey(pKey),
          m_stationId(-1),
		  m_taLocation(-1),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    VideoStationIdHelper::VideoStationIdHelper()
        : m_pkey(-1),
          m_stationId(-1),
		  m_taLocation(-1),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    VideoStationIdHelper::VideoStationIdHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_pkey(-1),
          m_stationId(-1),
		  m_taLocation(-1),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        m_pkey = data.getUnsignedLongData(row, PKEY_COL);

        reloadUsing(row, data);
    }


    VideoStationIdHelper::~VideoStationIdHelper()
    {
    }

    unsigned long VideoStationIdHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");
        TA_ASSERT(!m_isNew,"The data must be written to the database before the pkey can be retrieved");

		
        FUNCTION_EXIT;
        return m_pkey;
    }


    unsigned long VideoStationIdHelper::getStationId()
    {
        FUNCTION_ENTRY("getStationId");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		
        FUNCTION_EXIT;
        return m_stationId;
    }

    void VideoStationIdHelper::setStationId(unsigned long id)
    {
        FUNCTION_ENTRY("setStationId");
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_stationId = id;

        FUNCTION_EXIT;
        return;
    }

    unsigned long VideoStationIdHelper::getTaLocation()
    {
        FUNCTION_ENTRY("getTaLocation" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_taLocation;
    }


    void VideoStationIdHelper::setTaLocation(unsigned long taLocation)
    {
        FUNCTION_ENTRY("setTaLocation" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_taLocation = taLocation;
        FUNCTION_EXIT;
    }


    time_t VideoStationIdHelper::getDateCreated()
    {
        FUNCTION_ENTRY("getDateCreated" );
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateCreated;
    }


    time_t VideoStationIdHelper::getDateModified()
    {
        FUNCTION_ENTRY("getDateModified" );
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateModified;
    }


    void VideoStationIdHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new VideoStationId");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void VideoStationIdHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the VideoStationId has already been written to the database. It doesn't make
        // any sense to reload an VideoStationId that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This VideoStationId does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the data of the VideoStationId
        // based upon the key
       /* std::ostringstream sql;
		sql << "select "
            << PKEY_COL << ", "
            << STATION_COL << ", "
            << TA_LOCATION_COL << ", "
            << "TO_CHAR(" << DATECREATED_COL << ",'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(nvl(" << DATEMODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')"
            << " from " << STATION_ID_TABLE 
            << " where " << PKEY_COL << " = " << m_pkey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_STATION_ID_Oracle_SELECT_62052, m_pkey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_STATION_ID_SELECT_62052, m_pkey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_SELECT_62052, m_pkey);

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

        if (0 == data->getNumRows()) 
        {
            // VideoStationId record not found   
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for pkey = " << m_pkey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Pkey" ) );
        }
        // no need to check for multiple rows - the station id is the primary key

        // assign the data to the member variables
        try
        {
            reloadUsing(0, *data);
        }
        catch (...)
        {
            // prevent memory leaks
            delete data;
            data = NULL;
            throw;
        }
        
        // Now that we're done with the TA_Base_Core::IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        FUNCTION_EXIT;
    }

    void VideoStationIdHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw TA_Base_Core::DataExceptions, as documented in the comment of this method.
        m_pkey = data.getUnsignedLongData( row, PKEY_COL );
        m_stationId = data.getUnsignedLongData( row, STATION_COL );
        m_taLocation = data.getUnsignedLongData( row, TA_LOCATION_COL );

        m_dateCreated = data.getDateData( row, DATECREATED_COL );
        
        try
        {
            m_dateModified = data.getDateData( row, DATEMODIFIED_COL );
            struct tm newtm;

            // Set up the date created structure to equal the value we set it to when we get NULL
            // from the database
		    newtm.tm_sec = 0;		//seconds
		    newtm.tm_min = 0;		// minutes 
		    newtm.tm_hour = 12;		// hours 
		    newtm.tm_mday = 1;		// day of the month 
		    newtm.tm_mon = 0;	// month of Year - goes from 0 to 11
		    newtm.tm_year = 90;	// years since 1900 - this structure only deals with years 
									    // between 1901 and 2038
		    newtm.tm_isdst = 0;			// disable daylight savings - this could have implications

		    time_t value;
		    value = mktime(&newtm);

            if (value == m_dateModified) // The date was actually NULL
            {
                m_dateModified = 0;
            }
        }
        catch (TA_Base_Core::DataException& ex)
        {
            //If we catch this exception, need to check if its a "NO_VALUE" type
            if (TA_Base_Core::DataException::NO_VALUE != ex.getFailType())
            {   // Need to on-throw this exception
                throw;
            }
            else // The date-modified column was null - this is not fatal
            {
                m_dateModified = 0;
            }
        }

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void VideoStationIdHelper::writeVideoStationIdData()
    {       
        FUNCTION_ENTRY( "writeVideoStationIdData" );     

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if (-1 == m_stationId)
        {
            fieldNames.push_back(STATION_COL);
        }
        if (m_taLocation == -1)
        {
            fieldNames.push_back(TA_LOCATION_COL);
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("VideoStationId data not fully specified. VideoStationId cannot be written to database",
                                                         fieldNames));
        }

        // Write the VideoStationId to the database

        // Need to check if this is a new VideoStationId or an existing VideoStationId - 
        // as a new VideoStationId will be inserted, while an existing VideoStationId will be updated
        if (m_isNew) // This is a new VideoStationId
        {
            addNewVideoStationId();
        }
        else // This is an existing VideoStationId
        {
            modifyExistingVideoStationId();
        }


        // Now that the data has been written, what we hold is valid data.
        // well - except for the modified time
        m_isValidData = false;
        // but it is no longer new
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void VideoStationIdHelper::deleteVideoStationId(bool cascade)
    {
        FUNCTION_ENTRY("deleteVideoStationId");

        TA_ASSERT(!m_isNew, "This VideoStationId does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);
		//int nDbServerType = databaseConnection->GetDbServerType();
        //std::ostringstream sql;
		//std::string strSql;
		TA_Base_Core::SQLStatement strSql;
        
        if (!cascade)
        {
            // Check for references in VI_TRIGGERING_EVENTS
            std::vector<std::string> columnNames;
            columnNames.push_back(TRIGGERING_EVENTS_FK);
            TA_Base_Core::IData* data = NULL;

          /*  sql.str("");
            sql << "select " << TRIGGERING_EVENTS_FK << " from " << TRIGGERING_EVENTS_TABLE << " where " << TRIGGERING_EVENTS_FK <<" = " << m_pkey;*/
			//strSql = defPrepareSQLStatement(nDbServerType, VI_STATION_ID_STD_SELECT_62005, m_pkey);
//			strSql = databaseConnection->prepareSQLStatement(VI_STATION_ID_SELECT_62005, m_pkey);

		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_SELECT_62005, m_pkey);

            data = databaseConnection->executeQuery(strSql,columnNames);

            if (0 != data->getNumRows())
            {
                delete data;
                data = NULL;

                TA_THROW(TA_Base_Core::DataException("A reference to this Station exists in the VI_TRIGGERING_EVENTS table",
                    TA_Base_Core::DataException::CANNOT_DELETE,"Station Id"));
            }

            delete data;
            data = NULL;
        }

        // Good to go...
        
        /*sql.str("");
        sql << "delete from " << TRIGGERING_EVENTS_TABLE << " where " << TRIGGERING_EVENTS_FK << " = " << m_pkey;*/
		//strSql = defPrepareSQLStatement(nDbServerType, VI_STATION_ID_STD_DELETE_62401, m_pkey);
//		strSql = databaseConnection->prepareSQLStatement(VI_STATION_ID_DELETE_62401, m_pkey);

		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_DELETE_62401, m_pkey);
        databaseConnection->executeModification(strSql);

       /* sql.str("");
        sql << "delete from " << STATION_ID_TABLE << " where " << PKEY_COL << " = " << m_pkey;*/
		//strSql = defPrepareSQLStatement(nDbServerType, VI_STATION_ID_STD_DELETE_62402, m_pkey);
//		strSql = databaseConnection->prepareSQLStatement(VI_STATION_ID_DELETE_62402, m_pkey);

		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_DELETE_62402, m_pkey);
        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void VideoStationIdHelper::modifyExistingVideoStationId()
    {
        FUNCTION_ENTRY("modifyExistingVideoStationId");

        LOG ( SourceInfo, DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "VideoStationId id=%ul already exists. It's data will be updated.",m_stationId);

      /*  std::ostringstream sql;
        sql << "update "<< STATION_ID_TABLE << " set " 
            << STATION_COL << " = " << m_stationId << ", "
            << TA_LOCATION_COL << " = " << m_taLocation
            << " where " << PKEY_COL << " = " << m_pkey;	*/	

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), VI_STATION_ID_STD_UPDATE_62151, m_stationId,
//		std::string strSql  = databaseConnection->prepareSQLStatement(VI_STATION_ID_UPDATE_62151, m_stationId,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_UPDATE_62151, m_stationId,
			m_taLocation, m_pkey);
        databaseConnection->executeModification(strSql);

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void VideoStationIdHelper::addNewVideoStationId()
    {
        FUNCTION_ENTRY("addNewVideoStationId");

        LOG ( SourceInfo, DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
            "VideoStationId %ul is a new VideoStationId. It will be added to the database.",m_stationId);

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);
		//int nDbServerType = databaseConnection->GetDbServerType();

        // get a new pkey
      /*  std::ostringstream sql;
        sql << "select " << PKEY_SEQ << " from DUAL";*/
		//std::string strSql = defPrepareSQLStatement(nDbServerType, VI_STATION_ID_Oracle_SELECT_62053);
//		std::string strSql = databaseConnection->prepareSQLStatement(VI_STATION_ID_SELECT_62053);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_SELECT_62053);

        std::vector<std::string> columnNames;
        columnNames.push_back("NEXTVAL");
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (data->getNumRows() != 1)
        {
            TA_THROW(TA_Base_Core::DataException("Could not get primary key for new record",
                TA_Base_Core::DataException::MISSING_MANDATORY, PKEY_COL));
        }

        // get the pkey
        m_pkey = data->getUnsignedLongData(0, "NEXTVAL");
        

        // insert the data
       /* sql.str("");
        sql << "insert into " << STATION_ID_TABLE << " (" 
            << PKEY_COL << ", "
            << STATION_COL << ", "
            << TA_LOCATION_COL << ") "
            << "values ("
            << m_pkey << ","
            << m_stationId << "," 
            << m_taLocation << ")";*/
		//strSql = defPrepareSQLStatement(nDbServerType, VI_STATION_ID_STD_INSERT_62301, m_pkey, m_stationId, m_taLocation);
//		strSql = databaseConnection->prepareSQLStatement(VI_STATION_ID_INSERT_62301, m_pkey, m_stationId, m_taLocation);

		databaseConnection->prepareSQLStatement(strSql, VI_STATION_ID_INSERT_62301, m_pkey, m_stationId, m_taLocation);

        databaseConnection->executeModification(strSql);

        

        FUNCTION_EXIT;
    }

    
} // closes TA_IRS_Core

