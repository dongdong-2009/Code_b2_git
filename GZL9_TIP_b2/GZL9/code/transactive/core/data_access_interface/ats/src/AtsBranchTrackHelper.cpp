/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsBranchTrackHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * AtsBranchTrackHelper is an object that is held by AtsBranchTrack and ConfigAtsBranchTrack objects. 
  * It contains all data used by AtsBranchTracks, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsBranchTrack and ConfigAtsBranchTrack.
  */


#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4503)	// decorated name length exceeded, name was truncated
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/ats/src/AtsBranchTrackHelper.h"
#include "core/data_access_interface/ats/src/AtsBranchTrackAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

    static const std::string KEY_COL = "ATBTRA_ID";
    static const std::string TRACK_COL = "TRACK";

    static std::string toUpper(const std::string& in)
    {
        std::string out(in);
        std::transform(out.begin(), out.end(), out.begin(), toupper);
        return out;
    }



	AtsBranchTrackHelper::AtsBranchTrackHelper( const AtsBranchTrackHelper& theAtsBranchTrackHelper)
		: m_idKey(-1),
		  m_track(theAtsBranchTrackHelper.m_track),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    AtsBranchTrackHelper::AtsBranchTrackHelper(const unsigned long idKey)
        : m_idKey(idKey),
		  m_track(""),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    AtsBranchTrackHelper::AtsBranchTrackHelper()
        : m_idKey(-1),
		  m_track(""),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    AtsBranchTrackHelper::AtsBranchTrackHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_idKey(-1),
		  m_track(""),
          m_isValidData(false),
          m_isNew(false)
    {
        m_idKey = data.getUnsignedLongData(row, KEY_COL);

        reloadUsing(row, data);
    }


    AtsBranchTrackHelper::~AtsBranchTrackHelper()
    {
    }

	unsigned long AtsBranchTrackHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");

        FUNCTION_EXIT;
        return m_idKey;
    }

    std::string AtsBranchTrackHelper::getAtsBranchId()
    {
        FUNCTION_ENTRY("getAtsBranchId");

		std::ostringstream branchId;

		if (m_idKey != -1)
		{
			branchId << ((m_idKey & 0xF800) >> 11) << "." << ((m_idKey & 0x07C0) >> 6) << ".";
			branchId << ((m_idKey & 0x0030) >> 4) << "." << ((m_idKey & 0x000F)); 
		}
		
        FUNCTION_EXIT;
        return branchId.str();
    }


    void AtsBranchTrackHelper::setAtsBranchId(const std::string& id)
    {
        FUNCTION_ENTRY("setAtsBranchId");
        TA_ASSERT(m_isNew,"An existing Branch ID cannot be modified.");

		std::istringstream idStream(id);

		int part;
		char dot;
		int idKey = 0;

		part = 0;
		if (idStream >> part && idStream >> dot)
		{
			idKey |= ((part & 0x1F) << 11);
		}
		else
		{
			return;
		}

		part = 0;
		if (idStream >> part && idStream >> dot)
		{
			idKey |= ((part & 0x1F) << 6);
		}
		else
		{
			return;
		}

		part = 0;
		if (idStream >> part && idStream >> dot)
		{
			idKey |= ((part &0x03) << 4);
		}
		else
		{
			return;
		}

		part = 0;
		if (idStream >> part)
		{
			idKey |= (part & 0x0F);
		}
		else
		{
			return;
		}

		m_idKey = idKey;

        FUNCTION_EXIT;
        return;
    }

    std::string AtsBranchTrackHelper::getTrack()
    {
        FUNCTION_ENTRY("getTrack" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_track;
    }


    void AtsBranchTrackHelper::setTrack(const std::string& track)
    {
        FUNCTION_ENTRY("setTrack" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_track = track;
        FUNCTION_EXIT;
    }


    void AtsBranchTrackHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new AtsBranchTrack");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void AtsBranchTrackHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the AtsBranchTrack has already been written to the database. It doesn't make
        // any sense to reload an AtsBranchTrack that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This AtsBranchTrack does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);


        // create the SQL string to retrieve the data of the AtsBranchTrack
        // based upon the key
        /*std::ostringstream sql;
		sql << "select " << TRACK_COL << " from AT_BRANCH_TRACK where " 
            << KEY_COL << " = " << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_BRANCH_TRACK_STD_SELECT_46005, m_idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_BRANCH_TRACK_SELECT_46005, m_idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_BRANCH_TRACK_SELECT_46005, m_idKey);
 
        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(TRACK_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) 
        {
            // AtsBranchTrack record not found   
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for Branch Id = " << m_idKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Branch Id" ) );
        }
        
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_track = data->getStringData( 0, TRACK_COL );
        
        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsBranchTrackHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_track = data.getStringData( row, TRACK_COL );

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsBranchTrackHelper::writeAtsBranchTrackData() throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeAtsBranchTrackData" );     

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if (-1 == m_idKey)
        {
            fieldNames.push_back(KEY_COL);
        }
        if (m_track.empty())
        {
            fieldNames.push_back(TRACK_COL);
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("AtsBranchTrack data not fully specified. AtsBranchTrack cannot be written to database",
                                                         fieldNames));
        }

        // Write the AtsBranchTrack to the database

        // Need to check if this is a new AtsBranchTrack or an existing AtsBranchTrack - 
        // as a new AtsBranchTrack will be inserted, while an existing AtsBranchTrack will be updated
        if (m_isNew) // This is a new AtsBranchTrack
        {
            addNewAtsBranchTrack();
        }
        else // This is an existing AtsBranchTrack
        {
            modifyExistingAtsBranchTrack();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void AtsBranchTrackHelper::deleteAtsBranchTrack(bool cascade)
    {
        FUNCTION_ENTRY("deleteAtsBranchTrack");

        TA_ASSERT(!m_isNew, "This AtsBranchTrack does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
        //std::ostringstream sql;
		//std::string strSql;
		TA_Base_Core::SQLStatement strSql;
        
        if (!cascade)
        {
            // Check for references in AT_FIXED_BLOCK_MAP
            std::vector<std::string> columnNames;
            columnNames.push_back("ATFBLO_ID");
            TA_Base_Core::IData* data = NULL;

           /* sql.str("");
            sql << "select ATFBLO_ID from AT_FIXED_BLOCK_MAP where BRANCH_ID = " << m_idKey;*/
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_FIXED_BLOCK_MAP_STD_SELECT_47501, m_idKey);
//			strSql  = databaseConnection->prepareSQLStatement(AT_FIXED_BLOCK_MAP_SELECT_47501, m_idKey);

		databaseConnection->prepareSQLStatement(strSql, AT_FIXED_BLOCK_MAP_SELECT_47501, m_idKey);

            data = databaseConnection->executeQuery(strSql,columnNames);

            if (0 != data->getNumRows())
            {
                delete data;
                data = NULL;

                TA_THROW(TA_Base_Core::DataException("A reference to this AT_BRANCH_TRACK exists in the AT_FIXED_BLOCK_MAP table",
                    TA_Base_Core::DataException::CANNOT_DELETE,"AT_FIXED_BLOCK_MAP"));
            }

            delete data;
            data = NULL;
        }

        // Good to go...
        
        /*sql.str("");
        sql << "delete AT_BRANCH_TRACK where " << KEY_COL << " = " << m_idKey;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_BRANCH_TRACK_STD_DELETE_46401, m_idKey);
//		strSql  = databaseConnection->prepareSQLStatement(AT_BRANCH_TRACK_DELETE_46401, m_idKey);

		databaseConnection->prepareSQLStatement(strSql, AT_BRANCH_TRACK_DELETE_46401, m_idKey);
        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void AtsBranchTrackHelper::modifyExistingAtsBranchTrack() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingAtsBranchTrack");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "AtsBranchTrack id=%ul already exists. It's data will be updated.",m_idKey);

        validateBranchIdUniqueness();
    
        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        // get a connection to the database
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
       /* std::ostringstream sql;
        sql << "update AT_BRANCH_TRACK set " 
            << TRACK_COL << " = '"
            << (databaseConnection->escapeInsertString(m_track)).c_str() << "' " 
            << "where " << KEY_COL << " = " << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_BRANCH_TRACK_STD_UPDATE_46151, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_BRANCH_TRACK_UPDATE_46151, 
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_BRANCH_TRACK_UPDATE_46151,
			databaseConnection->escapeInsertString(m_track), m_idKey);

        databaseConnection->executeModification(strSql);

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void AtsBranchTrackHelper::addNewAtsBranchTrack() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewAtsBranchTrack");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "AtsBranchTrack %ul is a new AtsBranchTrack. It will be added to the database.",m_idKey);

        validateBranchIdUniqueness();

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        /*std::ostringstream sql;
        sql << "insert into AT_BRANCH_TRACK (" 
            << KEY_COL << ", "
            << TRACK_COL << ") "
            << "values ('"
            << m_idKey << "','" 
            << (databaseConnection->escapeInsertString(m_track)).c_str() << "')";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_BRANCH_TRACK_STD_INSERT_46301, m_idKey,
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_BRANCH_TRACK_INSERT_46301, m_idKey,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_BRANCH_TRACK_INSERT_46301, m_idKey,
			databaseConnection->escapeInsertString(m_track) );

        databaseConnection->executeModification(strSql);
        
        FUNCTION_EXIT;
    }

    void AtsBranchTrackHelper::validateBranchIdUniqueness() throw(TA_Base_Core::TransactiveException)
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // First ensure the uniqueness of the record.
        //
        ////////////////////////////////////////////////////////////////////////////

        if (m_isNew)
        {
            /*std::ostringstream sql;
            sql << "select " << KEY_COL << " from AT_BRANCH_TRACK where " << KEY_COL <<" = " << m_idKey;*/			

            // Set up the vector to pass to exectueQuery
            std::vector<std::string> columnNames;
            columnNames.push_back(KEY_COL);

            // execute the query
            TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_BRANCH_TRACK_STD_SELECT_46001, m_idKey);
//			std::string strSql  = databaseConnection->prepareSQLStatement(AT_BRANCH_TRACK_SELECT_46001, m_idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_BRANCH_TRACK_SELECT_46001, m_idKey);
            TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
            if (0 != data->getNumRows())
            {
                // ZoneId already exists
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "An AT_BRANCH_TRACK record exists with ZoneId=" << m_idKey;
                TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NOT_UNIQUE,"Branch ID"));
            }

            delete data;
            data = NULL;

        }
        else
        { 
            // Do nothing
        }
    }
} // closes TA_Core

