/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/stis_manager/src/TIRatisTagHelper.cpp $
  * @author:  chunzhong
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * TiRatisTagHelper is an object that is held by TiRatisTag and ConfigTiRatisTag objects. 
  * It contains all data used by TiRatisTags, and manipulation
  * methods for the data. It helps avoid re-writing code for both TiRatisTag and ConfigTiRatisTag.
  */


#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/stis_manager/src/TiRatisTagHelper.h"
#include "core/data_access_interface/stis_manager/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/stis_manager/src/TiRatisTagAccessFactory.h"
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
    static const std::string TI_RATIS_TAG_TABLE_NAME		= "TI_RATIS_TAG";
    static const std::string TI_RATIS_TAG_TABLE_SHORT_NAME	= "TIRATISTAG";

	static const std::string KEY_SEQ		= "TIRTAG_SEQ";

    static const std::string KEY_COL		= "TIRTAG_ID";
    static const std::string TAGGROUP_COL	= "TAG_GROUP";
	static const std::string DETAIL_COL		= "DETAIL";

	TiRatisTagHelper::TiRatisTagHelper( const TiRatisTagHelper& theTiRatisTagHelper)
		: m_key(theTiRatisTagHelper.m_key),
          m_isValidData(false),
          m_isNew(true)
    {	  
		  m_tagGroup = theTiRatisTagHelper.m_tagGroup;
		  m_detail	 = theTiRatisTagHelper.m_detail;
		  m_ratisTag = theTiRatisTagHelper.m_ratisTag;
	}


    TiRatisTagHelper::TiRatisTagHelper(const unsigned long key)
        : m_key(key),
		  m_tagGroup(""),
		  m_detail(""),
		  m_ratisTag(""),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    TiRatisTagHelper::TiRatisTagHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_key(data.getUnsignedLongData(row, KEY_COL)),
          m_isValidData(false),
          m_isNew(false)
    {
		m_tagGroup = data.getStringData(row,TAGGROUP_COL);
		m_detail = data.getStringData(row,DETAIL_COL);
		m_ratisTag = getRatisTag();
    }


    TiRatisTagHelper::TiRatisTagHelper()
        : //m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    TiRatisTagHelper::~TiRatisTagHelper()
    {
    }
  

    unsigned long TiRatisTagHelper::getKey()
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

            FUNCTION_EXIT;
            return m_key.getValue();
        }
        catch (::TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            TA_ASSERT(false, "m_key value not set after reload");
            throw;
        }
        FUNCTION_EXIT;
    }


    void TiRatisTagHelper::setKey(const unsigned long id)
    {
        FUNCTION_ENTRY("setKey" );
        TA_ASSERT(m_isNew,"An existing TI Ratis TagID cannot be modified.");

        m_key.setValue(id);

        FUNCTION_EXIT;
        return;
    }


    void TiRatisTagHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new TiRatisTag");

        m_isValidData = false;
        m_modifiedKey.resetValue();

        FUNCTION_EXIT;
    }


    const DatabaseQueryHelper::QueryData TiRatisTagHelper::getBasicQueryData()
    {
        DatabaseQueryHelper::QueryData result;

        result.primaryKeyCol    = KEY_COL;
	    result.tableName        = TI_RATIS_TAG_TABLE_NAME;

        result.columnNames.push_back(KEY_COL);
		result.columnNames.push_back(TAGGROUP_COL);
		result.columnNames.push_back(DETAIL_COL);

        result.statement = DatabaseQueryHelper::createBasicSelectStatement(result.columnNames,result.tableName,false);

        return result;
    }


    void TiRatisTagHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_key.setValue(data.getUnsignedLongData( row, KEY_COL ));

		m_tagGroup = data.getStringData(row,TAGGROUP_COL);

		m_detail   = data.getStringData(row,DETAIL_COL);

		m_ratisTag = m_tagGroup + m_detail;

        m_isValidData = true;

        FUNCTION_EXIT;
    }


    void TiRatisTagHelper::writeTiRatisTagData() //throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeTiRatisTagData" );     

        // Check ALL values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;

        validateGroupDetailPairUniqueness();
        
        if (m_tagGroup.empty())
        {
            fieldNames.push_back(TAGGROUP_COL);
        }

        if (m_detail.empty())
        {
            fieldNames.push_back(DETAIL_COL);
        }

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException(
                "TiRatisTag data not fully specified. TiRatisTag cannot be written to database",
                fieldNames));
        }


        // Write the TiRatisTag to the database
        try 
        {
            // Need to check if this is a new TiRatisTag or an existing TiRatisTag - 
            // as a new TiRatisTag will be inserted, while an existing TiRatisTag will be updated
            if (m_isNew) // This is a new TiRatisTag
            {
                addNewTiRatisTag();
            }
            else // This is an existing TiRatisTag
            {
                modifyExistingTiRatisTag();
            }
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "TiRatisTag data not fully specified. TiRatisTag cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void TiRatisTagHelper::deleteTiRatisTag(bool cascade)
    {
        FUNCTION_ENTRY("deleteTiRatisTag");

        TA_ASSERT(!m_isNew, "This TiRatisTag does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Write);
        std::ostringstream sql;
        
        if (!cascade)
        {
            std::string info;
            
            // If don't want cascade, must check for foreign links
            if (DatabaseQueryHelper::checkIfExistLinksToTableRow(
                                            TI_RATIS_TAG_TABLE_NAME,
                                            KEY_COL,
                                            m_key.getValue(),
                                            info))
            {
                TA_THROW(TA_Base_Core::DataException(info.c_str(), 
                                    TA_Base_Core::DataException::CANNOT_DELETE, 
                                    TI_RATIS_TAG_TABLE_NAME.c_str()));
            }
        }

        // Good to go...
        sql.str("");
        sql << "delete " << TI_RATIS_TAG_TABLE_NAME << " where " << KEY_COL << " = " << m_key.getValue();

        databaseConnection->executeModification(sql.str());

        FUNCTION_EXIT;
    }
   

    void TiRatisTagHelper::modifyExistingTiRatisTag() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingTiRatisTag");

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
           

        sql << "update " << TI_RATIS_TAG_TABLE_NAME << " set " 
            << TAGGROUP_COL << " = '" << (databaseConnection->escapeInsertString(m_tagGroup)).c_str() << "', " 
            << DETAIL_COL << " = '" << (databaseConnection->escapeInsertString(m_detail)).c_str() << "' " 
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


    void TiRatisTagHelper::addNewTiRatisTag() //throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewTiRatisTag");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "TI Ratis Tag is a new TiRatisTag. It will be added to the database.");
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Write);
		const unsigned long newSequenceNumber = (DatabaseQueryHelper::getNextSequenceNumber(databaseConnection,KEY_SEQ));

        m_key.setValue(newSequenceNumber);
        validateKeyAbsence();
       

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        std::ostringstream sql;
        sql << "insert into " << TI_RATIS_TAG_TABLE_NAME << " (" 
            << KEY_COL << ", "
            << TAGGROUP_COL << ", "
			<< DETAIL_COL << ") "
            << "values ("<< m_key.getValue() << ",'"
			<< m_tagGroup << "','"
            << m_detail << "')";

        databaseConnection->executeModification(sql.str());
        

        FUNCTION_EXIT;
    }


    void TiRatisTagHelper::validateKeyExistence() //throw(TA_Base_Core::TransactiveException)
    {

        std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from " << TI_RATIS_TAG_TABLE_NAME 
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

    void TiRatisTagHelper::validateKeyAbsence() //throw(TA_Base_Core::TransactiveException)
    {

        std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from " << TI_RATIS_TAG_TABLE_NAME 
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


    void TiRatisTagHelper::validateGroupDetailPairUniqueness()  //throw(TA_Base_Core::TransactiveException)
    {
        std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from " << TI_RATIS_TAG_TABLE_NAME 
            << " where " << TAGGROUP_COL << " = '" << m_tagGroup << "'"
            << " and " << DETAIL_COL << " = '" << m_detail << "'";

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
                  
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Read);        
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        if (0 != data->getNumRows()) 
        {
            delete data;
            data = NULL;

            TA_THROW( TA_Base_Core::DataException("Attempting to write RATIS Tag", 
                    TA_Base_Core::DataException::NOT_UNIQUE,
                    " RATIS Tag") );
        }

        delete data;
        data = NULL;
    }


    void TiRatisTagHelper::setKeyToUse(unsigned long keyToUse)
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


    unsigned long TiRatisTagHelper::getModifiedKey()
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


    bool TiRatisTagHelper::isKeyDuplicate(unsigned long keyToUse)
    {
        if (TA_Base_Core::DatabaseKey::isInvalidKey(keyToUse))
        {
            return false;
        }

        std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from " << TI_RATIS_TAG_TABLE_NAME 
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


    std::string TiRatisTagHelper::getTagGroupString() const
    {
        // Because of referential integrity constraint on the status entity key,
        // must use null entries where the key is set to invalid
        if (0 == m_tagGroup.length())
        {
            return "null";
        }
        else
        {
            std::ostringstream ostr;
            ostr << "'" 
                 << TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Read)->escapeInsertString(m_tagGroup).c_str()
                 << "'";
            return ostr.str();
        }
    }


    std::string TiRatisTagHelper::getDetailString() const
    {
        // Because of referential integrity constraint on the status entity key,
        // must use null entries where the key is set to invalid
        if (0 == m_detail.length())
        {
            return "null";
        }
        else
        {
            std::ostringstream ostr;
            ostr << "'" 
                 << TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Read)->escapeInsertString(m_detail).c_str()
                 << "'";
            return ostr.str();
        }
	}


    std::string TiRatisTagHelper::getTagGroup()
    {
        FUNCTION_ENTRY("getTagGroup" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_tagGroup;
    }


    std::string TiRatisTagHelper::getDetail()
    {
        FUNCTION_ENTRY("getDetail" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_detail;
    }


    std::string TiRatisTagHelper::getRatisTag()
    {
        FUNCTION_ENTRY("getRatisTag" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        std::ostringstream ostr;
		ostr << m_tagGroup << m_detail;
		m_ratisTag = ostr.str();

        FUNCTION_EXIT;
        return m_ratisTag;
    }


    void TiRatisTagHelper::setTagGroup(const std::string& tagGroup)
    {
        FUNCTION_ENTRY("setTagGroup" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_tagGroup = tagGroup;
        FUNCTION_EXIT;
    }


    void TiRatisTagHelper::setDetail(const std::string& detail)
    {
        FUNCTION_ENTRY("setDetail" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_detail = detail;
        FUNCTION_EXIT;
    }

} // closes TA_IRS_Core

