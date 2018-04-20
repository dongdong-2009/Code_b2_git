/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ats/src/AtsMssZoneHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/12/27 17:54:14 $
  * Last modified by:  $Author: weikun.lin $
  *
  * AtsMssZoneHelper is an object that is held by AtsMssZone and ConfigAtsMssZone objects. 
  * It contains all data used by AtsMssZones, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsMssZone and ConfigAtsMssZone.
  */


#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4503)	// decorated name length exceeded, name was truncated
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/ats/src/AtsMssZoneHelper.h"
#include "core/data_access_interface/ats/src/AtsMssZoneAccessFactory.h"
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

    static const std::string KEY_COL		= "ATMSSZ_ID";
    static const std::string STATIONID_COL	= "STATION_ID";
	static const std::string MSSZONE_COL	= "MSS_ZONE";

    static std::string toUpper(const std::string& in)
    {
        std::string out(in);
        std::transform(out.begin(), out.end(), out.begin(), toupper);
        return out;
    }



	AtsMssZoneHelper::AtsMssZoneHelper( const AtsMssZoneHelper& theAtsMssZoneHelper)
		: m_idKey(-1),
		  m_stationId(theAtsMssZoneHelper.m_stationId),
		  m_mssZone(theAtsMssZoneHelper.m_mssZone),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    AtsMssZoneHelper::AtsMssZoneHelper(const unsigned long idKey)
        : m_idKey(idKey),
		  m_stationId(-1),
		  m_mssZone(""),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    AtsMssZoneHelper::AtsMssZoneHelper()
        : m_idKey(-1),
		  m_stationId(-1),
		  m_mssZone(""),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    AtsMssZoneHelper::AtsMssZoneHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_idKey(-1),
		  m_stationId(-1),
		  m_mssZone(""),
          m_isValidData(false),
          m_isNew(false)
    {
        m_idKey = data.getUnsignedLongData(row, KEY_COL);

        reloadUsing(row, data);
    }


    AtsMssZoneHelper::~AtsMssZoneHelper()
    {
    }

	unsigned long AtsMssZoneHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");

        FUNCTION_EXIT;
        return m_idKey;
    }

    unsigned long AtsMssZoneHelper::getStationId()
    {
        FUNCTION_ENTRY("getStationId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_stationId;
    }


    void AtsMssZoneHelper::setStationId(unsigned long stationId)
    {
        FUNCTION_ENTRY("setStationId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_stationId = stationId;
        FUNCTION_EXIT;
    }

    std::string AtsMssZoneHelper::getMssZone()
    {
        FUNCTION_ENTRY("getMssZone" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_mssZone;
    }


    void AtsMssZoneHelper::setMssZone(const std::string& track)
    {
        FUNCTION_ENTRY("setMssZone" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_mssZone = track;
        FUNCTION_EXIT;
    }


    void AtsMssZoneHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new AtsMssZone");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void AtsMssZoneHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the AtsMssZone has already been written to the database. It doesn't make
        // any sense to reload an AtsMssZone that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This AtsMssZone does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);


        // create the SQL string to retrieve the data of the AtsMssZone
        // based upon the key
        /*std::ostringstream sql;
		sql << "select " << STATIONID_COL << ", " << MSSZONE_COL << " from AT_MSSZONE where " 
            << KEY_COL << " = " << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_MSSZONE_STD_SELECT_48005, m_idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_MSSZONE_SELECT_48005, m_idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_MSSZONE_SELECT_48005, m_idKey);
 
        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(STATIONID_COL);
		columnNames.push_back(MSSZONE_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // AtsMssZone record not found   
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for MSS Zone Key = " << m_idKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"MSS Zone Key" ) );
        }
        
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_stationId = data->getUnsignedLongData( 0, STATIONID_COL );
        m_mssZone = data->getStringData( 0, MSSZONE_COL );

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsMssZoneHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_stationId = data.getUnsignedLongData( row, STATIONID_COL );
        m_mssZone = data.getStringData( row, MSSZONE_COL );

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsMssZoneHelper::writeAtsMssZoneData() throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeAtsMssZoneData" );     

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
		if (m_stationId == -1)
        {
            fieldNames.push_back(STATIONID_COL);
        }
        if (m_mssZone.empty())
        {
            fieldNames.push_back(MSSZONE_COL);
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("AtsMssZone data not fully specified. AtsMssZone cannot be written to database",
                                                         fieldNames));
        }

        // Write the AtsMssZone to the database

        // Need to check if this is a new AtsMssZone or an existing AtsMssZone - 
        // as a new AtsMssZone will be inserted, while an existing AtsMssZone will be updated
        if (m_isNew) // This is a new AtsMssZone
        {
            addNewAtsMssZone();
        }
        else // This is an existing AtsMssZone
        {
            modifyExistingAtsMssZone();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void AtsMssZoneHelper::deleteAtsMssZone(bool cascade)
    {
        FUNCTION_ENTRY("deleteAtsMssZone");

        TA_ASSERT(!m_isNew, "This AtsMssZone does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
	    //int nDbType = databaseConnection->GetDbServerType();
        //std::ostringstream sql;
		//std::string strSql;
		TA_Base_Core::SQLStatement strSql;
        
        if (!cascade)
        {
            // Check for references in AT_POWER_ENTITY_MAP
            std::vector<std::string> columnNames;
            columnNames.push_back("ATPOEM_ID");
            TA_Base_Core::IData* data = NULL;

            /*sql.str("");
            sql << "SELECT M.ATPOEM_ID FROM AT_POWER_ENTITY_MAP M, ENTITY E "
				<< "WHERE M.DATA_POINT_ENTITY_KEY = E.PKEY AND E.LOCATIONKEY = " << m_stationId;*/
			//strSql = defPrepareSQLStatement(nDbType, AT_POWER_ENTITY_MAP_STD_SELECT_48501, m_stationId);
//			strSql = databaseConnection->prepareSQLStatement(AT_POWER_ENTITY_MAP_SELECT_48501, m_stationId);

		databaseConnection->prepareSQLStatement(strSql, AT_POWER_ENTITY_MAP_SELECT_48501, m_stationId);

            data = databaseConnection->executeQuery(strSql,columnNames);

            if (0 != data->getNumRows())
            {
                delete data;
                data = NULL;

                TA_THROW(TA_Base_Core::DataException("A reference to this AT_MSSZONE exists in the AT_POWER_ENTITY_MAP table",
                    TA_Base_Core::DataException::CANNOT_DELETE,"AT_POWER_ENTITY_MAP"));
            }

            delete data;
            data = NULL;

			// Check for references in AT_PSD_ENTITY_MAP
            columnNames.clear();
            columnNames.push_back("ATPEMA_ID");

   //         sql.str("");
   //         sql << "select M.ATPEMA_ID FROM AT_PSD_ENTITY_MAP M, AT_PLATFORM A ";			
			////TD14465 - wrong parameter key used ++lizettejl
			//// sql << "where M.ATPEMA_ID = A.PLATFORM_KEY and A.STATION_ID = " << m_stationId;
			//sql << "where M.ATPEMA_ID = A.ATPLAT_ID and A.STATION_ID = " << m_stationId;
			//strSql = defPrepareSQLStatement(nDbType, AT_PSD_ENTITY_MAP_STD_SELECT_49001, m_stationId);
//			strSql = databaseConnection->prepareSQLStatement(AT_PSD_ENTITY_MAP_SELECT_49001, m_stationId);

		databaseConnection->prepareSQLStatement(strSql, AT_PSD_ENTITY_MAP_SELECT_49001, m_stationId);
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
        
       /* sql.str("");
        sql << "delete AT_MSSZONE where " << KEY_COL << " = " << m_idKey;*/
		//strSql = defPrepareSQLStatement(nDbType, AT_MSSZONE_STD_DELETE_48401, m_idKey);
//		strSql = databaseConnection->prepareSQLStatement(AT_MSSZONE_DELETE_48401, m_idKey);

		databaseConnection->prepareSQLStatement(strSql, AT_MSSZONE_DELETE_48401, m_idKey);
        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void AtsMssZoneHelper::modifyExistingAtsMssZone() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingAtsMssZone");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "AtsMssZone id=%ul already exists. It's data will be updated.",m_idKey);
    
        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        /*std::ostringstream sql;
        sql << "update AT_MSSZONE set " 
			<< STATIONID_COL << " = "
            << m_stationId << ", " 
            << MSSZONE_COL << " = '"
            << (databaseConnection->escapeInsertString(m_mssZone)).c_str() << "' " 
            << "where " << KEY_COL << " = " << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_MSSZONE_STD_UPDATE_48151, m_stationId,
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_MSSZONE_UPDATE_48151, m_stationId,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_MSSZONE_UPDATE_48151, m_stationId,
			databaseConnection->escapeInsertString(m_mssZone), m_idKey);

        // get a connection to the database
        databaseConnection->executeModification(strSql);

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void AtsMssZoneHelper::addNewAtsMssZone() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewAtsMssZone");

		unsigned long newKey = getNextKey();

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "AtsMssZone %ul is a new AtsMssZone. It will be added to the database.",newKey);

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);

		/*std::ostringstream sql;
        sql << "insert into AT_MSSZONE (" 
            << KEY_COL << ", "
            << STATIONID_COL << ", " 
			<< MSSZONE_COL << ") "
            << "values ("
            << newKey << ", " 
            << m_stationId << ", '" 
			<< (databaseConnection->escapeInsertString(m_mssZone)).c_str() 
			<< "')";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_MSSZONE_STD_INSERT_48301, newKey, m_stationId,
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_MSSZONE_INSERT_48301, newKey, m_stationId,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_MSSZONE_INSERT_48301, newKey, m_stationId,
			databaseConnection->escapeInsertString(m_mssZone));

        databaseConnection->executeModification(strSql);

		m_idKey = newKey; 
        
        FUNCTION_EXIT;
    }

    unsigned long AtsMssZoneHelper::getNextKey()
	{
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// Get a suitable primary key generated for this new entry
		/*std::ostringstream sql;
        sql << "SELECT ATMSSZ_SEQ.NEXTVAL FROM DUAL";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_MSSZONE_Oracle_SELECT_48051);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_MSSZONE_SELECT_48051);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_MSSZONE_SELECT_48051);

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

