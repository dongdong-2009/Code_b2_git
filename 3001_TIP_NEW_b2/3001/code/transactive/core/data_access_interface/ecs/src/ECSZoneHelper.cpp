/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ecs/src/ECSZoneHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2013/10/07 08:59:52 $
  * Last modified by:  $Author: huangjian $
  *
  * ECSZoneHelper is an object that is held by ECSZone and ConfigECSZone objects. 
  * It contains all data used by ECSZones, and manipulation
  * methods for the data. It helps avoid re-writing code for both ECSZone and ConfigECSZone.
  */


#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/ecs/src/ECSZoneHelper.h"
#include "core/data_access_interface/ecs/src/ECSZoneAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

	const std::string TRUE_STRING = "Y";
	const std::string FALSE_STRING = "N";


    static std::string toUpper(const std::string& in)
    {
        std::string out(in);
        std::transform(out.begin(), out.end(), out.begin(), toupper);
        return out;
    }



	ECSZoneHelper::ECSZoneHelper( const ECSZoneHelper& theECSZoneHelper)
		: m_idKey(-1),
		  m_ecsLocation(theECSZoneHelper.m_ecsLocation),
		  m_track(theECSZoneHelper.m_track),
          m_occEquipmentEntityKey(theECSZoneHelper.m_occEquipmentEntityKey),
          m_occEquipmentEntityKeyHasBeenSet(true),
          m_stnEquipmentEntityKey(theECSZoneHelper.m_stnEquipmentEntityKey),
          m_stnEquipmentEntityKeyHasBeenSet(true),
          m_isIgnoringCongestion(theECSZoneHelper.m_isIgnoringCongestion),
          m_isIgnoringCongestionHasBeenSet(true),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    ECSZoneHelper::ECSZoneHelper(const unsigned long idKey)
        : m_idKey(idKey),
		  m_ecsLocation(""),
		  m_track(""),
          m_occEquipmentEntityKey(0),
          m_occEquipmentEntityKeyHasBeenSet(false),
          m_stnEquipmentEntityKey(0),
          m_stnEquipmentEntityKeyHasBeenSet(false),
          m_isIgnoringCongestion(false),
          m_isIgnoringCongestionHasBeenSet(false),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    ECSZoneHelper::ECSZoneHelper()
        : m_idKey(-1),
		  m_ecsLocation(""),
		  m_track(""),
          m_occEquipmentEntityKey(0),
          m_occEquipmentEntityKeyHasBeenSet(false),
          m_stnEquipmentEntityKey(0),
          m_stnEquipmentEntityKeyHasBeenSet(false),
          m_isIgnoringCongestion(false),
          m_isIgnoringCongestionHasBeenSet(false),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    ECSZoneHelper::ECSZoneHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_idKey(-1),
		  m_ecsLocation(""),
		  m_track(""),
          m_occEquipmentEntityKey(0),
          m_occEquipmentEntityKeyHasBeenSet(false),
          m_stnEquipmentEntityKey(0),
          m_stnEquipmentEntityKeyHasBeenSet(false),
          m_isIgnoringCongestion(false),
          m_isIgnoringCongestionHasBeenSet(false),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        m_idKey = data.getUnsignedLongData(row, "ECZONE_ID");

        reloadUsing(row, data);
    }


    ECSZoneHelper::~ECSZoneHelper()
    {
    }


    

    int ECSZoneHelper::getECSZoneId()
    {
        FUNCTION_ENTRY("getECSZoneId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_idKey;
    }


    void ECSZoneHelper::setECSZoneId(int idKey)
    {
        FUNCTION_ENTRY("setECSZoneId" );
		if (!m_isNew)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "An existing ECS Zone ID cannot be modified." );
			return;
		}

        m_idKey = idKey;

        FUNCTION_EXIT;
        return;
    }


    std::string ECSZoneHelper::getECSLocation()
    {
        FUNCTION_ENTRY("getECSLocation" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_ecsLocation;
    }


    void ECSZoneHelper::setECSLocation(const std::string& ecsLocation)
    {
        FUNCTION_ENTRY("setECSLocation" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_ecsLocation = ecsLocation;
        FUNCTION_EXIT;
    }

    std::string ECSZoneHelper::getTrack()
    {
        FUNCTION_ENTRY("getTrack" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_track;
    }


    void ECSZoneHelper::setTrack(const std::string& track)
    {
        FUNCTION_ENTRY("setTrack" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_track = track;
        FUNCTION_EXIT;
    }

	
    unsigned long ECSZoneHelper::getOccEquipmentEntityKey()
    {
        FUNCTION_ENTRY("getOccEquipmentEntityKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_occEquipmentEntityKey;
    }


    void ECSZoneHelper::setOccEquipmentEntityKey(unsigned long entityKey)
    {
        FUNCTION_ENTRY("setOccEquipmentEntityKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_occEquipmentEntityKey = entityKey;
        m_occEquipmentEntityKeyHasBeenSet = true;
        FUNCTION_EXIT;
    }


    unsigned long ECSZoneHelper::getStnEquipmentEntityKey()
    {
        FUNCTION_ENTRY("getStnEquipmentEntityKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_stnEquipmentEntityKey;
    }


    void ECSZoneHelper::setStnEquipmentEntityKey(unsigned long entityKey)
    {
        FUNCTION_ENTRY("setStnEquipmentEntityKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_stnEquipmentEntityKey = entityKey;
        m_stnEquipmentEntityKeyHasBeenSet = true;
        FUNCTION_EXIT;
    }


    bool ECSZoneHelper::getIsIgnoringCongestion()
    {
        FUNCTION_ENTRY("getIsIgnoringCongestion" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_isIgnoringCongestion;
    }


    void ECSZoneHelper::setIsIgnoringCongestion(bool isIgnoringCongestion)
    {
        FUNCTION_ENTRY("setIsIgnoringCongestion" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_isIgnoringCongestion = isIgnoringCongestion;
        m_isIgnoringCongestionHasBeenSet = true;
        FUNCTION_EXIT;
    }


    time_t ECSZoneHelper::getDateCreated()
    {
        FUNCTION_ENTRY("getDateCreated" );

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateCreated;
    }


    time_t ECSZoneHelper::getDateModified()
    {
        FUNCTION_ENTRY("getDateModified" );

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateModified;
    }




    void ECSZoneHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
		if (m_isNew)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Attempted to call invalidate() on a new ECSZone" );
			return;
		}

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void ECSZoneHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the ECSZone has already been written to the database. It doesn't make
        // any sense to reload an ECSZone that doesn't exist in the database.
		if (m_isNew)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "This ECSZone does not yet exist in the database. Therefore it cannot be reloaded." );
			return;
		}
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);


        // create the SQL string to retrieve the data of the ECSZone
        // based upon the key
       /* std::ostringstream sql;
		sql << "select ECS_LOCATION, TRACK, OCC_EQUIPMENT_ENTITYKEY, STN_EQUIPMENT_ENTITYKEY, IS_IGNORING_CONGESTION, "
            << "TO_CHAR(nvl(DATE_MODIFIED,TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from EC_ZONE where ECZONE_ID=" << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_ZONE_Oracle_SELECT_51553, m_idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_ZONE_SELECT_51553, m_idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_ZONE_SELECT_51553, m_idKey);
 
        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back("ECS_LOCATION");
		columnNames.push_back("TRACK");
        columnNames.push_back("OCC_EQUIPMENT_ENTITYKEY");
        columnNames.push_back("STN_EQUIPMENT_ENTITYKEY");
        columnNames.push_back("IS_IGNORING_CONGESTION");
		columnNames.push_back("DATE_MODIFIED");
		columnNames.push_back("DATE_CREATED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // ECS Zone record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for ECS Zone Id = " << m_idKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"ECS Zone Id" ) );
        }
        
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_ecsLocation					  = data->getStringData( 0, "ECS_LOCATION" );
		m_track							  = data->getStringData( 0, "TRACK" );
        m_occEquipmentEntityKey           = data->getUnsignedLongData( 0, "OCC_EQUIPMENT_ENTITYKEY" );
        m_occEquipmentEntityKeyHasBeenSet = true;
        m_stnEquipmentEntityKey           = data->getUnsignedLongData( 0, "STN_EQUIPMENT_ENTITYKEY" );
        m_stnEquipmentEntityKeyHasBeenSet = true;
        m_isIgnoringCongestion				  = (TRUE_STRING == data->getStringData( 0, "IS_IGNORING_CONGESTION" ));
        m_isIgnoringCongestionHasBeenSet		  = true;
        m_dateCreated                     = data->getDateData( 0, "DATE_CREATED" );

        try
        {
            m_dateModified = data->getDateData( 0, "DATE_MODIFIED");
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

    void ECSZoneHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_ecsLocation						= data.getStringData( row, "ECS_LOCATION" );
		m_track								= data.getStringData( row, "TRACK" );
        m_occEquipmentEntityKey             = data.getUnsignedLongData( row, "OCC_EQUIPMENT_ENTITYKEY" );
        m_occEquipmentEntityKeyHasBeenSet   = true;
        m_stnEquipmentEntityKey				= data.getUnsignedLongData( row, "STN_EQUIPMENT_ENTITYKEY" );
        m_stnEquipmentEntityKeyHasBeenSet	= true;
		// marvin++ 
        m_isIgnoringCongestion					= (TRUE_STRING == data.getStringData( row, "IS_IGNORING_CONGESTION" ));
		// ++marvin 
        m_isIgnoringCongestionHasBeenSet			= true;
        m_dateCreated						= data.getDateData( row, "DATE_CREATED" );

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

    void ECSZoneHelper::writeECSZoneData()
        //throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeECSZoneData" );     

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if (-1 == m_idKey)
        {
            fieldNames.push_back("ECZONE_ID");
        }
        if (m_ecsLocation.empty())
        {
            fieldNames.push_back("ECS_LOCATION");
        }
        if (m_track.empty())
        {
            fieldNames.push_back("TRACK");
        }
        if (false == m_occEquipmentEntityKeyHasBeenSet)
        {
            fieldNames.push_back("OCC_EQUIPMENT_ENTITYKEY");
        }
        if (false == m_stnEquipmentEntityKeyHasBeenSet)
        {
            fieldNames.push_back("STN_EQUIPMENT_ENTITYKEY");
        }
        if (false == m_isIgnoringCongestionHasBeenSet)
        {
            fieldNames.push_back("IS_IGNORING_CONGESTION");
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("ECSZone data not fully specified. ECSZone cannot be written to database", fieldNames));
        }

        // Write the ECSZone to the database

        // Need to check if this is a new ECSZone or an existing ECSZone - 
        // as a new ECSZone will be inserted, while an existing ECSZone will be updated
        if (m_isNew) // This is a new ECSZone
        {
            addNewECSZone();
        }
        else // This is an existing ECSZone
        {
            modifyExistingECSZone();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void ECSZoneHelper::deleteECSZone(bool cascade)
    {
        FUNCTION_ENTRY("deleteECSZone");

		if (m_isNew)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "This ECSZone does not yet exist in the database, and therefore cannot be deleted" );
			return;
		}
        

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Write);
		//int nDbType = databaseConnection->GetDbServerType();
        //std::ostringstream sql;
		//std::string strSql;
		TA_Base_Core::SQLStatement strSql;
        
        if (!cascade)
        {
            std::vector<std::string> columnNames;
            TA_Base_Core::IData* data = NULL;

            // Check for references in EC_MASTER_MODE
            columnNames.clear();
            columnNames.push_back("ECMMOD_ID");
            
            /*sql.str("");
            sql << "select ECMMOD_ID from EC_MASTER_MODE where ECZONE_ID = " << m_idKey;*/
			//strSql = defPrepareSQLStatement(nDbType, EC_MASTER_MODE_STD_SELECT_50504, m_idKey);
//			strSql = databaseConnection->prepareSQLStatement(EC_MASTER_MODE_SELECT_50504, m_idKey);

		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_SELECT_50504, m_idKey);

            // execute the query
            data = databaseConnection->executeQuery(strSql,columnNames);

            if (0 != data->getNumRows())
            {
                delete data;
                data = NULL;

                TA_THROW(TA_Base_Core::DataException("A reference to this EC_ZONE exists in the EC_MASTER_MODE table",
                    TA_Base_Core::DataException::CANNOT_DELETE,"EC_MASTER_MODE"));
            }

            delete data;
            data = NULL;
        }

        // Good to go...
        
      /*  sql.str("");
        sql << "delete EC_ZONE where ECZONE_ID = " << m_idKey;*/
		//strSql = defPrepareSQLStatement(nDbType, EC_ZONE_STD_DELETE_51901, m_idKey);
//		strSql = databaseConnection->prepareSQLStatement(EC_ZONE_DELETE_51901, m_idKey);

		databaseConnection->prepareSQLStatement(strSql, EC_ZONE_DELETE_51901, m_idKey);
        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void ECSZoneHelper::modifyExistingECSZone()
        //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingECSZone");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "ECSZone id=%ul already exists. It's data will be updated.",m_idKey);

        validateZoneIdUniqueness();
        validateLocationUniqueness();
        validateEquipmentExistence(m_occEquipmentEntityKey);
        validateEquipmentExistence(m_stnEquipmentEntityKey);
        validateEquipmentUniqueness("OCC_EQUIPMENT_ENTITYKEY", m_occEquipmentEntityKey);
        validateEquipmentUniqueness("STN_EQUIPMENT_ENTITYKEY", m_stnEquipmentEntityKey);
    
        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Write);
		//int nDbType = databaseConnection->GetDbServerType();
       /* std::ostringstream sql;
        sql << "update EC_ZONE set "
            << "ECS_LOCATION='" << (databaseConnection->escapeInsertString(m_ecsLocation)).c_str() << "', " 
            << "TRACK='" << (databaseConnection->escapeInsertString(m_track)).c_str() << "', " 
            << "OCC_EQUIPMENT_ENTITYKEY= '" << m_occEquipmentEntityKey << "', " 
            << "STN_EQUIPMENT_ENTITYKEY= '" << m_stnEquipmentEntityKey << "',"  
            << "IS_IGNORING_CONGESTION= '" << (m_isIgnoringCongestion?TRUE_STRING:FALSE_STRING) << "' "  
            << "where ECZONE_ID = " << m_idKey;*/
		std::string strIgnorCongestion = (m_isIgnoringCongestion?TRUE_STRING:FALSE_STRING);
		/*std::string strSql = defPrepareSQLStatement(nDbType, EC_ZONE_STD_UPDATE_51651, databaseConnection->escapeInsertString(m_ecsLocation),
			databaseConnection->escapeInsertString(m_track),m_occEquipmentEntityKey,m_stnEquipmentEntityKey,strIgnorCongestion,m_idKey);*/
//		std::string strSql = databaseConnection->prepareSQLStatement(EC_ZONE_UPDATE_51651, databaseConnection->escapeInsertString(m_ecsLocation),
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_ZONE_UPDATE_51651, databaseConnection->escapeInsertString(m_ecsLocation),
			databaseConnection->escapeInsertString(m_track),m_occEquipmentEntityKey,m_stnEquipmentEntityKey,strIgnorCongestion,m_idKey);

        // get a connection to the database
        databaseConnection->executeModification(strSql);


        // now need to update the date fields
       /* sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "from EC_ZONE where ECZONE_ID = " << m_idKey;*/
		//strSql = defPrepareSQLStatement(nDbType, EC_ZONE_Oracle_SELECT_51554, m_idKey);
//		strSql = databaseConnection->prepareSQLStatement(EC_ZONE_SELECT_51554, m_idKey);

		databaseConnection->prepareSQLStatement(strSql, EC_ZONE_SELECT_51554, m_idKey);

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back("DATE_CREATED");
        columnNames.push_back("DATE_MODIFIED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // ECS Zone record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for ECS Zone Id = " << m_idKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"ECS Zone Id" ) );
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


    void ECSZoneHelper::addNewECSZone()
        //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewECSZone");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "ECSZone %ul is a new ECSZone. It will be added to the database.",m_idKey);

        validateZoneIdUniqueness();
        validateLocationUniqueness();
        validateEquipmentExistence(m_occEquipmentEntityKey);
        validateEquipmentExistence(m_stnEquipmentEntityKey);
        validateEquipmentUniqueness("OCC_EQUIPMENT_ENTITYKEY", m_occEquipmentEntityKey);
        validateEquipmentUniqueness("STN_EQUIPMENT_ENTITYKEY", m_stnEquipmentEntityKey);


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Write);
		//int nDbType = databaseConnection->GetDbServerType();
       /* std::ostringstream sql;
        sql << "insert into EC_ZONE (ECZONE_ID, ECS_LOCATION, TRACK, OCC_EQUIPMENT_ENTITYKEY, STN_EQUIPMENT_ENTITYKEY, IS_IGNORING_CONGESTION) "
            << "values ('"
            << m_idKey << "','" 
            << (databaseConnection->escapeInsertString(m_ecsLocation)).c_str() << "','" 
            << (databaseConnection->escapeInsertString(m_track)).c_str() << "','" 
            << m_occEquipmentEntityKey << "','"
            << m_stnEquipmentEntityKey << "','"
			<< (m_isIgnoringCongestion?TRUE_STRING:FALSE_STRING) << "')";*/
		std::string strIgnoreCongestion = (m_isIgnoringCongestion ? TRUE_STRING : FALSE_STRING);
		/*std::string strSql = defPrepareSQLStatement(nDbType, EC_ZONE_STD_INSERT_51801, m_idKey,databaseConnection->escapeInsertString(m_ecsLocation),
			databaseConnection->escapeInsertString(m_track),m_occEquipmentEntityKey,m_stnEquipmentEntityKey,strIgnoreCongestion);*/
