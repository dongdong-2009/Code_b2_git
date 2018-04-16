/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/PidsManager/src/PidStationIdHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * PidStationIdHelper is an object that is held by PidStationId and ConfigPidStationId objects. 
  * It contains all data used by PidStationIds, and manipulation
  * methods for the data. It helps avoid re-writing code for both PidStationId and ConfigPidStationId.
  */


#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/PidsManager/src/PidStationIdHelper.h"
#include "core/data_access_interface/PidsManager/src/PidStationIdAccessFactory.h"
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

    static const std::string STATION_ID_TABLE = "TI_STATION_ID";
    static const std::string PKEY_COL = "TISTID_ID";
    static const std::string PKEY_SEQ = "TISTID_SEQ.NEXTVAL";
    static const std::string STATION_COL = "STATION_ID";
    static const std::string TA_LOCATION_COL = "TA_LOCATION";
    static const std::string DATECREATED_COL = "DATE_CREATED";
    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";

    PidStationIdHelper::PidStationIdHelper( const PidStationIdHelper& thePidStationIdHelper)
		: m_pkey(-1),
          m_stationId(thePidStationIdHelper.m_stationId),
		  m_taLocation(thePidStationIdHelper.m_taLocation),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
	}


    PidStationIdHelper::PidStationIdHelper(const unsigned long pKey)
        : m_pkey(pKey),
          m_stationId(-1),
		  m_taLocation(-1),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    PidStationIdHelper::PidStationIdHelper()
        : m_pkey(-1),
          m_stationId(-1),
		  m_taLocation(-1),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    PidStationIdHelper::PidStationIdHelper(unsigned long row, TA_Base_Core::IData& data)
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


    PidStationIdHelper::~PidStationIdHelper()
    {
    }

    unsigned long PidStationIdHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");
        TA_ASSERT(!m_isNew,"The data must be written to the database before the pkey can be retrieved");

		
        FUNCTION_EXIT;
        return m_pkey;
    }


    unsigned long PidStationIdHelper::getStationId()
    {
        FUNCTION_ENTRY("getStationId");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		
        FUNCTION_EXIT;
        return m_stationId;
    }

    void PidStationIdHelper::setStationId(unsigned long id)
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

    unsigned long PidStationIdHelper::getTaLocation()
    {
        FUNCTION_ENTRY("getTaLocation" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_taLocation;
    }


    void PidStationIdHelper::setTaLocation(unsigned long taLocation)
    {
        FUNCTION_ENTRY("setTaLocation" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_taLocation = taLocation;
        FUNCTION_EXIT;
    }


    time_t PidStationIdHelper::getDateCreated()
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


    time_t PidStationIdHelper::getDateModified()
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


    void PidStationIdHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PidStationId");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void PidStationIdHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the PidStationId has already been written to the database. It doesn't make
        // any sense to reload an PidStationId that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This PidStationId does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the data of the PidStationId
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

        TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, TI_STATION_ID_SELECT_95006, m_pkey);
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

        if (0 == data->getNumRows()) 
        {
            // PidStationId record not found   
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

    void PidStationIdHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
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

    void PidStationIdHelper::writePidStationIdData()
    {       
        FUNCTION_ENTRY( "writePidStationIdData" );     

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
            TA_THROW(TA_Base_Core::DataConfigurationException("PidStationId data not fully specified. PidStationId cannot be written to database",
                                                         fieldNames));
        }

        // Write the PidStationId to the database

        // Need to check if this is a new PidStationId or an existing PidStationId - 
        // as a new PidStationId will be inserted, while an existing PidStationId will be updated
        if (m_isNew) // This is a new PidStationId
        {
            addNewPidStationId();
        }
        else // This is an existing PidStationId
        {
            modifyExistingPidStationId();
        }


        // Now that the data has been written, what we hold is valid data.
        // well - except for the modified time
        m_isValidData = false;
        // but it is no longer new
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void PidStationIdHelper::deletePidStationId()
    {
        FUNCTION_ENTRY("deletePidStationId");

        TA_ASSERT(!m_isNew, "This PidStationId does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Write);
        /*std::ostringstream sql;
        sql << "delete from " << STATION_ID_TABLE << " where " << PKEY_COL << " = " << m_pkey;*/

        TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, TI_STATION_ID_DELETE_95007, m_pkey);
        databaseConnection->executeModification(sql);

        FUNCTION_EXIT;
    }


    void PidStationIdHelper::modifyExistingPidStationId()
    {
        FUNCTION_ENTRY("modifyExistingPidStationId");

        LOG ( SourceInfo, DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "PidStationId id=%ul already exists. It's data will be updated.",m_stationId);

       /* std::ostringstream sql;
        sql << "update "<< STATION_ID_TABLE << " set " 
            << STATION_COL << " = " << m_stationId << ", "
            << TA_LOCATION_COL << " = " << m_taLocation
            << " where " << PKEY_COL << " = " << m_pkey;*/

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Write);
        TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, TI_STATION_ID_UPDATE_95008, m_stationId,m_taLocation, m_pkey);
        databaseConnection->executeModification(sql);

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void PidStationIdHelper::addNewPidStationId()
    {
        FUNCTION_ENTRY("addNewPidStationId");

        LOG ( SourceInfo, DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
            "PidStationId %ul is a new PidStationId. It will be added to the database.",m_stationId);

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

        TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, TI_STATION_ID_SELECT_95009);

        std::vector<std::string> columnNames;
        columnNames.push_back("NEXTVAL");
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);

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

		databaseConnection->prepareSQLStatement(sql, TI_STATION_ID_INSERT_95010, m_pkey, m_stationId, m_taLocation);

        databaseConnection->executeModification(sql);

        

        FUNCTION_EXIT;
    }

    
} // closes TA_IRS_Core

