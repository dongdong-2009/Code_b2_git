
#include "core/data_access_interface/bas/src/ModeDeviceAccessFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
	const std::string ModeDeviceAccessFactory::TableName = "BA_MODE_DEVICES";
	const std::string ModeDeviceAccessFactory::TableName_V = "BA_MODE_DEVICES_V";
	const std::string ModeDeviceAccessFactory::ColumnName_Pkey = "PKEY";
	const std::string ModeDeviceAccessFactory::ColumnName_LocationKey = "LOCATIONKEY";
	const std::string ModeDeviceAccessFactory::ColumnName_ModeKey = "MODEKEY";
	const std::string ModeDeviceAccessFactory::ColumnName_ModeName = "MODENAME";
	const std::string ModeDeviceAccessFactory::ColumnName_DeviceName = "DEVICENAME";
	const std::string ModeDeviceAccessFactory::ColumnName_DatapointTag_Status = "STATUSDPTAG";
	const std::string ModeDeviceAccessFactory::ColumnName_DatapointTag_Auto = "AUTODPTAG";
	const std::string ModeDeviceAccessFactory::ColumnName_Description = "DEVICEDESC";
	const std::string ModeDeviceAccessFactory::ColumnName_ExpectStatus = "EXPECTSTATUS";
	ModeDeviceAccessFactory* ModeDeviceAccessFactory::m_instance = NULL;
	ModeDeviceAccessFactory::ModeDeviceAccessFactory(void)
	{
	}

	ModeDeviceAccessFactory::~ModeDeviceAccessFactory(void)
	{
	}

	ModeDeviceAccessFactory& ModeDeviceAccessFactory::getInstance()
	{
		FUNCTION_ENTRY("getInstance");
		if (NULL == m_instance)
		{
			m_instance = new ModeDeviceAccessFactory;
		}
		FUNCTION_EXIT;
		return *m_instance;
	}

	void ModeDeviceAccessFactory::getAllModeDevices(ModeDevices& modeDevices)
	{
		FUNCTION_ENTRY("getAllModeDevices");
		std::string strSql = "";
		getModeDevicesBySqlWhere(strSql, modeDevices);
		FUNCTION_EXIT;
	}
	
	void ModeDeviceAccessFactory::getModeDevicesByModeKey(unsigned long locationKey, unsigned long modeKey, ModeDevices& modeDevices)
	{
		FUNCTION_ENTRY("getModeDevicesByModeKey");
		std::ostringstream sql;
		sql << ColumnName_LocationKey << " = " << locationKey
			<< " AND " << ColumnName_ModeKey << " = " << modeKey;
		std::string strSql = sql.str();
		getModeDevicesBySqlWhere(strSql, modeDevices);
		FUNCTION_EXIT;
	}

	void ModeDeviceAccessFactory::getModeDevicesByModeName(unsigned long locationKey, const std::string& modeName, ModeDevices& modeDevices)
	{
		FUNCTION_ENTRY("getModeDevicesByModeName");
		std::ostringstream sql;
		sql << ColumnName_LocationKey << " = " << locationKey
			<< " AND " << ColumnName_ModeName << " = '" << modeName << "'";
		std::string strSql = sql.str();
		getModeDevicesBySqlWhereEx(strSql, modeDevices);
		FUNCTION_EXIT;
	}

	void ModeDeviceAccessFactory::getModeDevicesBySqlWhereEx(const std::string& strSqlWhere, ModeDevices& modeDevices)
	{
		FUNCTION_ENTRY("getModeDevicesBySqlWhereEx");

		/*std::ostringstream sql;
		sql << "SELECT " << ColumnName_Pkey << " , " << ColumnName_LocationKey << " , "
			<< ColumnName_ModeKey << " , " << ColumnName_DeviceName << " , "
			<< ColumnName_DatapointTag_Status << " , " << ColumnName_DatapointTag_Auto << " , "
			<< ColumnName_Description << " , "	<< ColumnName_ExpectStatus
			<< " FROM " << TableName_V;

		if (false == strSqlWhere.empty())
		 {
			sql << " WHERE " << strSqlWhere;
		 }
		 std::string strSql = sql.str();*/
		/*SELECT PKEY,LOCATIONKEY,MODEKEY,DEVICENAME,STATUSDPTAG,AUTODPTAG,DEVICEDESC,EXPECTSTATUS FROM BA_MODE_DEVICES_V WHERE %s*/
		TA_Base_Core::SQLStatement sql;

		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

		if (false == strSqlWhere.empty())
		{
			databaseConnection->prepareSQLStatement(sql, BA_MODE_DEVICES_V_SELECT_87002, strSqlWhere);
		}
		else
		{
			databaseConnection->prepareSQLStatement(sql, BA_MODE_DEVICES_V_SELECT_87001);
		}


		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ColumnName_Pkey);
		columnNames.push_back(ColumnName_LocationKey);
		columnNames.push_back(ColumnName_ModeKey);
		columnNames.push_back(ColumnName_DeviceName);
		columnNames.push_back(ColumnName_DatapointTag_Status);
		columnNames.push_back(ColumnName_DatapointTag_Auto);
		columnNames.push_back(ColumnName_Description);
		columnNames.push_back(ColumnName_ExpectStatus);

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		//TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
		TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);

		modeDevices.clear();
		do 
		{
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				ModeDeviceInfo modeDeviceInfo;
				modeDevices.push_back(modeDeviceInfo);
				(*modeDevices.rbegin()).pkey = data->getUnsignedLongData(i, ColumnName_Pkey);
				(*modeDevices.rbegin()).locationKey = data->getUnsignedLongData(i, ColumnName_LocationKey);
				(*modeDevices.rbegin()).modeKey = data->getUnsignedLongData(i, ColumnName_ModeKey);
				(*modeDevices.rbegin()).deviceName = data->getStringData(i, ColumnName_DeviceName);
				(*modeDevices.rbegin()).datapointTag_Status = data->getStringData(i, ColumnName_DatapointTag_Status);
				(*modeDevices.rbegin()).datapointTag_Auto = data->getStringData(i, ColumnName_DatapointTag_Auto);
				(*modeDevices.rbegin()).description = data->getStringData(i, ColumnName_Description);
				(*modeDevices.rbegin()).expectStatus = data->getStringData(i, ColumnName_ExpectStatus);
			}
			// if data->getNumRows() <= 0 then call databaseConnection->moreData
			// will assert. so we need have a judgement
			if (false != modeDevices.empty())
			{
				TA_THROW(TA_Base_Core::DataException("no value found in Database", TA_Base_Core::DataException::NO_VALUE, "what's the data should be." ));
			}
		} while (databaseConnection->moreData(data));
	}

	void ModeDeviceAccessFactory::getModeDevicesBySqlWhere(const std::string& strSqlWhere, ModeDevices& modeDevices)
	{
		FUNCTION_ENTRY("getModeDevicesBySqlWhere");

		/*std::ostringstream sql;
		sql << "SELECT " << ColumnName_Pkey << " , " << ColumnName_LocationKey << " , "
			<< ColumnName_ModeKey << " , " << ColumnName_DeviceName << " , "
			<< ColumnName_DatapointTag_Status << " , " << ColumnName_DatapointTag_Auto << " , "
			<< ColumnName_Description << " , "	<< ColumnName_ExpectStatus
			<< " FROM " << TableName;*/
		/*SELECT PKEY,LOCATIONKEY,MODEKEY,DEVICENAME,STATUSDPTAG,AUTODPTAG,DEVICEDESC,EXPECTSTATUS FROM BA_MODE_DEVICES*/
		/*SELECT PKEY,LOCATIONKEY,MODEKEY,DEVICENAME,STATUSDPTAG,AUTODPTAG,DEVICEDESC,EXPECTSTATUS FROM BA_MODE_DEVICES WHERE %s*/
		/*if (false == strSqlWhere.empty())
		{
			sql << " WHERE " << strSqlWhere;
		}
		std::string strSql = sql.str();*/
		TA_Base_Core::SQLStatement sql;
		
		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

		if (false == strSqlWhere.empty())
		{
			databaseConnection->prepareSQLStatement(sql, BA_MODE_DEVICES_SELECT_88002, strSqlWhere);
		}
		else
		{
			databaseConnection->prepareSQLStatement(sql, BA_MODE_DEVICES_SELECT_88001);
		}

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ColumnName_Pkey);
		columnNames.push_back(ColumnName_LocationKey);
		columnNames.push_back(ColumnName_ModeKey);
		columnNames.push_back(ColumnName_DeviceName);
		columnNames.push_back(ColumnName_DatapointTag_Status);
		columnNames.push_back(ColumnName_DatapointTag_Auto);
		columnNames.push_back(ColumnName_Description);
		columnNames.push_back(ColumnName_ExpectStatus);

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		//TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
		TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);
		
		modeDevices.clear();
		do 
		{
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				ModeDeviceInfo modeDeviceInfo;
				modeDevices.push_back(modeDeviceInfo);
				(*modeDevices.rbegin()).pkey = data->getUnsignedLongData(i, ColumnName_Pkey);
				(*modeDevices.rbegin()).locationKey = data->getUnsignedLongData(i, ColumnName_LocationKey);
				(*modeDevices.rbegin()).modeKey = data->getUnsignedLongData(i, ColumnName_ModeKey);
				(*modeDevices.rbegin()).deviceName = data->getStringData(i, ColumnName_DeviceName);
				(*modeDevices.rbegin()).datapointTag_Status = data->getStringData(i, ColumnName_DatapointTag_Status);
				(*modeDevices.rbegin()).datapointTag_Auto = data->getStringData(i, ColumnName_DatapointTag_Auto);
				(*modeDevices.rbegin()).description = data->getStringData(i, ColumnName_Description);
				(*modeDevices.rbegin()).expectStatus = data->getStringData(i, ColumnName_ExpectStatus);
			}
			// if data->getNumRows() <= 0 then call databaseConnection->moreData
			// will assert. so we need have a judgement
			if (modeDevices.empty())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[No Data]there is no data found in database");
				return;
			}
		} while (databaseConnection->moreData(data));

		FUNCTION_EXIT;
	}

}
