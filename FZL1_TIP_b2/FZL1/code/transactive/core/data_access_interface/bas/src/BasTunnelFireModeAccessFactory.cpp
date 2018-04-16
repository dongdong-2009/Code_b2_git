
#include "core/data_access_interface/bas/src/BasTunnelFireModeAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"


namespace TA_IRS_Core
{
	const std::string BasTunnelFireModeAccessFactory::FireModeTableName = "BA_TUNNEL_FIREMODE_MAP";
	const std::string BasTunnelFireModeAccessFactory::FireModeTableName_V = "BA_TUNNEL_FIREMODE_MAP_V";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_Pkey = "PKEY";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_TrackNumber = "TRACKNUMBER";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_FireCaseKey = "FIRECASEKEY";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_FireCaseName = "FIRECASENAME";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_FrontStationKey = "FRONTSTATION_LOCATIONKEY";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_FrontStation_ModeKey = "FRONTSTATION_MODEKEY";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_FrontStation_ModeType = "FRONTSTATION_MODETYPE";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_FrontStation_ModeName = "FRONTSTATION_MODENAME";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_FrontStation_ModeValue = "FRONTSTATION_MODEVALUE";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_FrontStation_ModeDescription = "FRONTSTATION_MODEDESCRIPTION";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_BackStationKey = "BACKSTATION_LOCATIONKEY";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_BackStation_ModeKey = "BACKSTATION_MODEKEY";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_BackStation_ModeType = "BACKSTATION_MODETYPE";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_BackStation_ModeName = "BACKSTATION_MODENAME";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_BackStation_ModeValue = "BACKSTATION_MODEVALUE";
	const std::string BasTunnelFireModeAccessFactory::ColumnName_BackStation_ModeDescription = "BACKSTATION_MODEDESCRIPTION";


	BasTunnelFireModeAccessFactory::BasTunnelFireModeAccessFactory(void)
	{
	}

	BasTunnelFireModeAccessFactory::~BasTunnelFireModeAccessFactory(void)
	{
	}

	void BasTunnelFireModeAccessFactory::getFrontAndBackStationByKey(unsigned long pkey, unsigned long& frontStationKey, unsigned long& backStationKey)
	{
		FUNCTION_ENTRY("getFrontAndBackStationByKey");

		using namespace TA_Base_Core;
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);

		/*std::ostringstream ss;
		ss << "SELECT " 
			<< ColumnName_FrontStationKey << " , "
			<< ColumnName_BackStationKey 
			<< " FROM " << FireModeTableName_V 
			<< " WHERE " << ColumnName_Pkey 
			<< " = " << pkey;*/
		/*SELECT FRONTSTATION_LOCATIONKEY,BACKSTATION_LOCATIONKEY FROM BA_TUNNEL_FIREMODE_MAP WHERE PKEY=%s*/
		TA_Base_Core::SQLStatement sql;
		//
		databaseConnection->prepareSQLStatement(sql, BA_TUNNEL_FIREMODE_MAP_SELECT_104001, pkey);
		// get data from database
		std::vector<std::string> columnNames;
		columnNames.push_back(ColumnName_FrontStationKey);
		columnNames.push_back(ColumnName_BackStationKey);
		//IData* data = databaseConnection->executeQuery(ss.str(), columnNames);
		IData* data = databaseConnection->executeQuery(sql, columnNames);
		if (data->getNumRows() == 1)
		{
			frontStationKey = static_cast<unsigned long>(data->getIntegerData(0, ColumnName_FrontStationKey));
			backStationKey = static_cast<unsigned long>(data->getIntegerData(0, ColumnName_BackStationKey));
		}
		else
		{
			delete data;
			data = NULL;
			TA_THROW(TA_Base_Core::DatabaseException("There no data found in database."));
		}
		delete data;
		data = NULL;

