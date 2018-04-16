
#include <time.h>
#include "core/data_access_interface/bas/src/BASTimeTableMappingAccessFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DataConversion.h"
#include "core/data_access_interface/bas/src/BASTimeTableTimeTableAccessFactory.h"

namespace TA_IRS_Core
{
	const std::string BASTimeTableMappingAccessFactory::TableName = "BA_TimeTable_TimeTableMapping";
	const std::string BASTimeTableMappingAccessFactory::ViewTableName = "BA_TIMETABLE_TIMETABLE_V";
	const std::string BASTimeTableMappingAccessFactory::ColumnName_Pkey = "PKEY";
	const std::string BASTimeTableMappingAccessFactory::ColumnName_TimeTableKey = "TIMETABLEKEY";
	const std::string BASTimeTableMappingAccessFactory::ColumnName_ModeTimeKey = "MODETIMEKEY";
	const std::string BASTimeTableMappingAccessFactory::ColumnName_LocationName_V = "LOCATIONNAME";
	const std::string BASTimeTableMappingAccessFactory::ColumnName_Description_V = "MODEDESCRIPTION";
	const std::string BASTimeTableMappingAccessFactory::ColumnName_ModeSubsystemKey_V = "MODESUBSYSTEMKEY";
	const std::string BASTimeTableMappingAccessFactory::ColumnName_ModeSubsystemName_V = "MODESUBSYSTEMNAME";
	const std::string BASTimeTableMappingAccessFactory::ColumnName_ModeName_V = "MODENAME";
	const std::string BASTimeTableMappingAccessFactory::ColumnName_ModeValue_V = "MODEVALUE";
	BASTimeTableMappingAccessFactory::BASTimeTableMappingAccessFactory(void)
	{
	}

	BASTimeTableMappingAccessFactory::~BASTimeTableMappingAccessFactory(void)
	{
	}

