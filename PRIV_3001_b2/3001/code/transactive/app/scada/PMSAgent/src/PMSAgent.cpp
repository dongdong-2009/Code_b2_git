/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/PRIV_3001/3001/transactive/app/scada/PMSAgent/src/PMSAgent.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2018/03/14 13:51:20 $
  * Last modified by:	$Author: lim.cy $
  *
  * Description:
  *
  *	PMSAgent implementes the GenericAgent interface.  PMSAgent monitors and controls the
  * the entities associated with its process, such as PMS RTU and the DataPoint entities.
  *
  */

#include "app/scada/PMSAgent/src/PMSAgent.h"
#include "app/scada/PMSAgent/src/PMSAgentParams.h"
#include "core/threads/src/LFThreadPoolManager.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/PMSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/data_access_interface/entity_access/src/StationSystemEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/src/DataPointStateChangeAccessFactory.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointAccessFactory.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/exceptions/src/DataException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/PMSAgentStateUpdate_MessageTypes.h"
#include "core/message/types/PMSAgentAudit_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/timers/src/StopwatchUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/threads/src/Thread.h"


using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
	const unsigned long PROXY_DP_MONITORING_PERIOD_SECS = 20;

	PMSAgent::PMSAgent (int argc, char* argv[])
	:
	m_pmsAgentPrimaryKey (0),
    m_proxyDpMonitoringPeriodSecs (PROXY_DP_MONITORING_PERIOD_SECS),
    m_operationMode (TA_Base_Core::NotApplicable),
    m_bulkInterfaceStarted (false),
    m_isRTUSwitchingDisabled (false),
    m_analogueDpUpdateInterval (0),
    m_stationName (""),
    m_agentName (""),
	m_agentAssetName (""),
    m_genericAgent (0),
	m_auditMessageSender (0),
    m_dpBulkPolledInterface (0),
    m_dpAccessInterface(0),
    m_dataPointFactory (0),
    m_scadaProxyFactory (0),
	m_dnBulkPolledInterface(NULL),
	m_dnAccessInterface(NULL),
	m_summaryProcessor(NULL),
	m_agentInitThread(NULL)
	{
		m_datapoints.clear();
		m_dataNodes.clear();
		TA_Base_Core::RunParams::getInstance().set(PARAM_SCADA_PROXY_CONSTRUCT_WO_START, "---");

		// create handle to DataPointFactory
		if (0 == m_dataPointFactory)
		{
			m_dataPointFactory = new TA_Base_Bus::DataPointFactory(this);
		}

		// get a handle to the ScadaProxyFactory
		m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());

		TA_ASSERT ((0 != m_scadaProxyFactory), "SCADA Proxy Factory not created");
		
		TA_Base_Bus::LFThreadPoolSingleton::getInstanceWithArgs(false);

		// create handle to Generic Agent
		m_genericAgent = new TA_Base_Bus::GenericAgent (argc, argv, this, NULL, AGENT_USER_TYPE_NORMAL);

		// get the agent name
		m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();

		// register the local entities into ScadaProxyFactory
		m_scadaProxyFactory->registerLocalEntities(m_agentName, m_genericAgent->getEntityMap());

		// set the proxies to monitor mode
		m_scadaProxyFactory->setOperationMode(TA_Base_Core::Standby);
		m_dataPointFactory->getMmsScadaUtility().registerLocalEntities(m_agentName, m_genericAgent->getEntityMap());

        initializeDPAfterProxyReady();

		// create SummaryProcessor
        m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());

        createRTUWorkers();
	}


	PMSAgent::~PMSAgent()
	{
		// set the proxies to monitor mode
		m_scadaProxyFactory->setOperationMode(TA_Base_Core::Standby);

		// clean up the list containing DataPoints instances created by
		// this Agent, without deleting the DataPoint instances themselves
		// since they are owned by the GenericAgent
		m_datapoints.clear();
        m_dataNodes.clear();

		// clean up handle to DataPointFactory
		if (0 != m_dataPointFactory)
		{
			delete m_dataPointFactory;
			m_dataPointFactory = 0;
		}

		// remove instance of audit message sender
		if (0 != m_auditMessageSender)
		{
			delete m_auditMessageSender; 
			m_auditMessageSender = 0;
		}
		
		if ( NULL != m_dpBulkPolledInterface )
		{
            delete m_dpBulkPolledInterface;
			m_dpBulkPolledInterface = NULL;
		}
		
        if (NULL != m_dpAccessInterface)
        {
            delete m_dpAccessInterface;
            m_dpAccessInterface = NULL;
		}
		if ( NULL != m_dnBulkPolledInterface )
		{
            delete m_dnBulkPolledInterface;
			m_dnBulkPolledInterface = NULL;
		}
		
        if (NULL != m_dnAccessInterface)
        {
            delete m_dnAccessInterface;
            m_dnAccessInterface = NULL;
		}

		// clean up the GenericAgent
		if (0 != m_genericAgent)
		{
			try
			{
				m_genericAgent->deactivateAndDeleteServant();
				m_genericAgent = 0;
			}
			catch (...)
			{
				// do nothing 
			}
		}

		// clean up handle to SummaryProcessor
		if ( NULL != m_summaryProcessor )
		{
			TA_Base_Bus::SummaryProcessor::removeInstance(m_summaryProcessor);
			m_summaryProcessor = NULL;
		}

		// remove handle to ScadaProxyFactory
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		m_scadaProxyFactory = 0;

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

		if (NULL != m_agentInitThread)
		{
			delete m_agentInitThread;
			m_agentInitThread = NULL;
		}
	}


	void PMSAgent::loadAgentParamValues()
	{
		// get configuration data for this PMS Agent
		TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();

		// dynamic cast the entity data to PMSAgentEntityData
		TA_Base_Core::PMSAgentEntityDataPtr pmsAgentEntityData;
		pmsAgentEntityData = 
			boost::dynamic_pointer_cast<TA_Base_Core::PMSAgentEntityData>(entityData);

		// get the primary key of this PMSAgent
		m_pmsAgentPrimaryKey = pmsAgentEntityData->getKey();

		// get the agent asset name
		m_agentAssetName = pmsAgentEntityData->getAssetName();

		// get indication if the RTU switching functionality is disabled
        m_isRTUSwitchingDisabled = pmsAgentEntityData->getIsRTUSwitchingDisabled();
        m_analogueDpUpdateInterval = pmsAgentEntityData->getAnalogueDataPointUpdateInterval();

		// get the configured station name
		m_stationName = pmsAgentEntityData->getStationName();

		// get the configured health monitoring period for the proxy data points
		// use the defaulted value if not configured in database
		m_proxyDpMonitoringPeriodSecs = pmsAgentEntityData->getProxyDpHealthMonitoringPeriodSecs();
		if (0 == m_proxyDpMonitoringPeriodSecs)
		{
			m_proxyDpMonitoringPeriodSecs = PROXY_DP_MONITORING_PERIOD_SECS;
		}
	}


	void PMSAgent::startProcessors()
	{
	}


	void PMSAgent::stopProcessors()
	{
	}


	void PMSAgent::createRTUWorkers()
	{
		// if does not have the agent's pkey
		if (0 == m_pmsAgentPrimaryKey)
		{
			loadAgentParamValues();
            
            m_rtuManager.setAgentAssetName(m_agentAssetName);
            m_rtuManager.setIsRTUSwitchingDisabled(m_isRTUSwitchingDisabled);
            m_rtuManager.setStationName(m_stationName);
            m_rtuManager.setAnalogueDpUpdateInterval(m_analogueDpUpdateInterval);
        }

        // create rtu workers
        m_rtuManager.createRTUWorkers();

        // add station systems to rtu manager
        std::vector<TA_Base_Core::IEntityData*> pmsAgentChildrenEntities;
        pmsAgentChildrenEntities = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf (m_pmsAgentPrimaryKey);
        
        // for each of the child entity of the PMSAgent
        std::vector<TA_Base_Core::IEntityData*>::iterator pmsAgentChildItr;
        for (pmsAgentChildItr = pmsAgentChildrenEntities.begin();
                pmsAgentChildItr != pmsAgentChildrenEntities.end();
                pmsAgentChildItr++)
        {
            // if the child entity is of the type "Station"
            if ((*pmsAgentChildItr)->getType() == "Station")
            {
                // access to child entities of the Station
                std::vector<TA_Base_Core::IEntityData*> stationChildrenEntities;
                stationChildrenEntities = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf ((*pmsAgentChildItr)->getKey());
                
                // for each of the child entity of the Station
                std::vector<TA_Base_Core::IEntityData*>::iterator stationChildItr;
                for (stationChildItr = stationChildrenEntities.begin();
                    stationChildItr != stationChildrenEntities.end();
                    stationChildItr++)
                {
                    // if the child entity is of the type "StationSystem"
                    if ((*stationChildItr)->getType() == "StationSystem")
                    {
                        m_rtuManager.addStationSystem(*(*stationChildItr));
                    }

                    delete (*stationChildItr);
					*stationChildItr = NULL;
                }
            }
            
            delete (*pmsAgentChildItr);
			*pmsAgentChildItr = NULL;
        }

        // add datapoint to rtu manager except virtual datapoint
        for (std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp = m_datapoints.begin(); itrDp != m_datapoints.end(); ++itrDp)
        {
            if (false == itrDp->second->isVirtual())
            {
                m_rtuManager.addDataPoint(itrDp->second);
            }
        }
    }


	TA_Base_Bus::IEntity* PMSAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
		//
		// Note: the objects of interface IEntityData and IEntity are not owned
		// by this specific agent, but by the GenericAgent.  As such no object
		// clean up shall be done with regards to these objects.
		//

		std::string entityDataType = EntityData->getType();

		// if the specified entity is of the RTU type
		if (TA_Base_Core::RTUEntityData::getStaticType() == entityDataType)
		{
			// create and return the RTU entity
			return m_rtuManager.createRTUEntity(EntityData);
		}

		// if the specified entity is of the type DataPoint
		else if (TA_Base_Core::DataPointEntityData::getStaticType() == entityDataType)
		{
			// create and return the DataPoint entity
			return createDataPointEntity(EntityData);
		}
		else if( TA_Base_Core::DataNodeEntityData::getStaticType() == entityDataType )
		{
			TA_Base_Bus::DataNode* dn = NULL;
			try
			{
				TA_Base_Bus::IEntity * theEntity = 0;
				dn = new TA_Base_Bus::DataNode (boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(EntityData), m_persistence);
				{
					TA_Base_Core::ThreadGuard guard (m_dataNodeListLock);
					m_dataNodes.push_back( dn );
				}
				theEntity = dn;
				return theEntity;
			}
			catch (...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                    EntityData->getName().c_str(), EntityData->getKey() );
				
				// To ensure the generic agent is constructed even if one DataNode creation fails
			}
		}

		return NULL;
	}

	bool PMSAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
									  const TA_Base_Core::SharedIEntityDataList& entityDataList,
									  TA_Base_Bus::IEntityList& createdEntities)
	{
		// here we will block main thread until cache initialization complete
		if (!m_agentInitThread->waitForCompelete())
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Initialize PMSAgent cache failed!" );
		}
		
		bool hasDataNode = false;
		TA_Base_Core::WorkItemExeSync* pItemSync = new TA_Base_Core::WorkItemExeSync();
		std::vector<TA_Base_Core::IThreadPoolExecuteItem*> workItems;
		workItems.reserve(entityDataList.size());
        for ( TA_Base_Core::SharedIEntityDataList::const_iterator it = entityDataList.begin();
			  it != entityDataList.end(); ++it )
        {
            std::string entityDataType = (*it)->getType();
            if (entityDataType == TA_Base_Core::DataNodeEntityData::getStaticType()) 
			{
				hasDataNode = true;
				m_scadaProxyFactory->registerEntityData(*it, entityDataType);
			}
			else if (entityDataType == TA_Base_Core::DataPointEntityData::getStaticType())
			{
				m_scadaProxyFactory->registerEntityData(*it, entityDataType);
			}
			
			workItems.push_back(new CreateEntityWorkItem(*this, *it, pItemSync));
        }

		if (hasDataNode)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin create DataNode Hierarchy Map" );
			TA_Base_Core::DataNodeAccessFactory::getInstance()->createDataNodeHierarchyMap(agentEntityData->getKey(), entityDataList);
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End create DataNode Hierarchy Map" );
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Thread pool begin create Entities" );

		int num = 8;
		if ( true == RunParams::getInstance().isSet ( "InitThreadedWorkers" ) )
		{
			num = atoi ( RunParams::getInstance().get( "InitThreadedWorkers" ).c_str() );
		}

		TA_Base_Core::LFThreadPoolManager threadPool(num);
		threadPool.queueWorkItemList(workItems);
		pItemSync->wait();
		delete pItemSync;
		pItemSync = NULL;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Thread pool end create Entities" );

		DataNodeList::iterator iterDn = m_dataNodes.begin();
		for (; iterDn != m_dataNodes.end(); ++iterDn)
		{
			createdEntities.push_back(TA_Base_Bus::IEntityList::value_type((*iterDn)->getEntityKey(), *iterDn));
		}

		std::vector<RTU*> rtuEntities = m_rtuManager.getRTUEntities();
		std::vector<RTU*>::iterator iterRtu = rtuEntities.begin();
		for (; iterRtu != rtuEntities.end(); ++iterRtu)
		{
			createdEntities.push_back(TA_Base_Bus::IEntityList::value_type((*iterRtu)->getPKey(), *iterRtu));
		}

		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator iterDp = m_datapoints.begin();
		for (; iterDp != m_datapoints.end(); ++iterDp)
		{
			createdEntities.push_back(TA_Base_Bus::IEntityList::value_type(iterDp->first, iterDp->second));
		}
		
		return true;
	}


	void PMSAgent::entityCreationComplete()
	{
		// loading complete, free some memory
		TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();
	}

	void PMSAgent::agentTerminate()
	{

        // make sure the processors are all terminated
		stopProcessors();

		m_rtuManager.terminate();

        // if bulk interface is created
		if (0 != m_dpBulkPolledInterface)
		{
			delete m_dpBulkPolledInterface;
			m_dpBulkPolledInterface = 0;
			m_bulkInterfaceStarted = false;
		}

        // if bulk interface is created
		if (0 != m_dpAccessInterface)
		{
			delete m_dpAccessInterface;
			m_dpAccessInterface = 0;
		}
		
		if ( NULL != m_dnBulkPolledInterface )
		{
            delete m_dnBulkPolledInterface;
			m_dnBulkPolledInterface = NULL;
		}
		
        if (NULL != m_dnAccessInterface)
        {
            delete m_dnAccessInterface;
            m_dnAccessInterface = NULL;
		}

		// unsubscribe this agent StateUpdate message
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

		// set the proxies to monitor mode
		m_scadaProxyFactory->setOperationMode(TA_Base_Core::Standby);

    }


	void PMSAgent::agentSetMonitor()
	{
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PMSAgent is being set to monitor...");

        if (m_operationMode == TA_Base_Core::Monitor)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetMonitor() called when agent is already in monitor");
			return;
        }
        // set the flag to indicate the agent is now in Standby mode
		m_operationMode = TA_Base_Core::Monitor;
        
		m_summaryProcessor->setOperatorMode(m_operationMode);

        m_rtuManager.setOperationMode(m_operationMode);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RTUManager is set to monitor.");

        setScadaInterfaceOperationMode(m_operationMode);

        // set all datanodes to monitor mode
		for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
		{
			(*itr)->changeOperationMode( m_operationMode );
		}

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datanode are set to monitor.");

		m_scadaProxyFactory->setOperationMode(m_operationMode);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ScadaProxyFactory is set to monitor.");
        
		// for each of the data point associated with this agent
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator it;
		for (it = m_datapoints.begin(); it != m_datapoints.end(); ++it)
		{
			// make the data point operated in Monitor mode
			it->second->setToMonitorMode();
		}
		TA_Base_Bus::LFThreadPoolSingleton::getInstance()->start();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datapoints are set to monitor.");
        
        // register this agent for Monitor messages
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
		registerForMonitorMessages();

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PMSAgent for Station %s has been started up in MONITOR mode",
            m_stationName.c_str());
	}

    void PMSAgent::agentSetControl()
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PMSAgent is being set to control...");

        if (m_operationMode == TA_Base_Core::Control)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetControl() called when agent is already in control");
			return;
        }

        // set the flag to indicate the agent is now in Control mode
        m_operationMode = TA_Base_Core::Control;
        
		m_summaryProcessor->setOperatorMode(m_operationMode);

        // for each of the data point associated with this agent
        std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp;
        for (itrDp = m_datapoints.begin(); itrDp != m_datapoints.end(); ++itrDp)
        {
            // make the data point operated in Control mode
            itrDp->second->setToControlMode();
        }
		TA_Base_Bus::LFThreadPoolSingleton::getInstance()->start();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datapoints are set to control.");
        
		m_scadaProxyFactory->setOperationMode(m_operationMode);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ScadaProxyFactory is set to control.");

        
        // set all datanodes to control mode
        for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
        {
            (*itr)->changeOperationMode( m_operationMode );
        }
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datanode are set to control.");
        
        
        // register this agent for Control messages
        //TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        registerForControlMessages();

        m_rtuManager.setOperationMode(m_operationMode);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RTUManager is set to control.");

		// check the runtime parameter OperationMode
		int sleepSecs = 5000;
		if ( true == TA_Base_Core::RunParams::getInstance().isSet ( "FailoverWait" ) )
		{
			int waitInMs = atoi ( TA_Base_Core::RunParams::getInstance().get( "FailoverWait" ).c_str() );
			sleepSecs = waitInMs > 0 ? waitInMs : sleepSecs;
		}
        TA_Base_Core::Thread::sleep( sleepSecs);

		setScadaInterfaceOperationMode(m_operationMode);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PMSAgent for Station %s has been started up in CONTROL mode",
            m_stationName.c_str());
    }
    
	void PMSAgent::agentSetStandby()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PMSAgent is being set to standby...");

        if (m_operationMode == TA_Base_Core::Standby)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetStandby() called when agent is already in Standby");
			return;
        }
        // set the flag to indicate the agent is now in Standby mode
		m_operationMode = TA_Base_Core::Standby;

		m_summaryProcessor->setOperatorMode(m_operationMode);

        m_rtuManager.setOperationMode(m_operationMode);        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RTUManager is set to Standby.");

		setScadaInterfaceOperationMode(m_operationMode);

        // set all datanodes to Standby mode
		for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
		{
			(*itr)->changeOperationMode( m_operationMode );
		}

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datanode are set to Standby.");

		m_scadaProxyFactory->setOperationMode(m_operationMode);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ScadaProxyFactory is set to Standby.");
        
		// for each of the data point associated with this agent
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator it;
		for (it = m_datapoints.begin(); it != m_datapoints.end(); ++it)
		{
			// make the data point operated in Standby mode
			it->second->setToStandbyMode();
		}
		TA_Base_Bus::LFThreadPoolSingleton::getInstance()->start();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datapoints are set to Standby.");
        
        // register this agent for Standby messages
		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PMSAgent for Station %s has been started up in Standby mode",
            m_stationName.c_str());
	}

	void PMSAgent::setScadaInterfaceOperationMode(TA_Base_Core::EOperationMode mode)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "going to set Scada corba Interface to OperationMode[%d]", mode);

		if (0 != m_dpBulkPolledInterface)
		{
			m_dpBulkPolledInterface->setOperationMode(mode);
			m_bulkInterfaceStarted = true;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataPointBulkPolledInterface operation mode has been seted.");
		}

        if ( NULL != m_dnBulkPolledInterface )
		{
			m_dnBulkPolledInterface->setOperationMode(mode);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataNodeBulkPolledInterface operation mode has been seted.");
		}

		if (0 != m_dpAccessInterface)
		{
			m_dpAccessInterface->setOperationMode(mode);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataPointAccessInterface operation mode has been seted.");
		}

        if (NULL != m_dnAccessInterface)
        {
			m_dnAccessInterface->setOperationMode(mode);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataNodeAccessInterface operation mode has been seted.");
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "finished to set Scada corba Interface to OperationMode[%d]", mode);
	}

    void PMSAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		unsigned long updateEventPKey = updateEvent.getKey();

		// if the update event is for PMSAgent
		if (m_pmsAgentPrimaryKey == updateEventPKey)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received config update for PMSAgent...");

			// update PMSAgent configuration
			updatePMSAgentConfiguration(updateEvent);
			return;
		}

		// if the update event is for RTU1
		if (m_rtuManager.updateRTUConfiguration(updateEvent))
		{
            // already processed, no need go further
			return;
		}

		// else update data point configuration
		updateDataPointConfiguration (updateEvent);
	}


	void PMSAgent::startPMSAgent()
	{
		//
		// solution for CASE 1193
		//
		// create an instance of the class DataPointAgentCorbaDef_Impl
        // create an instance of the class DataPointAgentCorbaDef_Impl
        m_dpBulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl ( m_agentName, this );
        m_dpAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl(m_agentName, this);
        m_dnBulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl ( m_agentName, this );
        m_dnAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl(m_agentName, this);
        
		// else do nothing

		// check the runtime parameter OperationMode
		std::string operationMode ("");
		operationMode = TA_Base_Core::RunParams::getInstance().get (RPARAM_OPERATIONMODE);

		// if defined as "Control"
		if (RPARAM_CONTROL == operationMode)
		{
			agentSetControl();
		}
		else if (RPARAM_MONITOR == operationMode)
		{
			agentSetMonitor();
		}
		else if (RPARAM_STANDBY == operationMode)
		{
			agentSetStandby();
		}

		// start the processors to process data point events
		startProcessors();


		// start the Generic Agent loop
		if (0 != m_genericAgent)
		{
			m_genericAgent->run();
		}
	}


	void PMSAgent::submitAuditMessage (const TA_Base_Core::DescriptionParameters & description,
										const TA_Base_Core::MessageType & messageType)
	{
		try
		{
			if (0 == m_auditMessageSender)
			{
				m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::PMSAgentAudit::Context);
			}

			m_auditMessageSender->sendAuditMessage(messageType,				// message type
												   m_pmsAgentPrimaryKey,		// entity key
												   description,					// description
												   "",							// details
												   TA_Base_Core::RunParams::getInstance().get (RPARAM_SESSIONID),	// session ID
												   "",							// alarm ID
												   "",							// incidentkey
												   "");						// parent event ID
		}
		catch (...)
		{
			// do nothing
		}
	}


	void PMSAgent::registerForControlMessages()
	{
		// nothing to do at this stage
	}


	void PMSAgent::registerForMonitorMessages()
	{
	}



	void PMSAgent::updatePMSAgentConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		std::string msg ("");

		switch (updateEvent.getModifications())
		{
			// only if the PMSAgent has been updated in the database
			case TA_Base_Core::Update:
			{
				// get the list of changed parameter from the config update
				const std::vector< std::string > * changes = &(updateEvent.getChangedParams());

				if ((0 != changes) && (false == changes->empty()))
				{
					// get configuration data for this PMS Agent
					TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();

					// dynamic cast the entity data to PMSAgentEntityData
					TA_Base_Core::PMSAgentEntityDataPtr pmsAgentEntityData;
					pmsAgentEntityData = 
						boost::dynamic_pointer_cast<TA_Base_Core::PMSAgentEntityData>(entityData);

					// invalidate existing data so that new data is loaded from database
					pmsAgentEntityData->invalidate();

					for (unsigned int i = 0; i < changes->size(); i++)
					{
						// RTU switching disabled
						if (TA_Base_Core::PMSAgentEntityData::RTU_SWITCHING_DISABLED == (*changes)[i])
						{
                            m_isRTUSwitchingDisabled = pmsAgentEntityData->getIsRTUSwitchingDisabled();
							m_rtuManager.setIsRTUSwitchingDisabled(m_isRTUSwitchingDisabled);
						}

						// Proxy DataPoint Health Monitoring Period
						else if (TA_Base_Core::PMSAgentEntityData::PROXY_DP_HEALTH_MONITORING_PERIOD_SECS == (*changes)[i])
						{
							// proxy health monitoring is no longer available in 
							// the latest ScadaProxyFactory --> therefore this will do nothing
						}

                        // Analogue DataPoint Update Interval // TD16169
                        else if ( TA_Base_Core::PMSAgentEntityData::ANALOGUE_DP_UPDATE_INTERVAL == (*changes)[i] )
                        {
                            m_analogueDpUpdateInterval = pmsAgentEntityData->getAnalogueDataPointUpdateInterval();
                            m_rtuManager.setAnalogueDpUpdateInterval(m_analogueDpUpdateInterval);
                        }
                        else
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Configuration update changes is not for PMSAgentEntityData");
						}
					}
				}

				break;
			}

			// don't care in other case
			default:
			{
				// log audit message?
				break;
			}
		}
	}


    void PMSAgent::updateDataPointConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		unsigned long updateEventPKey = updateEvent.getKey();

		// find the DataPoint object whose entity key match with the
		// key associated with the received message
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator dpIter = m_datapoints.find (updateEventPKey);

		// if found one
		if (dpIter != m_datapoints.end())
		{
			TA_Base_Bus::DataPoint * dp = dpIter->second;

			// get the type of updating event
			switch (updateEvent.getModifications())
			{
            case TA_Base_Core::Update:
                // pass the config update to the data point for updating
                dp->update(updateEvent);
                break;
            
            case TA_Base_Core::Delete:
                m_rtuManager.removeDataPoint(dp);
                break;
                
            case TA_Base_Core::Create:
                m_rtuManager.addDataPoint(dp);
                break;
            
            default:
                // do nothing, exit switch statement
                break;
			}


			//
			// check if the data point's update priority has been changed online
			//

			// get the list of changed parameter from the config update
			const std::vector< std::string > * configChanges = 0;
			configChanges = &updateEvent.getChangedParams();

			// for each of the changed parameters
			for (unsigned int i = 0; i < configChanges->size(); i++)
			{
				std::string paramName = (*configChanges)[i];

				// only interested in this parameter
				if (paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_UPDATE_PRIORITY)
				{
					//
					// update the RTU's with the data point whose update priority
					// has been changed
					//
					m_rtuManager.removeDataPoint (dp);
					m_rtuManager.addDataPoint (dp);
				}
			}

			dp = 0;
		}
	}

	void PMSAgent::notifyGroupOffline(const std::string& group)
	{
		// nothing to do at this stage
	}


	void PMSAgent::notifyGroupOnline(const std::string& group)
	{
		// nothing to do at this stage
	}


	TA_Base_Bus::IEntity* PMSAgent::createDataPointEntity (TA_Base_Core::IEntityDataPtr EntityData)
	{
		// cast the EntityData to DataPointEntityData
		TA_Base_Core::DataPointEntityDataPtr dataPointEntityData =
			boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData> (EntityData);

        TA_ASSERT(NULL != dataPointEntityData.get(), "dataPointEntityData is NULL in PMSAgent::createDataPointEntity");

        // create a data point based on the specified entity data
		TA_Base_Bus::DataPoint * dp = m_dataPointFactory->createDataPoint(dataPointEntityData);

        // insert the newly created data point to internal list for further
        // referencing.
        if (NULL != dp)
        {
			TA_Base_Core::ThreadGuard guard (m_dataPointListtLock);
            m_datapoints[ dp->getEntityKey() ] = dp;
            //dp->setToMonitorMode(); // force datapoint to load config data
        }
    
		return dp;
	}


	TA_Base_Bus::DataPoint * PMSAgent::getDataPoint (unsigned long entityKey)
	{
		// stop multi threads accessing the list of DataPoint objects at the same time
		TA_Base_Core::ThreadGuard guard (m_dataPointListtLock);

		TA_Base_Bus::DataPoint * theDataPoint = 0;

		// find if there is a data point whose pkey matches with the specified entity key
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itr = m_datapoints.find (entityKey);

		// if found one
		if (itr != m_datapoints.end())
		{
			// get the reference to the data point
			theDataPoint = itr->second;
		}
		// else do nothing

		return theDataPoint;
	}


    void PMSAgent::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
	{
		// stop multi threads accessing the list of DataPoint objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataPointListtLock );

        dataPointList.clear();

        dataPointList = m_datapoints;
	}

    TA_Base_Bus::DataNode * PMSAgent::getDataNode( unsigned long entityKey )
	{
		// stop multi threads accessing the list of DataNode objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
		TA_Base_Bus::DataNode * dn = NULL;
        
        const std::map<unsigned long, TA_Base_Bus::IEntity*>* dataNodes = m_genericAgent->getEntityMap();
		std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator itr = dataNodes->find( entityKey );
		
		if ( itr != dataNodes->end() )
		{
			dn = dynamic_cast<TA_Base_Bus::DataNode *>(itr->second);
			
            if ( NULL == dn )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not convert entity %ul to a DataNode", entityKey );
			}
        }
		
		return dn;
	}

	void PMSAgent::getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
    {
        TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
        dataNodeList.clear();
        for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
        {
            dataNodeList[ (*itr)->getEntityKey() ] = *itr;
        }        
    }


	TA_Base_Bus::DataPointFactory* PMSAgent::getDataPointFactory()
	{
		TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");
	       
		return m_dataPointFactory;
	}
		
    void PMSAgent::registerForStateUpdates()
    {
    }

	
    void PMSAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
    }

	void PMSAgent::beforeGetDescendantsOfAgent(TA_Base_Core::IEntityDataPtr agentEntity)
	{
		if (m_agentInitThread == NULL)
		{
			m_agentInitThread = new AgentInitThread(*this, &PMSAgent::initilizeCache, agentEntity);
		}

		m_agentInitThread->start();
	}

	void PMSAgent::initilizeCache(TA_Base_Core::IEntityDataPtr agentEntity)
	{
		m_persistence = TA_Base_Bus::ScadaPersistencePtr( TA_Base_Bus::ScadaPersistence::createScadaPersistenceForAgent(TA_Base_Bus::ScadaPersistence::DATANODE, agentEntity->getKey()) );
		if (NULL != m_dataPointFactory)
		{
			unsigned long agentKey = agentEntity->getAgent();
			std::string agentName = agentEntity->getAgentName();
			m_dataPointFactory->initialize(agentKey, agentName);
		}
		// intialize DataPointStateChangeAccessFactory first
		TA_Base_Core::DataPointStateChangeAccessFactory::getInstance();
		TA_Base_Core::DerivedDataPointAccessFactory::getInstance().bulkLoadByAgentKey(agentEntity->getAgent());
	}

	void PMSAgent::initializeDPAfterProxyReady()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "begin init data point after proxy factory ready");

		TA_Base_Core::WorkItemExeSync* pItemSync = new TA_Base_Core::WorkItemExeSync();
		std::vector<TA_Base_Core::IThreadPoolExecuteItem*> workItems;
		workItems.reserve(m_datapoints.size());

		//TA_Base_Core::LFThreadPoolManager* threadPool = new TA_Base_Core::LFThreadPoolManager(8, 8);
		int num = 8;
		if ( true == RunParams::getInstance().isSet ( "InitThreadedWorkers" ) )
		{
			num = atoi ( RunParams::getInstance().get( "InitThreadedWorkers" ).c_str() );
		}
		TA_Base_Core::LFThreadPoolManager threadPool(num);
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator it;
		for (it = m_datapoints.begin(); it != m_datapoints.end(); ++it)
		{
			workItems.push_back(new InitDataPointWorkItem(it->second, pItemSync));
		}
		threadPool.queueWorkItemList(workItems);
		pItemSync->wait();
		delete pItemSync;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "end init data point after proxy factory ready");
	}

	AgentInitThread::AgentInitThread(PMSAgent& agent, FuncInit func, TA_Base_Core::IEntityDataPtr agentEntityData)
		: m_semphore(0),
		  m_func(func),
		  m_agentEntityData(agentEntityData),
		  m_agent(agent),
		  m_executeSuccess(true)
	{
		
	}

	AgentInitThread::~AgentInitThread()
	{
		terminateAndWait();
	}

	bool AgentInitThread::waitForCompelete()
	{
		m_semphore.wait();
		return m_executeSuccess;
	}

	void AgentInitThread::run()
	{
		if (NULL != m_func)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "begin AgentInitThread");
			
			try
			{
				(m_agent.*m_func)(m_agentEntityData);
			}
			catch(...)
			{
				m_executeSuccess = false;
			}
			
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "end AgentInitThread" );
		}

		m_semphore.post();
	}

	void AgentInitThread::terminate()
	{
		m_semphore.post();
	}

	CreateEntityWorkItem::CreateEntityWorkItem(PMSAgent& agent, 
		TA_Base_Core::IEntityDataPtr entityData,
		TA_Base_Core::WorkItemExeSync* workSync)
		:
	m_parent(agent),
	m_entityData(entityData),
	m_sync(workSync)
	{
		m_sync->itemBegin();
	}

	void CreateEntityWorkItem::executeWorkItem()
	{
		try
        {
            m_parent.createEntity(m_entityData);
            m_sync->itemEnd();
        }
        catch(...)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error happend when create entity [%d]", m_entityData->getKey());
        }		
	}

	InitDataPointWorkItem::InitDataPointWorkItem(TA_Base_Bus::DataPoint* dp, TA_Base_Core::WorkItemExeSync* workSync)
		:
	m_dp(dp),
	m_sync(workSync)
	{
		m_sync->itemBegin();
	}

	void InitDataPointWorkItem::executeWorkItem()
	{
        try
        {
            m_dp->initAfterProxyFactoryReady();
            m_sync->itemEnd();
        }
        catch(...)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error happend when init datapoint [%d]", m_dp->getEntityKey());
        }		
	}
}
