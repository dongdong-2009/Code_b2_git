
#include "core/data_access_interface/bas/src/BasTunnelCongestionModeAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"


namespace TA_IRS_Core
{
	const std::string BasTunnelCongestionModeAccessFactory::CongestionModeTableName = "BA_TUNNEL_CONGESTIONMODE_MAP";
	const std::string BasTunnelCongestionModeAccessFactory::CongestionModeTableName_V = "BA_TUNNEL_CONGESTIONMODE_MAP_V";
	const std::string BasTunnelCongestionModeAccessFactory::ColumnName_Pkey = "PKEY";
	const std::string BasTunnelCongestionModeAccessFactory::ColumnName_TrackNumber = "TRACKNUMBER";
	const std::string BasTunnelCongestionModeAccessFactory::ColumnName_FrontStationKey = "FRONTSTATION_LOCATIONKEY";
	const std::string BasTunnelCongestionModeAccessFactory::ColumnName_FrontStation_ModeKey = "FRONTSTATION_MODEKEY";
	const std::string BasTunnelCongestionModeAccessFactory::ColumnName_FrontStation_ModeType = "FRONTSTATION_MODETYPE";
	const std::string BasTunnelCongestionModeAccessFactory::ColumnName_FrontStation_ModeName = "FRONTSTATION_MODENAME";
	const std::string BasTunnelCongestionModeAccessFactory::ColumnName_FrontStation_ModeValue = "FRONTSTATION_MODEVALUE";
	const std::string BasTunnelCongestionModeAccessFactory::ColumnName_FrontStation_ModeDescription = "FRONTSTATION_MODEDESCRIPTION";
	const std::string BasTunnelCongestionModeAccessFactory::ColumnName_BackStationKey = "BACKSTATION_LOCATIONKEY";
	const std::string BasTunnelCongestionModeAccessFactory::ColumnName_BackStation_ModeKey = "BACKSTATION_MODEKEY";
	const std::string BasTunnelCongestionModeAccessFactory::ColumnName_BackStation_ModeType = "BACKSTATION_MODETYPE";
	const std::string BasTunnelCongestionModeAccessFactory::ColumnName_BackStation_ModeName = "BACKSTATION_MODENAME";
	const std::string BasTunnelCongestionModeAccessFactory::ColumnName_BackStation_ModeValue = "BACKSTATION_MODEVALUE";
	const std::string BasTunnelCongestionModeAccessFactory::ColumnName_BackStation_ModeDescription = "BACKSTATION_MODEDESCRIPTION";

	BasTunnelCongestionModeAccessFactory::BasTunnelCongestionModeAccessFactory(void)
	{
	}

	BasTunnelCongestionModeAccessFactory::~BasTunnelCongestionModeAccessFactory(void)
	{
	}

	void BasTunnelCongestionModeAccessFactory::getCongestionModes(CongestionModeInfoVct& modeInfos)
	{
		FUNCTION_ENTRY("getModesKey");
		std::string strSql = "";
		getModesByWhereSql(strSql, modeInfos);
		FUNCTION_EXIT;
	}

	void BasTunnelCongestionModeAccessFactory::getCongestionModes(unsigned long trackNumber, CongestionModeInfoVct& modeInfos)
	{
		FUNCTION_ENTRY("getModesKey");
		std::ostringstream ss;
		ss << ColumnName_TrackNumber << " = " << trackNumber;

		std::string strSql = ss.str();

		getModesByWhereSql(strSql, modeInfos);
		FUNCTION_EXIT;
	}

	void BasTunnelCongestionModeAccessFactory::getModesByWhereSql(const std::string& strSqlWhere, CongestionModeInfoVct& modeInfos)
	{
		FUNCTION_ENTRY("getModesKeyByWhereSql");
		using namespace TA_Base_Core;
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);

