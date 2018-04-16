
#include "entity_table.h"

EntityTable::EntityTable(DataBase* dbCon, std::string locationName)
{
	m_data.Init();
	m_dbCon = dbCon;
	m_locationName = locationName;
	m_sql = "";
}

EntityTable::~EntityTable()
{
	m_data.clear();
}

 EntityTable::ExecuteSql(std::string andCondition = "")
{

}

