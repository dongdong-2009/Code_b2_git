/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsDssPlanMapHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * AtsDssPlanMapHelper is an object that is held by AtsDssPlanMap and ConfigAtsDssPlanMap objects. 
  * It contains all data used by AtsDssPlanMaps, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsDssPlanMap and ConfigAtsDssPlanMap.
  */


#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4503)	// decorated name length exceeded, name was truncated
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/ats/src/AtsDssPlanMapHelper.h"
#include "core/data_access_interface/ats/src/AtsDssPlanMapAccessFactory.h"
#include "core/data_access_interface/plans/src/PlanAccessFactory.h"
#include "core/data_access_interface/plans/src/IPlan.h"
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

    static const std::string KEY_COL			= "ATDPMA_ID";
	static const std::string DSSEVENT_COL		= "DSS_EVENT";
	static const std::string PLANPATH_COL		= "PLAN_PATH";

    static std::string toUpper(const std::string& in)
    {
        std::string out(in);
        std::transform(out.begin(), out.end(), out.begin(), toupper);
        return out;
    }



	AtsDssPlanMapHelper::AtsDssPlanMapHelper( const AtsDssPlanMapHelper& theAtsDssPlanMapHelper)
		: m_idKey(-1),
		  m_dssEvent(theAtsDssPlanMapHelper.m_dssEvent),
		  m_planPath(theAtsDssPlanMapHelper.m_planPath),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    AtsDssPlanMapHelper::AtsDssPlanMapHelper(const unsigned long idKey)
        : m_idKey(idKey),
		  m_dssEvent(-1),
		  m_planPath(""),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    AtsDssPlanMapHelper::AtsDssPlanMapHelper()
        : m_idKey(-1),
		  m_dssEvent(-1),
		  m_planPath(""),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    AtsDssPlanMapHelper::AtsDssPlanMapHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_idKey(-1),
		  m_dssEvent(-1),
		  m_planPath(""),
          m_isValidData(false),
          m_isNew(false)
    {
        m_idKey = data.getUnsignedLongData(row, KEY_COL);

        reloadUsing(row, data);
    }


    AtsDssPlanMapHelper::~AtsDssPlanMapHelper()
    {
    }

	unsigned long AtsDssPlanMapHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");

        FUNCTION_EXIT;
        return m_idKey;
    }

    unsigned long AtsDssPlanMapHelper::getDssEvent()
    {
        FUNCTION_ENTRY("getDssEvent" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;

		if (m_dssEvent != -1)
		{
			return m_dssEvent;
		}
		else
		{
			return 0;
		}
    }


    void AtsDssPlanMapHelper::setDssEvent(unsigned long id)
    {
        FUNCTION_ENTRY("setDssEvent" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_dssEvent = id;
        FUNCTION_EXIT;
    }


	std::string AtsDssPlanMapHelper::getPlanPath()
    {
        FUNCTION_ENTRY("getPlanPath" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;

		return m_planPath;
    }


    void AtsDssPlanMapHelper::setPlanPath(const std::string& planPath)
    {
        FUNCTION_ENTRY("setPlanPath" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
 
        // Create a Plan object given the specified path. This will validate the path for us
        // (DataException is thrown to indicate a bad path).
        std::auto_ptr<TA_Base_Core::IPlan> plan(TA_Base_Core::PlanAccessFactory::getInstance().getPlanByPath(planPath));
        m_planPath = plan->getPath();

        FUNCTION_EXIT;
    }


	std::string AtsDssPlanMapHelper::getName()
    {
        FUNCTION_ENTRY("getName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;

		std::ostringstream name;
		name << m_dssEvent ;
        return name.str();
    }


    void AtsDssPlanMapHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new AtsDssPlanMap");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void AtsDssPlanMapHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the AtsDssPlanMap has already been written to the database. It doesn't make
        // any sense to reload an AtsDssPlanMap that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This AtsDssPlanMap does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);


        // create the SQL string to retrieve the data of the AtsDssPlanMap
        // based upon the key
        /*std::ostringstream sql;
		sql << "select " << DSSEVENT_COL << ", "
			<< PLANPATH_COL << " from AT_DSS_PLAN_MAP where " 
            << KEY_COL << " = " << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_DSS_PLAN_MAP_STD_SELECT_46505, m_idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_DSS_PLAN_MAP_SELECT_46505, m_idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_DSS_PLAN_MAP_SELECT_46505, m_idKey);
 
        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(DSSEVENT_COL);
		columnNames.push_back(PLANPATH_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // AtsDssPlanMap record not found   
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for MSS Zone Key = " << m_idKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"MSS Zone Key" ) );
        }
        
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_dssEvent = data->getUnsignedLongData( 0, DSSEVENT_COL );
        m_planPath = data->getStringData( 0, PLANPATH_COL );

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsDssPlanMapHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_dssEvent = data.getUnsignedLongData( row, DSSEVENT_COL );
        m_planPath = data.getStringData( row, PLANPATH_COL );

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsDssPlanMapHelper::writeAtsDssPlanMapData() throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeAtsDssPlanMapData" );     

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
		if (m_dssEvent == -1)
        {
            fieldNames.push_back(DSSEVENT_COL);
        }
        if (m_planPath.empty() || m_planPath.find_first_not_of(" \t") == std::string::npos)
        {
            fieldNames.push_back(PLANPATH_COL);
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("AtsDssPlanMap data not fully specified. AtsDssPlanMap cannot be written to database",
                                                         fieldNames));
        }

        // Write the AtsDssPlanMap to the database

        // Need to check if this is a new AtsDssPlanMap or an existing AtsDssPlanMap - 
        // as a new AtsDssPlanMap will be inserted, while an existing AtsDssPlanMap will be updated
        if (m_isNew) // This is a new AtsDssPlanMap
        {
            addNewAtsDssPlanMap();
        }
        else // This is an existing AtsDssPlanMap
        {
            modifyExistingAtsDssPlanMap();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void AtsDssPlanMapHelper::deleteAtsDssPlanMap(bool cascade)
    {
        FUNCTION_ENTRY("deleteAtsDssPlanMap");

        TA_ASSERT(!m_isNew, "This AtsDssPlanMap does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        //std::ostringstream sql;

        // Good to go...
        
        /*sql.str("");
        sql << "delete AT_DSS_PLAN_MAP where " << KEY_COL << " = " << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_DSS_PLAN_MAP_STD_DELETE_46901, m_idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_DSS_PLAN_MAP_DELETE_46901, m_idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_DSS_PLAN_MAP_DELETE_46901, m_idKey);
        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void AtsDssPlanMapHelper::modifyExistingAtsDssPlanMap() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingAtsDssPlanMap");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "AtsDssPlanMap id=%ul already exists. It's data will be updated.",m_idKey);
    
        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);

        /*std::ostringstream sql;
        sql << "update AT_DSS_PLAN_MAP set " 
			<< DSSEVENT_COL << " = "
            << m_dssEvent << ", " 
            << PLANPATH_COL << " = '"
            << databaseConnection->escapeInsertString(m_planPath) << "' "
            << " where " << KEY_COL << " = " << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_DSS_PLAN_MAP_STD_UPDATE_46651, m_dssEvent, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_DSS_PLAN_MAP_UPDATE_46651, m_dssEvent, 
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_DSS_PLAN_MAP_UPDATE_46651, m_dssEvent,
			databaseConnection->escapeInsertString(m_planPath), m_idKey);

        databaseConnection->executeModification(strSql);

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void AtsDssPlanMapHelper::addNewAtsDssPlanMap() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewAtsDssPlanMap");

		unsigned long newKey = getNextKey();

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "AtsDssPlanMap %ul is a new AtsDssPlanMap. It will be added to the database.",newKey);

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);

		/*std::ostringstream sql;
        sql << "insert into AT_DSS_PLAN_MAP (" 
            << KEY_COL << ", "
			<< DSSEVENT_COL << ", " 
			<< PLANPATH_COL << ") "
            << "values ("
            << newKey << ", " 
			<< m_dssEvent << ", '" 
			<< databaseConnection->escapeInsertString(m_planPath) << "' "
			<< ")";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_DSS_PLAN_MAP_STD_INSERT_46801, newKey,
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_DSS_PLAN_MAP_INSERT_46801, newKey,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_DSS_PLAN_MAP_INSERT_46801, newKey,
			m_dssEvent, databaseConnection->escapeInsertString(m_planPath));

        databaseConnection->executeModification(strSql);

		m_idKey = newKey; 
        
        FUNCTION_EXIT;
    }

    unsigned long AtsDssPlanMapHelper::getNextKey()
	{
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);

		// Get a suitable primary key generated for this new entry
		/*std::ostringstream sql;
        sql << "SELECT ATDPMA_SEQ.NEXTVAL FROM DUAL";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_DSS_PLAN_MAP_Oracle_SELECT_46551);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_DSS_PLAN_MAP_SELECT_46551);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_DSS_PLAN_MAP_SELECT_46551);

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

