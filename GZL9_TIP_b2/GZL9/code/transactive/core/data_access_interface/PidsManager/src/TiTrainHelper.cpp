#include "TiTrainHelper.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include <string>
#include <sstream>

const static std::string TABLE_NAME = "TI_TRAIN";
const static std::string COLUMN_PKEY = "PKEY";
const static std::string COLUMN_TRAIN_NO = "TRAIN_NO";
const static std::string COLUMN_DISPLAY_TEXT = "DISPLAY_TEXT";

TiTrain TiTrainHelper::load(unsigned long key)
{
    TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);
    /*std::ostringstream sql;
    sql << "SELECT ";
    sql << COLUMN_TRAIN_NO << ", ";
    sql << COLUMN_DISPLAY_TEXT;
    sql << " FROM " << TABLE_NAME;
    sql << " WHERE " << COLUMN_PKEY << " = " << key;*/

	TA_Base_Core::SQLStatement strSql;
	databaseConnection->prepareSQLStatement(strSql, TI_TRAIN_SELECT_160001, key);

    std::vector<std::string> columnNames;
    columnNames.push_back(COLUMN_TRAIN_NO);
    columnNames.push_back(COLUMN_DISPLAY_TEXT);
    // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
    // This is documented in the comment of this method.
    // We are responsible for deleting the returned IData object when we're done with it
    std::auto_ptr< TA_Base_Core::IData > data;
    data = std::auto_ptr< TA_Base_Core::IData >(databaseConnection->executeQuery(strSql, columnNames));
    std::stringstream error;
    if(0 == data->getNumRows())  // No entry found with the specified pkey
    {
        error << "No train config for pkey = " << key;
        TA_THROW(TA_Base_Core::DataException(error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, ""));
    }
    unsigned long trainNo = 0;
	std::string displayText = "";
    trainNo = data->getUnsignedLongData(0, COLUMN_TRAIN_NO);
    displayText = data->getStringData(0, COLUMN_DISPLAY_TEXT);

    return TiTrain(key, trainNo, displayText);
}

std::vector<TiTrain> TiTrainHelper::load()
{
    TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);
    /*std::ostringstream sql;
    sql << "SELECT ";
    sql << COLUMN_PKEY << ", ";
    sql << COLUMN_TRAIN_NO << ", ";
    sql << COLUMN_DISPLAY_TEXT;
    sql << " FROM " << TABLE_NAME;*/

	TA_Base_Core::SQLStatement strSql;
	databaseConnection->prepareSQLStatement(strSql, TI_TRAIN_SELECT_160002);

    std::vector<std::string> columnNames;
    columnNames.push_back(COLUMN_PKEY);
    columnNames.push_back(COLUMN_TRAIN_NO);
    columnNames.push_back(COLUMN_DISPLAY_TEXT);
    // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
    // This is documented in the comment of this method.
    // We are responsible for deleting the returned IData object when we're done with it
    TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
    std::vector<TiTrain> records;
    unsigned long key = 0;
	int trainNo = 0;
	std::string displayText = "";
    do
    {
		std::auto_ptr<TA_Base_Core::IData> dataWrap(data);
        for(unsigned long i = 0, size = data->getNumRows(); i < size; ++i)
        {
            key = dataWrap->getUnsignedLongData(i, COLUMN_PKEY);
            trainNo = dataWrap->getIntegerData(i, COLUMN_TRAIN_NO);
            displayText = data->getStringData(i, COLUMN_DISPLAY_TEXT);

            records.push_back(TiTrain(key, trainNo, displayText));
        }
    }
    while(databaseConnection->moreData(data));
    return records;
}