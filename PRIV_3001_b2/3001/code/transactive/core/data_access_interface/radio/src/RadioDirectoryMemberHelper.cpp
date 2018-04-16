/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioDirectoryMemberHelper.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Implementation of the RadioDirectoryMemberHelper class.
  */

#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/radio/src/RadioDirectoryMemberHelper.h"

namespace
{
    //
    // Constants
    //
    
    // Table name
    static const std::string RA_DIRECTORYMEMBER_TBL  = "ra_directory_member";
	
    // Column names in RA_TEXTMESSAGE_INBOX
    static const std::string PARENT_COL              = "PARENT";
    static const std::string RESOURCETYPE_COL        = "RESOURCE_TYPE";
    static const std::string RESOURCEID_COL          = "RESOURCE_ID";
    // Used for data validation
    static const unsigned long  INVALID_KEY          =  -1;
    static const char INVALID_CHAR                   = ' ';
}

using namespace TA_Base_Core;
namespace TA_IRS_Core
{

    //
    // Constructors and destructor
    //
    
    RadioDirectoryMemberHelper::RadioDirectoryMemberHelper()
    :   m_isNew(true),
        m_isValidData(false),
        m_parent(INVALID_KEY),
        m_resourceType(INVALID_CHAR),
        m_resourceId(INVALID_KEY)
    {
        FUNCTION_ENTRY("RadioDirectoryMemberHelper ctor()");
        FUNCTION_EXIT;
    }

    RadioDirectoryMemberHelper::RadioDirectoryMemberHelper(const unsigned long row, TA_Base_Core::IData& data)
    :   m_isNew(false),
        m_isValidData(false),
        m_parent(INVALID_KEY),
        m_resourceType(INVALID_CHAR),
        m_resourceId(INVALID_KEY)
    {
        FUNCTION_ENTRY("RadioDirectoryMemberHelper ctor(const unsigned long row, TA_Base_Core::IData& data)");
        reloadUsing(row, data);
        FUNCTION_EXIT;    
    }

    RadioDirectoryMemberHelper::RadioDirectoryMemberHelper(const unsigned long parent, const char resource_type, const unsigned long resource_id)
    :   m_isNew(true),
        m_isValidData(false),
        m_parent(parent),
        m_resourceType(resource_type),
        m_resourceId(resource_id)
    {
        FUNCTION_ENTRY("RadioDirectoryMemberHelper ctor(const unsigned long parent, const char resource_type, const unsigned long resource_id)");
        FUNCTION_EXIT;
    }

    RadioDirectoryMemberHelper::RadioDirectoryMemberHelper(const RadioDirectoryMemberHelper& theRadioDirectoryMemberHelper)
    :   m_isNew(true),
        m_isValidData(false),
        m_parent(theRadioDirectoryMemberHelper.m_parent),
        m_resourceType(theRadioDirectoryMemberHelper.m_resourceType),
        m_resourceId(theRadioDirectoryMemberHelper.m_resourceId)
    {
        FUNCTION_ENTRY("RadioDirectoryMemberHelper copy ctor");
        FUNCTION_EXIT;
    }

    RadioDirectoryMemberHelper::RadioDirectoryMemberHelper(const unsigned long key)
    :   m_isNew(false),
        m_isValidData(false),
        m_parent(key),
        m_resourceType(INVALID_CHAR),
        m_resourceId(INVALID_KEY)
    {
        FUNCTION_ENTRY("RadioDirectoryMemberHelper ctor(const unsigned long)");
        FUNCTION_EXIT;
    }

    RadioDirectoryMemberHelper::~RadioDirectoryMemberHelper()
    {
        FUNCTION_ENTRY("~RadioDirectoryMemberHelper");
        FUNCTION_EXIT;
    }

    //
    // RadioDirectoryMemberHelper methods
    //
    unsigned long RadioDirectoryMemberHelper::getParent()
    {
        FUNCTION_ENTRY("getParent");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_parent;
    }

    char RadioDirectoryMemberHelper::getResourceType()
    {
        FUNCTION_ENTRY("getResourceType");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_resourceType;
    }

    unsigned long RadioDirectoryMemberHelper::getResourceId()
    {
        FUNCTION_ENTRY("getResourceId");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_resourceId;
    }


