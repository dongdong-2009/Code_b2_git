
#include <sstream>
#include "core/data_access_interface/bas/src/ModeAccessFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
	// the table name and column name of modes table
	const std::string ModeAccessFactory::m_tableName_modes_V = "BA_MODES_V";
	const std::string ModeAccessFactory::m_column_pkey = "PKEY";
	const std::string ModeAccessFactory::m_column_name = "MODENAME";
	const std::string ModeAccessFactory::m_column_value = "VALUE";
	const std::string ModeAccessFactory::m_column_locationKey = "LOCATIONKEY";
	const std::string ModeAccessFactory::m_column_locationName = "LOCATIONNAME";
	const std::string ModeAccessFactory::m_column_subsystemKey = "SUBSYSTEMKEY";
	const std::string ModeAccessFactory::m_column_subsystemName = "SUBSYSTEMNAME";
	const std::string ModeAccessFactory::m_column_typeKey = "TYPEKEY";
	const std::string ModeAccessFactory::m_column_typeName = "TYPENAME";
	const std::string ModeAccessFactory::m_column_description = "DESCRIPTION";
	ModeAccessFactory* ModeAccessFactory::m_instance = 0;
	ModeAccessFactory::ModeAccessFactory()
	{
		FUNCTION_ENTRY("ModeAccessFactory");
		FUNCTION_EXIT;
	}

	ModeAccessFactory::~ModeAccessFactory()
	{
		FUNCTION_ENTRY("~ModeAccessFactory");
		FUNCTION_EXIT;
	}

	ModeAccessFactory& ModeAccessFactory::getInstance()
	{
		FUNCTION_ENTRY("getInstance");
		if (0 == m_instance)
		{
			m_instance = new ModeAccessFactory();
		}
		FUNCTION_EXIT;
		return *m_instance;
	}

	bool ModeAccessFactory::getAllModes(std::vector< boost::shared_ptr<Mode> >& vctModes)
	{
		FUNCTION_ENTRY("getAllModes");
		bool bSeccess = false;

		/*std::ostringstream sql;
		sql << "SELECT " << m_column_pkey << " , "
			<< m_column_name << " , "
			<< m_column_value << " , "
			<< m_column_description << " , "
			<< m_column_locationKey << " , "
			<< m_column_locationName << " , "
			<< m_column_subsystemKey << " , "
			<< m_column_subsystemName << " , "
			<< m_column_typeKey << " , "
			<< m_column_typeName 
			<< " FROM " << m_tableName_modes_V
			<< " ORDER BY " << m_column_pkey << " ASC ";*/
		/*SELECT PKEY,MODENAME,VALUE,DESCRIPTION,LOCATIONKEY,LOCATIONNAME,SUBSYSTEMKEY,SUBSYSTEMNAME,TYPEKEY,TYPENAME FROM BA_MODES_V ORDER BY PKEY ASC*/
		TA_Base_Core::SQLStatement sql;
		try
		{
			// get a connection to the database
			TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

			// Set up the columnNames vector to be passed to executeQuery()
			std::vector<std::string> columnNames;
			columnNames.push_back(m_column_pkey);
			columnNames.push_back(m_column_name);
			columnNames.push_back(m_column_value);
			columnNames.push_back(m_column_description);
			columnNames.push_back(m_column_locationKey);
			columnNames.push_back(m_column_locationName);
			columnNames.push_back(m_column_subsystemKey);
			columnNames.push_back(m_column_subsystemName);
			columnNames.push_back(m_column_typeKey);
			columnNames.push_back(m_column_typeName);

			// Execute the query. The method can throw a DatabaseException.
			// This is documented in the comment of this method.
			// We are responsible for deleting the returned IData object when we're done with it
			//TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);
			databaseConnection->prepareSQLStatement(sql, BA_MODES_V_SELECT_86001);
			TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);

			// Loop for each row returned
			do 
			{
				for (unsigned long i = 0; i < data->getNumRows(); i++)
				{
					// The getUnsignedLongData() call can throw an exception. Need to catch
					// it to do pointer clean up.
					try
					{
						Mode_SmartPtr temp(new Mode(i, *data));
						vctModes.push_back(temp);
					}
					catch (TA_Base_Core::DataException& ex)
					{
						// Clean up the data pointer
						delete data;
						data = NULL;
						TA_THROW(TA_Base_Core::DataException(ex.what(), ex.getFailType(), ex.getWhichData()));
					}
				}
				delete data;
				data = NULL;

				// if data->getNumRows() <= 0 then call databaseConnection->moreData
				// will assert. so we need have a judgement
				if (false != vctModes.empty())
				{
					throw TA_Base_Core::DataException("no value found in Database", TA_Base_Core::DataException::NO_VALUE, "what's the data should be." );
				}
			} 
			while (databaseConnection->moreData(data));
			bSeccess = true;
		}
		catch(const TA_Base_Core::TransactiveException& err)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
			vctModes.clear();
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Unknown exception when ModeAccessFactory::getAllModes()");
			vctModes.clear();
		}

		FUNCTION_EXIT;
		return bSeccess;
	}

	std::vector< boost::shared_ptr<Mode> > ModeAccessFactory::getAllModes()
	{
		FUNCTION_ENTRY("getAllModes");
		std::vector<Mode_SmartPtr> modes;
		if(false == getAllModes(modes))
		{
			modes.clear();
		}
		FUNCTION_EXIT;
		return modes;
	}

	std::vector<Mode_SmartPtr> ModeAccessFactory::getModes(unsigned int stationKey, 
		unsigned int subsystemKey, unsigned int typeKey)
	{
		FUNCTION_ENTRY("getModes");
		std::vector<Mode_SmartPtr> modes;

		/*std::ostringstream sql;
		sql << "SELECT " << m_column_pkey << " , "
			<< m_column_name << " , "
			<< m_column_value << " , "
			<< m_column_description << " , "
			<< m_column_locationKey << " , "
			<< m_column_locationName << " , "
			<< m_column_subsystemKey << " , "
			<< m_column_subsystemName << " , "
			<< m_column_typeKey << " , "
			<< m_column_typeName 
			<< " FROM " << m_tableName_modes_V << " where " 
			<< m_column_locationKey << " = " << stationKey << " and "
			<< m_column_subsystemKey << " = " << subsystemKey << " and "
			<< m_column_typeKey << " = " << typeKey
			<< " ORDER BY " << m_column_pkey << " ASC ";*/
		/*SELECT PKEY,MODENAME,VALUE,DESCRIPTION,LOCATIONKEY,LOCATIONNAME,SUBSYSTEMKEY,SUBSYSTEMNAME,TYPEKEY,TYPENAME FROM BA_MODES_V WHERE LOCATIONKEY=%s AND SUBSYSTEMKEY=%s AND TYPEKEY=%s  ORDER BY PKEY ASC*/
		TA_Base_Core::SQLStatement sql;

		try
		{
			// get a connection to the database
			TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

			// Set up the columnNames vector to be passed to executeQuery()
			std::vector<std::string> columnNames;
			columnNames.push_back(m_column_pkey);
			columnNames.push_back(m_column_name);
			columnNames.push_back(m_column_value);
			columnNames.push_back(m_column_description);
			columnNames.push_back(m_column_locationKey);
			columnNames.push_back(m_column_locationName);
			columnNames.push_back(m_column_subsystemKey);
			columnNames.push_back(m_column_subsystemName);
			columnNames.push_back(m_column_typeKey);
			columnNames.push_back(m_column_typeName);

			// Execute the query. The method can throw a DatabaseException.
			// This is documented in the comment of this method.
			// We are responsible for deleting the returned IData object when we're done with it
			//TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

			databaseConnection->prepareSQLStatement(sql, BA_MODES_V_SELECT_86002, stationKey, subsystemKey, typeKey);
			TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);

			// Loop for each row returned
			do 
			{
				for (unsigned long i = 0; i < data->getNumRows(); i++)
				{
					// The getUnsignedLongData() call can throw an exception. Need to catch
					// it to do pointer clean up.
					try
					{
						Mode_SmartPtr temp(new Mode(i, *data));
						modes.push_back(temp);
					}
					catch (TA_Base_Core::DataException& ex)
					{
						// Clean up the data pointer
						delete data;
						data = NULL;
						TA_THROW(TA_Base_Core::DataException(ex.what(), ex.getFailType(), ex.getWhichData()));
					}
				}
				delete data;
				data = NULL;

				// if data->getNumRows() <= 0 then call databaseConnection->moreData
				// will assert. so we need have a judgement
				if (false != modes.empty())
				{
					throw TA_Base_Core::DataException("no value found in Database", TA_Base_Core::DataException::NO_VALUE, "what's the data should be." );
				}
			} 
			while (databaseConnection->moreData(data));
		}
		catch(const TA_Base_Core::TransactiveException& err)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
			modes.clear();
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Unknown exception when ModeAccessFactory::getAllModes()");
			modes.clear();
		}
		FUNCTION_EXIT;
		return modes;
	}

	bool ModeAccessFactory::getModes(unsigned int stationKey, std::vector<Mode_SmartPtr>& modes)
	{
		FUNCTION_ENTRY("getModes");
		bool bSeccess = false;

		/*std::ostringstream sql;
		sql << "SELECT " << m_column_pkey << " , "
			<< m_column_name << " , "
			<< m_column_value << " , "
			<< m_column_description << " , "
			<< m_column_locationKey << " , "
			<< m_column_locationName << " , "
			<< m_column_subsystemKey << " , "
			<< m_column_subsystemName << " , "
			<< m_column_typeKey << " , "
			<< m_column_typeName 
			<< " FROM " << m_tableName_modes_V << " WHERE " 
			<< m_column_locationKey << " = " << stationKey
			<< " ORDER BY " << m_column_pkey << " ASC ";*/
		/*SELECT PKEY,MODENAME,VALUE,DESCRIPTION,LOCATIONKEY,LOCATIONNAME,SUBSYSTEMKEY,SUBSYSTEMNAME,TYPEKEY,TYPENAME FROM BA_MODES_V WHERE LOCATIONKEY=%s ORDER BY PKEY ASC*/
		TA_Base_Core::SQLStatement sql;

		try
		{
			// get a connection to the database
			TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

			// Set up the columnNames vector to be passed to executeQuery()
			std::vector<std::string> columnNames;
			columnNames.push_back(m_column_pkey);
			columnNames.push_back(m_column_name);
			columnNames.push_back(m_column_value);
			columnNames.push_back(m_column_description);
			columnNames.push_back(m_column_locationKey);
			columnNames.push_back(m_column_locationName);
			columnNames.push_back(m_column_subsystemKey);
			columnNames.push_back(m_column_subsystemName);
			columnNames.push_back(m_column_typeKey);
			columnNames.push_back(m_column_typeName);

			// Execute the query. The method can throw a DatabaseException.
			// This is documented in the comment of this method.
			// We are responsible for deleting the returned IData object when we're done with it
			//TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.c_str(), columnNames);
			databaseConnection->prepareSQLStatement(sql, BA_MODES_V_SELECT_86003, stationKey);
			TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);

			// Loop for each row returned
			do 
			{
				for (unsigned long i = 0; i < data->getNumRows(); i++)
				{
					// The getUnsignedLongData() call can throw an exception. Need to catch
					// it to do pointer clean up.
					try
					{
						Mode_SmartPtr temp(new Mode(i, *data));
						modes.push_back(temp);
					}
					catch (TA_Base_Core::DataException& ex)
					{
						// Clean up the data pointer
						delete data;
						data = NULL;
						TA_THROW(TA_Base_Core::DataException(ex.what(), ex.getFailType(), ex.getWhichData()));
					}
				}
				delete data;
				data = NULL;

				// if data->getNumRows() <= 0 then call databaseConnection->moreData
				// will assert. so we need have a judgement
				if (false != modes.empty())
				{
					TA_THROW(TA_Base_Core::DataException("no value found in Database", TA_Base_Core::DataException::NO_VALUE, "what's the data should be." ));
				}
			} 
			while (databaseConnection->moreData(data));
			bSeccess = true;
		}
		catch(const TA_Base_Core::TransactiveException& err)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
			modes.clear();
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Unknown exception when ModeAccessFactory::getAllModes()");
			modes.clear();
		}

		FUNCTION_EXIT;
		return bSeccess;
	}
}
