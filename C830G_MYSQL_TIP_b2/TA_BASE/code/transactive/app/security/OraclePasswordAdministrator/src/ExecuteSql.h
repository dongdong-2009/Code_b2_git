
#ifndef _EXECUTESQL_H_
#define _EXECUTESQL_H_

#include "Database.h"

class ExecuteSql
{
public:
	ExecuteSql(DataBase* dbCon);

	~ExecuteSql();

	int Execute(std::string sql, tRecordSet& data, bool isSelect = true);

private:
	DataBase* m_dbCon;
};

#endif