    void RadioDirectoryMemberHelper::writeData()
    {
        FUNCTION_ENTRY("writeData");

        std::vector<std::string> fieldNames;

        // validate each field
        if (m_parent == INVALID_KEY)
        {
            fieldNames.push_back(PARENT_COL);
        }
        if (m_resourceType == INVALID_CHAR)
        {
            fieldNames.push_back(RESOURCETYPE_COL);
        }
        if (m_resourceId == INVALID_KEY)
        {
            fieldNames.push_back(RESOURCEID_COL);
        }

        // throw exception if any of the field is invalid
        if (! fieldNames.empty())
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Radio Directory member data not fully specified. " \
                                                              "Radio Directory member cannot be written to database.",
                                                              fieldNames));
        }

        // update the database...
        if (m_isNew)
        {
            addNewDirectoryMember();
        }
        else
        {
            modifyExistingDirectoryMember();
        }

        // after writing the data, what we hold is now valid
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }

    void RadioDirectoryMemberHelper::deleteData()
    {
        FUNCTION_ENTRY("deleteData");

        TA_ASSERT(! m_isNew, "This Radio Directory member does not yet exist in the database and therefore cannot be deleted."); 

       /* std::ostringstream sql;
        sql << "delete " << RA_DIRECTORYMEMBER_TBL << " where " << PARENT_COL << " = " << m_parent;*/

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_MEMBER_DELETE_68001, m_parent);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_MEMBER_DELETE_68001, m_parent);

        databaseConnection->executeModification( strSql );

        FUNCTION_EXIT;
    }

    unsigned long RadioDirectoryMemberHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_parent;//assuming the parent is the key
    }

    void RadioDirectoryMemberHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // This method cannot be called until the key has been set
        TA_ASSERT(! m_isNew, "Attempted to call invalidate() on a new Radio Directory Member.");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

    //
    // Private methods
    //

    void RadioDirectoryMemberHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // Ensure the Radio directory member has already been written to the database. It doesn't make
        // any sense to reload an Radio TextMessage that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This Radio Directory member does not yet exist in the database. " 
                            "Therefore it cannot be reloaded.");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }

        // create the SQL string to retrieve the data of the radio TextMessage based upon the key
      /*  std::ostringstream sql;
        sql << "select " 
                    << PARENT_COL << ", "
                    << RESOURCETYPE_COL << ", "
                    << RESOURCEID_COL << " from " 
                    << RA_DIRECTORYMEMBER_TBL << " where " 
                    << PARENT_COL << " = " << m_parent;*/
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_MEMBER_SELECT_68002, m_parent);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_MEMBER_SELECT_68002, m_parent);
             
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PARENT_COL);
        columnNames.push_back(RESOURCETYPE_COL);
		columnNames.push_back(RESOURCEID_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

        if (0 == data->getNumRows()) 
        {
            // radio TextMessage record not found
            std::ostringstream reason;
            reason << "No record found for Radio Directory member with key = " << m_parent << std::endl;

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                     TA_Base_Core::DataException::NO_VALUE, "Key Id" ) );
        }

        TA_ASSERT(data->getNumRows() == 1, "Invalid number of Radio Directory member records."); 
                
        reloadUsing(0, *data);

        FUNCTION_EXIT;  
    }

    void RadioDirectoryMemberHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_parent = data.getUnsignedLongData(row, PARENT_COL);        
        m_resourceId = data.getUnsignedLongData(row, RESOURCEID_COL);
        std::string str;
        str = data.getStringData(row, RESOURCETYPE_COL);
        m_resourceType = str.at(0);

        // Need to record that we now have valid data
        m_isValidData = true;
        
        FUNCTION_EXIT;
    }

    void RadioDirectoryMemberHelper::addNewDirectoryMember()
    {
        FUNCTION_ENTRY("addNewDirectoryMember");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
        // can proceed with adding the new record
        {       
          /*  std::ostringstream sql;
            sql << "insert into " 
                        << RA_DIRECTORYMEMBER_TBL 
                        << " (" 
                            << PARENT_COL << ", " 
                            << RESOURCEID_COL << ", "
                            << RESOURCETYPE_COL << ") "
                << "values ("
                        << m_parent << ", " 
                        << m_resourceId << ", '"
                        << m_resourceType << "')";*/
//			std::string strSql = databaseConnection->prepareSQLStatement( RA_DIRECTORY_MEMBER_INSERT_68004,m_parent,m_resourceId,m_resourceType );
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  RA_DIRECTORY_MEMBER_INSERT_68004,m_parent,m_resourceId,m_resourceType );
        
            databaseConnection->executeModification( strSql );
        }
        FUNCTION_EXIT;    
    }

    void RadioDirectoryMemberHelper::modifyExistingDirectoryMember()
    {
        FUNCTION_ENTRY("modifyExistingDirectoryMember");

        validateKeyExistence();
        
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
        /*std::ostringstream sql;
        sql << "update " 
                    << RA_DIRECTORYMEMBER_TBL << std::endl
            << "set "
                    << PARENT_COL  << " = " << m_parent << ", " << std::endl
                    << RESOURCEID_COL  << " = " << m_resourceId << ", "  << std::endl
                    << RESOURCETYPE_COL  << " = '" << m_resourceType << "'" << std::endl
            << "where " 
                    << PARENT_COL << " = " << m_parent;*/
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_MEMBER_UPDATE_68005, m_parent, 
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_MEMBER_UPDATE_68005, m_parent,
			m_resourceId, m_resourceType, m_parent);

        databaseConnection->executeModification( strSql );

        FUNCTION_EXIT;
    }

    void RadioDirectoryMemberHelper::validateKeyExistence()
    {
         // check if radio TextMessage id is in use
       /* std::ostringstream sql;  
        sql << "select " 
                    << PARENT_COL << std::endl
            <<"from " 
                    << RA_DIRECTORYMEMBER_TBL << std::endl
            << "where " 
                    << PARENT_COL << " = " << m_parent << std::endl;*/		

        // Set up the vector to pass to executeQuery
        std::vector<std::string> columnNames;
        columnNames.push_back(PARENT_COL);

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_DIRECTORY_MEMBER_SELECT_68003, m_parent);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_DIRECTORY_MEMBER_SELECT_68003, m_parent);
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

        if (1 != data->getNumRows())
        {
            // cannot find our key in the db

            std::ostringstream reason;
			reason  << "A record with primary key " << m_parent << " does not exist.";

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                     TA_Base_Core::DataException::NOT_UNIQUE, "Primary Key"));
        }
    }
}
