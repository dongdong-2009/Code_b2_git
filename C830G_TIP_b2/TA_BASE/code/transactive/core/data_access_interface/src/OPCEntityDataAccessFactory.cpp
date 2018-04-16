#include "core/data_access_interface/src/OPCEntityDataAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include <sstream>

namespace TA_Base_Core
{
	OPCEntityDataAccessFactory* OPCEntityDataAccessFactory::s_instance = NULL;

	OPCEntityDataAccessFactory::OPCEntityDataAccessFactory()
	{
	}

	OPCEntityDataAccessFactory::~OPCEntityDataAccessFactory()
	{
	}

	OPCEntityDataAccessFactory& OPCEntityDataAccessFactory::getInstance()
	{
		if (NULL == s_instance)
		{
			s_instance = new OPCEntityDataAccessFactory();
		}
		return *s_instance;
	}

	OPCDataPointDataList* OPCEntityDataAccessFactory::getOPCDpDataAtLocations(std::vector<unsigned long>& locationKeys)
	{
		OPCDataPointDataList* pDataList = new OPCDataPointDataList();
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		std::stringstream sqlLocationKeys;
		sqlLocationKeys << "(";
		int locSize = locationKeys.size();
		std::vector<unsigned long>::const_iterator itLoc = locationKeys.begin();
		for (int i = 0; i < locSize - 1; ++i)
		{
			sqlLocationKeys << *itLoc;
			sqlLocationKeys << ",";
			++itLoc;
		}
		sqlLocationKeys << *itLoc;
		sqlLocationKeys << ")";

		std::string dpTypeName = DataPointEntityData::getStaticType();

		std::stringstream sql;
		sql << "select e.pkey, e.agentkey, e.locationkey, e.name from entity e, entitytype et where e.deleted = 0 and et.name = '";
		sql << dpTypeName;
		sql << "' and e.typekey = et.pkey and locationkey in ";
		sql << sqlLocationKeys.str();

		getOPCDpData( databaseConnection, sql.str(), *pDataList );

		return pDataList;
	}

	OPCDataPointDataList* OPCEntityDataAccessFactory::getOPCDpDataAtAllLocations()
	{
		OPCDataPointDataList* pDataList = new OPCDataPointDataList();

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
		
		std::string dpTypeName = DataPointEntityData::getStaticType();

		std::stringstream sql;
		sql << "select e.pkey, e.agentkey, e.locationkey, e.name from entity e, entitytype et where e.deleted = 0 and et.name = '";
		sql << dpTypeName;
		sql << "' and e.typekey = et.pkey";

		getOPCDpData( databaseConnection, sql.str(), *pDataList );

		return pDataList;
	}

	OPCDataNodeDataList*  OPCEntityDataAccessFactory::getOPCDnDataAtLocations(std::vector<unsigned long>& locationKeys)
	{
		OPCDataNodeDataList* pDataList = new OPCDataNodeDataList();
		EntityParameterDataList dnParaList;

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		std::stringstream sqlLocationKeys;
		sqlLocationKeys << "(";
		int locSize = locationKeys.size();
		std::vector<unsigned long>::const_iterator itLoc = locationKeys.begin();
		for (int i = 0; i < locSize - 1; ++i)
		{
			sqlLocationKeys << *itLoc;
			sqlLocationKeys << ",";
			++itLoc;
		}
		sqlLocationKeys << *itLoc;
		sqlLocationKeys << ")";

		std::string sqlParameterNames("(");
		sqlParameterNames += "'";
		sqlParameterNames += DataNodeEntityData::DATANODE_SHORT_LABEL;
		sqlParameterNames += "'";
		sqlParameterNames += ",";

		sqlParameterNames += "'";
		sqlParameterNames += DataNodeEntityData::DATANODE_LABEL;
		sqlParameterNames += "'";
		sqlParameterNames += ",";

		sqlParameterNames += "'";
		sqlParameterNames += DataNodeEntityData::DATANODE_ASSET_NAME;
		sqlParameterNames += "'";
		sqlParameterNames += ")";

		std::string dnTypeName = DataNodeEntityData::getStaticType();
		
		std::stringstream sql;
		sql << "select e.pkey, e.agentkey, e.locationkey, e.name from entity e, entitytype et where e.deleted = 0 and et.name = '";
		sql << dnTypeName;
		sql << "' and e.typekey = et.pkey and locationkey in ";
		sql << sqlLocationKeys.str();

		std::stringstream sqlParameter;
		sqlParameter << "SELECT t1.entitykey, t3.name, t1.value from entityparametervalue t1, entity t2, entityparameter t3, entitytype t4 ";
		sqlParameter << "where t1.entitykey = t2.pkey and t2.locationkey in ";
		sqlParameter << sqlLocationKeys.str();
		sqlParameter << " and t2.typekey=t4.pkey and t4.name='"; 
		sqlParameter << dnTypeName;
		sqlParameter << "' and t2.deleted=0 and t1.parameterkey = t3.pkey and t3.name in ";
		sqlParameter << sqlParameterNames;

		getOPCDnData(databaseConnection, sql.str(), sqlParameter.str(), *pDataList, dnParaList);
		assembleDnOPCData(*pDataList, dnParaList);

		return pDataList;
	}

