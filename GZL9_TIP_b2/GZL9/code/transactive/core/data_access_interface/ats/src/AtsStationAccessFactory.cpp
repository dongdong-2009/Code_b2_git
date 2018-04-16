/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/core/data_access_interface/ats/src/Attic/AtsStationAccessFactory.cpp,v $
  * @author:  Jiang Haipeng
  * @version: $Revision: 1.1.2.5 $
  *
  * Last modification: $Date: 2006/06/06 10:15:02 $
  * Last modified by:  $Author: haipeng $
  *
  * <description>
  *
  */



#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
//#pragma warning(disable:4259)
#endif

#include "AtsStationAccessFactory.h"
#include "AtsStationHelper.h"
#include "AtsStation.h"
#include "ConfigAtsStation.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

 

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
 
#include "core/data_access_interface/src/SQLCode.h"
 

using namespace TA_Base_Core;

namespace TA_IRS_Core
{

	AtsStationAccessFactory* AtsStationAccessFactory::m_instance = 0;

	AtsStationAccessFactory& AtsStationAccessFactory::getInstance()
	{
		if( m_instance == 0 )
		{
			m_instance = new AtsStationAccessFactory();
		}
		return *m_instance;
	}
	
	AtsStationAccessFactory::AtsStationAccessFactory()
	{
		
	}
	
	
	AtsStationAccessFactory::~AtsStationAccessFactory()
	{
		
	}

	void AtsStationAccessFactory::removeInstance( )
	{
		if ( m_instance != NULL )
		{
			delete m_instance;
			m_instance = NULL;
		}
	}

	
	
	/**
	* Gets all stations configured in the db.
	* The caller must delete all objects.
	* 
	* @exception DataException if there was an error getting the data
	* @exception DatabaseException if there was an error connecting to the database
	*/
	std::vector<IAtsStation*> AtsStationAccessFactory::getAllStations()
	{
		FUNCTION_ENTRY("getAllStations");
		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
		if (databaseConnection == NULL)
		{
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE, ""));
		}
		
		// create the SQL string to retrieve all the records from the table 
// 		std::ostringstream sql;
// 		sql << "select " 
// 			<< AtsStationHelper::KEY_COL << ", " 
// 			<< AtsStationHelper::LOCATION_COL << ", " 
// 			<< AtsStationHelper::STATION_ID_COL
// 			<< " from " << AtsStationHelper::ATSSTATION_TBL;


		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_STATION_SELECT_200001);
		
		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		
		columnNames.push_back(AtsStationHelper::KEY_COL);
		columnNames.push_back(AtsStationHelper::LOCATION_COL);
		columnNames.push_back(AtsStationHelper::STATION_ID_COL);
		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
		
		std::vector<IAtsStation *> rows;
		
		do
		{
			for (unsigned long i = 0; i < data->getNumRows(); ++i )
			{   
				rows.push_back(new AtsStation(i, *data));
			}
			
			delete data;
			data = NULL;
		}
		while(databaseConnection->moreData(data) == true);
		
		FUNCTION_EXIT;
		return rows;	
	}

	/**
	* Gets all stations configured in the db.
	* The caller must delete all objects.
	* 
	* @exception DataException if there was an error getting the data
	* @exception DatabaseException if there was an error connecting to the database
	*/
	void AtsStationAccessFactory::getAllStations(std::vector<IConfigAtsStation*> & configStation)
	{
		FUNCTION_ENTRY("getAllStations");
		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
		if (databaseConnection == NULL)
		{
			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE, ""));
		}
		
		// create the SQL string to retrieve all the records from the table 
