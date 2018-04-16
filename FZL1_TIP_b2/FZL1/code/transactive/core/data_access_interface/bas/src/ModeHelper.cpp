
#include "core/data_access_interface/bas/src/ModeHelper.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
	ModeHelper::ModeHelper(const unsigned long row, TA_Base_Core::IData& data)
	: m_key(0),
	m_name(""),
	m_value(0),
	m_description(""),
	m_locationKey(0),
	m_subsystemKey(0),
	m_typeKey(0),
	m_isValidData(false)
	{
		FUNCTION_ENTRY("ModeHelper()");
		reloadUsing(row, data);
		FUNCTION_EXIT;
	}

	unsigned long ModeHelper::getKey()
	{
		FUNCTION_ENTRY("getKey");
		FUNCTION_EXIT;
		return m_key;
	}

	std::string ModeHelper::getName()
	{
		FUNCTION_ENTRY("getName");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_name;
	}

	unsigned int ModeHelper::getValue()
	{
		FUNCTION_ENTRY("getValue");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_value;
	}

	std::string ModeHelper::getDescription()
	{
		FUNCTION_ENTRY("getDescription");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_description;
	}

	unsigned int ModeHelper::getLocationKey()
	{
		FUNCTION_ENTRY("getLocationKey");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_locationKey;
	}

	unsigned int ModeHelper::getSubsystemKey()
	{
		FUNCTION_ENTRY("getSubsystemKey");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_subsystemKey;
	}

	unsigned int ModeHelper::getTypeKey()
	{
		FUNCTION_ENTRY("getTypeKey");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_typeKey;
	}

	std::string ModeHelper::getLocationName()
	{
		FUNCTION_ENTRY("getLocationName");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_locationName;
	}

	std::string ModeHelper::getSubsystemName()
	{
		FUNCTION_ENTRY("getSubsystemName");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_subsystemName;
	}

	std::string ModeHelper::getTypeName()
	{
		FUNCTION_ENTRY("getTypeName");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_typeName;
	}

	void ModeHelper::reload()
	{
		FUNCTION_ENTRY("reload");
		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

		/*std::ostringstream sql;
		sql << "select PKEY, MODENAME, VALUE, DESCRIPTION, LOCATIONKEY,LOCATIONNAME, SUBSYSTEMKEY,SUBSYSTEMNAME, TYPEKEY,TYPENAME ";
		sql << "FROM BA_MODES_V WHERE PKEY = " << m_key;*/
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, BA_MODES_V_SELECT_86004, m_key);

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		getColumnNames(columnNames);

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		//std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(sql.str(), columnNames));
		std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(sql, columnNames));

		// Need to bring in the DataException
		using TA_Base_Core::DataException;

		if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
		{
			std::ostringstream reasonMessage;
			reasonMessage << "No mode found for pkey = " << m_key;
			TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
		}

		if (data->getNumRows() != 1)
		{
			std::ostringstream reasonMessage;
			reasonMessage << "More than one mode record for pkey = " << m_key;
			TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"" ) );
		}

		reloadUsing(0, *data);
		FUNCTION_EXIT;
	}

	void ModeHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		FUNCTION_ENTRY("getColumnNames");
		columnNames.clear();

		columnNames.push_back("PKEY");
		columnNames.push_back("MODENAME");
		columnNames.push_back("VALUE");
		columnNames.push_back("DESCRIPTION");
		columnNames.push_back("LOCATIONKEY");
		columnNames.push_back("LOCATIONNAME");
		columnNames.push_back("SUBSYSTEMKEY");
		columnNames.push_back("SUBSYSTEMNAME");
		columnNames.push_back("TYPEKEY");
		columnNames.push_back("TYPENAME");
		FUNCTION_EXIT;
	}

	void ModeHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
	{
		FUNCTION_ENTRY("reloadUsing");
		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		getColumnNames(columnNames);

		// Assign the data as appropriate to the member variables.
		// These calls can throw DataExceptions, as documented in the comment of this method.
		m_key = data.getUnsignedLongData( row, columnNames[0] );
		m_name = data.getStringData( row, columnNames[1] );
		m_value = data.getIntegerData( row, columnNames[2] );
		m_description = data.getStringData( row, columnNames[3] );
		m_locationKey = data.getIntegerData( row, columnNames[4] );
		m_locationName = data.getStringData(row, columnNames[5]);
		m_subsystemKey = data.getIntegerData( row, columnNames[6]);
		m_subsystemName = data.getStringData(row, columnNames[7]);
		m_typeKey = data.getIntegerData( row, columnNames[8]);
		m_typeName = data.getStringData(row, columnNames[9]);

		m_isValidData = true;
		FUNCTION_EXIT;
	}


	void ModeHelper::invalidate()
	{
		FUNCTION_ENTRY("invalidate");
		m_isValidData = false;
		FUNCTION_EXIT;
	}

}
