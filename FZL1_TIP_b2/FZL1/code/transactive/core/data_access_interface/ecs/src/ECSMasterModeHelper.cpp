/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ecs/src/ECSMasterModeHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * ECSMasterModeHelper is an object that is held by ECSMasterMode and ConfigECSMasterMode objects. 
  * It contains all data used by ECSMasterModes, and manipulation
  * methods for the data. It helps avoid re-writing code for both ECSMasterMode and ConfigECSMasterMode.
  */


#ifdef __WIN32__
	#pragma warning(disable:4786)
	#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/ecs/src/ECSMasterModeHelper.h"
#include "core/data_access_interface/ecs/src/ECSMasterModeAccessFactory.h"
#include "core/data_access_interface/ecs/src/ECSStationModeAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

    static const std::string KEY_COL = "ECMMOD_ID";
    static const std::string MASTERMODE_COL = "MASTER_MODE";
    static const std::string CONDITION_COL = "CONDITION";
    static const std::string ECSZONEID_COL = "ECZONE_ID";
    static const std::string EVACDIR_COL = "EVACUATION_DIRECTION";
    static const std::string AIRDIR_COL = "AIRFLOW_DIRECTION";
	static const std::string SITUATION_COL = "SITUATION";
    static const std::string DISPDIR_COL = "DISPLAY_DIRECTION";
    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";
    static const std::string DATECREATED_COL = "DATE_CREATED";
    static const std::string MASTERMODEKEY_COL = "ECMMOD_ID";
    static const std::string STATIONMODEKEY_COL = "ECSMOD_ID";
	static const std::string VERIFMSG_COL = "VERIFICATION_MSG";


	ECSMasterModeHelper::ECSMasterModeHelper( const ECSMasterModeHelper& theECSMasterModeHelper)
		: m_key(0),
		  m_masterMode(theECSMasterModeHelper.m_masterMode),
          m_condition(theECSMasterModeHelper.m_condition),
          m_ECSZoneId(theECSMasterModeHelper.m_ECSZoneId),
          //m_ECSZoneIdIsNull(theECSMasterModeHelper.m_ECSZoneIdIsNull),
          m_evacDir(theECSMasterModeHelper.m_evacDir),
          m_airDir(theECSMasterModeHelper.m_airDir),
          m_dispDir(theECSMasterModeHelper.m_dispDir),
		  m_verificationMsg(theECSMasterModeHelper.m_verificationMsg),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true),
          m_associatedStationModes(theECSMasterModeHelper.m_associatedStationModes)
    {	  
	}


    ECSMasterModeHelper::ECSMasterModeHelper(const unsigned long key)
        : m_key(key),
		  m_masterMode(0),
          m_condition(""),
          m_ECSZoneId(-1),
          //m_ECSZoneIdIsNull(true),
          m_evacDir("NA"),
          m_airDir("NA"),
          m_dispDir("XX"),
		  m_verificationMsg(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    ECSMasterModeHelper::ECSMasterModeHelper()
        : m_key(0),
		  m_masterMode(0),
          m_condition(""),
          m_ECSZoneId(-1),
          //m_ECSZoneIdIsNull(true),
          m_evacDir("NA"),
          m_airDir("NA"),
          m_dispDir("XX"),
		  m_verificationMsg(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    ECSMasterModeHelper::ECSMasterModeHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_key(0),
		  m_masterMode(0),
          m_condition(""),
          m_ECSZoneId(-1),
          //m_ECSZoneIdIsNull(true),
          m_evacDir("NA"),
          m_airDir("NA"),
          m_dispDir("XX"),
		  m_verificationMsg(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        m_key = data.getUnsignedLongData(row, KEY_COL);

        reloadUsing(row, data);
    }


    ECSMasterModeHelper::~ECSMasterModeHelper()
    {
    }
  

    unsigned long ECSMasterModeHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_key;
    }

    int ECSMasterModeHelper::getMasterMode()
    {
        FUNCTION_ENTRY("getMasterMode" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_masterMode; 
    }

    void ECSMasterModeHelper::setMasterMode(const unsigned long masterMode)
    {
        FUNCTION_ENTRY("setMasterMode" ); 
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_masterMode = masterMode;
        FUNCTION_EXIT;
    }

    std::string ECSMasterModeHelper::getCondition()
    {
        FUNCTION_ENTRY("getCondition" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_condition;
    }


    void ECSMasterModeHelper::setCondition(const std::string& condition)
    {
        FUNCTION_ENTRY("setCondition" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_condition = condition;
        FUNCTION_EXIT;
    }

    unsigned long ECSMasterModeHelper::getECSZoneId()
    {
        FUNCTION_ENTRY("getECSZoneId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_ECSZoneId;
    }

    void ECSMasterModeHelper::setECSZoneId(const int zoneId)
    {
        FUNCTION_ENTRY("setECSZoneId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_ECSZoneId = zoneId;
        //m_ECSZoneIdIsNull = false;
        FUNCTION_EXIT;
    }

    std::string ECSMasterModeHelper::getEvacuationDirection()
    {
        FUNCTION_ENTRY("getEvacuationDirection" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_evacDir;
    }


    void ECSMasterModeHelper::setEvacuationDirection(const std::string& evacDir)
    {
        FUNCTION_ENTRY("setEvacuationDirection" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_evacDir = evacDir;
        FUNCTION_EXIT;
    }
    
    std::string ECSMasterModeHelper::getAirflowDirection()
    {
        FUNCTION_ENTRY("getAirflowDirection" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_airDir;
    }


    void ECSMasterModeHelper::setAirflowDirection(const std::string& airDir)
    {
        FUNCTION_ENTRY("setAirflowDirection" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_airDir = airDir;
        FUNCTION_EXIT;
    }
	std::string ECSMasterModeHelper::getSituation()
	{
		FUNCTION_ENTRY("getSituation" );
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_situation;
	}
	void ECSMasterModeHelper::setSituation(const std::string& situation)
	{
		FUNCTION_ENTRY("setSituation" );
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		m_situation = situation;
        FUNCTION_EXIT;
    }

    std::string ECSMasterModeHelper::getDisplayDirection()
    {
        FUNCTION_ENTRY("getDisplayDirection" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_dispDir;
    }


    void ECSMasterModeHelper::setDisplayDirection(const std::string& dispDir)
    {
        FUNCTION_ENTRY("setDisplayDirection" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_dispDir = dispDir;
        FUNCTION_EXIT;
    }


	std::string ECSMasterModeHelper::getVerificationMessage()
	{
		FUNCTION_ENTRY("setDisplayDirection" );
	    if (!m_isValidData && !m_isNew)
        {
            reload();
        } 
		FUNCTION_EXIT;
		return m_verificationMsg;
	}

    void ECSMasterModeHelper::setVerificationMessage(const std::string& msg)
    {
        FUNCTION_ENTRY("setVerificationMessage" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_verificationMsg = msg;
        FUNCTION_EXIT;
    }

    time_t ECSMasterModeHelper::getDateCreated()
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


    time_t ECSMasterModeHelper::getDateModified()
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


    IECSStationModes ECSMasterModeHelper::getAssociatedStationModes()
	{
        TA_ASSERT(!m_isNew,"The data must be written to the database before the associated StationModes can be retrieved");

        return ECSStationModeAccessFactory::getInstance().getECSStationModesByMasterModeKey( m_key, false);
	}

    
    std::vector<unsigned long> ECSMasterModeHelper::getAssociatedStationModeKeys()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        return m_associatedStationModes;
    }

    void ECSMasterModeHelper::setAssociatedStationMode( unsigned long stationModeKey, bool isAssociated)
    {
        FUNCTION_ENTRY("setAssociatedStationMode" );

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        std::vector<unsigned long>::iterator matchingStationMode = 
            std::find(m_associatedStationModes.begin(), m_associatedStationModes.end(), stationModeKey);

        if (isAssociated)
        {
            // we need to add this profile
            if ( matchingStationMode == m_associatedStationModes.end() )
            {
                // This is definitely not already in the list so add it
                m_associatedStationModes.push_back(stationModeKey);
            }
            // else the item is already in the list
        }
        else
        {
            // we need to remove this profile
            if ( matchingStationMode != m_associatedStationModes.end() )
            {
                // This is definitely in the list so we must remove it
                m_associatedStationModes.erase(matchingStationMode);
            }
            // else the item has already been removed from the list
        }

        FUNCTION_EXIT;
    }

    void ECSMasterModeHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new ECSMasterMode");

        m_isValidData = false;
		m_associatedStationModes.clear();

        FUNCTION_EXIT;
    }


    void ECSMasterModeHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the ECSMasterMode has already been written to the database. It doesn't make
        // any sense to reload an ECSMasterMode that doesn't exist in the database.
        TA_ASSERT(0 != m_key, "This ECSMasterMode does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the data of the ECSMasterMode
        // based upon the key
  //      std::ostringstream sql;
		//sql << "select " 
  //          << MASTERMODE_COL << ","
  //          << CONDITION_COL << ","
  //          << "nvl(" << ECSZONEID_COL << ",-1) as " <<ECSZONEID_COL <<","
  //          << EVACDIR_COL << ","
  //          << AIRDIR_COL << ","
  //          << DISPDIR_COL << ","
		//	<< VERIFMSG_COL << ","
			//<< "nvl(" << VERIFMSG_COL << ",'NA'), "
  //          << "TO_CHAR(nvl(DATE_MODIFIED,TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS'), "
  //          << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
  //          << "from EC_MASTER_MODE where "
  //          << KEY_COL << " = " << m_key;
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_SELECT_50553, m_key);
		    

        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(MASTERMODE_COL);
		columnNames.push_back(CONDITION_COL);
		columnNames.push_back(ECSZONEID_COL);
		columnNames.push_back(EVACDIR_COL);
		columnNames.push_back(AIRDIR_COL);
		columnNames.push_back(SITUATION_COL);
        columnNames.push_back(DISPDIR_COL);
		columnNames.push_back(VERIFMSG_COL);
        columnNames.push_back(DATEMODIFIED_COL);
        columnNames.push_back(DATECREATED_COL);
		

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // ECS Master Mode record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for ECS Master Mode key = " << m_key;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"ECS Master Mode key" ) );
        }
        
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_masterMode  = data->getUnsignedLongData( 0, MASTERMODE_COL );
		m_condition   = data->getStringData( 0, CONDITION_COL );

        // Note that the ECSZoneId is read as an integer as we need to represent a null value by a -1
		m_ECSZoneId   = data->getIntegerData( 0, ECSZONEID_COL );
		m_evacDir     = data->getStringData( 0, EVACDIR_COL );
        m_airDir      = data->getStringData( 0, AIRDIR_COL );
		m_situation   = data->getStringData( 0, SITUATION_COL );
        m_dispDir     = data->getStringData( 0, DISPDIR_COL );
        m_dateCreated = data->getDateData( 0, DATECREATED_COL );

        try
        {
            m_dateModified = data->getDateData( 0, DATEMODIFIED_COL );
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

		m_verificationMsg = data->getStringData( 0, VERIFMSG_COL);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;

        // Now load the associated StationModeKeys.
        std::vector<unsigned long> stationModeKeys = ECSStationModeAccessFactory::getInstance().getStationModeKeysByMasterModeKey( m_key );
	
        for(std::vector<unsigned long>::iterator iter = stationModeKeys.begin(); iter != stationModeKeys.end(); ++iter)
        {
            m_associatedStationModes.push_back( *iter );
        }
        stationModeKeys.clear();

        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void ECSMasterModeHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_masterMode  = data.getUnsignedLongData( row, MASTERMODE_COL );
		m_condition   = data.getStringData( row, CONDITION_COL );

        // Note that the ECSZoneId is read as an integer as we need to represent a null value by a -1
		m_ECSZoneId   = data.getIntegerData( row, ECSZONEID_COL );
		m_evacDir     = data.getStringData( row, EVACDIR_COL );
        m_airDir      = data.getStringData( row, AIRDIR_COL );
		m_situation   = data.getStringData( row, SITUATION_COL );
        m_dispDir     = data.getStringData( row, DISPDIR_COL );
		m_verificationMsg = data.getStringData(row, VERIFMSG_COL);
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
		
		/* zhangnan comment this for this will run in the SQL before now running, so us another way to call this
		   lines in the up level
        // Now load the associated StationModeKeys.
        std::vector<unsigned long> stationModeKeys = ECSStationModeAccessFactory::getInstance().getStationModeKeysByMasterModeKey( m_key );
	
        for(std::vector<unsigned long>::iterator iter = stationModeKeys.begin(); iter != stationModeKeys.end(); ++iter)
        {
            m_associatedStationModes.push_back( *iter );
        }
        stationModeKeys.clear();
        */
        // Need to record that we now have valid data, maybe should set this value up level, because comment the for statement
        m_isValidData = true;
		
        FUNCTION_EXIT;
    }


    void ECSMasterModeHelper::writeECSMasterModeData() throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeECSMasterModeData" );     

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if (0 == m_masterMode)
        {
            fieldNames.push_back(MASTERMODE_COL);
        }
        if (m_condition.empty())
        {
            fieldNames.push_back(CONDITION_COL);
        }
        //if (0 == m_ECSZoneId)
        //{
        //    fieldNames.push_back(ECSZONEID_COL);
        //}
        if (m_evacDir.empty())
        {
            fieldNames.push_back(EVACDIR_COL);
        }
        if (m_airDir.empty())
        {
            fieldNames.push_back(AIRDIR_COL);
        }
		if (m_situation.empty())
		{
			fieldNames.push_back(SITUATION_COL);
		}
        if (m_dispDir.empty())
        {
            fieldNames.push_back(DISPDIR_COL);
        }

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("ECSMasterMode data not fully specified. ECSMasterMode cannot be written to database",
                                                         fieldNames));
        }

        // Write the ECSMasterMode to the database

        
        // Need to check if this is a new ECSMasterMode or an existing ECSMasterMode - 
        // as a new ECSMasterMode will be inserted, while an existing ECSMasterMode will be updated
        if (m_isNew) // This is a new ECSMasterMode
        {
            addNewECSMasterMode();
        }
        else // This is an existing ECSMasterMode
        {
            modifyExistingECSMasterMode();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void ECSMasterModeHelper::deleteECSMasterMode(bool cascade)
    {
        FUNCTION_ENTRY("deleteECSMasterMode");

        TA_ASSERT(!m_isNew, "This ECSMasterMode does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Write);
        //std::ostringstream sql;
		TA_Base_Core::SQLStatement strSql;
        
        if (!cascade)
        {
            // Check for references in EC_MASTER_MODE_HELPER
            std::vector<std::string> columnNames;
            columnNames.push_back("ECMMHE_ID");
            TA_Base_Core::IData* data = NULL;

           /* sql.str("");
            sql << "select ECMMHE_ID from EC_MASTER_MODE_HELPER where " 
                << MASTERMODEKEY_COL << " = " << m_key;*/

		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_HELPER_SELECT_51001, m_key);
            // execute the query
            data = databaseConnection->executeQuery(strSql,columnNames);

            if (0 != data->getNumRows())
            {
                delete data;
                data = NULL;

                TA_THROW(TA_Base_Core::DataException("A reference to this EC_MASTER_MODE exists in the EC_MASTER_MODE_HELPER table",
                    TA_Base_Core::DataException::CANNOT_DELETE,"EC_MASTER_MODE_HELPER"));
            }

            delete data;
            data = NULL;

        }

        // Good to go...
       /* sql.str("");
        sql << "delete EC_MASTER_MODE where " << KEY_COL << " = " << m_key;*/
		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_DELETE_50901, m_key);
        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }

    void ECSMasterModeHelper::updateStationModeAssociations()
    {
        FUNCTION_ENTRY("updateStationModeAssociations");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Write);

       /* std::ostringstream sql;
        sql << "select " << STATIONMODEKEY_COL 
            << " from EC_MASTER_MODE_HELPER where " 
            << MASTERMODEKEY_COL << " = " << m_key;*/
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_HELPER_SELECT_51002, m_key);

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back(STATIONMODEKEY_COL);

        // execute the query
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        std::vector<unsigned long> localStationModesToAdd = m_associatedStationModes;
        std::vector<unsigned long> databaseStationModesToRemove;

        // Now we step through the keys retrieved from the database.
        // If we find the key in both lists then the database is up-to-date and nothing needs to be done to the
        // lists (ie we dont' add it to the databaseStationModesToRemove list). At the end the keys left in the localStationModesToAdd
        // list need to be added to the database and the StationModes left in databaseStationModesToRemove must be removed from the
        // database.
        for (unsigned int i = 0; i < data->getNumRows(); ++i)
        {
            unsigned long currentKey = data->getUnsignedLongData(i, STATIONMODEKEY_COL);

            std::vector<unsigned long>::iterator matching = std::find(localStationModesToAdd.begin(), localStationModesToAdd.end(), currentKey);
            if (matching != localStationModesToAdd.end())
            {
                // This key has been found so it has not changed
                // We remove it from the localStationModesToRemove list as we do not need to remove it from the database
                localStationModesToAdd.erase( matching );
            }
            else
            {
                // This key has obviously been removed from our local list so it will need to be removed from the database
                databaseStationModesToRemove.push_back( currentKey );
            }
        }

        delete data;
        data = NULL;

        // Now we add and delete from/to the database as required
       
        // First lets delete old values from the database
        if (!databaseStationModesToRemove.empty())
        {
            std::ostringstream sql;
         /*   sql << "delete EC_MASTER_MODE_HELPER where " 
                << STATIONMODEKEY_COL << " in (";*/
            for (std::vector<unsigned long>::iterator iterToRemove = databaseStationModesToRemove.begin();
                 iterToRemove != databaseStationModesToRemove.end(); ++iterToRemove)
            {
                if ( iterToRemove != databaseStationModesToRemove.begin())
                {
                    // If this is not the first item then we must insert a comma
                    sql << ",";
                }
                sql << *iterToRemove;
            }    
           /* sql << ") and "
                << MASTERMODEKEY_COL << " = " << m_key;*/

		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_HELPER_DELETE_51401, sql.str(), m_key);
            databaseConnection->executeModification(strSql);
        }

        // Now lets add new values to the database
        if (!localStationModesToAdd.empty())
        {
            for (std::vector<unsigned long>::iterator iterToAdd = localStationModesToAdd.begin();
                 iterToAdd != localStationModesToAdd.end(); ++iterToAdd)
            {

                validateStationModeKeyExistence(*iterToAdd);
                
                ////////////////////////////////////////////////////////////////////////////
                //
                // Good to go...
                //
                ////////////////////////////////////////////////////////////////////////////

               /* std::ostringstream sql;
                sql << "insert into EC_MASTER_MODE_HELPER (" 
                    << "ECMMHE_ID, " << MASTERMODEKEY_COL << ", " << STATIONMODEKEY_COL << ") "
                    << "values( ECMMHE_SEQ.NEXTVAL, " << m_key << "," << *iterToAdd << ")";*/

		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_HELPER_SELECT_51351,
            databaseConnection->getNextPkey("ECMMHE_SEQ"), m_key, *iterToAdd);
                databaseConnection->executeModification(strSql);
            }
        } 
                 
        FUNCTION_EXIT;
    }


    void ECSMasterModeHelper::modifyExistingECSMasterMode() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingECSMasterMode");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "Master Mode %ul already exists. It's data will be updated.",m_masterMode);

        validateMasterModeUniqueness();
        validateZoneIdExistence();


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Write);
        std::ostringstream sql;
        /*sql << "update EC_MASTER_MODE set " 
            << MASTERMODE_COL << " = " << m_masterMode << ", "
            << CONDITION_COL << " = '"
            << (databaseConnection->escapeInsertString(m_condition)).c_str() << "', " 
            << ECSZONEID_COL << " = '";*/

        if (-1 != m_ECSZoneId)
        {
            sql << m_ECSZoneId;
        }

        /*sql << "', "
            << EVACDIR_COL << " = '"
            << (databaseConnection->escapeInsertString(m_evacDir)).c_str() << "', " 
            << AIRDIR_COL << " = '"
            << (databaseConnection->escapeInsertString(m_airDir)).c_str() << "', " 
            << DISPDIR_COL << " = '"
            << (databaseConnection->escapeInsertString(m_dispDir)).c_str() << "', " 
            << VERIFMSG_COL << " = '"
            << (databaseConnection->escapeInsertString(m_verificationMsg)).c_str() << "' " 
            << "where " << KEY_COL << " = " << m_key;*/

		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_UPDATE_50701, m_masterMode,
			databaseConnection->escapeInsertString(m_condition),sql.str(),databaseConnection->escapeInsertString(m_evacDir),databaseConnection->escapeInsertString(m_airDir),
			databaseConnection->escapeInsertString(m_situation),databaseConnection->escapeInsertString(m_dispDir),databaseConnection->escapeInsertString(m_verificationMsg), m_key);
        databaseConnection->executeModification(strSql);

        // now need to update the date fields
        //sql.str("");
       /* sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
            << "from EC_MASTER_MODE where " << KEY_COL << " = " << m_key;*/

		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_SELECT_50554, m_key);
        std::vector<std::string> columnNames;
        columnNames.push_back(DATECREATED_COL);
        columnNames.push_back(DATEMODIFIED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // ECS Master Mode record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for ECS Master Mode key = " << m_key;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"ECS Master Mode key" ) );
        }

        try
        {
            m_dateCreated = data->getDateData(0,DATECREATED_COL);
            m_dateModified = data->getDateData(0,DATEMODIFIED_COL);
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

        // Now update the StationModes in the database
        updateStationModeAssociations();

        FUNCTION_EXIT;
    }


    void ECSMasterModeHelper::addNewECSMasterMode() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewECSMasterMode");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Master Mode %ul is a new ECSMasterMode. It will be added to the database.",m_masterMode);

        validateMasterModeUniqueness();
        validateZoneIdExistence();


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Write);
        std::ostringstream sql;
       /* sql << "insert into EC_MASTER_MODE (" 
            << KEY_COL << ", "
            << MASTERMODE_COL << ", "
            << CONDITION_COL << ", "
            << ECSZONEID_COL << ", "
            << EVACDIR_COL << ", "
            << AIRDIR_COL << ", "
            << DISPDIR_COL << ", "
			<< VERIFMSG_COL << ") "
            << "values (ECSMOD_SEQ.NEXTVAL,'"
            << m_masterMode << "','" 
            << (databaseConnection->escapeInsertString(m_condition)).c_str() << "','";*/

        if (-1 != m_ECSZoneId)
        {
            sql << m_ECSZoneId;
        }

       /* sql << "','"
            << (databaseConnection->escapeInsertString(m_evacDir)).c_str() << "','" 
            << (databaseConnection->escapeInsertString(m_airDir)).c_str() << "','"
            << (databaseConnection->escapeInsertString(m_dispDir)).c_str() << "','"
			<< (databaseConnection->escapeInsertString(m_verificationMsg)).c_str() << "')";*/
			
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_INSERT_50851,
            databaseConnection->getNextPkey("ECSMOD_SEQ"), m_masterMode,databaseConnection->escapeInsertString(m_condition),
			sql.str(),databaseConnection->escapeInsertString(m_evacDir),databaseConnection->escapeInsertString(m_airDir),databaseConnection->escapeInsertString(m_situation),databaseConnection->escapeInsertString(m_dispDir),
			databaseConnection->escapeInsertString(m_verificationMsg));
        databaseConnection->executeModification(strSql);

        //now need to get the PKEY back out for this location and update the date fields
       /* sql.str("");
        sql << "select " 
            << KEY_COL << ", "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from EC_MASTER_MODE where "
            << MASTERMODE_COL << " = " << m_masterMode;*/

		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_SELECT_50555, m_masterMode);
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(DATECREATED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // ECS Master Mode record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for ECS Master Mode = " << m_masterMode;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"ECS Master Mode" ) );
        }

        // Retrieve the pkey as an unsigned long. There should only be a single returned item 
        // (as any other circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0,KEY_COL);
            m_dateCreated = data->getDateData(0,DATECREATED_COL);
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

        // Now update the StationModes in the database
        updateStationModeAssociations();

        FUNCTION_EXIT;
    }


    void ECSMasterModeHelper::validateMasterModeUniqueness() throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // First ensure the uniqueness of the Master Mode.
        //
        ////////////////////////////////////////////////////////////////////////////
       /* std::ostringstream sql;
        sql << "select " << KEY_COL <<" from EC_MASTER_MODE where " 
            << MASTERMODE_COL << "="  << m_masterMode << " and "
            << KEY_COL        << "<>" << m_key;*/

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_MASTER_MODE_SELECT_50503, m_masterMode, m_key);
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 != data->getNumRows())
        {
            // Master Mode already exists
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "An EC_MASTER_MODE record exists with Master Mode = " << m_masterMode;
            TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NOT_UNIQUE,"ECS Master Mode"));
        }

        delete data;
        data = NULL;
    }

    void ECSMasterModeHelper::validateZoneIdExistence() throw(TA_Base_Core::TransactiveException)
    {

        ////////////////////////////////////////////////////////////////////////////
        //
        // Next check that the ECSZONEID already exists.
        //
        ////////////////////////////////////////////////////////////////////////////

        if (-1 == m_ECSZoneId)
        {
            // Special blank entry
            return;
        }

        //std::ostringstream sql;
        //sql << "select " << ECSZONEID_COL << " from EC_ZONE where " 
        //    << ECSZONEID_COL << " = " << m_ECSZoneId;
    
        std::vector<std::string> columnNames;
        columnNames.push_back(ECSZONEID_COL);

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_ZONE_SELECT_51501,m_ECSZoneId);
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // ECS Zone Id not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for ECS Zone Id = " << m_ECSZoneId;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"ECS Zone Id" ) );
        }

        delete data;
        data = NULL;

    }

    void ECSMasterModeHelper::validateStationModeKeyExistence(unsigned long stationModeKey) throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // Check that the STATION_MODE key already exists.
        //
        ////////////////////////////////////////////////////////////////////////////

       /* std::ostringstream sql;
        sql << "select ECSMOD_ID from EC_STATION_MODE where ECSMOD_ID = " << stationModeKey;*/

        std::vector<std::string> columnNames;
        columnNames.push_back("ECSMOD_ID");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_STATION_MODE_SELECT_52001, stationModeKey);
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // ECS Station Mode key not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for ECS Station Mode key = " << stationModeKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"ECS Station Mode key" ) );
        }

        delete data;
        data = NULL;

    }

} // closes TA_Core

