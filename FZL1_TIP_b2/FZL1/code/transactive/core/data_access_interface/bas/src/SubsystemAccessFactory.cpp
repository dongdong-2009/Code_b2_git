
#include <sstream>
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/bas/src/SubsystemAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
	SubsystemAccessFactory* SubsystemAccessFactory::m_instance = 0;
	SubsystemAccessFactory::SubsystemAccessFactory()
	{
		FUNCTION_ENTRY("SubsystemAccessFactory");
		FUNCTION_EXIT;
	}
	
	SubsystemAccessFactory::~SubsystemAccessFactory()
	{
		FUNCTION_ENTRY("~SubsystemAccessFactory");
		FUNCTION_EXIT;
	}

	SubsystemAccessFactory& SubsystemAccessFactory::getInstance()
	{
		FUNCTION_ENTRY("getInstance");
		if (0 == m_instance)
		{
			m_instance = new SubsystemAccessFactory();
		}
		FUNCTION_EXIT;
		return *m_instance;
	}

	AllLocationSubsystems SubsystemAccessFactory::getAllSubsystems()
	{
		FUNCTION_ENTRY("getAllSubsystems");
		AllLocationSubsystems ret;
		if (false == getAllSubsystems(ret))
		{
			ret.clear();
		}		
		FUNCTION_EXIT;
		return ret;
	}

	bool SubsystemAccessFactory::getAllSubsystems(AllLocationSubsystems& subSystems)
	{
		FUNCTION_ENTRY("getAllSubsystems");
		bool bSuccess = false;
		subSystems.clear();

		/*std::ostringstream sql;
        sql << "SELECT LOCATIONKEY, LOCATIONNAME, PKEY, NAME, ASSET, ISSYSTEM, ISMERGE, MERGENAME, "
            << "nvl((select PKEY from BA_SUBSYSTEMS t1 where t1.NAME = t2.MERGENAME and t1.ISMERGE = 'Y'), 0) as PARENTKEY, " 
            << "ORDER_ID FROM BA_SUBSYSTEM_MAP_V t2 ORDER BY LOCATIONKEY, ORDER_ID ASC";*/
		TA_Base_Core::SQLStatement sql;

		try
		{
			// get a connection to the database
			TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read); 

			// Set up the columnNames vector to be passed to executeQuery()
			std::vector<std::string> columnNames;
			columnNames.push_back("LOCATIONKEY");
			columnNames.push_back("LOCATIONNAME");
			columnNames.push_back("PKEY");
			columnNames.push_back("NAME");
			columnNames.push_back("ASSET");
			columnNames.push_back("ISSYSTEM");
			columnNames.push_back("ISMERGE");
            columnNames.push_back("MERGENAME");
            columnNames.push_back("PARENTKEY");
			columnNames.push_back("ORDER_ID");

			// Execute the query. The method can throw a DatabaseException.
			// This is documented in the comment of this method.
			// We are responsible for deleting the returned IData object when we're done with it
			//TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);
			databaseConnection->prepareSQLStatement(sql, BA_SUBSYSTEM_MAP_V_SELECT_91001);
			TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);

			unsigned long locationKey = 0;
			LocationSubsystem locationSubsystem;
			// Loop for each row returned
			do 
			{
				// if data->getNumRows() <= 0 then call databaseConnection->moreData
				// will assert. so we need have a judgement
				if (data->getNumRows() <= 0)
				{
					throw TA_Base_Core::DataException("no value found in Database", TA_Base_Core::DataException::NO_VALUE, "what's the data should be." );
				}

				for (unsigned long i = 0; i < data->getNumRows(); i++)
				{
					// The getUnsignedLongData() call can throw an exception. Need to catch
					// it to do pointer clean up.
					unsigned long tempLocationKey = data->getUnsignedLongData(i, "LOCATIONKEY");
					if (0 == locationKey)
					{
						locationKey = tempLocationKey;
					}

					if (!locationSubsystem.empty() && locationKey != tempLocationKey)
					{
						subSystems.insert(std::make_pair(locationKey, locationSubsystem));
						locationSubsystem.clear();
						locationKey = tempLocationKey;
					}

					try
					{
						SubSystem_SmartPtr temp(new Subsystem(i, *data));
						locationSubsystem.push_back(temp);
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
			} 
			while (databaseConnection->moreData(data));

			if (0 != locationKey && !locationSubsystem.empty())
			{
				subSystems.insert(std::make_pair(locationKey, locationSubsystem));
			}
			
			bSuccess = true;
		}
		catch(const TA_Base_Core::TransactiveException& err)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
			subSystems.clear();
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Unknown exception when ModeAccessFactory::getAllModes()");
			subSystems.clear();
		}
		
		FUNCTION_EXIT;
		return bSuccess;
	}

    unsigned long SubsystemAccessFactory::getAllSubSystemKey()
    {
        FUNCTION_ENTRY("getAllSubSystemKey");

        unsigned long ulRetKey = 0u;

        /*std::string strSql = "select PKEY, ORDER_ID from BA_SUBSYSTEMS where ISSYSTEM = 'Y' order by ORDER_ID";*/
		TA_Base_Core::SQLStatement sql;

        try
        {
            // get a connection to the database
            TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

            // Set up the columnNames vector to be passed to executeQuery()
            std::vector<std::string> columnNames;
            columnNames.push_back("PKEY");
            columnNames.push_back("ORDER_ID");

            // Execute the query. The method can throw a DatabaseException.
            // This is documented in the comment of this method.
            // We are responsible for deleting the returned IData object when we're done with it
            //TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
			databaseConnection->prepareSQLStatement(sql, BA_SUBSYSTEMS_SELECT_92001);
			TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);

            if ( data->getNumRows() == 0u )
            {
                throw TA_Base_Core::DataException("no value found in Database", TA_Base_Core::DataException::NO_VALUE, "PKEY" );
            }
            // Loop for each row returned
            do 
            {
                ulRetKey = data->getUnsignedLongData( 0u, "PKEY" );

                delete data;
                data = NULL;
            } 
            while (databaseConnection->moreData(data));
        }
        catch(const TA_Base_Core::TransactiveException& err)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Unknown exception when ModeAccessFactory::getAllSubSystemKey()");
        }

        FUNCTION_EXIT;
        return ulRetKey;
    }
}

