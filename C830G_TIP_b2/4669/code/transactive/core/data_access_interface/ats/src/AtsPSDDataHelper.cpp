/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/AtsPSDDataHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * AtsPSDDataHelper is an object that is held by AtsPSDDataHelper and ConfigAtsPSDData objects. 
  * It contains all data used by AtsPSDDatas, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsPSDDataHelper and ConfigAtsPSDData.
  */


#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4503)	// decorated name length exceeded, name was truncated
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/ats/src/AtsPSDDataHelper.h"
#include "core/data_access_interface/ats/src/AtsPSDDataFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
    static const std::string KEY_COL					= "ATPEMA_ID";
    static const std::string STATIONID_COL				= "STATION_ID";
	static const std::string PLATFORMKEY_COL			= "PLATFORM_KEY";
	static const std::string PLATFORMID_COL				= "PLATFORM_ID";
    static const std::string PLATFORMNAME_COL			= "PLATFORM_NAME";
    static const std::string TIS_PLATFORMID_COL         = "TIS_PLATFORM_ID";
	static const std::string MSSZONE_COL				= "MSS_ZONE";
	static const std::string PSDNUMBER_COL				= "PSD_NUMBER";
	static const std::string DATAPOINTENTITYKEY_COL		= "DATA_POINT_ENTITY_KEY";
	static const std::string DATAPOINTTYPE_COL			= "DATA_POINT_TYPE";
	static const std::string DATAPOINTNAMEOFTYPE_COL	= "DATA_POINT_NAMEOF_TYPE";
	
    static std::string toUpper(const std::string& in)
    {
        std::string out(in);
        std::transform(out.begin(), out.end(), out.begin(), toupper);
        return out;
    }

	AtsPSDDataHelper::AtsPSDDataHelper( const AtsPSDDataHelper& theAtsPSDDataHelper)
		: m_idKey(-1),
		  m_psdDataPointEntityKey(theAtsPSDDataHelper.m_psdDataPointEntityKey),
		  m_psdDataPointType(theAtsPSDDataHelper.m_psdDataPointType),
		  m_psdDataPointNameOfType(theAtsPSDDataHelper.m_psdDataPointNameOfType),
		  m_psdNumber(theAtsPSDDataHelper.m_psdNumber),
		  m_platformKey(theAtsPSDDataHelper.m_platformKey),
		  m_platformId(theAtsPSDDataHelper.m_platformId),
		  m_platformName(theAtsPSDDataHelper.m_platformName),
          m_tisPlatformId(theAtsPSDDataHelper.m_tisPlatformId),
		  m_stationId(theAtsPSDDataHelper.m_stationId),
		  m_name(theAtsPSDDataHelper.m_name),
          m_isValidData(false),
          m_isNew(true) 
    {	  
	}


    AtsPSDDataHelper::AtsPSDDataHelper(const unsigned long idKey)
        : m_idKey(idKey),
		  m_psdDataPointEntityKey(-1),
		  m_psdDataPointType(""),
		  m_psdDataPointNameOfType(""),
		  m_psdNumber(-1),
		  m_platformKey(-1),
		  m_platformId(-1),
		  m_platformName(""),
          m_tisPlatformId(-1),
		  m_stationId(-1),
		  m_name(""),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    AtsPSDDataHelper::AtsPSDDataHelper()
        : m_idKey(-1),
		  m_psdDataPointEntityKey(-1),
		  m_psdDataPointType(""),
		  m_psdDataPointNameOfType(""),
		  m_psdNumber(-1),
		  m_platformKey(-1),
		  m_platformId(-1),
		  m_platformName(""),
          m_tisPlatformId(-1),
		  m_stationId(-1),
		  m_name(""),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    AtsPSDDataHelper::AtsPSDDataHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_idKey(-1),
		  m_psdDataPointEntityKey(-1),
		  m_psdDataPointType(""),
		  m_psdDataPointNameOfType(""),
		  m_psdNumber(-1),
		  m_platformKey(-1),
		  m_platformId(-1),
		  m_platformName(""),
          m_tisPlatformId(-1),
		  m_stationId(-1),
		  m_name(""),
          m_isValidData(false),
          m_isNew(false)
    {
        m_idKey = data.getUnsignedLongData(row, KEY_COL);

        reloadUsing(row, data);
    }


    AtsPSDDataHelper::~AtsPSDDataHelper()
    {
    }

	unsigned long AtsPSDDataHelper::getKey()
	{
		return m_idKey;
	}

	unsigned long AtsPSDDataHelper::getPSDDataPointEntityKey()
	{
        FUNCTION_ENTRY("getPSDDataPointEntityKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_psdDataPointEntityKey;
    }

	std::string AtsPSDDataHelper::getPSDDataPointType()
	{
        FUNCTION_ENTRY("getPSDDataPointType" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_psdDataPointType;
    }

	std::string AtsPSDDataHelper::getPSDDataPointNameOfType()
	{
        FUNCTION_ENTRY("getPSDDataPointNameOfType" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_psdDataPointNameOfType;
    }

	unsigned long AtsPSDDataHelper::getPSDNumber()
	{
        FUNCTION_ENTRY("getPSDNumber" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_psdNumber;
    }

	unsigned long AtsPSDDataHelper::getPlatformKey()
    {
        FUNCTION_ENTRY("getPlatformKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_platformKey;
    }

	unsigned long AtsPSDDataHelper::getPlatformId()
    {
        FUNCTION_ENTRY("getPlatformId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_platformId;
    }

	std::string AtsPSDDataHelper::getPlatformName()
	{
        FUNCTION_ENTRY("getPlatformName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_platformName;
    }

    unsigned char AtsPSDDataHelper::getTisPlatformId()
    {
        FUNCTION_ENTRY("getTisPlatformId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_tisPlatformId;
    }

	unsigned long AtsPSDDataHelper::getStationId()
    {
        FUNCTION_ENTRY("getStationId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_stationId;
    }

	std::string AtsPSDDataHelper::getMSSZone()
	{
        FUNCTION_ENTRY("getMSSZone" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
		return m_mssZone;
    }

    void AtsPSDDataHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new AtsPSDDataHelper");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

	void AtsPSDDataHelper::setPSDDataPointEntityKey(unsigned long key)
	{
        FUNCTION_ENTRY("setPSDDataPointEntityKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_psdDataPointEntityKey = key;
        FUNCTION_EXIT;
    }

	void AtsPSDDataHelper::setPSDDataPointType(const std::string& type)
	{
        FUNCTION_ENTRY("setPSDDataPointType" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_psdDataPointType = type;
        FUNCTION_EXIT;
    }

	void AtsPSDDataHelper::setPSDDataPointNameOfType(const std::string& nameOfType)
	{
        FUNCTION_ENTRY("setPSDDataPointNameOfType" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_psdDataPointNameOfType = nameOfType;
        FUNCTION_EXIT;
    }
		
	void AtsPSDDataHelper::setPlatformKey(unsigned long key)
	{
        FUNCTION_ENTRY("setPlatformKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_platformKey = key;
        FUNCTION_EXIT;
    }
		
	void AtsPSDDataHelper::setPSDNumber(int psdNumber)
	{
        FUNCTION_ENTRY("setPSDNumber" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_psdNumber = psdNumber;
        FUNCTION_EXIT;
    }


    void AtsPSDDataHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the AtsPSDDataHelper has already been written to the database. It doesn't make
        // any sense to reload an AtsPSDDataHelper that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This AtsPSDDataHelper does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);


        // create the SQL string to retrieve the data of the AtsPSDDataHelper
        // based upon the key
        std::ostringstream sql;
		sql << "select " << STATIONID_COL << ", " << PLATFORMKEY_COL << ", " << PLATFORMID_COL << ", ";
		sql << PLATFORMNAME_COL << ", " << TIS_PLATFORMID_COL << ", ";
		sql << MSSZONE_COL << ", " << PSDNUMBER_COL << ", " << DATAPOINTENTITYKEY_COL << ", ";
		sql << DATAPOINTTYPE_COL << ", " << DATAPOINTNAMEOFTYPE_COL;
		sql << " from AT_PSD_ENTITY_MAP_V where ";
		sql << KEY_COL << " = " << m_idKey;
 
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(STATIONID_COL);
		columnNames.push_back(PLATFORMKEY_COL);
        columnNames.push_back(PLATFORMID_COL);
        columnNames.push_back(PLATFORMNAME_COL);
        columnNames.push_back(TIS_PLATFORMID_COL);
		columnNames.push_back(MSSZONE_COL);
        columnNames.push_back(PSDNUMBER_COL);
        columnNames.push_back(DATAPOINTENTITYKEY_COL);
        columnNames.push_back(DATAPOINTTYPE_COL);
		columnNames.push_back(DATAPOINTNAMEOFTYPE_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        if (0 == data->getNumRows()) 
        {
            // AtsPSDDataHelper record not found   
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PSD Entity Map Key = " << m_idKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PSD Entity Map Key" ) );
        }
        
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_platformKey = data->getUnsignedLongData(0, PLATFORMKEY_COL),
		m_platformId = data->getUnsignedLongData(0, PLATFORMID_COL),
		m_platformName = data->getStringData(0, PLATFORMNAME_COL),
        m_tisPlatformId = static_cast<unsigned char> (data->getUnsignedLongData(0, TIS_PLATFORMID_COL)),
		m_stationId = data->getUnsignedLongData(0, STATIONID_COL),
		m_mssZone = data->getStringData(0, MSSZONE_COL),
		m_psdNumber = data->getIntegerData(0, PSDNUMBER_COL),
		m_psdDataPointEntityKey = data->getUnsignedLongData(0, DATAPOINTENTITYKEY_COL),
		m_psdDataPointNameOfType = data->getStringData(0, DATAPOINTNAMEOFTYPE_COL),
		m_psdDataPointType = data->getStringData(0, DATAPOINTTYPE_COL);   

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsPSDDataHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_platformKey = data.getUnsignedLongData(row, PLATFORMKEY_COL),
        m_platformId = data.getUnsignedLongData(row, PLATFORMID_COL),						 
		m_platformName = data.getStringData(row, PLATFORMNAME_COL),
        m_tisPlatformId = static_cast<unsigned char> (data.getUnsignedLongData(row, TIS_PLATFORMID_COL)),
		m_stationId = data.getUnsignedLongData(row, STATIONID_COL),
		m_mssZone = data.getStringData(row, MSSZONE_COL),
		m_psdNumber = data.getIntegerData(row, PSDNUMBER_COL),
		m_psdDataPointEntityKey = data.getUnsignedLongData(row, DATAPOINTENTITYKEY_COL),
		m_psdDataPointNameOfType = data.getStringData(row, DATAPOINTNAMEOFTYPE_COL),
		m_psdDataPointType = data.getStringData(row, DATAPOINTTYPE_COL);  

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AtsPSDDataHelper::writeAtsPSDDataData() throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeAtsPSDDataData" );     

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
		if (m_platformKey == -1)
        {
            fieldNames.push_back(PLATFORMKEY_COL);
        }
		if (m_psdNumber == -1)
        {
            fieldNames.push_back(PSDNUMBER_COL);
        }
		if (m_psdDataPointEntityKey == -1)
        {
            fieldNames.push_back(DATAPOINTENTITYKEY_COL);
        }
		if (m_psdDataPointType == "")
        {
            fieldNames.push_back(DATAPOINTTYPE_COL);
        }
		if (m_psdDataPointNameOfType == "")
        {
            fieldNames.push_back(DATAPOINTNAMEOFTYPE_COL);
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("AtsPSDDataHelper data not fully specified. AtsPSDDataHelper cannot be written to database",
                                                         fieldNames));
        }

        // Write the AtsPSDDataHelper to the database

        // Need to check if this is a new AtsPSDDataHelper or an existing AtsPSDDataHelper - 
        // as a new AtsPSDDataHelper will be inserted, while an existing AtsPSDDataHelper will be updated
        if (m_isNew) // This is a new AtsPSDDataHelper
        {
            addNewAtsPSDData();
        }
        else // This is an existing AtsPSDDataHelper
        {
            modifyExistingAtsPSDData();
        }
        // Now that the data has been written, we need to refresh the data from the view.
        m_isValidData = false;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void AtsPSDDataHelper::deleteAtsPSDData(bool cascade)
    {
        FUNCTION_ENTRY("deleteAtsPSDData");

        TA_ASSERT(!m_isNew, "This AtsPSDDataHelper does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        std::ostringstream sql;
        
        if (!cascade)
        {
			// Nothing to check
        }

        // Good to go...
        
        sql.str("");
        sql << "delete AT_PSD_ENTITY_MAP where " << KEY_COL << " = " << m_idKey;
        databaseConnection->executeModification(sql.str());

        FUNCTION_EXIT;
    }


    void AtsPSDDataHelper::modifyExistingAtsPSDData() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingAtsPSDData");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "AtsPSDDataHelper id=%ul already exists. It's data will be updated.",m_idKey);
    
        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        std::ostringstream sql;
        sql << "update AT_PSD_ENTITY_MAP set " 
			<< PLATFORMKEY_COL << " = "
            << m_platformKey << ", " 
			<< PSDNUMBER_COL << " = "
            << m_psdNumber << ", " 
			<< DATAPOINTENTITYKEY_COL << " = "
            << m_psdDataPointEntityKey << ", " 
			<< DATAPOINTTYPE_COL << " = '"
            << m_psdDataPointType << "', " 
			<< DATAPOINTNAMEOFTYPE_COL << " = '"
            << m_psdDataPointNameOfType << "'"
            << " where " << KEY_COL << " = " << m_idKey;

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        databaseConnection->executeModification(sql.str().c_str());

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void AtsPSDDataHelper::addNewAtsPSDData() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewAtsPSDData");

		unsigned long newKey = getNextKey();

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "AtsPSDDataHelper %ul is a new AtsPSDDataHelper. It will be added to the database.",newKey);

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);

		std::ostringstream sql;
        sql << "insert into AT_PSD_ENTITY_MAP (" 
            << KEY_COL << ", "
			<< PLATFORMKEY_COL << ", " 
			<< PSDNUMBER_COL << ", " 
			<< DATAPOINTENTITYKEY_COL << ", " 
			<< DATAPOINTTYPE_COL << ", " 
			<< DATAPOINTNAMEOFTYPE_COL << ") "
            << "values ("
            << newKey << ", " 
			<< m_platformKey << ", " 
            << m_psdNumber << ", " 
            << m_psdDataPointEntityKey << ", '" 
            << m_psdDataPointType << "', '" 
            << m_psdDataPointNameOfType << "')";

        databaseConnection->executeModification(sql.str());

		m_idKey = newKey; 
        
        FUNCTION_EXIT;
    }

    unsigned long AtsPSDDataHelper::getNextKey()
	{
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		// Get a suitable primary key generated for this new entry
		std::ostringstream sql;
        sql << "SELECT ATPEMA_SEQ.NEXTVAL FROM DUAL";

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

