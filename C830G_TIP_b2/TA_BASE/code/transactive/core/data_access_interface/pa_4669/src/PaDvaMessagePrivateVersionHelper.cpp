/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/PaDvaMessagePrivateVersionHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * PaDvaMessagePrivateVersionHelper is an object that is held by PaDvaMessagePrivateVersion and ConfigPaDvaMessagePrivateVersion objects. 
  * It contains all data used by PaDvaMessagePrivateVersionGroups, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaDvaMessagePrivateVersion and ConfigPaDvaMessagePrivateVersion.
  *
  */

#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/pa_4669/src/PaDvaMessagePrivateVersionHelper.h"
#include "core/data_access_interface/pa_4669/src/PaDvaMessagePrivateVersionAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa_4669/src/DatabaseQueryHelper.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/ValueNotSetException.h"

#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    static const std::string PA_DVA_MESSAGE_VERSION_TABLE_NAME = "PA_DVA_MESSAGE_PRIVATE_VERSION";
    static const std::string PA_DVA_MESSAGE_VERSION_TABLE_SHORT_NAME = "PADMPRVE";
    static const std::string KEY_COL = "PADMPRVE_ID";

    static const std::string LOCATIONKEY_COL = "LOCATIONKEY";

    // NOTE: These must have same order as EVersionColumn enum
    static const std::string VERSION_COLUMNS[] = 
    {
        "PRIVATE_ADHOC1",
        "PRIVATE_ADHOC2",
        "PRIVATE_ADHOC3",
        "PRIVATE_ADHOC4",
        "PRIVATE_ADHOC5",
        "PRIVATE_ADHOC6",
        "PRIVATE_PRERECORDED"
    };
    
    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";
    static const std::string DATECREATED_COL = "DATE_CREATED";

	PaDvaMessagePrivateVersionHelper::PaDvaMessagePrivateVersionHelper( const PaDvaMessagePrivateVersionHelper& thePaDvaMessagePrivateVersionHelper)
		: // m_key(),   // Don't set the key upon copy..
          m_locationKey(thePaDvaMessagePrivateVersionHelper.m_locationKey),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {	
        for (int i = 0; i < IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX; i ++)
        {
            m_versionData[i] = thePaDvaMessagePrivateVersionHelper.m_versionData[i];
        }
	}


    PaDvaMessagePrivateVersionHelper::PaDvaMessagePrivateVersionHelper(const unsigned long key)
        : m_key(key),	
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        for (int i = 0; i < IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX; i ++)
        {
            m_versionData[i] = "";
        }
    }


    PaDvaMessagePrivateVersionHelper::PaDvaMessagePrivateVersionHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_key(data.getUnsignedLongData(row, KEY_COL)),	
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        for (int i = 0; i < IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX; i ++)
        {
            m_versionData[i] = "";
        }
        
        reloadUsing(row, data);
    }


    PaDvaMessagePrivateVersionHelper::PaDvaMessagePrivateVersionHelper()
        : m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
        for (int i = 0; i < IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX; i ++)
        {
            m_versionData[i] = "";
        }
    }


    PaDvaMessagePrivateVersionHelper::~PaDvaMessagePrivateVersionHelper()
    {
    }
  

    unsigned long PaDvaMessagePrivateVersionHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        
        if (!m_key.hasBeenSet())
        {
            TA_THROW(TA_Base_Core::DataException("Key not set",  
                            TA_Base_Core::DataException::NO_VALUE, "Primary key"));
        }        

        FUNCTION_EXIT;
        return m_key.getValue();
    }


    std::string PaDvaMessagePrivateVersionHelper::getMessageVersionUsingParams(
                                unsigned long stationDvaMessageId,
                                unsigned long startRange,
                                unsigned long endRange)
    {
        TA_ASSERT(stationDvaMessageId > startRange, "Message out of range");
        TA_ASSERT(stationDvaMessageId < endRange, "Message out of range");

        // There are 6 adhoc messages
        const unsigned int adhocUpperRange = 6;

        if (stationDvaMessageId < startRange + adhocUpperRange)
        {
            // This is an ad-hoc message, determine offset from first
            unsigned long offset = stationDvaMessageId - startRange;

            return getVersionData(IPaDvaMessagePrivateVersion::EVersionColumn
                                      (IPaDvaMessagePrivateVersion::PRIVATE_ADHOC1 + offset));
        }
        else
        {
            return getVersionData(IPaDvaMessagePrivateVersion::EVersionColumn
                                      (IPaDvaMessagePrivateVersion::PRIVATE_PRERECORDED));
        }
    }


    std::string PaDvaMessagePrivateVersionHelper::getMessageVersion(
                                            IPaDvaMessagePrivateVersion::ELocation location,
                                            unsigned long stationDvaMessageId)
    {
        return getMessageVersionUsingParams(stationDvaMessageId, 1, 500);
    }


    std::string PaDvaMessagePrivateVersionHelper::getVersionData(
        IPaDvaMessagePrivateVersion::EVersionColumn column)
    {
        TA_ASSERT(column < IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX, "Out of range");

        FUNCTION_ENTRY("getVersionData" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;

        return m_versionData[column];        
    }


    unsigned long PaDvaMessagePrivateVersionHelper::getLocationKey()
    {
        FUNCTION_ENTRY("getLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;

        // Following the reload of data, all values should have been set
        try
        {
            return m_locationKey.getValue();
        }
        catch (::TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            TA_ASSERT(false, "m_locationKey value not set after reload");
            throw;
        }
    }


    void PaDvaMessagePrivateVersionHelper::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_locationKey.setValue(locationKey);
        
        FUNCTION_EXIT;
    }


    void PaDvaMessagePrivateVersionHelper::setVersionData(
            const std::string& newVersion,
            IPaDvaMessagePrivateVersion::EVersionColumn column)
    {
        TA_ASSERT(column < IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX, "Out of range");

        FUNCTION_ENTRY("setVersionData" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_versionData[column] = newVersion;
        
        FUNCTION_EXIT;
    }


    time_t PaDvaMessagePrivateVersionHelper::getDateCreated()
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


    time_t PaDvaMessagePrivateVersionHelper::getDateModified()
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



    void PaDvaMessagePrivateVersionHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PaDvaMessagePrivateVersion");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

    std::string PaDvaMessagePrivateVersionHelper::getVersionDataColumns()
    {
        std::ostringstream result;

        for (int i = 0; i < IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX; i ++)
        {
            result << VERSION_COLUMNS[i];
            if (i < (IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX - 1))
            {
                // Add commas for all items except last
                result << ", ";
            }
        }

        return result.str();
    }


    const DatabaseQueryHelper::QueryData 
        PaDvaMessagePrivateVersionHelper::getBasicQueryData()
    {
        DatabaseQueryHelper::QueryData result;

        result.primaryKeyCol    = KEY_COL;
	    result.tableName        = PA_DVA_MESSAGE_VERSION_TABLE_NAME;

        result.columnNames.push_back(KEY_COL);
        result.columnNames.push_back(LOCATIONKEY_COL);

        int i;

        for (i = 0; i < IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX; i ++)
        {
            result.columnNames.push_back(VERSION_COLUMNS[i]);
        }

        result.columnNames.push_back(DATECREATED_COL);
        result.columnNames.push_back(DATEMODIFIED_COL);

        result.statement = 
            DatabaseQueryHelper::createBasicSelectStatement(result.columnNames, 
                                                            result.tableName);        
        return result;
    }


    void PaDvaMessagePrivateVersionHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing" );
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_locationKey.setValue(data.getUnsignedLongData( row, LOCATIONKEY_COL ));
    
        int i;
        for (i = 0; i < IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX; i ++)
        {
            m_versionData[i] = data.getStringData( row, VERSION_COLUMNS[i]);
        }

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


    void PaDvaMessagePrivateVersionHelper::writePaDvaMessagePrivateVersionData() //throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writePaDvaMessagePrivateVersionData" );     

        // Check ALL values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;        
        if (!m_locationKey.hasBeenSet())
        {
            fieldNames.push_back(LOCATIONKEY_COL);
        }
        
        /*
        // Can have empty version data values ""
        int i;
        for (i = 0; i < IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX; i ++)
        {
            if (m_versionData[i].length() == 0)
            {
                fieldNames.push_back(VERSION_COLUMNS[i]);
            }
        }*/

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("PaDvaMessagePrivateVersion data not fully specified. PaDvaMessagePrivateVersion cannot be written to database",
                                                         fieldNames));
        }
        
        // Write the PaDvaMessagePrivateVersion to the database

        try 
        {
            // Need to check if this is a new PaDvaMessagePrivateVersion or an existing PaDvaMessagePrivateVersion - 
            // as a new PaDvaMessagePrivateVersion will be inserted, while an existing PaDvaMessagePrivateVersion will be updated
            if (m_isNew) // This is a new PaDvaMessagePrivateVersion
            {
                addNewPaDvaMessagePrivateVersion();
            }
            else // This is an existing PaDvaMessagePrivateVersion
            {
                modifyExistingPaDvaMessagePrivateVersion();
            }
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "PaDvaMessagePrivateVersion data not fully specified. PaDvaMessagePrivateVersion cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }
   

    void PaDvaMessagePrivateVersionHelper::deletePaDvaMessagePrivateVersion()
    {
        FUNCTION_ENTRY("deletePaDvaMessagePrivateVersion");

        TA_ASSERT(!m_isNew, "This PaDvaMessagePrivateVersion does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);
        std::ostringstream sql;        

        // Good to go...
        sql.str("");
        sql << "delete " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << " where " << KEY_COL << " = " << m_key.getValue();

        databaseConnection->executeModification(sql.str());

        FUNCTION_EXIT;
    }


    void PaDvaMessagePrivateVersionHelper::modifyExistingPaDvaMessagePrivateVersion() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingPaDvaMessagePrivateVersion");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "PA DVA Message Version %u already exists. It's data will be updated.", m_key.getValue());


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);
        std::ostringstream sql;

//        sql << "update " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << " set " 
//            << LOCATIONKEY_COL << " = "
//            << m_locationKey.getValue()
//            << ", ";
//
//        for (int i = 0; i < IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX; i ++)
//        {
//            sql << VERSION_COLUMNS[i] << " = '" 
//                << (databaseConnection->escapeInsertString(m_versionData[i])).c_str()
//                << "'";
//
//            if (i < (IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX - 1))
//            {
//                // Don't append comma on last entry
//                sql << ", ";
//            }
//        }
//
//        sql << " where " << KEY_COL << " = " << m_key.getValue();        
//
//        databaseConnection->executeModification(sql.str().c_str());
		std::ostringstream formatSQL;
		formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "', 'UPDATE', '";
        formatSQL << "update " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << " set ";
		formatSQL << LOCATIONKEY_COL << " = ";
		formatSQL << m_locationKey.getValue() << ", ";

		for (int i = 0; i < IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX; i++)
		{
			formatSQL << VERSION_COLUMNS[i] << " = ''";
			formatSQL << (databaseConnection->escapeAQSQLString(m_versionData[i])).c_str();
			formatSQL << "''";
			if (i < (IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX - 1))
			{
				formatSQL << ", ";
			}
		}
		
		formatSQL << " where " << KEY_COL << " = " << m_key.getValue();

	    formatSQL << "');";
        formatSQL << "END;";
        databaseConnection->executeModification(formatSQL.str());  

		

        // now need to update the date fields
        sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
            << "from " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << 
            " where " << KEY_COL << " = " << m_key.getValue();

        std::vector<std::string> columnNames;
        columnNames.push_back(DATECREATED_COL);
        columnNames.push_back(DATEMODIFIED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA DVA Message Version record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA DVA Message Version key = " << m_key.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA DVA Message Version key" ) );
        }

        try
        {
            m_dateCreated = data->getDateData(0, DATECREATED_COL);
            m_dateModified = data->getDateData(0, DATEMODIFIED_COL);
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


    void PaDvaMessagePrivateVersionHelper::addNewPaDvaMessagePrivateVersion() 
                    //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewPaDvaMessagePrivateVersion");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "PA DVA Message Version %u is a new PaDvaMessagePrivateVersion. It will be added to the database.", m_key.getValue());


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);

        std::ostringstream sql;
