#include "executeSql.h"
#include "SimpleLog.h"

#include <sstream>

ExecuteSql::ExecuteSql(DataBase* dbCon)
{
	m_dbCon = dbCon;
}

ExecuteSql::~ExecuteSql()
{
}

int ExecuteSql::Execute(std::string sql, RecordCollection & data)
{
    int flag = DB_SUCCESS;
    data.reset();
    flag = m_dbCon->executeSql_(sql);
    if (DB_FAILURE == flag)
    {
        simpleLog__(__FILE__, __LINE__, "executeSql_ return failed");
    }
    
    m_dbCon->recordSet_(data);
    std::stringstream ss;
    ss << "colnum " << data.column << ", rownum " << data.rownum << ", size " << data.Records.size();
    std::string msg = ss.str();
    simpleLog__(__FILE__, __LINE__, msg.c_str());
    return flag;
}