		FUNCTION_EXIT;
	}

	void BasTunnelFireModeAccessFactory::getFireModes(TunnelFireModeInfoVct& fireModeInfos)
	{
		FUNCTION_ENTRY("getFireModes");
		std::stringstream ss;
		getFireModesByWhereSql(ss.str(), fireModeInfos);
		FUNCTION_EXIT;
	}

	void BasTunnelFireModeAccessFactory::getFireModes(unsigned long trackNumber, TunnelFireModeInfoVct& fireModeInfos)
	{
		FUNCTION_ENTRY("getFireModes");
		std::stringstream ss;
		ss << ColumnName_TrackNumber << " = " << trackNumber;

		getFireModesByWhereSql(ss.str(), fireModeInfos);
		FUNCTION_EXIT;
	}

	void BasTunnelFireModeAccessFactory::getFireModes(unsigned long trackNumber, unsigned long firecaseKey, TunnelFireModeInfoVct& fireModeInfos)
	{
		FUNCTION_ENTRY("getFireModes");
		std::stringstream ss;
		ss << ColumnName_TrackNumber << " = " << trackNumber
			<< " AND " << ColumnName_FireCaseKey << " = " << firecaseKey;

		getFireModesByWhereSql(ss.str(), fireModeInfos);
		FUNCTION_EXIT;
	}

	void BasTunnelFireModeAccessFactory::getFireModesByWhereSql(const std::string& strSql, TunnelFireModeInfoVct& fireModes)
	{
		FUNCTION_ENTRY("getFireModesBySql");
		using namespace TA_Base_Core;
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);

		/*std::ostringstream ss;
		ss << "SELECT " << ColumnName_Pkey << " , "
			<< ColumnName_TrackNumber << " , "
			<< ColumnName_FireCaseKey << " , "  
			<< ColumnName_FireCaseName << " , "  
			<< ColumnName_FrontStation_ModeKey << " , "  
			<< ColumnName_FrontStation_ModeType << " , "  
			<< ColumnName_FrontStation_ModeName << " , "  
			<< ColumnName_FrontStation_ModeValue << " , " 
			<< ColumnName_FrontStation_ModeDescription << " , "
			<< ColumnName_BackStation_ModeKey << " , "
			<< ColumnName_BackStation_ModeType << " , "
			<< ColumnName_BackStation_ModeName << " , "
			<< ColumnName_BackStation_ModeValue << " , "
			<< ColumnName_BackStation_ModeDescription 
			<< " FROM " << FireModeTableName_V;
		if (false == strSql.empty())
		{
			ss << " WHERE " << strSql;
		}
		std::string strSqlString = ss.str();
		*/

		TA_Base_Core::SQLStatement sql;	
		if (false == strSql.empty())
		{
			databaseConnection->prepareSQLStatement(sql, BA_TUNNEL_FIREMODE_MAP_V_SELECT_105002, strSql);
		}
		else
		{
			databaseConnection->prepareSQLStatement(sql, BA_TUNNEL_FIREMODE_MAP_V_SELECT_105001);
		}

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ColumnName_Pkey);
		columnNames.push_back(ColumnName_TrackNumber);
		columnNames.push_back(ColumnName_FireCaseKey);
		columnNames.push_back(ColumnName_FireCaseName);
		columnNames.push_back(ColumnName_FrontStation_ModeKey);
		columnNames.push_back(ColumnName_FrontStation_ModeType);
		columnNames.push_back(ColumnName_FrontStation_ModeName);
		columnNames.push_back(ColumnName_FrontStation_ModeValue);
		columnNames.push_back(ColumnName_FrontStation_ModeDescription);
		columnNames.push_back(ColumnName_BackStation_ModeKey);
		columnNames.push_back(ColumnName_BackStation_ModeType);
		columnNames.push_back(ColumnName_BackStation_ModeName);
		columnNames.push_back(ColumnName_BackStation_ModeValue);
		columnNames.push_back(ColumnName_BackStation_ModeDescription);
		// get data from database
		//IData* data = databaseConnection->executeQuery(strSqlString, columnNames);
		IData* data = databaseConnection->executeQuery(sql, columnNames);

		fireModes.clear();
		do 
		{
			for (unsigned long index = 0; index < data->getNumRows(); index++)
			{
				TunnelFireModeInfo_SmartPtr fireMode = TunnelFireModeInfo_SmartPtr(new TunnelFireModeInfo);
				fireMode->Pkey = data->getUnsignedLongData(index , ColumnName_Pkey);
				fireMode->TrackNumber = data->getUnsignedLongData(index, ColumnName_TrackNumber);
				fireMode->FireCaseKey = data->getUnsignedLongData(index, ColumnName_FireCaseKey);
				fireMode->FireCaseName = data->getStringData(index, ColumnName_FireCaseName);
				fireMode->FrontStation_ModeKey = static_cast<unsigned long>(data->getIntegerData(index, ColumnName_FrontStation_ModeKey));
				fireMode->FrontStation_ModeName = data->getStringData(index, ColumnName_FrontStation_ModeName);
				fireMode->FrontStation_ModeType = data->getStringData(index, ColumnName_FrontStation_ModeType);
				fireMode->FrontStation_ModeValue = data->getUnsignedLongData(index, ColumnName_FrontStation_ModeValue);
				fireMode->FrontStation_ModeDescription = data->getStringData(index, ColumnName_FrontStation_ModeDescription);
				fireMode->BackStation_ModeKey = static_cast<unsigned long>(data->getIntegerData(index, ColumnName_BackStation_ModeKey));
				fireMode->BackStation_ModeName = data->getStringData(index, ColumnName_BackStation_ModeName);
				fireMode->BackStation_ModeType = data->getStringData(index, ColumnName_BackStation_ModeType);
				fireMode->BackStation_ModeValue = data->getUnsignedLongData(index, ColumnName_BackStation_ModeValue);
				fireMode->BackStation_ModeDescription = data->getStringData(index, ColumnName_BackStation_ModeDescription);

				fireModes.push_back(fireMode);
			}

			if (fireModes.empty())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[No Data]there is no data found in database");
				return;
			}
		} while (databaseConnection->moreData(data));

		FUNCTION_EXIT;
	}


}
