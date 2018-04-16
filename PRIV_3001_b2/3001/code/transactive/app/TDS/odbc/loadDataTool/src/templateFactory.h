#ifndef _TEMPLATEFACTORY_H_
#define _TEMPLATEFACTORY_H_

#include "executeSql.h"

class ExcelHelper;

class TemplateFactory
{
public:
	TemplateFactory(ExecuteSql* exeSql);

	~TemplateFactory();

    void execute(std::map<std::string, std::string>& paras, std::string filePath, ExcelHelper eh);
    void GetTabelValue(RecordCollection& data, ostringstream& oStr);
	void parseSql(std::string sql, std::vector<std::string>& colNames);
	void WriteColNames(std::vector<std::string>& colNames, ostringstream& oStr);

private:
	ExecuteSql* m_executeSql;

	std::vector<std::string> m_allSqlStrs;
};

#endif
