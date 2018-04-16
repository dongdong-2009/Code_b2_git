
#include <sstream>
#include "core/data_access_interface/bas/src/BasFireAccessFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
	BasFireAccessFactory* BasFireAccessFactory::m_instance = NULL;
	const std::string BasFireAccessFactory::COLUMN_PKEY = "PKEY";
	const std::string BasFireAccessFactory::COLUMN_LOCATIONKEY = "LOCATIONKEY";
	const std::string BasFireAccessFactory::COLUMN_FIRETOKEN = "FIRETOKEN";
	const std::string BasFireAccessFactory::COLUMN_DESCRIPTION = "DESCRIPTION";
	const std::string BasFireAccessFactory::COLUMN_MODEVALUE = "MODEVALUE";
	const std::string BasFireAccessFactory::COLUMN_SHOWING_MFT = "SHOWING_MFT";
	const std::string BasFireAccessFactory::COLUMN_LOGIN_USER = "LOGIN_USER";
	const std::string BasFireAccessFactory::TABLE_NAME = "BA_FIRE_TRIGGER";
	BasFireAccessFactory::BasFireAccessFactory(void)
	{
	}

	BasFireAccessFactory::~BasFireAccessFactory(void)
	{
	}

	BasFireAccessFactory& BasFireAccessFactory::getInstance()
	{
		if (NULL == m_instance)
		{
			m_instance = new BasFireAccessFactory;
		}
		return *m_instance;
	}

	void BasFireAccessFactory::getAllBasFireEquipment(BasFireEquipmentMap& basFireEquipments)
	{
		FUNCTION_ENTRY("getAllBasFireEquipment");

		/*std::ostringstream sql;
		sql << "SELECT " << COLUMN_PKEY << " , "
			<< COLUMN_LOCATIONKEY << " , "
			<< COLUMN_FIRETOKEN << " , "
			<< COLUMN_DESCRIPTION << " , "
			<< COLUMN_MODEVALUE << " , "
			<< COLUMN_LOGIN_USER << " , "
			<< COLUMN_SHOWING_MFT
			<< " FROM " << TABLE_NAME
			<< " ORDER BY " << COLUMN_LOCATIONKEY;*/
		/*SELECT PKEY,LOCATIONKEY,FIRETOKEN,DESCRIPTION,MODEVALUE,LOGIN_USER,SHOWING_MFT FROM BA_FIRE_TRIGGER ORDER BY LOCATIONKEY*/
		TA_Base_Core::SQLStatement sql;

		try
		{
			// get a connection to the database
			TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);
			// Set up the columnNames vector to be passed to executeQuery()
			std::vector<std::string> columnNames;
			columnNames.push_back(COLUMN_PKEY);
			columnNames.push_back(COLUMN_LOCATIONKEY);
			columnNames.push_back(COLUMN_FIRETOKEN);
			columnNames.push_back(COLUMN_DESCRIPTION);
			columnNames.push_back(COLUMN_MODEVALUE);
			columnNames.push_back(COLUMN_LOGIN_USER);
			columnNames.push_back(COLUMN_SHOWING_MFT);

			// Execute the query. The method can throw a DatabaseException.
			// This is documented in the comment of this method.
			// We are responsible for deleting the returned IData object when we're done with it
			//TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);
			databaseConnection->prepareSQLStatement(sql, BA_FIRE_TRIGGER_SELECT_85001);
			TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);
			if (NULL == data || data->getNumRows() <= 0)
			{
				TA_THROW(TA_Base_Core::DataException("no fire trigger value found in Database", TA_Base_Core::DataException::NO_VALUE, "what's the data should be." )) ;
			}
			
			// Loop for each row returned
			basFireEquipments.clear();
			do 
			{
				for (unsigned long i = 0; i < data->getNumRows(); i++)
				{
					BasFireEquipmentPtr ptr(new BasFireEquipment);
					ptr->pkey = data->getUnsignedLongData(i, COLUMN_PKEY);
					ptr->locationKey = data->getUnsignedLongData(i, COLUMN_LOCATIONKEY);
					ptr->fireToken = data->getStringData(i, COLUMN_FIRETOKEN);
					ptr->description = data->getStringData(i, COLUMN_DESCRIPTION);
					ptr->modeValue = data->getUnsignedLongData(i, COLUMN_MODEVALUE);
					ptr->users = data->getStringData(i, COLUMN_LOGIN_USER);
					ptr->strMftNames = data->getStringData(i, COLUMN_SHOWING_MFT);
					basFireEquipments[ptr->locationKey].push_back(ptr);
				}
				delete data;
				data = NULL;
			} 
			while (databaseConnection->moreData(data));
		}
		catch(const TA_Base_Core::TransactiveException& err)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
			basFireEquipments.clear();
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Unknown exception when ModeAccessFactory::getAllModes()");
			basFireEquipments.clear();
		}

		FUNCTION_EXIT;
	}

	void BasFireAccessFactory::getAllBasFireEquipmentByLocation(unsigned long locationKey, BasFireEquipmentVct& locationFireEquipments)
	{
		FUNCTION_ENTRY("getAllBasFireEquipmentByLocation");
		BasFireEquipmentMap basFireEquipments;
		getAllBasFireEquipment(basFireEquipments);
		if (basFireEquipments.end() != basFireEquipments.find(locationKey))
		{
			locationFireEquipments = basFireEquipments[locationKey];
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "can not find the location[%d] fire data config.", locationKey);
		}
		FUNCTION_EXIT;
	}
}