#include "stdafx.h"
#include "ExecuteSql.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

ExecuteSql::ExecuteSql(DataBase* dbCon)
{
	m_dbCon = dbCon;
}

ExecuteSql::~ExecuteSql()
{
}

int ExecuteSql::Execute(std::string sql, tRecordSet& data, bool isSelect)
{
	int flag = 0;

	data.clear();
	
	flag = m_dbCon->_DB_PrepareSQL(sql);

	if( flag != OCI_SUCCESS )
	{
		cout<<"[ "<<__FILE__<<" ] [ "<<__LINE__<<" ] db._DB_PrepareSQL() = "<<flag<<endl;
	}

	if (isSelect)
		flag = m_dbCon->_DB_ExecuteSQL();
	else
		flag = m_dbCon->_DB_ExecuteSQL_P();

	if( flag != OCI_SUCCESS )
	{
		cout<<"[ "<<__FILE__<<" ] [ "<<__LINE__<<" ] db._DB_ExecuteSQL() = "<<flag<<endl;
	}
	tRecordSet& tb = data;
	tb.Init();
	m_dbCon->_DB_RecordSet(tb);

	cout<<"[ "<<__FILE__<<" ] [ "<<__LINE__<<" ] RecordSet.colnum = "<< tb.colnum<<endl;
	cout<<"[ "<<__FILE__<<" ] [ "<<__LINE__<<" ] RecordSet.rownum = "<< tb.rownum<<endl;
	cout<<"[ "<<__FILE__<<" ] [ "<<__LINE__<<" ] RecordSet.size() = "<< tb.size()<<endl;

	return 0;
}