//        sql << "insert into " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << " (" 
//            << KEY_COL << ", "
//            << LOCATIONKEY_COL << ", "
//            << getVersionDataColumns() << ") "
//            << "values (" << m_key.getValue() << ","
//            << m_locationKey.getValue() << ", ";
//
//        for (int i = 0; i < IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX; i ++)
//        {
//            sql << "'" << 
//                (databaseConnection->escapeInsertString(m_versionData[i])).c_str()
//                << "'";
//
//            if (i < (IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX - 1))
//            {
//                sql << ", ";
//            }
//            else
//            {
//                sql << ")";
//            }
//        }
//
//        databaseConnection->executeModification(sql.str());
		std::ostringstream formatSQL;
		formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "', 'INSERT', '";
        formatSQL << "INSERT INTO " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << " (";
		formatSQL << KEY_COL << ", ";
		formatSQL << LOCATIONKEY_COL << ", ";
		formatSQL << getVersionDataColumns() << ") ";
		formatSQL << "values (" << m_key.getValue() << ", ";
		formatSQL << m_locationKey.getValue() << ", ";

		for (int i = 0; i < IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX; i++)
		{
			formatSQL << "''" <<
				(databaseConnection->escapeAQSQLString(m_versionData[i])).c_str()
				<< "''";
			if (i < (IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX - 1))
			{
				formatSQL << ", ";
			}
			else
			{
				formatSQL << ")";
			}
		}
		
	    formatSQL << "');";
        formatSQL << "END;";
        databaseConnection->executeModification(formatSQL.str());  

        sql.str("");
        sql << "select " 
            << KEY_COL << ", "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << " where "
            << KEY_COL << " = " << m_key.getValue();

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(DATECREATED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA DVA Message Version record not found            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA DVA Message Version key = " << m_key.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA DVA Message Version key" ) );
        }

        // Retrieve the pkey as an unsigned long. There should only be a single returned item 
        // (as any other circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key.setValue(data->getUnsignedLongData(0,KEY_COL));
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

        FUNCTION_EXIT;
    }

	std::string PaDvaMessagePrivateVersionHelper::getLocalDatabaseName()
    {
		std::string localDatabaseName;
		if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
		{
			throw DatabaseException("db-connection-file not set");
		}
		try
		{
			DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			DataConnections connections = connectionStrings.getConnectionList(Pa_Cd, Write);
			localDatabaseName = connections[0].first;  
		}
		catch(...) // most likely a file not found error
		{
			throw DatabaseException("Unable to find database connection infomation");
		}
	return localDatabaseName;
	}
} // closes TA_Base_Core