	OPCDataNodeDataList* OPCEntityDataAccessFactory::getOPCDnDataAtAllLocations()
	{
		OPCDataNodeDataList* pDataList = new OPCDataNodeDataList();
		EntityParameterDataList dnParaList;

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		std::string dnTypeName = DataNodeEntityData::getStaticType();

		std::string sqlParameterNames("(");
		sqlParameterNames += "'";
		sqlParameterNames += DataNodeEntityData::DATANODE_SHORT_LABEL;
		sqlParameterNames += "'";
		sqlParameterNames += ",";

		sqlParameterNames += "'";
		sqlParameterNames += DataNodeEntityData::DATANODE_LABEL;
		sqlParameterNames += "'";
		sqlParameterNames += ",";

		sqlParameterNames += "'";
		sqlParameterNames += DataNodeEntityData::DATANODE_ASSET_NAME;
		sqlParameterNames += "'";
		sqlParameterNames += ")";

		std::stringstream sql;
		sql << "select e.pkey, e.agentkey, e.locationkey, e.name from entity e, entitytype et where e.deleted = 0 and et.name = '";
		sql << dnTypeName;
		sql << "' and e.typekey = et.pkey";

		std::stringstream sqlParameter;
		sqlParameter << "SELECT t1.entitykey, t3.name, t1.value from entityparametervalue t1, entity t2, entityparameter t3, entitytype t4 ";
		sqlParameter << "where t1.entitykey = t2.pkey and t2.typekey=t4.pkey and t4.name='";
		sqlParameter << dnTypeName;
		sqlParameter << "' and t2.deleted=0 and t1.parameterkey = t3.pkey and t3.name in ";
		sqlParameter << sqlParameterNames;

		getOPCDnData(databaseConnection, sql.str(), sqlParameter.str(), *pDataList, dnParaList);
		assembleDnOPCData(*pDataList, dnParaList);

		return pDataList;
	}

	void OPCEntityDataAccessFactory::getOPCDpData( IDatabase* pDatabase, const std::string& sql, OPCDataPointDataList& dataList )
	{
		std::string entityKeyColumn = "ENTITYKEY";
		std::string entityNameColumn = "ENTITYNAME";
		std::string agentKeyColumn = "AGENTKEY";
		std::string locationKeyColumn = "LOCATIONKEY";
		std::vector<std::string> columnNames;
		columnNames.push_back( entityKeyColumn );
		columnNames.push_back( agentKeyColumn );
		columnNames.push_back( locationKeyColumn );
		columnNames.push_back( entityNameColumn );

		TA_ASSERT(0 != pDatabase, "the database connection is null");

		IData* data = pDatabase->executeQuery( sql, columnNames);

		do
		{
			for (unsigned long i = 0; i < data->getNumRows() ; i++ )
			{
				dataList.push_back( 
					OPCDataPointData( data->getUnsignedLongData( i, entityKeyColumn ),
					data->getUnsignedLongData( i, agentKeyColumn ),
					data->getUnsignedLongData( i, locationKeyColumn ),
					data->getStringData( i, entityNameColumn ) ) );
			}

			delete data;
			data = NULL;
		}
		while( pDatabase->moreData(data) );
	}

