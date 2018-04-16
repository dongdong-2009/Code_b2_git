/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/core/data_access_interface/video_switch_agent/src/TVSSStationIdHelper.cpp $
  * @author:  Zhang hongzhi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * TVSSStationIdHelper is an object that is held by TVSSStationId and ConfigTVSSStationId objects. 
  * It contains all data used by TVSSStationIds, and manipulation
  * methods for the data. It helps avoid re-writing code for both TVSSStationId and ConfigTVSSStationId.
  */


#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/video_switch_agent/src/TVSSStationIdHelper.h"
#include "core/data_access_interface/video_switch_agent/src/TVSSStationIdAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

    static const std::string STATION_ID_TABLE = "TVSS_STATION_ID";
    static const std::string PKEY_COL = "TVSSSTID_ID";
    static const std::string PKEY_SEQ = "TVSSSTID_SEQ.NEXTVAL";
    static const std::string STATION_COL = "STATION_ID";
    static const std::string TA_LOCATION_COL = "TA_LOCATION";
    static const std::string DATECREATED_COL = "DATE_CREATED";
    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";

    TVSSStationIdHelper::TVSSStationIdHelper( const TVSSStationIdHelper& theTVSSStationIdHelper)
		: m_pkey(-1),
          m_stationId(theTVSSStationIdHelper.m_stationId),
		  m_taLocation(theTVSSStationIdHelper.m_taLocation),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
	}


    TVSSStationIdHelper::TVSSStationIdHelper(const unsigned long pKey)
        : m_pkey(pKey),
          m_stationId(-1),
		  m_taLocation(-1),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    TVSSStationIdHelper::TVSSStationIdHelper()
        : m_pkey(-1),
          m_stationId(-1),
		  m_taLocation(-1),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    TVSSStationIdHelper::TVSSStationIdHelper(unsigned long row, TA_Base_Core::IData& data)
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


    TVSSStationIdHelper::~TVSSStationIdHelper()
    {
    }

    unsigned long TVSSStationIdHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");
        TA_ASSERT(!m_isNew,"The data must be written to the database before the pkey can be retrieved");

		
        FUNCTION_EXIT;
        return m_pkey;
    }


    unsigned long TVSSStationIdHelper::getStationId()
    {
        FUNCTION_ENTRY("getStationId");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		
        FUNCTION_EXIT;
        return m_stationId;
    }

    void TVSSStationIdHelper::setStationId(unsigned long id)
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

    unsigned long TVSSStationIdHelper::getTaLocation()
    {
        FUNCTION_ENTRY("getTaLocation" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_taLocation;
    }


    void TVSSStationIdHelper::setTaLocation(unsigned long taLocation)
    {
        FUNCTION_ENTRY("setTaLocation" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_taLocation = taLocation;
        FUNCTION_EXIT;
    }


    time_t TVSSStationIdHelper::getDateCreated()
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


    time_t TVSSStationIdHelper::getDateModified()
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


    void TVSSStationIdHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new TVSSStationId");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void TVSSStationIdHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the TVSSStationId has already been written to the database. It doesn't make
        // any sense to reload an TVSSStationId that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This TVSSStationId does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the data of the TVSSStationId
        // based upon the key
        /*std::ostringstream sql;
		sql << "select "
            << PKEY_COL << ", "
            << STATION_COL << ", "
            << TA_LOCATION_COL << ", "
            << "TO_CHAR(" << DATECREATED_COL << ",'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(nvl(" << DATEMODIFIED_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS')"
            << " from " << STATION_ID_TABLE 
            << " where " << PKEY_COL << " = " << m_pkey;*/
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TVSS_STATION_ID_SELECT_86006, m_pkey);

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
            // TVSSStationId record not found   
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

    void TVSSStationIdHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw TA_Base_Core::DataExceptions, as documented in the comment of this method.
        m_pkey = data.getUnsignedLongData( row, PKEY_COL );
        m_stationId = data.getUnsignedLongData( row, STATION_COL );
        m_taLocation = data.getUnsignedLongData( row, TA_LOCATION_COL );

        //m_dateCreated = data.getDateData( row, DATECREATED_COL );
        m_dateCreated = 0;
        
        try
        {
            //m_dateModified = data.getDateData( row, DATEMODIFIED_COL );
            m_dateModified = 0;
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

    void TVSSStationIdHelper::writeTVSSStationIdData()
    {       
        FUNCTION_ENTRY( "writeTVSSStationIdData" );     

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
            TA_THROW(TA_Base_Core::DataConfigurationException("TVSSStationId data not fully specified. TVSSStationId cannot be written to database",
                                                         fieldNames));
        }

        // Write the TVSSStationId to the database

        // Need to check if this is a new TVSSStationId or an existing TVSSStationId - 
        // as a new TVSSStationId will be inserted, while an existing TVSSStationId will be updated
        if (m_isNew) // This is a new TVSSStationId
        {
            addNewTVSSStationId();
        }
        else // This is an existing TVSSStationId
        {
            modifyExistingTVSSStationId();
        }


        // Now that the data has been written, what we hold is valid data.
        // well - except for the modified time
        m_isValidData = false;
        // but it is no longer new
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void TVSSStationIdHelper::deleteTVSSStationId()
    {
        FUNCTION_ENTRY("deleteTVSSStationId");

        TA_ASSERT(!m_isNew, "This TVSSStationId does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Write);
        /*std::ostringstream sql;
        sql << "delete from " << STATION_ID_TABLE << " where " << PKEY_COL << " = " << m_pkey;*/
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TVSS_STATION_ID_DELETE_86007, m_pkey);
        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void TVSSStationIdHelper::modifyExistingTVSSStationId()
    {
        FUNCTION_ENTRY("modifyExistingTVSSStationId");

        LOG ( SourceInfo, DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "TVSSStationId id=%ul already exists. It's data will be updated.",m_stationId);

        /*std::ostringstream sql;
        sql << "update "<< STATION_ID_TABLE << " set " 
            << STATION_COL << " = " << m_stationId << ", "
            << TA_LOCATION_COL << " = " << m_taLocation
            << " where " << PKEY_COL << " = " << m_pkey;*/

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Write);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TVSS_STATION_ID_UPDATE_86008, m_pkey);
		databaseConnection->executeModification(strSql);

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void TVSSStationIdHelper::addNewTVSSStationId()
    {
        FUNCTION_ENTRY("addNewTVSSStationId");

        LOG ( SourceInfo, DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
            "TVSSStationId %ul is a new TVSSStationId. It will be added to the database.",m_stationId);

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Write);

        // get a new pkey
        /*std::ostringstream sql;
        sql << "select " << PKEY_SEQ << " from DUAL";*/
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TVSS_STATION_ID_SELECT_86009);

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
        /*sql.str("");
        sql << "insert into " << STATION_ID_TABLE << " (" 
            << PKEY_COL << ", "
            << STATION_COL << ", "
            << TA_LOCATION_COL << ") "
            << "values ("
            << m_pkey << ","
            << m_stationId << "," 
            << m_taLocation << ")";*/
		databaseConnection->prepareSQLStatement(strSql, TVSS_STATION_ID_INSERT_86010, m_pkey, m_stationId, m_taLocation);

        databaseConnection->executeModification(strSql);

        

        FUNCTION_EXIT;
    }

    
} // closes TA_IRS_Core

