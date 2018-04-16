#include "BasTunnelFireCaseAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

namespace TA_IRS_Core
{
	const std::string BasTunnelFireCaseAccessFactory::TableName = "BA_TUNNEL_FIRECASE";
	const std::string BasTunnelFireCaseAccessFactory::ColumnName_Pkey = "PKEY";
	const std::string BasTunnelFireCaseAccessFactory::ColumnName_Name = "NAME";
	const std::string BasTunnelFireCaseAccessFactory::ColumnName_Description = "DESCRIPTION";
	BasTunnelFireCaseAccessFactory::BasTunnelFireCaseAccessFactory(void)
	{
	}

	BasTunnelFireCaseAccessFactory::~BasTunnelFireCaseAccessFactory(void)
	{
	}
	
	void BasTunnelFireCaseAccessFactory::getAllFireCaseInfo(TunnelFireCaseInfoVct& fireCaseVct)
	{
		FUNCTION_ENTRY("getAllFireCaseInfo");
		using namespace TA_Base_Core;
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);

		/*std::ostringstream ss;
		ss << "SELECT " << ColumnName_Pkey << " , "
			<< ColumnName_Name << " , "
			<< ColumnName_Description
			<< " FROM " << TableName;*/
		/*SELECT PKEY,NAME,DESCRIPTION FROM BA_TUNNEL_FIRECASE*/
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, BA_TUNNEL_FIRECASE_SELECT_103001);

		std::vector<std::string> columnNames;
		columnNames.push_back(ColumnName_Pkey);
		columnNames.push_back(ColumnName_Name);
		columnNames.push_back(ColumnName_Description);

		// get data from database
		//IData* data = databaseConnection->executeQuery(ss.str(), columnNames);
		IData* data = databaseConnection->executeQuery(sql, columnNames);

		fireCaseVct.clear();
		do 
		{
			for (unsigned long index = 0; index < data->getNumRows(); index++)
			{
				TunnelFireCaseInfo tunnelFireCaseInfo;
				tunnelFireCaseInfo.pkey = data->getUnsignedLongData(index, ColumnName_Pkey);
				tunnelFireCaseInfo.name = data->getStringData(index, ColumnName_Name);
				tunnelFireCaseInfo.description = data->getStringData(index, ColumnName_Description);
				fireCaseVct.push_back(tunnelFireCaseInfo);
			}
			delete data;
			data = NULL;
			if (fireCaseVct.empty())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[No Data]there is no data found in database");
				return;
			}
		} while (databaseConnection->moreData(data));

		FUNCTION_EXIT;
	}
}