	void OPCEntityDataAccessFactory::getOPCDnData( IDatabase* pDatabase, const std::string& sql, const std::string& sqlParameter, 
		OPCDataNodeDataList& dataList, EntityParameterDataList& dnParameterList )
	{
		std::vector<std::string> columnNames;
		std::string entityKeyColumn("ENTITYKEY");
		std::string entityNameColumn("ENTITYNAME");
		std::string agentKeyColumn("AGENTKEY");
		std::string locationKeyColumn("LOCATIONKEY");
		columnNames.push_back( entityKeyColumn );
		columnNames.push_back( agentKeyColumn );
		columnNames.push_back( locationKeyColumn );
		columnNames.push_back( entityNameColumn );

		std::vector<std::string> paraColumnNames;
		std::string p1("ENTITYKEY");
		std::string p2("NAME");
		std::string p3("VALUE");
		paraColumnNames.push_back(p1);
		paraColumnNames.push_back(p2);
		paraColumnNames.push_back(p3);

		TA_ASSERT(0 != pDatabase, "the database connection is null");

		IData* dataParameter = pDatabase->executeQuery( sqlParameter, paraColumnNames);
		do
		{
			for (unsigned long i = 0; i < dataParameter->getNumRows() ; i++ )
			{
				EntityParameterData parameterData;
				parameterData.entityKey = dataParameter->getUnsignedLongData( i, p1 );
				parameterData.parameterName = dataParameter->getStringData( i, p2 );
				parameterData.parameterValue = dataParameter->getStringData( i, p3 );
				dnParameterList.push_back(parameterData);
			}

			delete dataParameter;
			dataParameter = NULL;
		}
		while( pDatabase->moreData(dataParameter) );

		std::string emptyStr("");
		IData* data = pDatabase->executeQuery( sql, columnNames);
		do
		{
			for (unsigned long i = 0; i < data->getNumRows() ; i++ )
			{
				dataList.push_back( 
					OPCDataNodeData( data->getUnsignedLongData( i, entityKeyColumn ),
					data->getUnsignedLongData( i, agentKeyColumn ),
					data->getUnsignedLongData( i, locationKeyColumn ),
					data->getStringData( i, entityNameColumn ),
					emptyStr, emptyStr, emptyStr) );
			}

			delete data;
			data = NULL;
		}
		while( pDatabase->moreData(data) );	
	}

	void OPCEntityDataAccessFactory::assembleDnOPCData(OPCDataNodeDataList& dataList, EntityParameterDataList& dnParameterList)
	{
		std::map<unsigned long, OPCDataNodeData*> tmpDataMap;

		OPCDataNodeDataList::iterator itData;
		for (itData = dataList.begin(); itData != dataList.end(); ++itData)
		{
			tmpDataMap[itData->entityKey] = &(*itData);
		}

		unsigned long preEntityKey = ULONG_MAX;
		unsigned long entityKey    = ULONG_MAX;
		OPCDataNodeData* opcDnData = NULL;
		EntityParameterDataList::iterator itParameter;
		std::map<unsigned long, OPCDataNodeData*>::iterator itDataMap = tmpDataMap.end();
		for (itParameter = dnParameterList.begin(); itParameter != dnParameterList.end(); ++itParameter)
		{
			unsigned long entityKey = itParameter->entityKey;
			if (preEntityKey != entityKey)
			{
				preEntityKey = entityKey;
				itDataMap = tmpDataMap.find(entityKey);
				if (itDataMap != tmpDataMap.end())
				{
					opcDnData = itDataMap->second;
				}
				else
				{
					opcDnData = NULL;
				}
			}

			if ( NULL == opcDnData ) continue;

			if (itParameter->parameterName == TA_Base_Core::DataNodeEntityData::DATANODE_SHORT_LABEL)
			{
				opcDnData->shortLabel = itParameter->parameterValue;
			}
			else if (itParameter->parameterName == TA_Base_Core::DataNodeEntityData::DATANODE_LABEL)
			{
				opcDnData->label = itParameter->parameterValue;
			}
			else if (itParameter->parameterName == TA_Base_Core::DataNodeEntityData::DATANODE_ASSET_NAME)
			{
				opcDnData->assetName = itParameter->parameterValue;
			}	
		}
	}
}