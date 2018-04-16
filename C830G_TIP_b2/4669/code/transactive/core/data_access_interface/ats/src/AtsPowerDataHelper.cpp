/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/AtsPowerDataHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * AtsPowerDataHelper is an object that is held by AtsPowerDataHelper and ConfigAtsPowerData objects. 
  * It contains all data used by AtsPowerDatas, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsPowerDataHelper and ConfigAtsPowerData.
  */


#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4503)	// warning C4503: '  ' : decorated name length exceeded, name was truncated
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/ats/src/AtsPowerDataHelper.h"
#include "core/data_access_interface/ats/src/AtsPowerDataFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
    static const std::string KEY_COL					= "ATPOEM_ID";
    static const std::string STATIONID_COL				= "STATION_ID";
	static const std::string LOCATIONKEY_COL			= "LOCATION_KEY";
	static const std::string BITPOSITION_COL			= "BIT_POSITION";		//CL20376, hongzhi
    static const std::string MSSZONE_COL				= "MSS_ZONE";
	static const std::string SUBSECTIONNUMBER_COL		= "SUBSECTION_ID";
	static const std::string DATAPOINTENTITYKEY_COL		= "DATA_POINT_ENTITY_KEY";
	
    static std::string toUpper(const std::string& in)
    {
        std::string out(in);
        std::transform(out.begin(), out.end(), out.begin(), toupper);
        return out;
    }

	AtsPowerDataHelper::AtsPowerDataHelper( const AtsPowerDataHelper& theAtsPowerDataHelper)
		: m_idKey(-1),
		  m_powerDataPointEntityKey(theAtsPowerDataHelper.m_powerDataPointEntityKey),
		  m_subsectionNumber(theAtsPowerDataHelper.m_subsectionNumber),
		  m_locationId(theAtsPowerDataHelper.m_locationId),
		  m_bitPosition(theAtsPowerDataHelper.m_bitPosition),
		  m_mssZone(theAtsPowerDataHelper.m_mssZone),
          m_isValidData(false),
          m_isNew(true) 
    {	  
	}


    AtsPowerDataHelper::AtsPowerDataHelper(const unsigned long idKey)
        : m_idKey(idKey),
		  m_powerDataPointEntityKey(-1),
		  m_subsectionNumber(-1),
		  m_locationId(-1),
		  m_bitPosition(-1),
		  m_mssZone(""),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    AtsPowerDataHelper::AtsPowerDataHelper()
        : m_idKey(-1),
		  m_powerDataPointEntityKey(-1),
		  m_subsectionNumber(-1),
		  m_locationId(-1),
		  m_bitPosition(-1),
		  m_mssZone(""),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    AtsPowerDataHelper::AtsPowerDataHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_idKey(-1),
		  m_powerDataPointEntityKey(-1),
		  m_subsectionNumber(-1),
		  m_locationId(-1),
		  m_bitPosition(-1),
		  m_mssZone(""),
          m_isValidData(false),
          m_isNew(false)
    {
        m_idKey = data.getUnsignedLongData(row, KEY_COL);

        reloadUsing(row, data);
    }


    AtsPowerDataHelper::~AtsPowerDataHelper()
    {
    }

	unsigned long AtsPowerDataHelper::getKey()
	{
		return m_idKey;
	}

	unsigned long AtsPowerDataHelper::getTractionPowerDataPointEntityKey()
	{
        FUNCTION_ENTRY("getTractionPowerDataPointEntityKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_powerDataPointEntityKey;
    }

	unsigned long AtsPowerDataHelper::getElectricalSubSectionId()
	{
        FUNCTION_ENTRY("getElectricalSubSectionId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_subsectionNumber;
    }

	unsigned long AtsPowerDataHelper::getLocationId()
    {
        FUNCTION_ENTRY("getLocationId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_locationId;
    }

	unsigned long AtsPowerDataHelper::getBitPosition()
    {
        FUNCTION_ENTRY("getBitPosition" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_bitPosition;
    }

	std::string AtsPowerDataHelper::getMSSZone()
	{
        FUNCTION_ENTRY("getMSSZone" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_mssZone;
    }

    void AtsPowerDataHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new AtsPowerDataHelper");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

	void AtsPowerDataHelper::setTractionPowerDataPointEntityKey(unsigned long key)
	{
        FUNCTION_ENTRY("setPowerDataPointEntityKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_powerDataPointEntityKey = key;
        FUNCTION_EXIT;
    }

	void AtsPowerDataHelper::setLocationId(unsigned long id)
	{
        FUNCTION_ENTRY("setLocationId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_locationId = id;
        FUNCTION_EXIT;
    }

	void AtsPowerDataHelper::setBitPosition(int bitPosition)
	{
        FUNCTION_ENTRY("setBitPosition" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_bitPosition = bitPosition;
        FUNCTION_EXIT;
    }
		
	void AtsPowerDataHelper::setElectricalSubSectionId(int psdNumber)
	{
        FUNCTION_ENTRY("setElectricalSubSectionId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_subsectionNumber = psdNumber;
        FUNCTION_EXIT;
    }


    void AtsPowerDataHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the AtsPowerDataHelper has already been written to the database. It doesn't make
        // any sense to reload an AtsPowerDataHelper that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This AtsPowerDataHelper does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);


        // create the SQL string to retrieve the data of the AtsPowerDataHelper
        // based upon the key
        std::ostringstream sql;
		sql << "select " << STATIONID_COL << ", " << BITPOSITION_COL << ", " << MSSZONE_COL << ", ";
		sql << SUBSECTIONNUMBER_COL << ", " << DATAPOINTENTITYKEY_COL;
		sql << " from AT_POWER_ENTITY_MAP_V where ";
		sql << KEY_COL << " = " << m_idKey;
 
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(STATIONID_COL);
		columnNames.push_back(BITPOSITION_COL);
        columnNames.push_back(MSSZONE_COL);
        columnNames.push_back(SUBSECTIONNUMBER_COL);
        columnNames.push_back(DATAPOINTENTITYKEY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        if (0 == data->getNumRows()) 
        {
            // AtsPowerDataHelper record not found   
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PSD Entity Map Key = " << m_idKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PSD Entity Map Key" ) );
        }
        
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_locationId = data->getUnsignedLongData(0, STATIONID_COL),
		m_bitPosition = data->getIntegerData(0, BITPOSITION_COL),
		m_mssZone = data->getStringData(0, MSSZONE_COL),
		m_subsectionNumber = data->getIntegerData(0, SUBSECTIONNUMBER_COL),
		m_powerDataPointEntityKey = data->getUnsignedLongData(0, DATAPOINTENTITYKEY_COL),

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsPowerDataHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_locationId = data.getUnsignedLongData(row, STATIONID_COL),
		m_bitPosition = data.getIntegerData(row, BITPOSITION_COL),
		m_mssZone = data.getStringData(row, MSSZONE_COL),
		m_subsectionNumber = data.getIntegerData(row, SUBSECTIONNUMBER_COL),
		m_powerDataPointEntityKey = data.getUnsignedLongData(row, DATAPOINTENTITYKEY_COL),

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsPowerDataHelper::writeAtsPowerDataData() throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeAtsPowerDataData" );     

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
		if (m_subsectionNumber == -1)
        {
            fieldNames.push_back(SUBSECTIONNUMBER_COL);
        }
		if (m_bitPosition == -1)
        {
            fieldNames.push_back(BITPOSITION_COL);
        }
		if (m_powerDataPointEntityKey == -1)
        {
            fieldNames.push_back(DATAPOINTENTITYKEY_COL);
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("AtsPowerDataHelper data not fully specified. AtsPowerDataHelper cannot be written to database",
                                                         fieldNames));
        }

        // Write the AtsPowerDataHelper to the database

        // Need to check if this is a new AtsPowerDataHelper or an existing AtsPowerDataHelper - 
        // as a new AtsPowerDataHelper will be inserted, while an existing AtsPowerDataHelper will be updated
        if (m_isNew) // This is a new AtsPowerDataHelper
        {
            addNewAtsPowerData();
        }
        else // This is an existing AtsPowerDataHelper
        {
            modifyExistingAtsPowerData();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void AtsPowerDataHelper::deleteAtsPowerData(bool cascade)
    {
        FUNCTION_ENTRY("deleteAtsPowerData");

        TA_ASSERT(!m_isNew, "This AtsPowerDataHelper does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        std::ostringstream sql;
        
        if (!cascade)
        {
			// Nothing to check
        }

        // Good to go...
        
        sql.str("");
        sql << "delete AT_POWER_ENTITY_MAP where " << KEY_COL << " = " << m_idKey;
        databaseConnection->executeModification(sql.str());

        FUNCTION_EXIT;
    }


    void AtsPowerDataHelper::modifyExistingAtsPowerData() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingAtsPowerData");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "AtsPowerDataHelper id=%ul already exists. It's data will be updated.",m_idKey);
    
        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        std::ostringstream sql;
        sql << "update AT_POWER_ENTITY_MAP set " 
			<< SUBSECTIONNUMBER_COL << " = "
            << m_subsectionNumber << ", " 
			<< LOCATIONKEY_COL << " = "
            << m_locationId << ", " 
			<< BITPOSITION_COL << " = "
            << m_bitPosition << ", " 
			<< DATAPOINTENTITYKEY_COL << " = "
            << m_powerDataPointEntityKey << " " 
            << " where " << KEY_COL << " = " << m_idKey;

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        databaseConnection->executeModification(sql.str().c_str());

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void AtsPowerDataHelper::addNewAtsPowerData() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewAtsPowerData");

		unsigned long newKey = getNextKey();

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "AtsPowerDataHelper %ul is a new AtsPowerDataHelper. It will be added to the database.",newKey);

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);

		std::ostringstream sql;
        sql << "insert into AT_POWER_ENTITY_MAP (" 
            << KEY_COL << ", "
			<< LOCATIONKEY_COL << ", "
			<< BITPOSITION_COL << ", "
			<< SUBSECTIONNUMBER_COL << ", " 
			<< DATAPOINTENTITYKEY_COL << ") " 
            << "values ("
            << newKey << ", "
			<< m_locationId << ", "
			<< m_bitPosition << ", "
            << m_subsectionNumber << ", " 
            << m_powerDataPointEntityKey << ")";

        databaseConnection->executeModification(sql.str());

		m_idKey = newKey; 
        
        FUNCTION_EXIT;
    }

    unsigned long AtsPowerDataHelper::getNextKey()
	{
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// Get a suitable primary key generated for this new entry
		std::ostringstream sql;
        sql << "SELECT ATPOEM_SEQ.NEXTVAL FROM DUAL";

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

