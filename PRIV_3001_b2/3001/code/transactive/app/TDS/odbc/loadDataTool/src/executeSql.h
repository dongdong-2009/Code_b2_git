
#ifndef _EXECUTESQL_H_
#define _EXECUTESQL_H_

#include "database.h"

class ExecuteSql
{
public:
	ExecuteSql(DataBase* dbCon);

	~ExecuteSql();
	
    int Execute(std::string sql, RecordCollection& data);

private:
	DataBase* m_dbCon;
};

#endif