		/*std::ostringstream ss;
		ss << "SELECT " 
			<< ColumnName_Pkey << " , "
			<< ColumnName_TrackNumber << " , "
			<< ColumnName_FrontStation_ModeKey << " , "
			<< ColumnName_FrontStation_ModeType << " , "
			<< ColumnName_FrontStation_ModeName << " , "
			<< ColumnName_FrontStation_ModeValue << " , "
			<< ColumnName_FrontStation_ModeDescription << " , "
			<< ColumnName_BackStation_ModeKey << " , "
			<< ColumnName_BackStation_ModeType << " , "
			<< ColumnName_BackStation_ModeName << " , "
			<< ColumnName_BackStation_ModeValue<< " , "
			<< ColumnName_BackStation_ModeDescription 
			<< " FROM " << CongestionModeTableName_V;
		if (false == strSqlWhere.empty())
		{
			ss << " WHERE " << strSqlWhere;
		}
		std::string strSql = ss.str();*/
		/*
		SELECT PKEY,TRACKNUMBER,FRONTSTATION_MODEKEY,FRONTSTATION_MODETYPE,FRONTSTATION_MODENAME,FRONTSTATION_MODEVALUE,FRONTSTATION_MODEDESCRIPTION,
			BACKSTATION_LOCATIONKEY,BACKSTATION_MODETYPE,BACKSTATION_MODENAME,BACKSTATION_MODEVALUE,BACKSTATION_MODEDESCRIPTION FROM BA_TUNNEL_CONGESTIONMODE_MAP_V
		*/
		/*
		SELECT PKEY,TRACKNUMBER,FRONTSTATION_MODEKEY,FRONTSTATION_MODETYPE,FRONTSTATION_MODENAME,FRONTSTATION_MODEVALUE,FRONTSTATION_MODEDESCRIPTION,
			BACKSTATION_LOCATIONKEY,BACKSTATION_MODETYPE,BACKSTATION_MODENAME,BACKSTATION_MODEVALUE,BACKSTATION_MODEDESCRIPTION FROM BA_TUNNEL_CONGESTIONMODE_MAP_V WHERE %s
		*/
		TA_Base_Core::SQLStatement sql;
		if (false == strSqlWhere.empty())
		{
			databaseConnection->prepareSQLStatement(sql, BA_TUNNEL_CONGESTIONMODE_MAP_V_SELECT_101002, strSqlWhere);
		}
		else
		{
			databaseConnection->prepareSQLStatement(sql, BA_TUNNEL_CONGESTIONMODE_MAP_V_SELECT_101001); 
		}


		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ColumnName_Pkey);
		columnNames.push_back(ColumnName_TrackNumber);
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
		//IData* data = databaseConnection->executeQuery(strSql, columnNames);
		IData* data = databaseConnection->executeQuery(sql, columnNames);

		do 
		{
			for (unsigned long index = 0; index < data->getNumRows(); index++)
			{
				CongestionModeInfo_SmartPtr mode = CongestionModeInfo_SmartPtr(new CongestionModeInfo);
				mode->pkey = data->getUnsignedLongData(index, ColumnName_Pkey);
				mode->trackNumber = data->getUnsignedLongData(index, ColumnName_TrackNumber);
				mode->frontStation_modeKey = static_cast<unsigned long>(data->getIntegerData(index, ColumnName_FrontStation_ModeKey));
				mode->frontStation_modeName = data->getStringData(index, ColumnName_FrontStation_ModeName);
				mode->frontStation_modeType = data->getStringData(index, ColumnName_FrontStation_ModeType);
				mode->frontStation_modeValue = data->getUnsignedLongData(index, ColumnName_FrontStation_ModeValue);
				mode->frontStation_modeDesc = data->getStringData(index, ColumnName_FrontStation_ModeDescription);
				mode->backStation_modeKey = static_cast<unsigned long>(data->getIntegerData(index, ColumnName_BackStation_ModeKey));
				mode->backStation_modeName = data->getStringData(index, ColumnName_BackStation_ModeName);
				mode->backStation_modeType = data->getStringData(index, ColumnName_BackStation_ModeType);
				mode->backStation_modeValue = data->getUnsignedLongData(index, ColumnName_BackStation_ModeValue);
				mode->backStation_modeDesc = data->getStringData(index, ColumnName_BackStation_ModeDescription);
				modeInfos.push_back(mode);
			}
			delete data;
			data = NULL;
			if (modeInfos.empty())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[No Data]there is no data found in database");
				return;
			}
		} while (databaseConnection->moreData(data));


		FUNCTION_EXIT;
	}

	void BasTunnelCongestionModeAccessFactory::getFrontAndBackStationByKey(unsigned long pkey, unsigned long& frontStationKey, unsigned long& backStationKey)
	{
		FUNCTION_ENTRY("getFrontAndBackStationKey");
		using namespace TA_Base_Core;
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);
		TA_ASSERT(NULL != databaseConnection, "can not get the database connection");

		/*std::ostringstream ss;
		ss << "SELECT " 
			<< ColumnName_FrontStationKey << " , "
			<< ColumnName_BackStationKey 
			<< " FROM " << CongestionModeTableName_V 
			<< " WHERE " << ColumnName_Pkey 
			<< " = " << pkey;*/
		/*SELECT FRONTSTATION_LOCATIONKEY,BACKSTATION_LOCATIONKEY FROM BA_TUNNEL_CONGESTIONMODE_MAP WHERE PKEY=%s*/
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, BA_TUNNEL_CONGESTIONMODE_MAP_SELECT_102001,pkey);

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
}
