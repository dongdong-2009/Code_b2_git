#ifndef PROXY_LIBRARY_HELP_H
#define PROXY_LIBRARY_HELP_H

static std::string g_agentName = "Dt17PmsAgent";

#define LOCATION_KEY 50

#define DATA_NODE_ENTITY_KEY_DB 10023667

#define OCC_PMS_AGENT_KEY 10012685

#define  DATA_NODE_ENTITY_KEY 10023661

#define  DATA_POINT_ENTITY_KEY 10020355

//#define  CREATE_DATA_NODE_BOOST_PTR()  \																		
//	::boost::shared_ptr<::TA_Base_Core::DataNodeEntityData>  pDataNodeEntityData(new ::TA_Base_Core::DataNodeEntityData(DATA_NODE_ENTITY_KEY));		
//
//
//#define  CREATE_DATA_NODE_SCADEA_PERSISTENCE_BOOST_PTR()\
//	::boost::shared_ptr< ::TA_Base_Bus::ScadaPersistence>  pScadaPersistence (new ::TA_Base_Bus::ScadaPersistence(::TA_Base_Bus::ScadaPersistence::DATANODE));						
//
//
//#define CREATE_DATA_NODE()																								\
//	::boost::shared_ptr<::TA_Base_Core::DataNodeEntityData>  pDataNodeEntityData(new ::TA_Base_Core::DataNodeEntityData(DATA_NODE_ENTITY_KEY));						\
//	::boost::shared_ptr< ::TA_Base_Bus::ScadaPersistence>  pScadaPersistence (new ::TA_Base_Bus::ScadaPersistence(::TA_Base_Bus::ScadaPersistence::DATANODE));	\
//	::TA_Base_Bus::DataNode dnObj(pDataNodeEntityData, pScadaPersistence);												
//

//#define CREATE_DATA_NODE_PROXY()																					\
//	::boost::shared_ptr<::TA_Base_Core::DataNodeEntityData>  pDataNodeEntityData(new ::TA_Base_Core::DataNodeEntityData(DATA_NODE_ENTITY_KEY));			\
//	::TA_Base_Bus::DataNodeSyncManager dataNodeManager;																	\
//	::TA_Base_Bus::ObserverUpdateQueueProcessor ouqpObj(1);																\
//	::TA_Base_Bus::DataNodeProxy proxyObj(pDataNodeEntityData,dataNodeManager,ouqpObj);
//

//#define CREATE_DATA_NODE_PROXY()																					\
//	g_pDefine->reset();\
//	::boost::shared_ptr<::TA_Base_Core::DataNodeEntityData>  pDataNodeEntityData(new ::TA_Base_Core::DataNodeEntityData(DATA_NODE_ENTITY_KEY));			\
//	pDataNodeEntityData->m_entityHelper->m_isValidData = true;\
//	::TA_Base_Bus::DataNodeSyncManager dataNodeManager;																	\
//	::TA_Base_Bus::ObserverUpdateQueueProcessor ouqpObj(1);																\
//	::TA_Base_Bus::DataNodeProxy proxyObj(pDataNodeEntityData,dataNodeManager,ouqpObj);
//


//#define CREATE_DATA_NODE_LOCAL_ACCESS_OBJ()																				\
//	::boost::shared_ptr<::TA_Base_Core::DataNodeEntityData>  pDataNodeEntityData(new ::TA_Base_Core::DataNodeEntityData(DATA_NODE_ENTITY_KEY));		\
//	::boost::shared_ptr< ::TA_Base_Bus::ScadaPersistence>  pScadaPersistence (new ::TA_Base_Bus::ScadaPersistence(::TA_Base_Bus::ScadaPersistence::DATANODE));						\
//	::TA_Base_Bus::DataNode dnObj(pDataNodeEntityData, pScadaPersistence);		\
//	::TA_Base_Bus::DataNodeLocalAccess dnlocalAceesObj(&dnObj);	
//
//
//#define CREATE_DATA_NODE_PROXY_MANAGER()\
//	::TA_Base_Bus::DataNodeSyncManager syncManager;\
//	::TA_Base_Bus::ObserverUpdateQueueProcessor observerUpdateQueueProcessor(1);\
//	::TA_Base_Bus::DataNodeProxyManager dnpmObj(syncManager,observerUpdateQueueProcessor);
//

//#define CREATE_DATA_NODE_LOCAL_SYNCHRONIZER()\
//	std::string agentName = "OccPmsAgent";\
//	unsigned long locationKey = 1;\
//	::TA_Base_Bus::DataNodeSyncManager dnsyncManager;\
//	::TA_Base_Bus::ObserverUpdateQueueProcessor observerUpdateQueueProcessor(1);\
//	::TA_Base_Bus::DataNodeProxyManager dnpmObj(dnsyncManager,observerUpdateQueueProcessor);\
//	TA_Base_Bus::DataNodeLocalSynchronizer dnlsObj(  agentName,	 locationKey,	dnsyncManager, dnpmObj)
//

// only for DataNodeProxyManager
//#define PROXY_MAP_INSERT()\
//	::boost::shared_ptr<::TA_Base_Core::DataNodeEntityData>  entityData(new ::TA_Base_Core::DataNodeEntityData(DATA_NODE_ENTITY_KEY));		\
//	dnpmObj.addProxy (DATA_NODE_ENTITY_KEY, entityData);


//#define  CREATE_DATA_NODE_BULK_POLLING_SYNCHRONIZER()\
//	std::string agentName = g_agentName;\
//	unsigned long locationKey = LOCATION_KEY;\
//	::TA_Base_Bus::DataNodeSyncManager syncManager;\
//	::TA_Base_Bus::ObserverUpdateQueueProcessor observerUpdateQueueProcessor(1);\
//	::TA_Base_Bus::DataNodeProxyManager dnpmObj(syncManager,observerUpdateQueueProcessor);\
//	::TA_Base_Bus::DataNodeBulkPollingSynchronizer dnbpsObj(agentName,locationKey,syncManager,dnpmObj);
//
//


#endif //PROXY_LIBRARY_HELP_H


//boost::shared_ptr<TA_Base_Core::DataPointEntityData> dpEntityData = boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData>(*it);