#include "ViTriggeringMapHelper.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include <string>
#include <sstream>

const static std::string TABLE_NAME = "VI_TRIGGERING_MAP";
const static std::string COLUMN_PKEY = "TRIGGERING_ENTITY_KEY";
const static std::string COLUMN_CAMERA_ENTITYKEY = "CAMERA_ENTITY_KEY";

ViTriggeringMapHelper::ViTriggeringMapHelper(void)
{
}

ViTriggeringMapHelper::~ViTriggeringMapHelper(void)
{
}

ViTriggeringMap ViTriggeringMapHelper::load(unsigned long triggeringEntity)
{
    TA_Base_Core::IDatabase* databaseConnection =
		TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);
    /*std::ostringstream sql;
    sql << "SELECT " << COLUMN_CAMERA_ENTITYKEY << " "
	    << "FROM " << TABLE_NAME << " "
		<< "WHERE " << COLUMN_PKEY << " = " << triggeringEntity;*/
    std::vector<std::string> columnNames;
    columnNames.push_back(COLUMN_CAMERA_ENTITYKEY);
    // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
    // This is documented in the comment of this method.
    // We are responsible for deleting the returned IData object when we're done with it

    TA_Base_Core::SQLStatement sql;
    databaseConnection->prepareSQLStatement(sql, VI_TRIGGERING_MAP_130001, triggeringEntity);

    std::auto_ptr< TA_Base_Core::IData > data;
    data = std::auto_ptr< TA_Base_Core::IData >(databaseConnection->executeQuery(sql, columnNames));
    std::stringstream error;
    if(0 == data->getNumRows())  // No entry found with the specified pkey
    {
        error << "No config for " << COLUMN_PKEY << " = " << triggeringEntity; 
        TA_THROW(TA_Base_Core::DataException(error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, ""));
    }
	unsigned long cameraEntity = data->getUnsignedLongData(0, COLUMN_CAMERA_ENTITYKEY);
    return ViTriggeringMap(triggeringEntity, cameraEntity);
}

std::vector<ViTriggeringMap> ViTriggeringMapHelper::load()
{
    TA_Base_Core::IDatabase* databaseConnection =
		TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);
    /*std::ostringstream sql;
    sql << "SELECT " << COLUMN_PKEY << "," << COLUMN_CAMERA_ENTITYKEY << " "
	    << "FROM " << TABLE_NAME;*/
    std::vector<std::string> columnNames;
    columnNames.push_back(COLUMN_PKEY);
    columnNames.push_back(COLUMN_CAMERA_ENTITYKEY);
    // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
    // This is documented in the comment of this method.
    // We are responsible for deleting the returned IData object when we're done with it
    
    TA_Base_Core::SQLStatement sql;
    databaseConnection->prepareSQLStatement(sql, VI_TRIGGERING_MAP_130002);

    TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);
    std::vector<ViTriggeringMap> records;
	unsigned long triggeringEntityKey;
	unsigned long cameraEntityKey;
    do
    {
		std::auto_ptr<TA_Base_Core::IData> dataWrap(data);
        for(unsigned long i = 0, size = data->getNumRows(); i < size; ++i)
        {
			triggeringEntityKey = dataWrap->getUnsignedLongData(i, COLUMN_PKEY);
            cameraEntityKey = dataWrap->getUnsignedLongData(i, COLUMN_CAMERA_ENTITYKEY);
            records.push_back(ViTriggeringMap(triggeringEntityKey, cameraEntityKey));
        }
    }
    while(databaseConnection->moreData(data));
    return records;
}
