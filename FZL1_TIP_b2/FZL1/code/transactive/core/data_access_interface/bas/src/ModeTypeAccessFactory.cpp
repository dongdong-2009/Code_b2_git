
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/bas/src/ModeTypeAccessFactory.h"

namespace TA_IRS_Core
{
	ModeTypeAccessFctory* ModeTypeAccessFctory::m_instance = NULL;
	ModeTypeAccessFctory::ModeTypeAccessFctory()
	{
		FUNCTION_ENTRY("ModeTypeAccessFctory");
		FUNCTION_EXIT;
	}

	ModeTypeAccessFctory::~ModeTypeAccessFctory()
	{
		FUNCTION_ENTRY("~ModeTypeAccessFctory");
		FUNCTION_EXIT;
	}

	ModeTypeAccessFctory& ModeTypeAccessFctory::getInstance()
	{
		FUNCTION_ENTRY("getInstance");
		if (NULL == m_instance)
		{
			m_instance = new ModeTypeAccessFctory();
		}
		FUNCTION_EXIT;
		return *m_instance;
	}

	ModeTypeMap ModeTypeAccessFctory::getModeTypes()
	{
		FUNCTION_ENTRY("getModeTypes");
		ModeTypeMap modeTypes;
		if (false == getModeTypes(modeTypes))
		{
			modeTypes.clear();
		}
		FUNCTION_EXIT;
		return modeTypes;
	}

	bool ModeTypeAccessFctory::getModeTypes(ModeTypeMap& modeTypes)
	{
		FUNCTION_ENTRY("getModeTypes");
		bool bSuccess = false;
		modeTypes.clear();

		/*std::ostringstream sql;
		sql << "SELECT PKEY, NAME, DESCRIPTION, ISSYSTEM FROM BA_MODETYPES"
			<< " ORDER BY PKEY ASC";*/
		TA_Base_Core::SQLStatement sql;
		try
		{
			// get a connection to the database
			TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
			databaseConnection->prepareSQLStatement(sql, BA_MODETYPES_SELECT_89001);
			// Set up the columnNames vector to be passed to executeQuery()
			std::vector<std::string> columnNames;
			columnNames.push_back("PKEY");
			columnNames.push_back("NAME");
			columnNames.push_back("DESCRIPTION");
			columnNames.push_back("ISSYSTEM");

			// Execute the query. The method can throw a DatabaseException.
			// This is documented in the comment of this method.
			// We are responsible for deleting the returned IData object when we're done with it
			//TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
			TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);
		
			// Loop for each row returned
			do 
			{
				for (unsigned long i = 0; i < data->getNumRows(); i++)
				{
					// The getUnsignedLongData() call can throw an exception. 
					// Need to catch it to do pointer clean up.
					ModeTypeInfo info;
					info.pkey = data->getUnsignedLongData(i, "PKEY");
					info.modeTypeName = data->getStringData(i, "NAME");
					info.Description = data->getStringData(i, "DESCRIPTION");
					info.isSystem = ModeTypeAccessFctory::stringToBoolean(data->getStringData(i,"ISSYSTEM"));
					modeTypes[info.pkey] = info;
				}
				delete data;
				data = NULL;
				// if data->getNumRows() <= 0 then call databaseConnection->moreData
				// will assert. so we need have a judgement
				if (false != modeTypes.empty())
				{
					throw TA_Base_Core::DataException("no value found in Database", TA_Base_Core::DataException::NO_VALUE, "what's the data should be." );
				}
			} 
			while (databaseConnection->moreData(data));
			bSuccess = true;
		}
		catch(const TA_Base_Core::TransactiveException& err)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
			modeTypes.clear();
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Unknown exception when ModeAccessFactory::getAllModes()");
			modeTypes.clear();
		}

		FUNCTION_EXIT;
		return bSuccess;
	}

	bool ModeTypeAccessFctory::stringToBoolean(const std::string& boolean)
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
}
