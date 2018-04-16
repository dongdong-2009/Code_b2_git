

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/bas/src/SubsystemHelper.h"
#include <algorithm>
namespace TA_IRS_Core
{
	SubsystemHelper::SubsystemHelper(const unsigned long row, TA_Base_Core::IData& data)
		: m_key(0),
		m_name(""),
		m_isValidData(false)
	{
		FUNCTION_ENTRY("SubsystemHelper()");
		reloadUsing(row, data);
		FUNCTION_EXIT;
	}

	unsigned long SubsystemHelper::getKey()
	{
		FUNCTION_ENTRY("getKey");
		FUNCTION_EXIT;
		return m_key;
	}

	std::string SubsystemHelper::getName()
	{
		FUNCTION_ENTRY("getName");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_name;
	}

	std::string SubsystemHelper::getAssetName()
	{
		FUNCTION_ENTRY("getAssetName");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_assetName;
	}

	bool SubsystemHelper::getIsSystem()
	{
		FUNCTION_ENTRY("getIsSystem");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_isSystem;
	}
	void SubsystemHelper::reload()
	{
		FUNCTION_ENTRY("reload");
		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

		/*std::ostringstream sql;
        sql << "select PKEY, NAME, ASSET, ISSYSTEM, ISMERGE, MERGENAME, "
            << "nvl((select PKEY from BA_SUBSYSTEMS t1 where t1.NAME = t2.MERGENAME and t1.ISMERGE = 'Y'), 0) as PARENTKEY, "
            << "ORDER_ID from BA_SUBSYSTEMS t2 WHERE t2.PKEY = " << m_key;*/
		TA_Base_Core::SQLStatement sql;

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		getColumnNames(columnNames);

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		//std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(sql.str(), columnNames));
		databaseConnection->prepareSQLStatement(sql, BA_SUBSYSTEMS_SELECT_92002, m_key);
		std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(sql, columnNames));
		
		// Need to bring in the DataException
		using TA_Base_Core::DataException;

		if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified pkey
		{
			std::ostringstream reasonMessage;
			reasonMessage << "No subsystem found for pkey = " << m_key;
			TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
		}

		if (data->getNumRows() != 1)
		{
			std::ostringstream reasonMessage;
			reasonMessage << "More than one subsystem record for pkey = " << m_key;
			TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"" ) );
		}

		reloadUsing(0, *data);
		FUNCTION_EXIT;
	}

	void SubsystemHelper::getColumnNames(std::vector<std::string>& columnNames)
	{
		FUNCTION_ENTRY("getColumnNames");
		columnNames.clear();

		columnNames.push_back("PKEY");
		columnNames.push_back("NAME");
		columnNames.push_back("ASSET");
		columnNames.push_back("ISSYSTEM");
		columnNames.push_back("ISMERGE");
        columnNames.push_back("MERGENAME");
        columnNames.push_back("PARENTKEY");
		columnNames.push_back("ORDER_ID");
		FUNCTION_EXIT;
	}

	void SubsystemHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
	{
		FUNCTION_ENTRY("reloadUsing");
		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		getColumnNames(columnNames);

		// Assign the data as appropriate to the member variables.
		// These calls can throw DataExceptions, as documented in the comment of this method.
        int nColIndex = 0;
		m_key = data.getUnsignedLongData( row, columnNames[nColIndex++] );
		m_name = data.getStringData( row, columnNames[nColIndex++] );
		m_assetName = data.getStringData( row, columnNames[nColIndex++] );
		m_isSystem = SubsystemHelper::stringToBoolean(data.getStringData( row, columnNames[nColIndex++] ));
		m_isMerge = SubsystemHelper::stringToBoolean(data.getStringData(row, columnNames[nColIndex++]));
        m_mergeName = data.getStringData(row, columnNames[nColIndex++]);
        m_ulParentKey = data.getUnsignedLongData(row, columnNames[nColIndex++]);
        m_ulParentKey = ( 0 == m_ulParentKey ) ? TA_Base_Core::DatabaseKey::getInvalidKey() : m_ulParentKey;
        m_orderID = data.getUnsignedLongData(row, columnNames[nColIndex++]);
		m_isValidData = true;
		FUNCTION_EXIT;
	}

	
	void SubsystemHelper::invalidate()
	{
		FUNCTION_ENTRY("invalidate");
		FUNCTION_EXIT;
		m_isValidData = false;
	}

	bool SubsystemHelper::stringToBoolean(const std::string& boolean)
	{
		FUNCTION_ENTRY("stringToBoolean");
		using namespace std;
		bool bRet = false;
		if (false == boolean.empty())
		{
			std::string temp = boolean;
			transform(temp.begin(), temp.end(), temp.begin(), (int (*)(int))toupper);
			if ('Y' == temp[0] || 'T' == temp[0])
			{
				bRet = true;
			}
		}
		FUNCTION_EXIT;
		return bRet;
	}

	bool SubsystemHelper::getIsMerge()
	{
		FUNCTION_ENTRY("getIsMerge");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_isMerge;
    }

    unsigned long SubsystemHelper::getParentKey()
    {
        FUNCTION_ENTRY("getMergeName");
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_ulParentKey;
    }

	std::string SubsystemHelper::getMergeName()
	{
		FUNCTION_ENTRY("getMergeName");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_mergeName;
	}

	unsigned long SubsystemHelper::getOrderID()
	{
		FUNCTION_ENTRY("getOrderID");
		if (!m_isValidData)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_orderID;
	}
}