//		std::string strSql = databaseConnection->prepareSQLStatement(EC_ZONE_INSERT_51801, m_idKey,databaseConnection->escapeInsertString(m_ecsLocation),
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_ZONE_INSERT_51801, m_idKey,databaseConnection->escapeInsertString(m_ecsLocation),
			databaseConnection->escapeInsertString(m_track),m_occEquipmentEntityKey,m_stnEquipmentEntityKey,strIgnoreCongestion);

        databaseConnection->executeModification(strSql);

        //now need to get the update of the date fields
      /*  sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from EC_ZONE where ECZONE_ID = '" << m_idKey << "'";*/
		//strSql = defPrepareSQLStatement(nDbType, EC_ZONE_Oracle_SELECT_51555, m_idKey);
//		strSql = databaseConnection->prepareSQLStatement(EC_ZONE_SELECT_51555, m_idKey);

		databaseConnection->prepareSQLStatement(strSql, EC_ZONE_SELECT_51555, m_idKey);

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back("DATE_CREATED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // ECS Zone record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for ECS Zone Id = " << m_idKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"ECS Zone Id" ) );
        }

        // Retrieve the pkey as an unsigned long. There should only be a single returned item 
        // (as any other circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
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

    void ECSZoneHelper::validateZoneIdUniqueness()
        //throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // First ensure the uniqueness of the record.
        //
        ////////////////////////////////////////////////////////////////////////////

        if (m_isNew)
        {
           /* std::ostringstream sql;
            sql << "select ECZONE_ID from EC_ZONE where ECZONE_ID = " << m_idKey;	*/		

            // Set up the vector to pass to exectueQuery
            std::vector<std::string> columnNames;
            columnNames.push_back("ECZONE_ID");

            // execute the query
            TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_ZONE_STD_SELECT_51501, m_idKey);
