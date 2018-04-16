/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/stis_manager/src/TiRatisDestinationHelper.cpp $
  * @author:  chunzhong
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * TiRatisDestinationHelper is an object that is held by TiRatisDestination and ConfigTiRatisDestination objects. 
  * It contains all data used by TiRatisDestinations, and manipulation
  * methods for the data. It helps avoid re-writing code for both TiRatisDestination and ConfigTiRatisDestination.
  */


#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/stis_manager/src/TiRatisDestinationHelper.h"
#include "core/data_access_interface/stis_manager/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/stis_manager/src/TiRatisDestinationAccessFactory.h"
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
    static const std::string TI_RATIS_DESTINATION_TABLE	 = "TI_RATIS_DEST";

    static const std::string KEY_COL		 = "TIRDES_ID";

	static const std::string KEY_SEQ		 = "TIRDES_SEQ";

    static const std::string DESTINATION_COL = "DESTINATION";

	TiRatisDestinationHelper::TiRatisDestinationHelper( const TiRatisDestinationHelper& theTiRatisDestinationHelper)
		: m_key(theTiRatisDestinationHelper.m_key),
          m_isValidData(false),
          m_isNew(true)
    {	  
		  m_destination = theTiRatisDestinationHelper.m_destination;
	}


    TiRatisDestinationHelper::TiRatisDestinationHelper(const unsigned long key)
        : m_key(key),
		  m_destination(""),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    TiRatisDestinationHelper::TiRatisDestinationHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_isNew(false)
    {
        reloadUsing(row, data);
    }


    TiRatisDestinationHelper::TiRatisDestinationHelper()
        : m_destination(""),
		  m_isValidData(false),
          m_isNew(true)
    {
    }


    TiRatisDestinationHelper::~TiRatisDestinationHelper()
    {
    }
  

    unsigned long TiRatisDestinationHelper::getKey()
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

    void TiRatisDestinationHelper::setKey(const unsigned long id)
    {
        FUNCTION_ENTRY("setKey" );
        TA_ASSERT(m_isNew,"An existing TI Ratis TagID cannot be modified.");

        m_key.setValue(id);

        FUNCTION_EXIT;
        return;
    }

    void TiRatisDestinationHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new TiRatisDestination");

        m_isValidData = false;
        m_modifiedKey.resetValue();

        FUNCTION_EXIT;
    }


    const DatabaseQueryHelper::QueryData TiRatisDestinationHelper::getBasicQueryData()
    {
        DatabaseQueryHelper::QueryData result;

        result.primaryKeyCol    = KEY_COL;
	    result.tableName        = TI_RATIS_DESTINATION_TABLE;

        result.columnNames.push_back(KEY_COL);
		result.columnNames.push_back(DESTINATION_COL);

        result.statement = DatabaseQueryHelper::createBasicSelectStatement(result.columnNames,result.tableName,false);

        return result;
    }


    void TiRatisDestinationHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.

        m_key.setValue(data.getUnsignedLongData( row, KEY_COL ));

		m_destination = data.getStringData(row,DESTINATION_COL);

        m_isValidData = true;

        FUNCTION_EXIT;
    }


    void TiRatisDestinationHelper::writeTiRatisDestinationData() //throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeTiRatisDestinationData" );     

        // Check ALL values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;

        if (m_destination.empty())
        {
            fieldNames.push_back(DESTINATION_COL);
        }

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException(
                "TiRatisDestination data not fully specified. TiRatisDestination cannot be written to database",
                fieldNames));
        }

        validateDestinationAbsence();
        
        // Write the TiRatisDestination to the database

        try 
        {
            // Need to check if this is a new TiRatisDestination or an existing TiRatisDestination - 
            // as a new TiRatisDestination will be inserted, while an existing TiRatisDestination will be updated
            if (m_isNew) // This is a new TiRatisDestination
            {
                addNewTiRatisDestination();
            }
            else // This is an existing TiRatisDestination
            {
                modifyExistingTiRatisDestination();
            }
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "TiRatisDestination data not fully specified. TiRatisDestination cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void TiRatisDestinationHelper::deleteTiRatisDestination(bool cascade)
    {
        FUNCTION_ENTRY("deleteTiRatisDestination");

        TA_ASSERT(!m_isNew, "This TiRatisDestination does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Write);

        std::ostringstream sql;
        
        if (!cascade)
        {
            std::string info;
            
            // If don't want cascade, must check for foreign links
            if (DatabaseQueryHelper::checkIfExistLinksToTableRow(
                                            TI_RATIS_DESTINATION_TABLE,
                                            KEY_COL,
                                            m_key.getValue(),
                                            info))
            {
                TA_THROW(TA_Base_Core::DataException(info.c_str(), 
                                    TA_Base_Core::DataException::CANNOT_DELETE, 
                                    TI_RATIS_DESTINATION_TABLE.c_str()));
            }
        }

        // Good to go...
        sql.str("");
        sql << "delete " << TI_RATIS_DESTINATION_TABLE << " where " << KEY_COL << " = " << m_key.getValue();

        databaseConnection->executeModification(sql.str());

        FUNCTION_EXIT;
    }
   

    void TiRatisDestinationHelper::modifyExistingTiRatisDestination() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingTiRatisDestination");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "TI Ratis Tag%u already exists. It's data will be updated.", m_key.getValue());

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Write);
        std::ostringstream sql;

        unsigned long updatedKeyVal = m_key.getValue();

        // If a modified key has been specified, then we update the key column
        if (m_modifiedKey.hasBeenSet())
        {
            updatedKeyVal = m_modifiedKey.getValue();
        }
           
        sql << "update " << TI_RATIS_DESTINATION_TABLE << " set " 
            << DESTINATION_COL << " = '" << (databaseConnection->escapeInsertString(m_destination)).c_str() << "' " 
			<< "where " << KEY_COL << " = '" << m_key.getValue() << "'";

        databaseConnection->executeModification(sql.str().c_str());

        // Update the internal key value, as it may have changed by the update request
        m_key.setValue(updatedKeyVal);

        // Clear the modified key flag, as now current key will reflect modified key
        // (if in fact the modified key was set)
            m_modifiedKey.resetValue();

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void TiRatisDestinationHelper::addNewTiRatisDestination() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewTiRatisDestination");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "TI Ratis Destination is a new TiRatisDestination. It will be added to the database.");

        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Read);
		const unsigned long newSequenceNumber = (DatabaseQueryHelper::getNextSequenceNumber(databaseConnection,KEY_SEQ));
        m_key.setValue(newSequenceNumber);
        validateKeyAbsence();

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        std::ostringstream sql;
        sql << "insert into " << TI_RATIS_DESTINATION_TABLE << " (" 
            << KEY_COL << ", "
			<< DESTINATION_COL << ") "
            << "values ("<< m_key.getValue() << ",'"
			<< m_destination <<  "')";

        databaseConnection->executeModification(sql.str());
        

        FUNCTION_EXIT;
    }


    void TiRatisDestinationHelper::validateKeyExistence() //throw(TA_Base_Core::TransactiveException)
    {

        std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from "  << TI_RATIS_DESTINATION_TABLE 
            << " where " << KEY_COL << " = " << m_key.getValue();

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
                  
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Read);
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        if (0 == data->getNumRows()) 
        {
            // key not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for TI Ratis Tagkey = " << m_key.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"TI Ratis Tagkey" ) );
        }

        delete data;
        data = NULL;
    }

    void TiRatisDestinationHelper::validateKeyAbsence() //throw(TA_Base_Core::TransactiveException)
    {

        std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from "  << TI_RATIS_DESTINATION_TABLE 
            << " where " << KEY_COL << " = " << m_key.getValue();

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
                  
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Read);        
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        if (0 != data->getNumRows()) 
        {
            // key found
	        delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "Existing data found for TI Ratis Tag key = " << m_key.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NOT_UNIQUE,"TI Ratis Tagkey" ) );
        }

        delete data;
        data = NULL;
    }

    void TiRatisDestinationHelper::validateDestinationAbsence()
    {
        std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from "  << TI_RATIS_DESTINATION_TABLE 
            << " where " << DESTINATION_COL << " = '" << m_destination <<"'" ;

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
                  
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Read);        
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        if (0 != data->getNumRows()) 
        {
            // key found
	        delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "Existing data found for destination = " << m_destination;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NOT_UNIQUE,"Destination" ) );
        }

        delete data;
        data = NULL;
    }

    void TiRatisDestinationHelper::setKeyToUse(unsigned long keyToUse)
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


    unsigned long TiRatisDestinationHelper::getModifiedKey()
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


    bool TiRatisDestinationHelper::isKeyDuplicate(unsigned long keyToUse)
    {
        if (TA_Base_Core::DatabaseKey::isInvalidKey(keyToUse))
        {
            return false;
        }

        std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from "  << TI_RATIS_DESTINATION_TABLE 
            << " where " << KEY_COL << " = " << keyToUse;
            
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
                  
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Read);
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

    std::string TiRatisDestinationHelper::getDestination()
    {
        FUNCTION_ENTRY("getDestination" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_destination;
    }


    void TiRatisDestinationHelper::setDestination(const std::string& destination)
    {
        FUNCTION_ENTRY("setDestination" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_destination = destination;
        FUNCTION_EXIT;
    }

} // closes TA_IRS_Core

