/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsPlatformHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * AtsPlatformHelper is an object that is held by AtsPlatform and ConfigAtsPlatform objects. 
  * It contains all data used by AtsPlatforms, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsPlatform and ConfigAtsPlatform.
  */


#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4503)	// decorated name length exceeded, name was truncated
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/ats/src/AtsPlatformHelper.h"
#include "core/data_access_interface/ats/src/AtsPlatformAccessFactory.h"
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

    static const std::string KEY_COL		= "ATPLAT_ID";
	static const std::string PLATFORMID_COL	= "PLATFORM_ID";
    static const std::string STATIONID_COL	= "STATION_ID";
	static const std::string NAME_COL		= "PLATFORM_NAME";
    static const std::string TIS_PLATFORMID_COL	= "TIS_PLATFORM_ID";
	

    static std::string toUpper(const std::string& in)
    {
        std::string out(in);
        std::transform(out.begin(), out.end(), out.begin(), toupper);
        return out;
    }



	AtsPlatformHelper::AtsPlatformHelper( const AtsPlatformHelper& theAtsPlatformHelper)
		: m_idKey(-1),
		  m_platformId(theAtsPlatformHelper.m_platformId),
		  m_stationId(theAtsPlatformHelper.m_stationId),
          m_tisPlatformId(theAtsPlatformHelper.m_tisPlatformId),
		  m_name(theAtsPlatformHelper.m_name),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    AtsPlatformHelper::AtsPlatformHelper(const unsigned long idKey)
        : m_idKey(idKey),
		  m_platformId(-1),
		  m_stationId(-1),
          m_tisPlatformId(-1),
		  m_name(""),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    AtsPlatformHelper::AtsPlatformHelper()
        : m_idKey(-1),
		  m_platformId(-1),
		  m_stationId(-1),
          m_tisPlatformId(-1),
		  m_name(""),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    AtsPlatformHelper::AtsPlatformHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_idKey(-1),
		  m_platformId(-1),
		  m_stationId(-1),
          m_tisPlatformId(-1),
		  m_name(""),
          m_isValidData(false),
          m_isNew(false)
    {
        m_idKey = data.getUnsignedLongData(row, KEY_COL);

        reloadUsing(row, data);
    }


    AtsPlatformHelper::~AtsPlatformHelper()
    {
    }

	unsigned long AtsPlatformHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");

        FUNCTION_EXIT;
        return m_idKey;
    }

    unsigned long AtsPlatformHelper::getStationId()
    {
        FUNCTION_ENTRY("getStationId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_stationId;
    }


    void AtsPlatformHelper::setStationId(unsigned long stationId)
    {
        FUNCTION_ENTRY("setStationId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_stationId = stationId;
        FUNCTION_EXIT;
    }

    unsigned long AtsPlatformHelper::getPlatformId()
    {
        FUNCTION_ENTRY("getPlatformId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_platformId;
    }

    void AtsPlatformHelper::setPlatformId(unsigned long id)
    {
        FUNCTION_ENTRY("setPlatformId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_platformId = id;
        FUNCTION_EXIT;
    }

    unsigned char AtsPlatformHelper::getTisPlatformId()
    {
        FUNCTION_ENTRY("getTisPlatformId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_tisPlatformId;
    }

    void AtsPlatformHelper::setTisPlatformId(unsigned char id)
    {
        FUNCTION_ENTRY("setTisPlatformId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        // make sure the ID is not greater than 9 (or it wont save to the database)
        if (id > 9)
        {
            TA_THROW( TA_Base_Core::DataException("The TIS Platform Id must be between 1 and 9",TA_Base_Core::DataException::INVALID_VALUE,"Tis Platform Id" ) );
        }

        m_tisPlatformId = id;
        FUNCTION_EXIT;
    }

	std::string AtsPlatformHelper::getName()
    {
        FUNCTION_ENTRY("getName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_name;
    }


    void AtsPlatformHelper::setName(const std::string& name)
    {
        FUNCTION_ENTRY("setPlatformId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_name = name;
        FUNCTION_EXIT;
    }


    void AtsPlatformHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new AtsPlatform");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void AtsPlatformHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the AtsPlatform has already been written to the database. It doesn't make
        // any sense to reload an AtsPlatform that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This AtsPlatform does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);


        // create the SQL string to retrieve the data of the AtsPlatform
        // based upon the key
       /* std::ostringstream sql;
		sql << "select " << PLATFORMID_COL << ", " << STATIONID_COL << ", " 
			<< NAME_COL << ", " << TIS_PLATFORMID_COL << " from AT_PLATFORM where " 
            << KEY_COL << " = " << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_PLATFORM_STD_SELECT_49505, m_idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_PLATFORM_SELECT_49505, m_idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_PLATFORM_SELECT_49505, m_idKey);
 
        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(PLATFORMID_COL);
		columnNames.push_back(STATIONID_COL);
		columnNames.push_back(NAME_COL);
        columnNames.push_back(TIS_PLATFORMID_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // AtsPlatform record not found   
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for Platform Key = " << m_idKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"MSS Zone Key" ) );
        }
        
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_platformId = data->getUnsignedLongData( 0, PLATFORMID_COL );
        m_stationId = data->getUnsignedLongData( 0, STATIONID_COL );
        m_name = data->getStringData( 0, NAME_COL );
        m_tisPlatformId = static_cast<unsigned char> (data->getUnsignedLongData( 0, TIS_PLATFORMID_COL ));

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsPlatformHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_platformId = data.getUnsignedLongData( row, PLATFORMID_COL );
        m_stationId = data.getUnsignedLongData( row, STATIONID_COL );
        m_name = data.getStringData( row, NAME_COL );
        m_tisPlatformId = static_cast<unsigned char> (data.getUnsignedLongData( row, TIS_PLATFORMID_COL ));

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsPlatformHelper::writeAtsPlatformData() throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeAtsPlatformData" );     

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
		if (m_platformId == -1)
        {
            fieldNames.push_back(PLATFORMID_COL);
        }
		if (m_stationId == -1)
        {
            fieldNames.push_back(STATIONID_COL);
        }
        if (m_name.empty())
        {
            fieldNames.push_back(NAME_COL);
        }
        if (m_tisPlatformId == -1)
        {
            fieldNames.push_back(TIS_PLATFORMID_COL);
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("AtsPlatform data not fully specified. AtsPlatform cannot be written to database",
                                                         fieldNames));
        }

        // Write the AtsPlatform to the database

        // Need to check if this is a new AtsPlatform or an existing AtsPlatform - 
        // as a new AtsPlatform will be inserted, while an existing AtsPlatform will be updated
        if (m_isNew) // This is a new AtsPlatform
        {
            addNewAtsPlatform();
        }
        else // This is an existing AtsPlatform
        {
            modifyExistingAtsPlatform();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void AtsPlatformHelper::deleteAtsPlatform(bool cascade)
    {
        FUNCTION_ENTRY("deleteAtsPlatform");

        TA_ASSERT(!m_isNew, "This AtsPlatform does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
		//int nDbType = databaseConnection->GetDbServerType();
        //std::ostringstream sql;
		//std::string strSql;
		TA_Base_Core::SQLStatement strSql;
        
        if (!cascade)
        {
			// Check for references in AT_PSD_ENTITY_MAP
            std::vector<std::string> columnNames;
            columnNames.push_back("ATPEMA_ID");
			TA_Base_Core::IData* data = NULL;

			/*sql << "select A.ATPEMA_ID FROM AT_PSD_ENTITY_MAP A,AT_PLATFORM B ";
			sql << "where  A.PLATFORM_key=B.ATPLAT_ID  AND B.PLATFORM_ID =  " << m_platformId;*/
			//strSql = defPrepareSQLStatement(nDbType, AT_PSD_ENTITY_MAP_STD_SELECT_49002, m_platformId);
//			strSql = databaseConnection->prepareSQLStatement(AT_PSD_ENTITY_MAP_SELECT_49002, m_platformId);

		databaseConnection->prepareSQLStatement(strSql, AT_PSD_ENTITY_MAP_SELECT_49002, m_platformId);

            data = databaseConnection->executeQuery(strSql,columnNames);

            if (0 != data->getNumRows())
            {
                delete data;
                data = NULL;

                TA_THROW(TA_Base_Core::DataException("A reference to this AT_MSSZONE exists in the AT_PSD_ENTITY_MAP table",
                    TA_Base_Core::DataException::CANNOT_DELETE,"AT_PSD_ENTITY_MAP"));
            }

            delete data;
            data = NULL;
        }

        // Good to go...
        
        /*sql.str("");
        sql << "delete AT_PLATFORM where " << KEY_COL << " = " << m_idKey;*/
		//strSql = defPrepareSQLStatement(nDbType, AT_PLATFORM_STD_DELETE_49901, m_idKey);
//		strSql = databaseConnection->prepareSQLStatement(AT_PLATFORM_DELETE_49901, m_idKey);

		databaseConnection->prepareSQLStatement(strSql, AT_PLATFORM_DELETE_49901, m_idKey);
        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void AtsPlatformHelper::modifyExistingAtsPlatform() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingAtsPlatform");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "AtsPlatform id=%ul already exists. It's data will be updated.",m_idKey);
    
        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        /*std::ostringstream sql;
        sql << "update AT_PLATFORM set " 
			<< PLATFORMID_COL << " = "
            << m_platformId << ", " 
			<< STATIONID_COL << " = "
            << m_stationId << ", " 
            << NAME_COL << " = '"
            << (databaseConnection->escapeInsertString(m_name)).c_str() << "', " 
            << TIS_PLATFORMID_COL << " = "
            << static_cast<unsigned long>(m_tisPlatformId) << " " 
            << "where " << KEY_COL << " = " << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_PLATFORM_STD_UPDATE_49651, m_platformId,
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_PLATFORM_UPDATE_49651, m_platformId,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_PLATFORM_UPDATE_49651, m_platformId,
			m_stationId,databaseConnection->escapeInsertString(m_name),static_cast<unsigned long>(m_tisPlatformId), m_idKey);

        // get a connection to the database
        databaseConnection->executeModification(strSql);

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void AtsPlatformHelper::addNewAtsPlatform() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewAtsPlatform");

		unsigned long newKey = getNextKey();

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "AtsPlatform %ul is a new AtsPlatform. It will be added to the database.",newKey);

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);

		/*std::ostringstream sql;
        sql << "insert into AT_PLATFORM (" 
            << KEY_COL << ", "
			<< PLATFORMID_COL << ", " 
            << STATIONID_COL << ", " 
            << NAME_COL << ", " 
			<< TIS_PLATFORMID_COL << ") "
            << "values ("
            << newKey << ", " 
			<< m_platformId << ", " 
            << m_stationId << ", '" 
			<< (databaseConnection->escapeInsertString(m_name)).c_str() << "', "
            << static_cast<unsigned long>(m_tisPlatformId) << ")" ;*/

//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_PLATFORM_STD_INSERT_49801, newKey, m_platformId,
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_PLATFORM_INSERT_49801, newKey, m_platformId,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_PLATFORM_INSERT_49801, newKey, m_platformId,
			m_stationId, databaseConnection->escapeInsertString(m_name), static_cast<unsigned long>(m_tisPlatformId));
        databaseConnection->executeModification(strSql);

		m_idKey = newKey; 
        
        FUNCTION_EXIT;
    }

    unsigned long AtsPlatformHelper::getNextKey()
	{
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// Get a suitable primary key generated for this new entry
		/*std::ostringstream sql;
        sql << "SELECT ATPLAT_SEQ.NEXTVAL FROM DUAL";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_PLATFORM_Oracle_SELECT_49551);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_PLATFORM_SELECT_49551);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_PLATFORM_SELECT_49551);

        std::vector<std::string> columnNames;
        columnNames.push_back("NEXTVAL");

        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        try 
        {
            // Should always be single row result
            TA_ASSERT(data->getNumRows() == 1, "Invalid result from getNextSequenceNumber");

            // Try and retrieve the value of auto increment ID
            unsigned long result = data->getUnsignedLongData(0, "NEXTVAL");

            // Clean up
            delete data;
            data = NULL;

            return result;
        }
        catch (TA_Base_Core::DataException&) 
        {
            // Clean up before rethrowing
            delete data;
            data = NULL;

            throw;
        }
	}

} // closes TA_Core

