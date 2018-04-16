
#include "core/data_access_interface/bas/src/BASTimeTableModeTimeAccessFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

namespace TA_IRS_Core
{
	const std::string BASTimeTableModeTimeAccessFactory::TableName = "BA_TIMETABLE_MODETIME";
	const std::string BASTimeTableModeTimeAccessFactory::ColumnName_Pkey = "PKEY";
	const std::string BASTimeTableModeTimeAccessFactory::ColumnName_ModeKey = "MODEKEY";
	const std::string BASTimeTableModeTimeAccessFactory::ColumnName_BeginTime = "BEGINTIME";
	std::string BASTimeTableModeTimeAccessFactory::m_localDatabase = "";
	BASTimeTableModeTimeAccessFactory::BASTimeTableModeTimeAccessFactory(void)
	{
		FUNCTION_ENTRY("BASTimeTableModeTimeAccessFactory");
		FUNCTION_EXIT;
	}

	BASTimeTableModeTimeAccessFactory::~BASTimeTableModeTimeAccessFactory(void)
	{
		FUNCTION_ENTRY("~BASTimeTableModeTimeAccessFactory");
		FUNCTION_EXIT;
	}

	ModeTimeInfo BASTimeTableModeTimeAccessFactory::getModeTimeInfoByKey(unsigned long pkey)
	{
		FUNCTION_ENTRY("getAllModeTimeInfoByKey");
		ModeTimeInfoMap ReturnValue;
		std::vector<unsigned long> modeTimeKeyVct;
		modeTimeKeyVct.push_back(pkey);
		getAllModeTimeInfoByKey(ReturnValue, modeTimeKeyVct);
		FUNCTION_EXIT;
		return ReturnValue[pkey];
	}

