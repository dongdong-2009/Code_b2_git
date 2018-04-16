
#include "core/data_access_interface/bas/src/BasStationSectionAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"


namespace TA_IRS_Core
{
	const std::string BasStationSectionAccessFactory::StationSectionTableName = "BA_TUNNEL_SECTION";
	const std::string BasStationSectionAccessFactory::ColumnName_Pkey = "PKEY";
	const std::string BasStationSectionAccessFactory::ColumnName_Name = "NAME";

	BasStationSectionAccessFactory::BasStationSectionAccessFactory(void)
	{
	}

	BasStationSectionAccessFactory::~BasStationSectionAccessFactory(void)
	{
	}

	void BasStationSectionAccessFactory::getStationSectionInfo(std::vector<StationSectionInfo>& StationSectionInfos)
	{
		FUNCTION_ENTRY("getStationSectionInfo");
		using namespace TA_Base_Core;
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);
		TA_ASSERT(NULL != databaseConnection, "can not get the database connection");

		/*std::ostringstream ss;
		ss << "SELECT " << ColumnName_Pkey << " , "
			<< ColumnName_Name  
			<< " FROM " << StationSectionTableName
			<< " ORDER BY " << ColumnName_Pkey << " ASC";
		std::string strSql = ss.str();*/
		/*SELECT PKEY,NAME FROM BA_TUNNEL_SECTION ORDER BY PKEY ASC*/
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, BA_TUNNEL_SECTION_SELECT_99001);

		//BA_TUNNEL_SECTION_SELECT_99001
		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ColumnName_Pkey);
		columnNames.push_back(ColumnName_Name);

		// get data from database
		//IData* data = databaseConnection->executeQuery(strSql, columnNames);
		IData* data = databaseConnection->executeQuery(sql, columnNames);
		
		StationSectionInfos.clear();
		do 
		{
			for (unsigned long index = 0; index < data->getNumRows(); index++)
			{
				StationSectionInfo sectionInfo;
				StationSectionInfos.push_back(sectionInfo);
				StationSectionInfos.rbegin()->pkey = data->getUnsignedLongData(index, ColumnName_Pkey);
				StationSectionInfos.rbegin()->strName = data->getStringData(index, ColumnName_Name);
			}

			delete data;
			data = NULL;

			if (StationSectionInfos.empty())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[No Data]there is no data found in database");
				return;
			}
		} while (databaseConnection->moreData(data));
		
		FUNCTION_EXIT;
	}

	StationSectionInfo BasStationSectionAccessFactory::getStationSectionInfoByKey(unsigned long sectionKey)
	{
		FUNCTION_ENTRY("getStationSectionInfoByKey");
		using namespace TA_Base_Core;
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);

		/*std::ostringstream ss;
		ss << "SELECT " << ColumnName_Pkey << " , "
			<< ColumnName_Name 
			<< " FROM " << StationSectionTableName
			<< " WHERE " << ColumnName_Pkey << " = " << sectionKey;
		std::string strSql = ss.str();*/
		/*SELECT PKEY,NAME FROM BA_TUNNEL_SECTION WHERE PKEY=%s*/
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, BA_TUNNEL_SECTION_SELECT_99002, sectionKey);

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ColumnName_Pkey);
		columnNames.push_back(ColumnName_Name);

		// get data from database
		//IData* data = databaseConnection->executeQuery(strSql, columnNames);
		IData* data = databaseConnection->executeQuery(sql, columnNames);
		
		if (data->getNumRows() <= 0) 
		{
			TA_THROW(TA_Base_Core::DatabaseException("there no data found in database"));
		}

		StationSectionInfo sectionInfo;
		sectionInfo.pkey = data->getUnsignedLongData(0, ColumnName_Pkey);
		sectionInfo.strName = data->getStringData(0, ColumnName_Name);

		FUNCTION_EXIT;
		return sectionInfo;
	}
}
