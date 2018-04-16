
#include <sstream>
#include "core/data_access_interface/bas/src/SubsystemModeTypeMappingAccessFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
namespace TA_IRS_Core
{
	const std::string SubsystemModeTypeMappingAccessFactory::TableName = "BA_SUBSYSTEM_MODETYPE_MAP";
	const std::string SubsystemModeTypeMappingAccessFactory::ColumnName_Pkey = "PKEY";
	const std::string SubsystemModeTypeMappingAccessFactory::ColumnName_LocationKey = "LOCATIONKEY";
	const std::string SubsystemModeTypeMappingAccessFactory::ColumnName_SubsystemKey = "SUBSYSTEMKEY";
	const std::string SubsystemModeTypeMappingAccessFactory::ColumnName_TypeKey = "TYPEKEY";
	SubsystemModeTypeMappingAccessFactory::SubsystemModeTypeMappingAccessFactory(void)
	{
	}

	SubsystemModeTypeMappingAccessFactory::~SubsystemModeTypeMappingAccessFactory(void)
	{
	}

	void SubsystemModeTypeMappingAccessFactory::getSubSystemModeTypeMapByLocation(unsigned long locationKey, SubSystemModeTypeMap& subSystemTypeMaps)
	{
		FUNCTION_ENTRY("getSubSystemModeTypeMapByLocation");
#define InvalidSubsystemKey 0xFFFFFFFF

		/*std::stringstream ss;
		ss << "SELECT " << ColumnName_Pkey << " , "
			<< ColumnName_LocationKey << " , "
			<< ColumnName_SubsystemKey << " , "
			<< ColumnName_TypeKey 
			<< " FROM " << TableName
			<< " WHERE " << ColumnName_LocationKey << " = " << locationKey 
			<< " ORDER BY " << ColumnName_SubsystemKey << " , " << ColumnName_TypeKey;*/
		/*SELECT PKEY,LOCATIONKEY,SUBSYSTEMKEY,TYPEKEY FROM BA_SUBSYSTEM_MODETYPE_MAP WHERE LOCATIONKEY=%s ORDER BY SUBSYSTEMKEY,TYPEKEY*/
		TA_Base_Core::SQLStatement sql;
		//std::string strSql = ss.str();
		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ColumnName_Pkey);
		columnNames.push_back(ColumnName_LocationKey);
		columnNames.push_back(ColumnName_SubsystemKey);
		columnNames.push_back(ColumnName_TypeKey);

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		//TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
		databaseConnection->prepareSQLStatement(sql, BA_SUBSYSTEM_MODETYPE_MAP_SELECT_93001, locationKey);
		TA_Base_Core::IData* data = databaseConnection->executeQuery(sql,columnNames);
		
		subSystemTypeMaps.clear();
		modeTypes modetypes;
		unsigned long preSubsystemKey = InvalidSubsystemKey;
		do 
		{
			for (unsigned long index = 0; index < data->getNumRows(); index++)
			{
				unsigned long subsystemKey = data->getUnsignedLongData(index, ColumnName_SubsystemKey);
				unsigned long modeTypeKey = data->getUnsignedLongData(index, ColumnName_TypeKey);
				if (preSubsystemKey != InvalidSubsystemKey && preSubsystemKey != subsystemKey)
				{
					subSystemTypeMaps[preSubsystemKey] = modetypes; //save the  pre modetypes
					modetypes.clear();
				}
				modetypes.push_back(modeTypeKey);
				preSubsystemKey = subsystemKey;
			}
			
			if (subSystemTypeMaps.empty())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[No Data]there is no data found in database");
				return;
			}
			subSystemTypeMaps[preSubsystemKey] = modetypes; //save the  last modetypes
		} while (databaseConnection->moreData(data));

		FUNCTION_EXIT;
    }

    std::set<unsigned long> SubsystemModeTypeMappingAccessFactory::getAllValidLocationKeys()
    {
        FUNCTION_ENTRY("getAllValidLocationKeys");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ecs_Cd, TA_Base_Core::Read);

        /*std::string strSql = "select unique(LOCATIONKEY) from BA_SUBSYSTEM_MAP";*/
		TA_Base_Core::SQLStatement sql;

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back( "LOCATIONKEY" );

        //TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
		databaseConnection->prepareSQLStatement(sql, BA_SUBSYSTEM_MAP_SELECT_94001);
		TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);

        std::set<unsigned long> setKeys;

        do 
        {
            if ( 0u == data->getNumRows() )
            {
                break;
            }

            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                setKeys.insert( data->getUnsignedLongData( i, "LOCATIONKEY" ));
            }

            delete data;
            data = NULL;
        } while (databaseConnection->moreData(data));

        FUNCTION_EXIT;
        return setKeys;
    }
}
