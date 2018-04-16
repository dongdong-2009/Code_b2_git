#include "BasParameterValueType.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
	BasParameterValueTypeAccessFactory* BasParameterValueTypeAccessFactory::m_Instance = NULL;
	const std::string BasParameterValueTypeAccessFactory::TableName = "BA_PARAMETERVALUETYPE";
	const std::string BasParameterValueTypeAccessFactory::ColumnName_Pkey = "PKEY";
	const std::string BasParameterValueTypeAccessFactory::ColumnName_Name = "NAME";
	const std::string BasParameterValueTypeAccessFactory::ColumnName_AIToken = "AITOKEN";
	const std::string BasParameterValueTypeAccessFactory::ColumnName_AOToken = "AOTOKEN";

	BasParameterValueTypeAccessFactory::BasParameterValueTypeAccessFactory(void)
	{
	}

	BasParameterValueTypeAccessFactory::~BasParameterValueTypeAccessFactory(void)
	{
	}

	BasParameterValueTypeAccessFactory& BasParameterValueTypeAccessFactory::getInstance()
	{
		if (NULL == m_Instance)
		{
			m_Instance = new BasParameterValueTypeAccessFactory;
		}
		return *m_Instance;
	}

	void BasParameterValueTypeAccessFactory::getParameterValueTypeInfos(ParameterValueTypeInfoVct& parameterValueTypeInfoVct)
	{
		FUNCTION_ENTRY("getParameterValueTypeInfos");
		/*std::stringstream sql;
		sql << "SELECT " << ColumnName_Pkey << " , "
			<< ColumnName_Name << " , "
			<< ColumnName_AIToken << " , "
			<< ColumnName_AOToken 
			<< " FROM " << TableName;*/
		TA_Base_Core::SQLStatement sql;

		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ColumnName_Pkey);
		columnNames.push_back(ColumnName_Name);
		columnNames.push_back(ColumnName_AIToken);
		columnNames.push_back(ColumnName_AOToken);

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		//TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
		databaseConnection->prepareSQLStatement(sql, BA_PARAMETERVALUETYPE_SELECT_90001); 
		TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);

		// Loop for each row returned
		parameterValueTypeInfoVct.clear();
		do 
		{
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				// The getUnsignedLongData() call can throw an exception. 
				// Need to catch it to do pointer clean up.
				ParameterValueTypeInfo info;
				info.pkey = data->getUnsignedLongData(i, ColumnName_Pkey);
				info.name = data->getStringData(i, ColumnName_Name);
				info.aiToken = data->getStringData(i, ColumnName_AIToken);
				info.aoToken = data->getStringData(i, ColumnName_AOToken);
				parameterValueTypeInfoVct.push_back(info);
			}
			delete data;
			data = NULL;
			// if data->getNumRows() <= 0 then call databaseConnection->moreData
			// will assert. so we need have a judgement
			if (parameterValueTypeInfoVct.empty())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[No Data]there is no data found in database");
				return;
			}
		} 
		while (databaseConnection->moreData(data));

		FUNCTION_EXIT;
	}
}
