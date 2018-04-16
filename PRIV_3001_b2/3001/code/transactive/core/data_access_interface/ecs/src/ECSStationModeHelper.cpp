/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ecs/src/ECSStationModeHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * ECSStationModeHelper is an object that is held by ECSStationMode and ConfigECSStationMode objects. 
  * It contains all data used by ECSStationModes, and manipulation
  * methods for the data. It helps avoid re-writing code for both ECSStationMode and ConfigECSStationMode.
  */


#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/ecs/src/ECSStationModeHelper.h"
#include "core/data_access_interface/ecs/src/ECSStationModeAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

	ECSStationModeHelper::ECSStationModeHelper( const ECSStationModeHelper& theECSStationModeHelper)
		: m_key(0),
		  m_stationMode(theECSStationModeHelper.m_stationMode),
		  m_locationKey(theECSStationModeHelper.m_locationKey),
          m_locationKeyHasBeenSet(true),
          m_operation(theECSStationModeHelper.m_operation),
          m_track(theECSStationModeHelper.m_track),
          m_condition(theECSStationModeHelper.m_condition),
          m_congestionLogic(theECSStationModeHelper.m_congestionLogic),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    ECSStationModeHelper::ECSStationModeHelper(const unsigned long key)
        : m_key(key),
		  m_stationMode(0),
		  m_locationKey(0),
          m_locationKeyHasBeenSet(false),
          m_operation(""),
          m_track(""),
          m_condition(""),
          m_congestionLogic(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    ECSStationModeHelper::ECSStationModeHelper()
        : m_key(0),
		  m_stationMode(0),
		  m_locationKey(0),
          m_locationKeyHasBeenSet(false),
          m_operation(""),
          m_track(""),
          m_condition(""),
          m_congestionLogic(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    ECSStationModeHelper::ECSStationModeHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_key(0),
		  m_stationMode(0),
		  m_locationKey(0),
          m_locationKeyHasBeenSet(false),
          m_operation(""),
          m_track(""),
          m_condition(""),
          m_congestionLogic(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        m_key = data.getUnsignedLongData(row, "ECSMOD_ID");

        reloadUsing(row, data);
    }


    ECSStationModeHelper::~ECSStationModeHelper()
    {
    }
  

    unsigned long ECSStationModeHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_key;
    }

    unsigned long ECSStationModeHelper::getStationMode()
    {
        FUNCTION_ENTRY("getStationMode" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_stationMode;
    }

    void ECSStationModeHelper::setStationMode(const unsigned long stationMode)
    {
        FUNCTION_ENTRY("setStationMode" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_stationMode = stationMode;
        FUNCTION_EXIT;
    }

    unsigned long ECSStationModeHelper::getLocationKey()
    {
        FUNCTION_ENTRY("getLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_locationKey;
    }


    void ECSStationModeHelper::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_locationKey = locationKey;
        m_locationKeyHasBeenSet = true;
        FUNCTION_EXIT;
    }

    std::string ECSStationModeHelper::getOperation()
    {
        FUNCTION_ENTRY("getOperation" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_operation;
    }


    void ECSStationModeHelper::setOperation(const std::string& operation)
    {
        FUNCTION_ENTRY("setOperation" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_operation = operation;
        FUNCTION_EXIT;
    }

    std::string ECSStationModeHelper::getTrack()
    {
        FUNCTION_ENTRY("getTrack" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_track;
    }


    void ECSStationModeHelper::setTrack(const std::string& track)
    {
        FUNCTION_ENTRY("setTrack" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_track = track;
        FUNCTION_EXIT;
    }
    
    std::string ECSStationModeHelper::getCondition()
    {
        FUNCTION_ENTRY("getCondition" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_condition;
    }
    
    void ECSStationModeHelper::setCondition(const std::string& condition)
    {
        FUNCTION_ENTRY("setCondition" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_condition = condition;
        FUNCTION_EXIT;
    }


    std::string ECSStationModeHelper::getCongestionLogic()
    {
        FUNCTION_ENTRY("getCongestionLogic" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_congestionLogic;
    }
    
    void ECSStationModeHelper::setCongestionLogic(const std::string& congestionLogic)
    {
        FUNCTION_ENTRY("setCongestionLogic" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_congestionLogic = congestionLogic;
        FUNCTION_EXIT;
    }

    time_t ECSStationModeHelper::getDateCreated()
    {
        FUNCTION_ENTRY("getDateCreated" );

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateCreated;
    }


    time_t ECSStationModeHelper::getDateModified()
    {
        FUNCTION_ENTRY("getDateModified" );

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateModified;
    }



    void ECSStationModeHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
		if (m_isNew)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Attempted to call invalidate() on a new ECSStationMode" );
			return;
		}

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void ECSStationModeHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the ECSStationMode has already been written to the database. It doesn't make
        // any sense to reload an ECSStationMode that doesn't exist in the database.
		if (0 == m_key)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "This ECSStationMode does not yet exist in the database. Therefore it cannot be reloaded." );
			return;
		}
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the data of the ECSStationMode
        // based upon the key
       /* std::ostringstream sql;
		sql << "select STATION_MODE, LOCATIONKEY, OPERATION, TRACK, CONDITION, CONGESTION_LOGIC,"
            << "TO_CHAR(nvl(DATE_MODIFIED,TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from EC_STATION_MODE where ECSMOD_ID=" << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_STATION_MODE_Oracle_SELECT_52054, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_STATION_MODE_SELECT_52054, m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_STATION_MODE_SELECT_52054, m_key);

        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back("STATION_MODE");
        columnNames.push_back("LOCATIONKEY");
		columnNames.push_back("OPERATION");
		columnNames.push_back("TRACK");
		columnNames.push_back("CONDITION");
		columnNames.push_back("CONGESTION_LOGIC");
        columnNames.push_back("DATE_CREATED");
        columnNames.push_back("DATE_MODIFIED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // ECS Station Mode record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for ECS Station Mode key = " << m_key;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"ECS Station Mode key" ) );
        }
        
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_stationMode = data->getUnsignedLongData( 0, "STATION_MODE" );
		m_locationKey = data->getUnsignedLongData( 0, "LOCATIONKEY" );
        m_locationKeyHasBeenSet = true;
		m_operation   = data->getStringData( 0, "OPERATION" );
		m_track       = data->getStringData( 0, "TRACK" );
        m_condition   = data->getStringData( 0, "CONDITION" );
        m_congestionLogic   = data->getStringData( 0, "CONGESTION_LOGIC" );
        m_dateCreated = data->getDateData( 0, "DATE_CREATED" );

        try
        {
            m_dateModified = data->getDateData( 0, "DATE_MODIFIED" );
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
        {//If we catch this exception, need to check if its a "NO_VALUE" type
            if (TA_Base_Core::DataException::NO_VALUE != ex.getFailType())
            {   // Need to on-throw this exception
                throw;
            }
            else // The date-modified column was null - this is not fatal
            {
                m_dateModified = 0;
            }
        }

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void ECSStationModeHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_stationMode = data.getUnsignedLongData( row, "STATION_MODE" );
		m_locationKey = data.getUnsignedLongData( row, "LOCATIONKEY" );
        m_locationKeyHasBeenSet = true;
		m_operation   = data.getStringData( row, "OPERATION" );
		m_track       = data.getStringData( row, "TRACK" );
        m_condition   = data.getStringData( row, "CONDITION" );
        m_congestionLogic   = data.getStringData( row, "CONGESTION_LOGIC" );
        m_dateCreated = data.getDateData( row, "DATE_CREATED" );

        try
        {
            m_dateModified = data.getDateData( row, "DATE_MODIFIED" );
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
        {//If we catch this exception, need to check if its a "NO_VALUE" type
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


    void ECSStationModeHelper::writeECSStationModeData()
        //throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeECSStationModeData" );     

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if (0 == m_stationMode)
        {
            fieldNames.push_back("STATION_MODE");
        }
        if (false == m_locationKeyHasBeenSet)
        {
            fieldNames.push_back("LOCATIONKEY");
        }
        if (m_operation.empty())
        {
            fieldNames.push_back("OPERATION");
        }
        //if (m_track.empty())
        //{
        //    fieldNames.push_back(TRACK_COL);
        //}
        if (m_condition.empty())
        {
            fieldNames.push_back("CONDITION");
        }
        //if (m_congestionLogic.empty())
        //{
        //    fieldNames.push_back(TRACK_COL);
        //}
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("ECSStationMode data not fully specified. ECSStationMode cannot be written to database", fieldNames));
        }

        // Write the ECSStationMode to the database

        // Need to check if this is a new ECSStationMode or an existing ECSStationMode - 
        // as a new ECSStationMode will be inserted, while an existing ECSStationMode will be updated
        if (m_isNew) // This is a new ECSStationMode
        {
            addNewECSStationMode();
        }
        else // This is an existing ECSStationMode
        {
            modifyExistingECSStationMode();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void ECSStationModeHelper::deleteECSStationMode(bool cascade)
    {
        FUNCTION_ENTRY("deleteECSStationMode");

		if (m_isNew)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "This ECSStationMode does not yet exist in the database, and therefore cannot be deleted" );
			return;
		}
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Write);
		//int nDbType = databaseConnection->GetDbServerType();
        //std::ostringstream sql;
		//std::string strSql;
		TA_Base_Core::SQLStatement strSql;
        
        if (!cascade)
        {
            // Check for references in EC_MASTER_MODE_HELPER
            std::vector<std::string> columnNames;
            columnNames.push_back("ECMMHE_ID");
            TA_Base_Core::IData* data = NULL;

           /* sql.str("");
            sql << "select ECMMHE_ID from EC_MASTER_MODE_HELPER where ECSMOD_ID = " << m_key;*/
			//strSql = defPrepareSQLStatement(nDbType, EC_MASTER_MODE_HELPER_STD_SELECT_51003, m_key);
//			strSql = databaseConnection->prepareSQLStatement(EC_MASTER_MODE_HELPER_SELECT_51003, m_key);

		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_HELPER_SELECT_51003, m_key);

            // execute the query
            data = databaseConnection->executeQuery(strSql,columnNames);

            if (0 != data->getNumRows())
            {
                delete data;
                data = NULL;

                TA_THROW(TA_Base_Core::DataException("A reference to this EC_STATION_MODE exists in the EC_MASTER_MODE_HELPER table",
                    TA_Base_Core::DataException::CANNOT_DELETE,"EC_MASTER_MODE_HELPER"));
            }

            delete data;
            data = NULL;

        }

        // Good to go...
       /* sql.str("");
        sql << "delete EC_STATION_MODE where ECSMOD_ID=" << m_key;*/
		//strSql = defPrepareSQLStatement(nDbType, EC_STATION_MODE_STD_DELETE_52401, m_key);
//		strSql = databaseConnection->prepareSQLStatement(EC_STATION_MODE_DELETE_52401, m_key);

		databaseConnection->prepareSQLStatement(strSql, EC_STATION_MODE_DELETE_52401, m_key);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void ECSStationModeHelper::modifyExistingECSStationMode()
        //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingECSStationMode");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "Station Mode %ul already exists. It's data will be updated.",m_stationMode);

        validateLocationKeyExistence();
        validateLocationStationModeUniqueness();

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Write);
		//int nDbType = databaseConnection->GetDbServerType();
       /* std::ostringstream sql;
        sql << "update EC_STATION_MODE set " 
            << "STATION_MODE = "        << m_stationMode << ", "
            << "LOCATIONKEY = "         << m_locationKey << ", "
            << "OPERATION = '"          << (databaseConnection->escapeInsertString(m_operation)).c_str() << "', " 
            << "TRACK = '"              << (databaseConnection->escapeInsertString(m_track)).c_str() << "', " 
            << "CONDITION = '"          << (databaseConnection->escapeInsertString(m_condition)).c_str() << "', " 
            << "CONGESTION_LOGIC = '"   << (databaseConnection->escapeInsertString(m_congestionLogic)).c_str() << "' " 
            << "where ECSMOD_ID = "     << m_key;*/
		/*std::string strSql = defPrepareSQLStatement(nDbType, EC_STATION_MODE_Oracle_UPDATE_52201, m_stationMode,
			m_locationKey,databaseConnection->escapeInsertString(m_operation),databaseConnection->escapeInsertString(m_track),
			databaseConnection->escapeInsertString(m_condition),databaseConnection->escapeInsertString(m_congestionLogic),m_key);*/
//		std::string strSql = databaseConnection->prepareSQLStatement(EC_STATION_MODE_UPDATE_52201, m_stationMode,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_STATION_MODE_UPDATE_52201, m_stationMode,
			m_locationKey,databaseConnection->escapeInsertString(m_operation),databaseConnection->escapeInsertString(m_track),
			databaseConnection->escapeInsertString(m_condition),databaseConnection->escapeInsertString(m_congestionLogic),m_key);

        databaseConnection->executeModification(strSql);

        // now need to update the date fields
      /*  sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
            << "from EC_STATION_MODE where ECSMOD_ID = " << m_key;*/
		//strSql = defPrepareSQLStatement(nDbType, EC_STATION_MODE_Oracle_SELECT_52055, m_key);
//		strSql = databaseConnection->prepareSQLStatement(EC_STATION_MODE_SELECT_52055, m_key);

		databaseConnection->prepareSQLStatement(strSql, EC_STATION_MODE_SELECT_52055, m_key);

        std::vector<std::string> columnNames;
        columnNames.push_back("DATE_CREATED");
        columnNames.push_back("DATE_MODIFIED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // ECS Station Mode record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for ECS Station Mode key = " << m_key;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"ECS Station Mode key" ) );
        }

        try
        {
            m_dateCreated = data->getDateData(0,"DATE_CREATED");
            m_dateModified = data->getDateData(0,"DATE_MODIFIED");
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void ECSStationModeHelper::addNewECSStationMode()
        //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewECSStationMode");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Station Mode %ul is a new ECSStationMode. It will be added to the database.",m_stationMode);

        validateLocationKeyExistence();
        validateLocationStationModeUniqueness();


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Write);
		//int nDbType = databaseConnection->GetDbServerType();
        /*std::ostringstream sql;
        sql << "insert into EC_STATION_MODE (ECSMOD_ID, STATION_MODE, LOCATIONKEY, OPERATION, TRACK, CONDITION, CONGESTION_LOGIC) "
            << "values (ECSMOD_SEQ.NEXTVAL,"
            << m_stationMode << "," 
            << m_locationKey << ",'" 
            << (databaseConnection->escapeInsertString(m_operation)).c_str() << "','" 
            << (databaseConnection->escapeInsertString(m_track)).c_str() << "','" 
            << (databaseConnection->escapeInsertString(m_condition)).c_str() << "','"
            << (databaseConnection->escapeInsertString(m_congestionLogic)).c_str() << "')";*/
		/*std::string strSql = defPrepareSQLStatement(nDbType, EC_STATION_MODE_Oracle_INSERT_52351, m_stationMode, m_locationKey,
			databaseConnection->escapeInsertString(m_operation), databaseConnection->escapeInsertString(m_track),
			databaseConnection->escapeInsertString(m_condition), databaseConnection->escapeInsertString(m_congestionLogic));*/
//		std::string strSql = databaseConnection->prepareSQLStatement(EC_STATION_MODE_INSERT_52351, m_stationMode, m_locationKey,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_STATION_MODE_INSERT_52351,
            databaseConnection->getNextPkey("ECSMOD_SEQ"), m_stationMode, m_locationKey,
			databaseConnection->escapeInsertString(m_operation), databaseConnection->escapeInsertString(m_track),
			databaseConnection->escapeInsertString(m_condition), databaseConnection->escapeInsertString(m_congestionLogic));

        databaseConnection->executeModification(strSql);

        //now need to get the PKEY back out for this location and update the date fields
       /* sql.str("");
        sql << "select ECSMOD_ID, "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from EC_STATION_MODE where "
            << "STATION_MODE = " << m_stationMode << " and "
            << "LOCATIONKEY = " << m_locationKey;*/
		//strSql = defPrepareSQLStatement(nDbType, EC_STATION_MODE_Oracle_SELECT_52056, m_stationMode, m_locationKey);
//		strSql = databaseConnection->prepareSQLStatement(EC_STATION_MODE_SELECT_52056, m_stationMode, m_locationKey );

		databaseConnection->prepareSQLStatement(strSql, EC_STATION_MODE_SELECT_52056, m_stationMode, m_locationKey );

        std::vector<std::string> columnNames;
        columnNames.push_back("ECSMOD_ID");
        columnNames.push_back("DATE_CREATED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // ECS Station Mode record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for ECS Station Mode = " << m_stationMode 
                          << " and Location key = " << m_locationKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"ECS Station Mode and Location key" ) );
        }

        // Retrieve the pkey as an unsigned long. There should only be a single returned item 
        // (as any other circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0,"ECSMOD_ID");
            m_dateCreated = data->getDateData(0,"DATE_CREATED");
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }


    void ECSStationModeHelper::validateLocationKeyExistence()
        //throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // First check that the LOCATIONKEY already exists.
        //
        ////////////////////////////////////////////////////////////////////////////
        
       /* std::ostringstream sql;
        sql << "select PKEY from LOCATION where PKEY = " << m_locationKey;	*/	
        
        std::vector<std::string> columnNames;
        columnNames.clear();
        columnNames.push_back("PKEY");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), LOCATION_STD_SELECT_7003, m_locationKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(LOCATION_SELECT_7003, m_locationKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, LOCATION_SELECT_7003, m_locationKey);
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // Location key not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for Location key = " << m_locationKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Location key" ) );
        }

        delete data;
        data = NULL;
    }

    void ECSStationModeHelper::validateLocationStationModeUniqueness()
        //throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // Next we check whether this combination of Location key and Station Mode
        // already exists.
        //
        ////////////////////////////////////////////////////////////////////////////

       /* std::ostringstream sql;
        sql << "select ECSMOD_ID from EC_STATION_MODE where " 
            << "STATION_MODE="  << m_stationMode << " and "
            << "LOCATIONKEY="  << m_locationKey << " and "
            << "ECSMOD_ID<>" << m_key;*/

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back("ECSMOD_ID");

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_STATION_MODE_STD_SELECT_52004, m_stationMode, m_locationKey,m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_STATION_MODE_SELECT_52004, m_stationMode, m_locationKey,m_key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_STATION_MODE_SELECT_52004, m_stationMode, m_locationKey,m_key);
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 != data->getNumRows())
        {
            // Location key and Station Mode combination already exists
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "An EC_STATION_MODE record exists with " 
                          << "Location Key - Station Mode = " 
                          << m_locationKey << "-" << m_stationMode;
            TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NOT_UNIQUE, "LocationKey-StationMode"));
        }
        
        delete data;
        data = NULL;
    }


} // closes TA_Core