// 		std::ostringstream sql;
// 		sql << "select " 
// 			<< AtsStationHelper::KEY_COL << ", " 
// 			<< AtsStationHelper::LOCATION_COL << ", " 
// 			<< AtsStationHelper::STATION_ID_COL
// 			<< " from " << AtsStationHelper::ATSSTATION_TBL;

		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_STATION_SELECT_200001);
		
		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		
		columnNames.push_back(AtsStationHelper::KEY_COL);
		columnNames.push_back(AtsStationHelper::LOCATION_COL);
		columnNames.push_back(AtsStationHelper::STATION_ID_COL);
		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
		
		configStation.clear();
		do
		{
			for (unsigned long i = 0; i < data->getNumRows(); ++i )
			{   
				configStation.push_back(new ConfigAtsStation(i, *data));
			}
			
			delete data;
			data = NULL;
		}
		while(databaseConnection->moreData(data) == true);
		
		FUNCTION_EXIT;
	}

	bool AtsStationAccessFactory::stationIdExists(const std::string & stationID, unsigned long pkey)
	{
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
// 			<< AtsStationHelper::STATION_ID_COL << ", "
// 			<< AtsStationHelper::KEY_COL << ", "
// 			<< AtsStationHelper::LOCATION_COL 
// 			<< " from " 
// 			<< AtsStationHelper::ATSSTATION_TBL
// 			<< " where " << AtsStationHelper::KEY_COL << " = " << pkey 
// 			<< " and " << AtsStationHelper::STATION_ID_COL << " = " << "'" << stationID << "'";

		
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_STATION_SELECT_200002,pkey,stationID);
		
		// Set up the column Names vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(AtsStationHelper::STATION_ID_COL);
		columnNames.push_back(AtsStationHelper::KEY_COL);
		columnNames.push_back(AtsStationHelper::LOCATION_COL);
		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
		const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
		
		if (data->getNumRows() <= 0) 
		{
			// radio subscriber record not found
			return false;
		}
		return true;
	}

	bool AtsStationAccessFactory::locationIdExists(unsigned long locationID, unsigned long pkey)
	{
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
// 			<< AtsStationHelper::STATION_ID_COL << ", "
// 			<< AtsStationHelper::KEY_COL << ", "
// 			<< AtsStationHelper::LOCATION_COL 
// 			<< " from " 
// 			<< AtsStationHelper::ATSSTATION_TBL
// 			<< " where " << AtsStationHelper::KEY_COL << " = " << pkey 
// 			<< " and " << AtsStationHelper::LOCATION_COL << " = "  << locationID ;
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_STATION_SELECT_200003,pkey,locationID);
		
		// Set up the column Names vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(AtsStationHelper::STATION_ID_COL);
		columnNames.push_back(AtsStationHelper::KEY_COL);
		columnNames.push_back(AtsStationHelper::LOCATION_COL);
		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
		const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
		
		if (data->getNumRows() <= 0) 
		{
			// radio subscriber record not found
			return false;
		}
		return true;
	}

	/**
	* Creates a new station. The caller must delete the object.
	*/
	IConfigAtsStation* AtsStationAccessFactory::createStation(unsigned long locationID, const std::string & stationID)
	{
		ConfigAtsStation * station = new ConfigAtsStation();
		station->setLocationKey(locationID);
		station->setStationId(stationID);
		//station->addObject();
		return station;
	}

	/**
	* Creates a new station. The caller must delete the object.
	*/
	IConfigAtsStation* AtsStationAccessFactory::createStation()
	{
		ConfigAtsStation * station = new ConfigAtsStation();
		station->setLocationKey(station->getNextAvailableIdentifier());
		std::string strStationID = "";
		station->setStationId(strStationID);
		//station->addObject();
		return station;
	}
	
	/**
	* Creates a new station. The caller must delete the object.
	*/
	IConfigAtsStation* AtsStationAccessFactory::createStation(const std::string & stationID)
	{
		ConfigAtsStation * station = new ConfigAtsStation();
		station->setLocationKey(station->getNextAvailableIdentifier());
		station->setStationId(stationID);
		//station->addObject();
		return station;
	}
	/**
	* edit a new station. The caller must delete the object.
	*/
	IConfigAtsStation * AtsStationAccessFactory::editStation(unsigned long key, unsigned long locationID, const std::string & stationID)
	{
		ConfigAtsStation * atsStation = new ConfigAtsStation(key);
		atsStation->setLocationKey(locationID);
		atsStation->setStationId(stationID);
		//atsStation->editThisObject();
		return atsStation;
	}

	/**
	* delete a new station. The caller must delete the object.
	*/
	IConfigAtsStation * AtsStationAccessFactory::deleteStation(unsigned long key)
	{
		ConfigAtsStation * station = new ConfigAtsStation(key);
		//station->deleteThisObject();
		return station;
	}
	
}