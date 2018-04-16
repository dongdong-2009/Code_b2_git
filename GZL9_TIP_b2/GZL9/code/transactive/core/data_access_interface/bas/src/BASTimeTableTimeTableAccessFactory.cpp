
#include "core/data_access_interface/bas/src/BASTimeTableTimeTableAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

namespace TA_IRS_Core
{
	const std::string BASTimeTableTimeTableAccessFactory::TableName = "BA_TIMETABLE_TIMETABLE";
	const std::string BASTimeTableTimeTableAccessFactory::ColumnName_Pkey = "PKEY";
	const std::string BASTimeTableTimeTableAccessFactory::ColumnName_locationKey = "LOCATIONKEY";
	const std::string BASTimeTableTimeTableAccessFactory::ColumnName_TypeName = "TYPENAME";
    const std::string BASTimeTableTimeTableAccessFactory::ColumnName_DisplayName = "DISPLAYNAME";

	BASTimeTableTimeTableAccessFactory::BASTimeTableTimeTableAccessFactory(void)
	{
	}

	BASTimeTableTimeTableAccessFactory::~BASTimeTableTimeTableAccessFactory(void)
	{
	}

	void BASTimeTableTimeTableAccessFactory::getAllLocationAndTimeTableType(LocationAndTimeTableTypeMap& locationAndTypeMap)
	{
		FUNCTION_ENTRY("getAllLocationAndTimeTableType");
		using namespace TA_Base_Core;

		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(BasTimetable_Cd, Read);

		/*std::ostringstream ss;
		ss << "SELECT " << ColumnName_Pkey << " , "
			<< ColumnName_locationKey << " , "<< ColumnName_TypeName << " "
			<< " FROM " << TableName;
		std::string strSql = ss.str();*/
		/*SELECT PKEY,LOCATIONKEY,TYPENAME FROM BA_TIMETABLE_TIMETABLE*/
		//
		TA_Base_Core::SQLStatement sql;

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back( ColumnName_Pkey );
		columnNames.push_back( ColumnName_locationKey );
		columnNames.push_back( ColumnName_TypeName );
        columnNames.push_back( ColumnName_DisplayName );

		databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_TIMETABLE_SELECT_97002);
		//IData* data = databaseConnection->executeQuery( strSql, columnNames );
		IData* data = databaseConnection->executeQuery( sql, columnNames );
		locationAndTypeMap.clear();
		do 
		{
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				unsigned long pkey = data->getIntegerData(i, ColumnName_Pkey);
				unsigned long locationKey = data->getIntegerData(i, ColumnName_locationKey);
				std::string typeName = data->getStringData(i, ColumnName_TypeName);
                std::string displayName = data->getStringData(i, ColumnName_DisplayName);
				locationAndTypeMap[pkey] = LocationAndTimeTableType(pkey, locationKey, typeName, displayName);
			}
			delete data;
			data = NULL;
		} while (databaseConnection->moreData(data));

		FUNCTION_EXIT;
	}

	void BASTimeTableTimeTableAccessFactory::getLocationAndTimeTableTypeByLocation(LocationAndTimeTableTypeMap& locationAndTypeMap, unsigned long locationKey)
	{
		FUNCTION_ENTRY("getLocationAndTimeTableTypeByLocation");
		using namespace TA_Base_Core;

		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(BasTimetable_Cd, Read);

		/*std::ostringstream ss;
		ss << "SELECT " << ColumnName_Pkey << " , "
			<< ColumnName_locationKey << " , "<< ColumnName_TypeName << " "
			<< " FROM " << TableName
			<< " WHERE " << ColumnName_locationKey << " = " << locationKey;
		std::string strSql = ss.str();*/
		/*SELECT PKEY,LOCATIONKEY,TYPENAME FROM BA_TIMETABLE_TIMETABLE WHERE LOCATIONKEY=%s*/
		TA_Base_Core::SQLStatement sql;

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back( ColumnName_Pkey );
		columnNames.push_back( ColumnName_locationKey );
		columnNames.push_back( ColumnName_TypeName );
        columnNames.push_back( ColumnName_DisplayName );

		databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_TIMETABLE_SELECT_97003, locationKey);
		//IData* data = databaseConnection->executeQuery( strSql, columnNames );
		IData* data = databaseConnection->executeQuery(sql, columnNames );
		locationAndTypeMap.clear();
		do 
		{
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				unsigned long pkey = data->getIntegerData(i, ColumnName_Pkey);
				unsigned long locationKey = data->getIntegerData(i, ColumnName_locationKey);
				std::string typeName = data->getStringData(i, ColumnName_TypeName);
                std::string displayName = data->getStringData(i, ColumnName_DisplayName);
				locationAndTypeMap[pkey] = LocationAndTimeTableType(pkey, locationKey, typeName, displayName);
			}
			delete data;
			data = NULL;
		} while (databaseConnection->moreData(data));

		FUNCTION_EXIT;
	}

	unsigned long BASTimeTableTimeTableAccessFactory::getTimeTableKeyByLocationkeyAndType(unsigned long locationKey, const std::string& strType)
	{
		FUNCTION_ENTRY("getTimeTableKeyByLocationkeyAndType");
		using namespace TA_Base_Core;

		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(BasTimetable_Cd, Read);

		/*std::ostringstream ss;
		ss << "SELECT " << ColumnName_Pkey 
			<< " FROM " << TableName
			<< " WHERE " << ColumnName_locationKey << " = " << locationKey 
			<< " AND " << ColumnName_TypeName << " = " << "'" << strType << "'";
		std::string strSql = ss.str();*/
		/*SELECT PKEY FROM BA_TIMETABLE_TIMETABLE WHERE LOCATIONKEY=%s AND TYPENAME='%s'*/
		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back( ColumnName_Pkey );
		
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_TIMETABLE_SELECT_97004, locationKey, strType);

		//IData* data = databaseConnection->executeQuery( strSql, columnNames );
		IData* data = databaseConnection->executeQuery( sql, columnNames );
		unsigned long timetableKey;
		if (data->getNumRows() == 1)
		{
			timetableKey = data->getUnsignedLongData(0, ColumnName_Pkey);
		}
		else
		{
			
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "config data error in DB tableName:%s locationKey:%d TypeName:%s", TableName.c_str(), locationKey, strType.c_str());
			TA_THROW(TA_Base_Core::DatabaseException("Please check the config data in  table BA_TIMETABLE_TIMETABLE"));
		}
		FUNCTION_EXIT;
		return timetableKey;
	}

	void BASTimeTableTimeTableAccessFactory::getLocationKeyAndTypeByKey(unsigned long pkey, unsigned long& locationKey, std::string& strType)
	{
		FUNCTION_ENTRY("getLocationKeyAndTypeByKey");
		using namespace TA_Base_Core;

		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(BasTimetable_Cd, Read);

		/*std::ostringstream ss;
		ss << "SELECT " 
			<< ColumnName_locationKey << " , "<< ColumnName_TypeName << " "
			<< " FROM " << TableName
			<< " WHERE " << ColumnName_Pkey << " = " << pkey;
		std::string strSql = ss.str();*/
		/*SELECT LOCATIONKEY,TYPENAME FROM BA_TIMETABLE_TIMETABLE WHERE PKEY=%s*/
		TA_Base_Core::SQLStatement sql;

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back( ColumnName_locationKey );
		columnNames.push_back( ColumnName_TypeName );

		databaseConnection->prepareSQLStatement(sql,BA_TIMETABLE_TIMETABLE_SELECT_97005,pkey);
		//IData* data = databaseConnection->executeQuery( strSql, columnNames );
		IData* data = databaseConnection->executeQuery( sql, columnNames );
		if (data->getNumRows() != 1)
		{
			TA_THROW(TransactiveException("not found the time table data from database."))
		}

		locationKey = data->getUnsignedLongData(0, ColumnName_locationKey);
		strType = data->getStringData(0, ColumnName_TypeName);

		delete data;
		data = NULL;
		FUNCTION_EXIT;
	}
}
