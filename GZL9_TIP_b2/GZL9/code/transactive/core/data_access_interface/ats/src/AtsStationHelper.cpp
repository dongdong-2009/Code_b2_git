/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/core/data_access_interface/ats/src/Attic/AtsStationHelper.cpp,v $
  * @author:  Jiang Haipeng
  * @version: $Revision: 1.1.2.6.16.1 $
  *
  * Last modification: $Date: 2007/03/21 06:59:37 $
  * Last modified by:  $Author: changxing $
  *
  * <description>
  *
  */

// AtsStationHelper.cpp: implementation of the AtsStationHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "AtsStationHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
using namespace TA_Base_Core;

namespace TA_IRS_Core
{
	const std::string AtsStationHelper::STATION_ID_COL = "station_id";	
	const std::string AtsStationHelper::KEY_COL = "atslma_id";
	const std::string AtsStationHelper::LOCATION_COL = "location";
	const std::string AtsStationHelper::ATSSTATION_TBL = "at_station_location_map";

	AtsStationHelper::AtsStationHelper()
		:   m_isNew(true),
		m_isValidData(false),
		m_location(),
		m_key(-1)
	{
		
	}
	
	AtsStationHelper::AtsStationHelper(unsigned long key ) 
        : m_isValidData( false ),
		m_key( key ),
		m_isNew( true )
	{
		
	}

	AtsStationHelper::AtsStationHelper(unsigned long location, const std::string& stationID)
        : m_isValidData( true ),
		m_key(-1),
		m_isNew( true ),
		m_location(location),
		m_stationID(stationID)
	{

	}

	AtsStationHelper::~AtsStationHelper()
	{
		
	}
	
	AtsStationHelper::AtsStationHelper(const unsigned long row, TA_Base_Core::IData& data)
		:   m_isNew(false),
		m_isValidData(false),
		m_location(),
		m_key(-1),
		m_stationID()
	{
		FUNCTION_ENTRY("AtsStationHelper ctor(const unsigned long row, TA_Base_Core::IData& data)");
		reloadUsing(row, data);
		FUNCTION_EXIT; 
	}
	
	
	unsigned long AtsStationHelper::getKey()
	{
		FUNCTION_ENTRY("getOrder");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		
		FUNCTION_EXIT;
		return this->m_key;
	}
	
	std::string AtsStationHelper::getName()
	{
		FUNCTION_ENTRY("getName");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		
		FUNCTION_EXIT;
		return this->m_stationID;
	}
	
	std::string AtsStationHelper::getStationId()
	{
		FUNCTION_ENTRY("getStationId");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		
		FUNCTION_EXIT;
		return this->m_stationID;
	}
	
	unsigned long AtsStationHelper::getLocation()
	{
		FUNCTION_ENTRY("getLocation");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		
		FUNCTION_EXIT;
		return this->m_location;
	}
	
	void AtsStationHelper::reload()
	{
		FUNCTION_ENTRY("reload");
		
		// Ensure the Radio Subscriber has already been written to the database. It doesn't make
		// any sense to reload an Radio Subscriber that doesn't exist in the database.
		TA_ASSERT(!m_isNew, "This Ats Subscriber does not yet exist in the database. " 
			"Therefore it cannot be reloaded.");
		
		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
		if (databaseConnection == NULL)
		{
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
		}
		
		// create the SQL string to retrieve the data of the radio subscriber based upon the key
// 		std::ostringstream sql;
// 		sql << "select " 
// 			<< STATION_ID_COL << ", "
// 			<< KEY_COL << ", "
// 			<< LOCATION_COL 
// 			<< " from " 
// 			<< ATSSTATION_TBL
// 			<< " where " << KEY_COL << " = " << m_key;
		
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_STATION_SELECT_200009,m_key);
		
		// Set up the column Names vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(STATION_ID_COL);
		columnNames.push_back(KEY_COL);
		columnNames.push_back(LOCATION_COL);
		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
		const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
		
		if (0 == data->getNumRows()) 
		{
			// radio subscriber record not found
			std::ostringstream reason;
			reason << "No record found for Radio emergency regroup member" << std::endl;
			
			TA_THROW(TA_Base_Core::DataException(reason.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "") );
		}
		
		TA_ASSERT(data->getNumRows() == 1, "Invalid Ats of Radio Subscriber records."); 
		
		reloadUsing(0, *data);
		
		FUNCTION_EXIT;  	
	}
	
	void AtsStationHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)	
	{
		FUNCTION_ENTRY("reloadUsing");
		
		// Assign the data as appropriate to the member variables.
		// These calls can throw DataExceptions, as documented in the comment of this method.
        this->m_stationID = data.getStringData(row, STATION_ID_COL);
		if (this->m_stationID.size() < 1)
		{
			TA_THROW(TA_Base_Core::DataException("The length of the Array is zero",
				TA_Base_Core::DataException::NO_VALUE, "Can not get the subscriber information from database" ) );
		}
		
		this->m_key = data.getUnsignedLongData(row, KEY_COL);
		this->m_location = data.getUnsignedLongData(row, LOCATION_COL);
		
		// Need to record that we now have valid data
		m_isValidData = true;
		
		FUNCTION_EXIT;
	}	

	void AtsStationHelper::insert()
	{
		FUNCTION_ENTRY("insert");
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }	
		
		unsigned long nextAvailableKey = getNextSequenceNumber();
		std::string strStationID;
		if (m_stationID.empty())
		{
			strStationID = "' '";
		}else{
			strStationID = databaseConnection->escapeInsertString(m_stationID);
		}
		//insert  fisrt.