	void BASTimeTableModeTimeAccessFactory::getAllModeTimeInfoByKey(ModeTimeInfoMap& modeTimeMap, 
								const std::vector<unsigned long>& modeTimeKeyVct)
	{
		FUNCTION_ENTRY("getAllModeTimeInfoByKey");
		using namespace TA_Base_Core;
		if (modeTimeKeyVct.empty())
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "There no mode time key to filter!");
			return ;
		}
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(BasTimetable_Cd, Read);

		/*std::ostringstream ss;
		ss << "SELECT " << ColumnName_Pkey << " , "
			<< ColumnName_ModeKey << " , " << ColumnName_BeginTime 
			<< " FROM " << TableName  
			<< " WHERE " << ColumnName_Pkey << " in " << getSqlInPart(modeTimeKeyVct, false) << " )";
		std::string strSql = ss.str();*/
		/*SELECT PKEY,MODEKEY,BEGINTIME FROM BA_TIMETABLE_MODETIME WHERE PKEY in (%s)*/
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_MODETIME_SELECT_98001, getSqlInPart(modeTimeKeyVct, false));

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back( ColumnName_Pkey );
		columnNames.push_back( ColumnName_ModeKey );
		columnNames.push_back( ColumnName_BeginTime );

		ModeTimeInfo ReturnValue;
		//IData* data = databaseConnection->executeQuery(strSql, columnNames);
		IData* data = databaseConnection->executeQuery(sql, columnNames);
		if (0 >= data->getNumRows())
		{
			std::string strErrorMsg = "There no data found in DB ";
			TA_THROW(TA_Base_Core::DatabaseException(strErrorMsg));
		}

		modeTimeMap.clear();
		do 
		{
			for (unsigned long i = 0; i < data->getNumRows(); i++)
			{
				ReturnValue.pkey = data->getUnsignedLongData(i, ColumnName_Pkey);
				ReturnValue.modeKey = data->getUnsignedLongData(i, ColumnName_ModeKey);
				ReturnValue.beginTime = data->getStringData(i, ColumnName_BeginTime);
				modeTimeMap.insert(std::make_pair(ReturnValue.pkey, ReturnValue));
			}
			delete data;
			data = NULL;
		} while (databaseConnection->moreData(data));

		FUNCTION_EXIT;
	}

	unsigned long BASTimeTableModeTimeAccessFactory::addModeTime(unsigned long modeKey, ModeTime beginTime)
	{
		FUNCTION_ENTRY("addModeTime");
		using namespace TA_Base_Core;
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(BasTimetable_Cd, Write);
		bool isExist = false;
		{
			/*std::ostringstream ss;
			ss << "SELECT  " << ColumnName_Pkey 
				<< " FROM "<< TableName
				<< " WHERE " 
				<< ColumnName_ModeKey << " = " << modeKey << " AND "
				<< ColumnName_BeginTime << " = '" << beginTime.getTimeString() << "'";
			std::string strSql = ss.str();*/
			/*SELECT PKEY FROM  BA_TIMETABLE_MODETIME WHERE MODEKEY=%s AND BEGINTIME='%s'*/
			TA_Base_Core::SQLStatement sql;
			databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_MODETIME_SELECT_98002, modeKey, beginTime.getTimeString());

			std::vector<std::string> columnNames;
			columnNames.push_back( ColumnName_Pkey );

			//IData* data = databaseConnection->executeQuery(strSql, columnNames);
			IData* data = databaseConnection->executeQuery(sql, columnNames);
			if (data->getNumRows() > 0)
			{
				return data->getUnsignedLongData(0, ColumnName_Pkey);
			}
		}

		unsigned long timetableKey = getNextPkey();
		/*std::ostringstream ss;
		ss << "INSERT INTO " << TableName << " ( "
			<< ColumnName_Pkey << " , "
			<< ColumnName_ModeKey << " , "
			<< ColumnName_BeginTime << " ) "
			<< " VALUES ("
			<< timetableKey << " , "
			<< modeKey << " , ''"
			<< beginTime.getTimeString() << "'')";
		std::string strSql =ss.str();*/
		/*AQ:('%s','PUBLIC', 'INSERT INTO BA_TIMETABLE_MODETIME(PKEY,MODEKEY,BEGINTIME) VALUES(%s,%s,''%s'')')*/
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_MODETIME_INSERT_98003,
			BASTimeTableModeTimeAccessFactory::getLocalDatabaseName(), timetableKey, modeKey, beginTime.getTimeString());

		//databaseConnection->executeModification(BASTimeTableModeTimeAccessFactory::constructAQString(strSql));
		databaseConnection->executeModification(sql);
		FUNCTION_EXIT;
		return timetableKey;
	}

	void BASTimeTableModeTimeAccessFactory::updateModeTime(unsigned long pkey, ModeTime newTime)
	{
		FUNCTION_ENTRY("updateModeTime");
		using namespace TA_Base_Core;

		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(BasTimetable_Cd, Write);

		std::ostringstream ss;
		ss << "UPDATE " << TableName
			<< " SET " << ColumnName_BeginTime << " = ''" << newTime.getTimeString() << "''"
			<< " WHERE " << ColumnName_Pkey << " = " << pkey;
		std::string strSql = ss.str();
		/*AQ:('%s','PUBLIC', 'UPDATE BA_TIMETABLE_MODETIME SET BEGINTIME=''%s'' WHERE PKEY=%s')*/
		//BA_TIMETABLE_MODETIME_UPDATE_98004
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_MODETIME_UPDATE_98004,
			BASTimeTableModeTimeAccessFactory::getLocalDatabaseName(), newTime.getTimeString(), pkey);

		//databaseConnection->executeModification(BASTimeTableModeTimeAccessFactory::constructAQString(strSql));
		databaseConnection->executeModification(sql);
		FUNCTION_EXIT;
	}

	void BASTimeTableModeTimeAccessFactory::deleteModeTime(unsigned long pkey)
	{
		FUNCTION_ENTRY("deleteModeTime");
		std::vector<unsigned long> modeTimeVct;
		modeTimeVct.push_back(pkey);
		deleteModeTime(modeTimeVct);
		FUNCTION_EXIT;
	}

	void BASTimeTableModeTimeAccessFactory::deleteModeTime(std::vector<unsigned long> pkeyVct)
	{
		FUNCTION_ENTRY("deleteModeTime");
		using namespace TA_Base_Core;
		if (pkeyVct.empty())
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "There no mode time to delete!");
			return ;
		}

		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(BasTimetable_Cd, Write);

		/*std::ostringstream ss;
		ss << "DELETE FROM " << TableName
			<< " WHERE " << ColumnName_Pkey << " in ( " << getSqlInPart(pkeyVct, false) << ")";
		std::string strSql = ss.str() ;*/
		/*AQ:('%s','PUBLIC', 'DELETE FROM BA_TIMETABLE_MODETIME WHERE PKEY in (%s)')*/
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_MODETIME_DELETE_98005,
			BASTimeTableModeTimeAccessFactory::getLocalDatabaseName(), getSqlInPart(pkeyVct, false));

		//databaseConnection->executeModification(BASTimeTableModeTimeAccessFactory::constructAQString(strSql));
		databaseConnection->executeModification(sql);
		FUNCTION_EXIT;
	}

	std::string BASTimeTableModeTimeAccessFactory::getSqlInPart(std::vector<unsigned long> vctValue, bool withBrackets)
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


	unsigned long BASTimeTableModeTimeAccessFactory::getNextPkey()
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
		/*SELECT PKEY FROM BA_TIMETABLE_MODETIME WHERE ROWNUM <= 1 ORDER BY PKEY DESC*/
		TA_Base_Core::SQLStatement sql;

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ColumnName_Pkey);

		databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_MODETIME_DELETE_98006);
		//IData* data = databaseConnection->executeQuery(strSql, columnNames);
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

	std::string BASTimeTableModeTimeAccessFactory::getLocalDatabaseName()
	{
		using namespace TA_Base_Core;
		if (m_localDatabase.empty())
		{
			// have to find out what is the local db name
			// this should be the first in the connection list so grab it
			if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
			{
				throw DatabaseException("db-connection-file not set");
			}
			try
			{
				DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				DataConnections connections = connectionStrings.getConnectionList(Sessions_Ad, Write);
				m_localDatabase = connections[0].first;  
			}
			catch(...) // most likely a file not found error
			{
				throw DatabaseException("Unable to find database connection information");
			}

		}
		return m_localDatabase;
	}

	std::string BASTimeTableModeTimeAccessFactory::constructAQString(const std::string& strSqlString)
	{
		std::ostringstream formatSQL;
		formatSQL << "BEGIN ";
		formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
		formatSQL << strSqlString << "');";
		formatSQL << "END;";

		return formatSQL.str();
	}

}
