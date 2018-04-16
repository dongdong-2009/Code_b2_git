/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/AtsFbMapRecordHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * AtsFbMapRecordHelper is an object that is held by AtsFbMapRecord and ConfigAtsFbMapRecord objects. 
  * It contains all data used by AtsFbMapRecords, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsFbMapRecord and ConfigAtsFbMapRecord.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4503)	// decorated name length exceeded, name was truncated
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/ats/src/AtsFbMapRecordHelper.h"
#include "core/data_access_interface/ats/src/AtsFixedBlockMapAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

    static const std::string KEY_COL = "ATFBLO_ID";
    static const std::string BRANCHID_COL = "BRANCH_ID";
	static const std::string TRACK_COL = "TRACK";
    static const std::string MINABSCISSA_COL = "MIN_ABSCISSA";
    static const std::string MAXABSCISSA_COL = "MAX_ABSCISSA";
    static const std::string GENERICID_COL = "GENERIC_ID";
	static const std::string ZONETYPE_COL = "ZONE_TYPE";
	static const std::string ZONETYPENAME_COL = "ZONE_TYPE_NAME";

    static std::string toUpper(const std::string& in)
    {
        std::string out(in);
        std::transform(out.begin(), out.end(), out.begin(), toupper);
        return out;
    }



	AtsFbMapRecordHelper::AtsFbMapRecordHelper( const AtsFbMapRecordHelper& theAtsFbMapRecordHelper)
		: m_idKey(-1),
		  m_branchId(theAtsFbMapRecordHelper.m_branchId),
		  m_track(theAtsFbMapRecordHelper.m_track),
		  m_minAbscissa(theAtsFbMapRecordHelper.m_minAbscissa),
		  m_maxAbscissa(theAtsFbMapRecordHelper.m_maxAbscissa),
		  m_zoneId(theAtsFbMapRecordHelper.m_zoneId),
		  m_zoneType(theAtsFbMapRecordHelper.m_zoneType),
		  m_zoneTypeName(theAtsFbMapRecordHelper.m_zoneTypeName),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    AtsFbMapRecordHelper::AtsFbMapRecordHelper(const unsigned long idKey)
        : m_idKey(idKey),
		  m_branchId(0),
		  m_track(""),
		  m_minAbscissa(0),
		  m_maxAbscissa(0),
		  m_zoneId(0),
		  m_zoneType(0),
		  m_zoneTypeName(""),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    AtsFbMapRecordHelper::AtsFbMapRecordHelper()
        : m_idKey(-1),
		  m_branchId(0),
		  m_track(""),
		  m_minAbscissa(0),
		  m_maxAbscissa(0),
		  m_zoneId(0),
		  m_zoneType(0),
		  m_zoneTypeName(""),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    AtsFbMapRecordHelper::AtsFbMapRecordHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_idKey(-1),
		  m_branchId(0),
		  m_track(""),
		  m_minAbscissa(0),
		  m_maxAbscissa(0),
		  m_zoneId(0),
		  m_zoneType(0),
		  m_zoneTypeName(""),
          m_isValidData(false),
          m_isNew(false)
    {
        m_idKey = data.getUnsignedLongData(row, KEY_COL);

        reloadUsing(row, data);
    }


    AtsFbMapRecordHelper::~AtsFbMapRecordHelper()
    {
    }

	unsigned long AtsFbMapRecordHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");

        FUNCTION_EXIT;
        return m_idKey;
    }

	unsigned long AtsFbMapRecordHelper::getBranchId()
    {
        FUNCTION_ENTRY("getBranchId");
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_branchId;
    }


    std::string AtsFbMapRecordHelper::getBranchIdAsString()
    {
        FUNCTION_ENTRY("getBranchIdAsString");
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		std::ostringstream branchId;

		branchId << ((m_branchId & 0xF800) >> 11) << "." << ((m_branchId & 0x07C0) >> 6) << ".";
		branchId << ((m_branchId & 0x0030) >> 4) << "." << ((m_branchId & 0x000F)); 
		
        FUNCTION_EXIT;
        return branchId.str();
    }


    void AtsFbMapRecordHelper::setBranchIdAsString(const std::string& id)
    {
        FUNCTION_ENTRY("setBranchIdAsString");
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		std::istringstream idStream(id);

		int part;
		char dot;
		int branchId = 0;

		part = 0;
		if (idStream >> part && idStream >> dot)
		{
			branchId |= ((part & 0x1F) << 11);
		}
		else
		{
			return;
		}

		part = 0;
		if (idStream >> part && idStream >> dot)
		{
			branchId |= ((part & 0x1F) << 6);
		}
		else
		{
			return;
		}

		part = 0;
		if (idStream >> part && idStream >> dot)
		{
			branchId |= ((part &0x03) << 4);
		}
		else
		{
			return;
		}

		part = 0;
		if (idStream >> part)
		{
			branchId |= (part & 0x0F);
		}
		else
		{
			return;
		}

		m_branchId = branchId;

        FUNCTION_EXIT;
        return;
    }

    std::string AtsFbMapRecordHelper::getTrack()
    {
        FUNCTION_ENTRY("getTrack" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_track;
    }

	unsigned long AtsFbMapRecordHelper::getMinAbscissa()
    {
        FUNCTION_ENTRY("getMinAbscissa" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_minAbscissa;
    }


    void AtsFbMapRecordHelper::setMinAbscissa(unsigned long minAbscissa)
    {
        FUNCTION_ENTRY("setMinAbscissa" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_minAbscissa = minAbscissa;
        FUNCTION_EXIT;
    }

	unsigned long AtsFbMapRecordHelper::getMaxAbscissa()
    {
        FUNCTION_ENTRY("getMaxAbscissa" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_maxAbscissa;
    }


    void AtsFbMapRecordHelper::setMaxAbscissa(unsigned long maxAbscissa)
    {
        FUNCTION_ENTRY("setMaxAbscissa" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_maxAbscissa = maxAbscissa;
        FUNCTION_EXIT;
    }

	double AtsFbMapRecordHelper::getZoneId()
    {
        FUNCTION_ENTRY("getZoneId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_zoneId;
    }

    void AtsFbMapRecordHelper::setZoneId(double zoneId)
    {
        FUNCTION_ENTRY("setZoneId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_zoneId = zoneId;
        FUNCTION_EXIT;
    }

	void AtsFbMapRecordHelper::setZoneTypeName(const std::string& zoneTypeName)
	{
		FUNCTION_ENTRY("setZoneTypeName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
		
		// Need to get the zone ID for this name
		TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create the SQL string to retrieve the name of the AtsFixedBlockMap
        // this also doubles as a check to ensure an AtsFixedBlockMap with the specified
        // PKEY actually exists.
        std::ostringstream sql;
		sql << "SELECT PKEY FROM ZONETYPE WHERE NAME = '" << zoneTypeName << "'";

        std::vector<std::string> columnNames;
        columnNames.push_back("PKEY");
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        if (0 == data->getNumRows()) 
        {
            // AtsFbMapRecord record not found   
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for Zone Type Name = " << zoneTypeName;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Key" ) );
        }
        
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_zoneType = data->getUnsignedLongData(0, "PKEY");
		m_zoneTypeName = zoneTypeName;

		FUNCTION_EXIT
	}

	std::string AtsFbMapRecordHelper::getZoneTypeName()
    {
        FUNCTION_ENTRY("getZoneIdName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_zoneTypeName;
    }

    void AtsFbMapRecordHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new AtsFbMapRecord");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void AtsFbMapRecordHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the AtsFbMapRecord has already been written to the database. It doesn't make
        // any sense to reload an AtsFbMapRecord that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This AtsFbMapRecord does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// create the SQL string to retrieve the name of the AtsFixedBlockMap
        // this also doubles as a check to ensure an AtsFixedBlockMap with the specified
        // PKEY actually exists.
        std::ostringstream sql;
		sql << "SELECT " << BRANCHID_COL << ", " << TRACK_COL << ", ";
		sql << MINABSCISSA_COL << ", " << MAXABSCISSA_COL << ", ";
		sql << GENERICID_COL << ", " << ZONETYPE_COL << ", " <<ZONETYPENAME_COL;
		sql << " FROM AT_FIXED_BLOCK_MAP_V WHERE ";
		sql << KEY_COL << " = " << m_idKey;

        std::vector<std::string> columnNames;
        columnNames.push_back(BRANCHID_COL);
		columnNames.push_back(TRACK_COL);
        columnNames.push_back(MINABSCISSA_COL);
        columnNames.push_back(MAXABSCISSA_COL);
        columnNames.push_back(GENERICID_COL);
		columnNames.push_back(ZONETYPE_COL);
		columnNames.push_back(ZONETYPENAME_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        if (0 == data->getNumRows()) 
        {
            // AtsFbMapRecord record not found   
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for Key = " << m_idKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Key" ) );
        }
        
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_branchId = data->getUnsignedLongData(0, BRANCHID_COL);
		m_track = data->getStringData(0, TRACK_COL);
		m_minAbscissa = data->getUnsignedLongData(0, MINABSCISSA_COL);
		m_maxAbscissa = data->getUnsignedLongData(0, MAXABSCISSA_COL);
		m_zoneId = data->getRealData(0, GENERICID_COL);
		m_zoneType = data->getUnsignedLongData(0, ZONETYPE_COL);
		m_zoneTypeName = data->getStringData(0, ZONETYPENAME_COL);
        
        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsFbMapRecordHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_branchId = data.getUnsignedLongData(row, BRANCHID_COL);
		m_track = data.getStringData(row, TRACK_COL);
		m_minAbscissa = data.getUnsignedLongData(row, MINABSCISSA_COL);
		m_maxAbscissa = data.getUnsignedLongData(row, MAXABSCISSA_COL);
		m_zoneId = data.getRealData(row, GENERICID_COL);
		m_zoneType = data.getUnsignedLongData(row, ZONETYPE_COL);
		m_zoneTypeName = data.getStringData(row, ZONETYPENAME_COL);

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsFbMapRecordHelper::writeAtsFbMapRecordData() throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeAtsFbMapRecordData" );     

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
		if (m_branchId == 0)
		{
			fieldNames.push_back(BRANCHID_COL);
		}
		if (m_zoneId == 0)
		{
			fieldNames.push_back(GENERICID_COL);
		}
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("AtsFbMapRecord data not fully specified. AtsFbMapRecord cannot be written to database",
                                                         fieldNames));
        }

        // Write the AtsFbMapRecord to the database

        // Need to check if this is a new AtsFbMapRecord or an existing AtsFbMapRecord - 
        // as a new AtsFbMapRecord will be inserted, while an existing AtsFbMapRecord will be updated
        if (m_isNew) // This is a new AtsFbMapRecord
        {
            addNewAtsFbMapRecord();
        }
        else // This is an existing AtsFbMapRecord
        {
            modifyExistingAtsFbMapRecord();
        }
        // Now that the data has been written, we need to reload the view data.
        m_isValidData = false;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void AtsFbMapRecordHelper::deleteAtsFbMapRecord(bool cascade)
    {
        FUNCTION_ENTRY("deleteAtsFbMapRecord");

        TA_ASSERT(!m_isNew, "This AtsFbMapRecord does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        std::ostringstream sql;
        
        if (!cascade)
        {
        }

        // Good to go...
        
        sql.str("");
		//TD14468 - deleting record at the wrong table
        sql << "delete AT_FIXED_BLOCK_MAP where " << KEY_COL << " = " << m_idKey;
		//sql << "delete AT_BRANCH_TRACK where " << KEY_COL << " = " << m_idKey;
        databaseConnection->executeModification(sql.str());

        FUNCTION_EXIT;
    }


    void AtsFbMapRecordHelper::modifyExistingAtsFbMapRecord() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingAtsFbMapRecord");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "AtsFbMapRecord id=%ul already exists. It's data will be updated.",m_idKey);

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        std::ostringstream sql;
        sql << "update AT_FIXED_BLOCK_MAP set " 
			<< BRANCHID_COL << " = " << m_branchId << ", "
			<< MINABSCISSA_COL << " = " << m_minAbscissa << ", "
			<< MAXABSCISSA_COL << " = " << m_maxAbscissa << ", "
			<< GENERICID_COL << " = " << m_zoneId << ", "
			<< ZONETYPE_COL << " = " << m_zoneType
            << " where " << KEY_COL << " = " << m_idKey;

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        databaseConnection->executeModification(sql.str().c_str());

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void AtsFbMapRecordHelper::addNewAtsFbMapRecord() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewAtsFbMapRecord");

		unsigned long newKey = getNextKey();

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "AtsFbMapRecord %ul is a new AtsFbMapRecord. It will be added to the database.",newKey);

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);

		std::ostringstream sql;
        sql << "insert into AT_FIXED_BLOCK_MAP (" 
            << KEY_COL << ", "
            << BRANCHID_COL << ", " 
			<< MINABSCISSA_COL << ", " 
			<< MAXABSCISSA_COL << ", "  
			<< GENERICID_COL << ", "
			<< ZONETYPE_COL << ") "
            << "values ("
            << newKey << ", " 
            << m_branchId << ", " 
			<< m_minAbscissa << ", " 
			<< m_maxAbscissa << ", " 
			<< m_zoneId << ", "
			<< m_zoneType << ")";


        databaseConnection->executeModification(sql.str());

		m_idKey = newKey; 
        
        FUNCTION_EXIT;
    }

	unsigned long AtsFbMapRecordHelper::getNextKey()
	{
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// Get a suitable primary key generated for this new entry
		std::ostringstream sql;
        sql << "SELECT ATFBLO_SEQ.NEXTVAL FROM DUAL";

        std::vector<std::string> columnNames;
        columnNames.push_back("NEXTVAL");

        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);
        
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