//			std::string strSql  = databaseConnection->prepareSQLStatement(EC_ZONE_SELECT_51501, m_idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_ZONE_SELECT_51501, m_idKey);
            TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
            if (0 != data->getNumRows())
            {
                // ZoneId already exists
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "An EC_ZONE record exists with ZoneId=" << m_idKey;
                TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NOT_UNIQUE,"ECS Zone ID"));
            }

            delete data;
            data = NULL;

        }
        else
        { 
            // Do nothing
        }

    }


    void ECSZoneHelper::validateLocationUniqueness()
        //throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // Since case insensitive comparisons are performed on the TRACK and 
        // ECS_LOCATION fields, we do a select on these to check if it currently 
        // exists for another key.
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
       /* std::ostringstream sql;
        sql << "select ECZONE_ID from EC_ZONE where UPPER(TRACK) = '"
            << databaseConnection->escapeInsertString(toUpper(m_track))
            << "' and UPPER(ECS_LOCATION) = '"
            << databaseConnection->escapeInsertString(toUpper(m_ecsLocation))
            << "' and ECZONE_ID<>" << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_ZONE_STD_SELECT_51503,
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_ZONE_SELECT_51503,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_ZONE_SELECT_51503,
			databaseConnection->escapeInsertString(toUpper(m_track)),databaseConnection->escapeInsertString(toUpper(m_ecsLocation)),m_idKey);

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back("ECZONE_ID");

        // execute the query
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 != data->getNumRows())
        {
            // ECS Location / Track combination already exists
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "An EC_ZONE record exists with " 
                          << "Location-Track = " 
                          << m_ecsLocation   << "-" << m_track;
            TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NOT_UNIQUE, "Location-Track (case-insensitive)"));
        }
        
        delete data;
        data = NULL;
    }

    void ECSZoneHelper::validateEquipmentExistence(unsigned long entityKey)
        //throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // Next check that the XXX_EQUIPMENT_ENTITYKEY already exists.
        //
        ////////////////////////////////////////////////////////////////////////////

       /* std::ostringstream sql;
        sql << "select PKEY from ENTITY where PKEY = " << entityKey;*/
        
        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back("PKEY");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_SELECT_4511, entityKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4511, entityKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4511, entityKey);
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // Status datapoint entitykey not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for entity key = " << entityKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Equipment entity key" ) );
        }

        delete data;
        data = NULL;

    }


    void ECSZoneHelper::validateEquipmentUniqueness(const std::string& equipmentColumnName, unsigned long entityKey)
        //throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // Next check that the columnName is not used elsewhere.
        //
        ////////////////////////////////////////////////////////////////////////////

       /* std::ostringstream sql;
        sql << "select ECZONE_ID from EC_ZONE where " << equipmentColumnName << "=" << entityKey
            << " and ECZONE_ID!=" << m_idKey;*/

       
        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back("ECZONE_ID");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_ZONE_STD_SELECT_51504, equipmentColumnName, entityKey, m_idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_ZONE_SELECT_51504, equipmentColumnName, entityKey, m_idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_ZONE_SELECT_51504, equipmentColumnName, entityKey, m_idKey);
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 != data->getNumRows())
        {
            // Status DataPoint entity key is referenced by another ECS Zone 
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "An EC_ZONE record exists referencing equipment entity key = " << entityKey;
            TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NOT_UNIQUE,equipmentColumnName.c_str()));
        }

        delete data;
        data = NULL;

    }

} // closes TA_Core

