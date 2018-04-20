/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/PaStationHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * PaStationHelper is an object that is held by PaStation and ConfigPaStation objects. 
  * It contains all data used by PaStations, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaStation and ConfigPaStation.
  */


#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/pa/src/PaStationHelper.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/pa/src/PaStationAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/ValueNotSetException.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
    static const std::string PA_STATION_TABLE_NAME = "PA_STATION";
    static const std::string PA_STATION_TABLE_SHORT_NAME = "PASTAT";
    static const std::string KEY_COL = "PASTAT_ID";
    static const std::string LOCATIONKEY_COL = "LOCATIONKEY";

    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";
    static const std::string DATECREATED_COL = "DATE_CREATED";

	PaStationHelper::PaStationHelper( const PaStationHelper& thePaStationHelper)
		: m_key(thePaStationHelper.m_key),
		  m_locationKey(thePaStationHelper.m_locationKey),          
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    PaStationHelper::PaStationHelper(const unsigned long key)
        : m_key(key),		  
		  // m_locationKey(0),          
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    PaStationHelper::PaStationHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_key(data.getUnsignedLongData(row, KEY_COL)),		  
		  //m_locationKey(0),          
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        reloadUsing(row, data);
    }


    PaStationHelper::PaStationHelper()
        : //m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
		  //m_locationKey(0),          
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    PaStationHelper::~PaStationHelper()
    {
    }
  

    unsigned long PaStationHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        // Following the reload of data, all values should have been set
        try
        {
            if (isNew() && !m_key.hasBeenSet())
            {
                // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
                // value for an unset key (otherwise, if loaded from DB, it must be set)
                return TA_Base_Core::DatabaseKey::getInvalidKey();                
            }

            return m_key.getValue();
        }
        catch (::TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            TA_ASSERT(false, "m_key value not set after reload");
            throw;
        }

    }

    unsigned long PaStationHelper::getLocationKey()
    {
        FUNCTION_ENTRY("getLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        // Following the reload of data, all values should have been set
        try
        {
            if (isNew() && !m_locationKey.hasBeenSet())
            {
                // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
                // value for an unset key (otherwise, if loaded from DB, it must be set)
                return TA_Base_Core::DatabaseKey::getInvalidKey();                
            }

            return m_locationKey.getValue();
        }
        catch (::TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            TA_ASSERT(false, "m_locationKey value not set after reload");
            throw;
        }

        FUNCTION_EXIT;
    }

    void PaStationHelper::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_locationKey.setValue(locationKey);
        
        FUNCTION_EXIT;
    }


    void PaStationHelper::setKey(const unsigned long id)
    {
        FUNCTION_ENTRY("setKey" );
        TA_ASSERT(m_isNew,"An existing PA Station ID cannot be modified.");

        m_key.setValue(id);

        FUNCTION_EXIT;
        return;
    }


    time_t PaStationHelper::getDateCreated()
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


    time_t PaStationHelper::getDateModified()
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



    void PaStationHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PaStation");

        m_isValidData = false;
        m_modifiedKey.resetValue();

        FUNCTION_EXIT;
    }


    const DatabaseQueryHelper::QueryData 
        PaStationHelper::getBasicQueryData()
    {
        DatabaseQueryHelper::QueryData result;

        result.primaryKeyCol    = KEY_COL;
	    result.tableName        = PA_STATION_TABLE_NAME;

        result.columnNames.push_back(KEY_COL);
        result.columnNames.push_back(LOCATIONKEY_COL);
        result.columnNames.push_back(DATECREATED_COL);
        result.columnNames.push_back(DATEMODIFIED_COL);

        result.statement = 
            DatabaseQueryHelper::createBasicSelectStatement(result.columnNames, 
                                                            result.tableName);

        return result;
    }


    void PaStationHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_locationKey.setValue(data.getUnsignedLongData( row, LOCATIONKEY_COL ));
        m_key.setValue(data.getUnsignedLongData( row, KEY_COL ));

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


    void PaStationHelper::writePaStationData() //throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writePaStationData" );     

        // Check ALL values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if (!m_locationKey.hasBeenSet())
        {
            fieldNames.push_back(LOCATIONKEY_COL);
        }
        if (!m_key.hasBeenSet())
        {
            fieldNames.push_back(KEY_COL);
        }
        
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException(
                "PaStation data not fully specified. PaStation cannot be written to database",
                fieldNames));
        }


        // Write the PaStation to the database

        try 
        {
            // Need to check if this is a new PaStation or an existing PaStation - 
            // as a new PaStation will be inserted, while an existing PaStation will be updated
            if (m_isNew) // This is a new PaStation
            {
                addNewPaStation();
            }
            else // This is an existing PaStation
            {
                modifyExistingPaStation();
            }
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "PaStation data not fully specified. PaStation cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void PaStationHelper::deletePaStation(bool cascade)
    {
        FUNCTION_ENTRY("deletePaStation");

        TA_ASSERT(!m_isNew, "This PaStation does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);
        std::ostringstream sql;
        
        if (!cascade)
        {
            std::string info;
            
            // If don't want cascade, must check for foreign links
            if (DatabaseQueryHelper::checkIfExistLinksToTableRow(
                                            PA_STATION_TABLE_NAME,
                                            KEY_COL,
                                            m_key.getValue(),
                                            info))
            {
                TA_THROW(TA_Base_Core::DataException(info.c_str(), 
                                    TA_Base_Core::DataException::CANNOT_DELETE, 
                                    PA_STATION_TABLE_NAME.c_str()));
            }
        }

        // Good to go...
        sql.str("");
        sql << "delete " << PA_STATION_TABLE_NAME << " where " << KEY_COL << " = " << m_key.getValue();

        databaseConnection->executeModification(sql.str());

        FUNCTION_EXIT;
    }
   

    void PaStationHelper::modifyExistingPaStation() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingPaStation");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "PA Station %u already exists. It's data will be updated.", m_key.getValue());

        // Once created, the Key cannot be changed.  
        validateLocationKeyExistence();
        validateLocationUniqueness();
        

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        std::ostringstream sql;

        unsigned long updatedKeyVal = m_key.getValue();

        // If a modified key has been specified, then we update the key column
        if (m_modifiedKey.hasBeenSet())
        {
            updatedKeyVal = m_modifiedKey.getValue();
        }

        sql << "update " << PA_STATION_TABLE_NAME << " set " 
            << LOCATIONKEY_COL << " = " << m_locationKey.getValue() << ", "
            << KEY_COL << " = " << updatedKeyVal
            << " where " << KEY_COL << " = " << m_key.getValue();        

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);
        databaseConnection->executeModification(sql.str().c_str());

        // Update the internal key value, as it may have changed by the update request
        m_key.setValue(updatedKeyVal);

        // now need to update the date fields
        sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
            << "from " << PA_STATION_TABLE_NAME << " where " << KEY_COL << " = " << m_key.getValue();

        std::vector<std::string> columnNames;
        columnNames.push_back(DATECREATED_COL);
        columnNames.push_back(DATEMODIFIED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA Zone record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA Station key = " << m_key.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Station key" ) );
        }

        try
        {
            // Clear the modified key flag, as now current key will reflect modified key
            // (if in fact the modified key was set)
            m_modifiedKey.resetValue();

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


    void PaStationHelper::addNewPaStation() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewPaStation");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "PA Station %u is a new PaStation. It will be added to the database.",m_key.getValue());

        validateKeyAbsence();
        validateLocationKeyExistence();
        validateLocationUniqueness();
        


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);

        std::ostringstream sql;
        sql << "insert into " << PA_STATION_TABLE_NAME << " (" 
            << KEY_COL << ", "
            << LOCATIONKEY_COL << ") "
            << "values (" << m_key.getValue() << ","
            << m_locationKey.getValue() << ")";

        databaseConnection->executeModification(sql.str());

        sql.str("");

        // Obtain the newly created record
        sql << "select " 
            << KEY_COL << ", "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from " << PA_STATION_TABLE_NAME << " where "
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
            // PA Zone record not found            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA Station key = " << m_key.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Station key" ) );
        }

        // Retrieve the pkey as an unsigned long. There should only be a single returned item 
        // (as any other circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
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


    void PaStationHelper::validateLocationKeyExistence() //throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // First check that the LOCATIONKEY already exists.
        //
        ////////////////////////////////////////////////////////////////////////////
        
        std::ostringstream sql;
        sql << "select PKEY from LOCATION where PKEY = " << m_locationKey.getValue();
        
        std::vector<std::string> columnNames;
        columnNames.clear();
        columnNames.push_back("PKEY");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        if (0 == data->getNumRows()) 
        {
            // Location key not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for Location key = " << m_locationKey.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Location key" ) );
        }

        delete data;
        data = NULL;
    }

    void PaStationHelper::validateKeyExistence() //throw(TA_Base_Core::TransactiveException)
    {

        std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from " << PA_STATION_TABLE_NAME 
            << " where " << KEY_COL << " = " << m_key.getValue();

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
                  
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);        
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        if (0 == data->getNumRows()) 
        {
            // key not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA Station key = " << m_key.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Station key" ) );
        }

        delete data;
        data = NULL;
    }

    void PaStationHelper::validateKeyAbsence() //throw(TA_Base_Core::TransactiveException)
    {

        std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from " << PA_STATION_TABLE_NAME 
            << " where " << KEY_COL << " = " << m_key.getValue();

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
                  
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);        
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        if (0 != data->getNumRows()) 
        {
            // key found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "Existing data found for PA Station key = " << m_key.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NOT_UNIQUE,"PA Station key" ) );
        }

        delete data;
        data = NULL;
    }

    void PaStationHelper::validateLocationUniqueness() 
    {   
        std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from " << PA_STATION_TABLE_NAME 
            << " where " << LOCATIONKEY_COL << " = " << m_locationKey.getValue();

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
                  
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);        
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        if (0 != data->getNumRows()) 
        {        
            unsigned long duplicateItemKey = data->getUnsignedLongData(0, KEY_COL);

            // Ignore ourself..
            if (duplicateItemKey != m_key.getValue())
            {
                std::ostringstream str;
                str << "Attempting to write Pa Station record that doesn't "
                    << "have unique location (existing duplicate has pKey="
                    << duplicateItemKey
                    << ")";
    

                delete data;
                data = NULL;

                TA_THROW( TA_Base_Core::DataException(str.str().c_str(), 
                            TA_Base_Core::DataException::NOT_UNIQUE,
                            "LocationKey") );
            }
        }

        delete data;
        data = NULL;

    }


    void PaStationHelper::setKeyToUse(unsigned long keyToUse)
    {
        FUNCTION_ENTRY("setKeyToUse" );

        if (!isNew())
        {
            // Not setting for a new object (key already allocated)
            m_modifiedKey.setValue(keyToUse);
            return;
        }

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_key.setValue(keyToUse);
        FUNCTION_EXIT;
    }


    unsigned long PaStationHelper::getModifiedKey()
    {
        if (m_modifiedKey.hasBeenSet())
        {
            return m_modifiedKey.getValue();
        }
        else
        {
            return getKey();
        }
    }


    bool PaStationHelper::isKeyDuplicate(unsigned long keyToUse)
    {
        if (TA_Base_Core::DatabaseKey::isInvalidKey(keyToUse))
        {
            return false;
        }

        std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from " << PA_STATION_TABLE_NAME 
            << " where " << KEY_COL << " = " << keyToUse;
            
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
                  
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);        
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(sql.str(), columnNames));

        if (0 != data.get() && 0 == data->getNumRows()) 
        {   
            // Must not be a duplicate, no rows returned
            return false;
        }
        else
        {
            // Otherwise have to assume it's a duplicate
            return true;
        }
    }


} // closes TA_IRS_Core

