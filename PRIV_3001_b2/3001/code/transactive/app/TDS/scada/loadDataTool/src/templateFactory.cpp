
#include "excelHelper.h"
#include "templateFactory.h"

TemplateFactory::TemplateFactory(ExecuteSql* exeSql)
{
	m_executeSql = exeSql;
}

TemplateFactory::~TemplateFactory()
{
}

void TemplateFactory::excute(std::map<std::string, std::string>& paras, std::string filePath, ExcelHelper eh)
{
	std::string sql_name;
	std::string sql_str;
	tRecordSet data;
	data.Init();

	std::vector<std::string> colNames;
	ostringstream oStr;
	
	std::map<std::string, std::string>::iterator it;
	bool sheetVisible = true;
	if ((it = paras.find("HideLoadSheets")) != paras.end())
	{
		if("1" == it->second)
			sheetVisible = false;
	}
	it = paras.begin();
	while (paras.end() != it)
	{
		if (std::string::npos != it->first.find("_sql") &&
			std::string::npos != it->second.find("select"))
		{
			sql_name = it->first.substr(0, it->first.find("_sql"));
			sql_str  = it->second;
			std::string sheetName = sql_name;

			m_executeSql->Execute(sql_str, data);

			oStr.clear();
			oStr.str("");
			colNames.clear();
			parseSql(sql_str, colNames);

			WriteColNames(colNames, oStr);
			GetTabelValue(data, oStr);
			eh.WriteTxtFromClipBoard(filePath, sheetName, oStr.str().c_str());
			eh.SetSheetVisible(filePath, sheetName, sheetVisible);
		}
		++it;
	}
}

void TemplateFactory::GetTabelValue(tRecordSet& table, ostringstream& oStr)
{
	int i, j;
	int rows = table.rownum;
	int colsM1 = table.colnum - 1;
	for (i = 0; i < rows; ++i)
	{
		for(j = 0; j < colsM1; ++j)
		{
			oStr << table.vRecord[i][j] << "\t";
		}
		oStr << table.vRecord[i][j] << "\n";
	}
}

void TemplateFactory::parseSql(std::string sql, std::vector<std::string>& colNames)
{
	size_t sPos = sql.find("select");
	size_t fPos = sql.find("from");
	if(std::string::npos == sPos || std::string::npos == fPos)
	{
		return;
	}
	std::string colStr = sql.substr(sPos, fPos - sPos);
	printf("cols:%s\n", colStr.c_str());
	int i;
	char sSql[2000] = {0};
	char s[200] = {0};
	int n = colStr.length();
	strcpy(sSql, colStr.c_str() + 6);	// 6 is 'select' length.
	int j = 0;
	bool preIsBlank = false;
	for(i = 0; i <= n; ++i)	// to char 0;
	{
		if(' ' == sSql[i])
		{
			preIsBlank = true;
		}
		else if(',' == sSql[i] || 0 == sSql[i])
		{
			if (j)
			{
				s[j] = 0;
				colNames.push_back(s);
				j = 0;
			}
			preIsBlank = false;
		}
		else if('.' == sSql[i])
		{
			j = 0;
			preIsBlank = false;
		}
		else
		{
			if(preIsBlank)
			{
				j = 0;	// clear prexxx, eg, en.pkey entitykey;
			}
			s[j++] = sSql[i];
			preIsBlank = false;
		}
	}
}

void TemplateFactory::WriteColNames(std::vector<std::string>& colNames, ostringstream& oStr)
{
	int i = 0;
	int n = colNames.size();
	for (i = 0; i < n; ++i)
	{
		if(i)
		{
			oStr << "\t";
		}
		oStr << colNames[i];
	}
	oStr << "\n";
}