	void BASTimeTableMappingAccessFactory::addTimeTableMapping(unsigned long timeTableKey, unsigned long modeTimeKey)
	{
		FUNCTION_ENTRY("addTimeTableMapping");
		using namespace TA_Base_Core;
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(BasTimetable_Cd, Write);

		/*std::ostringstream ss;
		ss << "INSERT INTO " << TableName
			<< "( " << ColumnName_Pkey << " , " << ColumnName_TimeTableKey << " , " << ColumnName_ModeTimeKey << " ) "
			<< "VALUES (" << getNextPkey() << " , " << timeTableKey << " , " << modeTimeKey << ")";
		std::string strSql = ss.str();*/
		//AQ:('%s','PUBLIC', 'INSERT INTO BA_TIMETABLE_TIMETABLEMAPPING(PKEY,TIMETABLEKEY,MODETIMEKEY) VALUES(%s,%s,%s)')
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_TIMETABLEMAPPING_INSERT_95001, 
			BASTimeTableModeTimeAccessFactory::getLocalDatabaseName(), getNextPkey(), timeTableKey, modeTimeKey);
		//databaseConnection->executeModification(BASTimeTableModeTimeAccessFactory::constructAQString(strSql));
		databaseConnection->executeModification(sql);
		FUNCTION_EXIT;
	}

	void BASTimeTableMappingAccessFactory::deleteTimeTableMapping(unsigned long pKey)
	{
		FUNCTION_ENTRY("deleteTimeTableMapping");
		std::vector<unsigned long> pkeyVct;
		pkeyVct.push_back(pKey);
		deleteTimeTableMapping(pkeyVct);
		FUNCTION_EXIT;
	}

	void BASTimeTableMappingAccessFactory::deleteTimeTableMapping(const std::vector<unsigned long>& pKeyVct)
	{
		FUNCTION_ENTRY("deleteTimeTableMapping");
		using namespace TA_Base_Core;
		if (pKeyVct.empty())
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "there no data to delete.");
			return;
		}
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(BasTimetable_Cd, Write);

		/*std::ostringstream ss;
		ss << "DELETE FROM " << TableName
			<< " WHERE " << ColumnName_Pkey << " in " << getSqlInPart(pKeyVct,true);
		std::string strSql = ss.str();*/
		/*DELETE FROM BA_TIMETABLE_TIMETABLEMAPPING WHERE PKEY IN %s*/
		/*AQ:('%s','PUBLIC', 'DELETE FROM BA_TIMETABLE_TIMETABLEMAPPING WHERE PKEY IN %s')*/
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_TIMETABLEMAPPING_DELETE_95002, 
			BASTimeTableModeTimeAccessFactory::getLocalDatabaseName(), getSqlInPart(pKeyVct,true));
		//databaseConnection->executeModification(BASTimeTableModeTimeAccessFactory::constructAQString(strSql));
		databaseConnection->executeModification(sql);
		FUNCTION_EXIT;
	}

	unsigned long BASTimeTableMappingAccessFactory::getNextPkey()
	{
		FUNCTION_ENTRY("getNextPkey");
		using namespace TA_Base_Core;
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(BasTimetable_Cd, Write);

		/*std::ostringstream ss;
		ss << "SELECT " << ColumnName_Pkey
			<< " FROM " << TableName 
			<< " WHERE ROWNUM <= 1"
			<< " ORDER BY " << ColumnName_Pkey << " DESC";
		std::string strSql = ss.str();*/
		/*SELECT PKEY FROM BA_TIMETABLE_TIMETABLEMAPPING WHERE ROWNUM<=1 ORDER BY PKEY DESC*/
		TA_Base_Core::SQLStatement sql;

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ColumnName_Pkey);

		//IData* data = databaseConnection->executeQuery(strSql, columnNames);
		databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_TIMETABLEMAPPING_SELECT_95003); 
		IData* data = databaseConnection->executeQuery(sql, columnNames);
		unsigned long newPkey = 0;

		if (data->getNumRows() > 0)
		{
			newPkey = data->getUnsignedLongData(0, ColumnName_Pkey) + 1;
		}
		delete data;
		data = NULL;

		FUNCTION_EXIT;
		return newPkey;
	}

	void BASTimeTableMappingAccessFactory::getAllTimeTableScheduleInfo(TimeTableScheduleInfoVct& timeTableScheduleVct)
	{
		FUNCTION_ENTRY("getAllTimeTableScheduleInfo");
		std::string strWherePart("");
		getAllTimeTableScheduleInfo(timeTableScheduleVct, strWherePart);
		FUNCTION_EXIT;
	}

	void BASTimeTableMappingAccessFactory::getAllTimeTableScheduleInfoByLocationAndType(
					unsigned long locationKey, const std::string& typeName, 
					TimeTableScheduleInfoVct& timeTableScheduleVct)
	{
		FUNCTION_ENTRY("getAllTimeTableScheduleInfoByLocationAndType");

		std::ostringstream ss;
		ss << BASTimeTableTimeTableAccessFactory::ColumnName_locationKey << " = " << locationKey;
		if (!typeName.empty())
		{
			ss << " AND " << BASTimeTableTimeTableAccessFactory::ColumnName_TypeName << " = '" << typeName << "'";
		}
        ss  << " ORDER BY " << ColumnName_Pkey << " ASC";
		std::string strWherePart = ss.str();
		getAllTimeTableScheduleInfo(timeTableScheduleVct, strWherePart);
		FUNCTION_EXIT;
	}

	void BASTimeTableMappingAccessFactory::getAllTimeTableScheduleInfoByTableTableKey(unsigned long timeTableKey, TimeTableScheduleInfoVct& timeTableScheduleVct)
	{
		FUNCTION_ENTRY("getAllTimeTableScheduleInfoByTableTableKey");
		unsigned long locationKey;
		std::string typeName;
		BASTimeTableTimeTableAccessFactoryInstance::instance()->getLocationKeyAndTypeByKey(timeTableKey, locationKey, typeName);

		std::ostringstream ss;
		ss << BASTimeTableTimeTableAccessFactory::ColumnName_locationKey << " = " << locationKey;
		if (false == typeName.empty())
		{
			ss << " AND " << BASTimeTableTimeTableAccessFactory::ColumnName_TypeName << " = '" << typeName << "'";
		}
		ss  << " ORDER BY " << ColumnName_Pkey << " ASC";
		std::string strWherePart = ss.str();
		getAllTimeTableScheduleInfo(timeTableScheduleVct, strWherePart);
		FUNCTION_EXIT;
	}

	void BASTimeTableMappingAccessFactory::getAllTimeTableScheduleInfo(TimeTableScheduleInfoVct& timeTableScheduleVct, const std::string& strSqlWherePart)
	{
		FUNCTION_ENTRY("getAllTimeTableScheduleInfoByLocationAndType");
		using namespace TA_Base_Core;
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(BasTimetable_Cd, Read);

		/*std::ostringstream ss;
		ss << "SELECT " << ColumnName_Pkey << " , "
			<< BASTimeTableTimeTableAccessFactory::ColumnName_locationKey << " , "
			<< ColumnName_LocationName_V << " , "
			<< BASTimeTableTimeTableAccessFactory::ColumnName_TypeName << " , "
			<< BASTimeTableModeTimeAccessFactory::ColumnName_ModeKey << " , "
			<< ColumnName_ModeName_V << " , " 
			<< ColumnName_ModeValue_V << " , "
			<< ColumnName_Description_V << " , "
			<< BASTimeTableModeTimeAccessFactory::ColumnName_BeginTime
			<< " FROM " << ViewTableName;
		if (false == strSqlWherePart.empty())
		{
			ss << " WHERE " << strSqlWherePart;
		}		
		std::string strSql = ss.str();
		*/
		/*SELECT PKEY,LOCATIONKEY,LOCATIONNAME,TYPENAME,MODEKEY,MODENAME,MODEVALUE,MODEDESCRIPTION,BEGINTIME FROM BA_TIMETABLE_TIMETABLE_V*/
		/*SELECT PKEY,LOCATIONKEY,LOCATIONNAME,TYPENAME,MODEKEY,MODENAME,MODEVALUE,MODEDESCRIPTION,BEGINTIME FROM BA_TIMETABLE_TIMETABLE_V WHERE %s*/
		TA_Base_Core::SQLStatement sql;

		if (false == strSqlWherePart.empty())
		{
			databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_TIMETABLE_V_SELECT_96002, strSqlWherePart);			
		}
		else
		{
			databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_TIMETABLE_V_SELECT_96001);
		}

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back( ColumnName_Pkey );
		columnNames.push_back( BASTimeTableTimeTableAccessFactory::ColumnName_locationKey );
		columnNames.push_back( ColumnName_LocationName_V );
		columnNames.push_back( BASTimeTableTimeTableAccessFactory::ColumnName_TypeName );
		columnNames.push_back( BASTimeTableModeTimeAccessFactory::ColumnName_ModeKey );
		columnNames.push_back( ColumnName_ModeName_V );
		columnNames.push_back( ColumnName_ModeValue_V );
		columnNames.push_back( ColumnName_Description_V );
		columnNames.push_back( BASTimeTableModeTimeAccessFactory::ColumnName_BeginTime );

		//IData* data = databaseConnection->executeQuery(strSql, columnNames);
		IData* data = databaseConnection->executeQuery(sql, columnNames);
		if (0 >= data->getNumRows())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[No Data]there is no data found in database");
			return;
		}

		timeTableScheduleVct.clear();
		do 
		{
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				TimeTableScheduleInfo_SmartPtr ttsi(new TimeTableScheduleInfo);
				ttsi->m_pkey = data->getUnsignedLongData(i, ColumnName_Pkey);
				ttsi->m_locationKey = data->getUnsignedLongData(i, BASTimeTableTimeTableAccessFactory::ColumnName_locationKey);
				ttsi->m_locationName = data->getStringData(i, ColumnName_LocationName_V);
				ttsi->m_typeName = data->getStringData(i, BASTimeTableTimeTableAccessFactory::ColumnName_TypeName);
				ttsi->m_modeKey = data->getUnsignedLongData(i, BASTimeTableModeTimeAccessFactory::ColumnName_ModeKey);
				ttsi->m_modeName = data->getStringData(i, ColumnName_ModeName_V);
				ttsi->m_modeValue = data->getStringData(i, ColumnName_ModeValue_V);
				ttsi->m_modeDescription = data->getStringData(i, ColumnName_Description_V);
                ttsi->m_tmBegin = transformTimeString(data->getStringData(i, BASTimeTableModeTimeAccessFactory::ColumnName_BeginTime));
				timeTableScheduleVct.push_back(ttsi);
			}
			delete data;
			data = NULL;
		} while (databaseConnection->moreData(data));
		FUNCTION_EXIT;
	}

	std::string BASTimeTableMappingAccessFactory::getSqlInPart(std::vector<unsigned long> vctValue, bool withBrackets)
	{
		FUNCTION_ENTRY("getSqlInPart");
		std::ostringstream ss;

		std::vector<unsigned long>::iterator it;
		for (it = vctValue.begin(); it != vctValue.end(); it++)
		{
			ss << *it << " , ";
		}

		std::string strSql = ss.str();
		if (false == strSql.empty())
		{
			strSql = strSql.substr(0, strSql.find_last_of(','));
		}

		if (withBrackets)
		{
			strSql = "( " + strSql + " )";
		}
		FUNCTION_EXIT;
		return strSql;
	}

    time_t BASTimeTableMappingAccessFactory::transformTimeString( const std::string& strSrc )
    {
        time_t tmNow = time( NULL );
        tm tmConvert = *(localtime( &tmNow ));

        if ( strSrc.size() > 1 )
        {
            tmConvert.tm_min = TA_Base_Core::DataConversion::toInteger( strSrc.substr( 2 ));
            tmConvert.tm_hour = TA_Base_Core::DataConversion::toInteger( strSrc.substr( 0, 2 ));
        }
        else
        {
            tmConvert.tm_hour = 0;
            tmConvert.tm_min = 0;
        }

        tmNow = mktime( &tmConvert );

        return tmNow;
    }

    std::set<unsigned long> BASTimeTableMappingAccessFactory::getAllValidLocationKeys()
    {
        FUNCTION_ENTRY("getAllValidLocationKeys");

        // get a connection to the database
       TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::BasTimetable_Cd, TA_Base_Core::Read);

        /*std::string strSql = "select unique(LOCATIONKEY) from BA_TIMETABLE_TIMETABLE";*/
		TA_Base_Core::SQLStatement sql;

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back( "LOCATIONKEY" );

		databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_TIMETABLE_SELECT_97001);
        //TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
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