// 		std::ostringstream sql;
// 		sql << "insert into " 
// 			<<  AtsStationHelper::ATSSTATION_TBL
// 			<< " (" 
// 			<< AtsStationHelper::KEY_COL << ", " 
// 			<< AtsStationHelper::LOCATION_COL << ", " 
// 			<< AtsStationHelper::STATION_ID_COL 
// 			<< ") "
// 			<< "values ("
// 			<< nextAvailableKey << ", " 
// 			<< m_location << ", "
// 			<< strStationID
// 			<< ")";


		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_STATION_INSERT_200004,nextAvailableKey,m_location,strStationID);
			
		databaseConnection->executeModification(strSql);

		m_isNew = false;
		m_isValidData = true;
		
		FUNCTION_EXIT;
	}

    unsigned long AtsStationHelper::getNextSequenceNumber()
    {
        const std::string NEXTVAL("NEXTVAL");
		
        // Create a query to get the next number in the sequence from the database
        //const std::string sql("select ATSLMA_SEQ.NEXTVAL from DUAL");
		
		
		
        std::vector<std::string> columnNames;
        columnNames.push_back(NEXTVAL);
		
        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_STATION_SELECT_200005);
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
        
        // Should always be single row result
        TA_ASSERT(data->getNumRows() == 1, "Invalid result from next sequence number query.");
		
        // return the next auto-number 
        return data->getUnsignedLongData(0, NEXTVAL);
    }

	void AtsStationHelper::update()
	{
		FUNCTION_ENTRY("update");
		
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }	
		
		//update regroup message.
// 		std::ostringstream sql;
// 		sql << "update " << AtsStationHelper::ATSSTATION_TBL
// 			<< " set " 
// 			<< AtsStationHelper::LOCATION_COL << " = " << m_location << " "
// 			<< AtsStationHelper::STATION_ID_COL << " = " << databaseConnection->escapeInsertString(m_stationID)
// 			<< " where " << AtsStationHelper::KEY_COL << " = " << m_key;
		
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_STATION_UPDATE_200006,m_location,databaseConnection->escapeInsertString(m_stationID),m_key);		
		databaseConnection->executeModification(strSql);
		FUNCTION_EXIT;
	}

	bool AtsStationHelper::isRowDeleted(unsigned long key)
	{
        FUNCTION_ENTRY("isRowDeleted");
		
        // Ensure the Radio Subscriber has already been written to the database. It doesn't make
        // any sense to reload an Radio Subscriber that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This Radio Subscriber does not yet exist in the database. " 
			"Therefore it cannot be reloaded.");
		
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE,""));
        }
		
        // create the SQL string to retrieve the data of the radio subscriber based upon the key
//         std::ostringstream sql;
// 		sql << "select " 
// 			<< KEY_COL 
//             << " from " << ATSSTATION_TBL
// 			<< " where " << KEY_COL << "=" << key;

		
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_STATION_SELECT_200007);
		
		
        // Set up the column Names vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
		
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
		
        if (0 == data->getNumRows()) 
        {
			FUNCTION_EXIT; 
			return true;
        }
		
        FUNCTION_EXIT;  	
		return false;
	}

	void AtsStationHelper::deleteObject()
	{
        FUNCTION_ENTRY("deletePaZone");
		if (isRowDeleted(m_key)) //row is deleted alreay, no need to delete again.
		{
			FUNCTION_EXIT;
			return;
		}
		
        TA_ASSERT(!m_isNew, "This PaZone does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
//         std::ostringstream sql;
//         
//         // Good to go...
//         sql.str("");
//         sql << "delete " << ATSSTATION_TBL << " where " << KEY_COL << " = " << m_key;


		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_STATION_DELETE_200008);
		
        databaseConnection->executeModification(strSql);
		
        FUNCTION_EXIT;
	}
	
	void AtsStationHelper::save(bool toInsert)
	{
		if (toInsert)
		{
			insert();
		}else{
			update();
		}
	}

	void AtsStationHelper::setStationId(const std::string& id)
	{
		m_stationID = id;
	}

	void AtsStationHelper::setLocationKey(unsigned long location)
	{
		m_location = location;
	}

	//*******IConfigItem interface************************

	bool AtsStationHelper::isNew()
	{
		return m_isNew;
	}
	
    void AtsStationHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );
		
        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new VideoStationId");
		
        m_isValidData = false;
		
        FUNCTION_EXIT;
    }
}