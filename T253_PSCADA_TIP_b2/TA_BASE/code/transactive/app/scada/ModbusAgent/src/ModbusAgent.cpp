/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/ModbusAgent/src/ModbusAgent.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #8 $
  *
  * Last modification:	$DateTime: 2011/03/17 16:52:06 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  *	ModbusAgent implementes the GenericAgent interface.  ModbusAgent monitors and controls the
  * the entities associated with its process, such as RTU and the DataPoint entities.
  *
  */
#include <sstream>

#include "ModbusAgent.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/business_library/src/BusinessManager.h"
#include "bus/scada/rtu_library/src/CommonDpProcessor.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ModbusAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/data_access_interface/entity_access/src/StationSystemEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/exceptions/src/DataException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/GenericAgentAudit_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/timers/src/StopwatchUtil.h"
#include "core/utilities/src/RunParams.h"


using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
	ModbusAgent::ModbusAgent (int argc, char* argv[])
	:
	m_ModbusAgentPrimaryKey (0),
    m_operationMode (TA_Base_Core::NotApplicable),
    m_bulkInterfaceStarted (false),
    m_bOverlapModbusRequest (false),
	m_bStandardModbusLengthLimit (false),
	m_iMaxModbusLength (TA_Base_Bus::MAX_STANDARD_MODBUS_LENGTH_LIMIT),
	m_iIntervalBetweenModbusTransaction (0),
    m_stationName (""),
    m_agentName (""),
	m_agentAssetName (""),
    m_genericAgent (0),
	m_auditMessageSender (0),
    m_dpBulkPolledInterface (0),
    m_dpAccessInterface(0),
    m_dataPointFactory (0),
    m_scadaProxyFactory (0),
    m_threadPoolSingletonManager (0),
	m_dnBulkPolledInterface(NULL),
	m_dnAccessInterface(NULL),
	m_summaryProcessor(NULL),
	m_hasOtherBusiness(false)
	{
		m_datapoints.clear();
		m_dataNodes.clear();

		// create handle to DataPointFactory
		if (0 == m_dataPointFactory)
		{
			m_dataPointFactory = new TA_Base_Bus::DataPointFactory(this);
		}

		// get a handle to the ScadaProxyFactory
		m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());

		TA_ASSERT ((0 != m_scadaProxyFactory), "SCADA Proxy Factory not created");

		// create handle to Generic Agent
		m_genericAgent = new TA_Base_Bus::GenericAgent (argc, argv, this);

		// get the agent name
		m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();

		// register the local entities into ScadaProxyFactory
		m_scadaProxyFactory->registerLocalEntities(m_agentName, m_genericAgent->getEntityMap());

		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

        // force datapoints to load config data which will be used used by createEMSWorkers
        std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator it;
        for (it = m_datapoints.begin(); it != m_datapoints.end(); ++it)
        {
            // make the data point operated in Monitor mode
            it->second->setToMonitorMode();
        }
        
        // get handle to ThreadPoolSingletonManager
		m_threadPoolSingletonManager = TA_Base_Core::ThreadPoolSingletonManager::getInstance();
		// create SummaryProcessor
        m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());

        createModbusWorkers();
		std::string val = TA_Base_Core::RunParams::getInstance().get("OtherBusiness");	
 		if (val == "YES")
 		{
			m_hasOtherBusiness = true;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ModbusAgent start business module");
		}
		
	}


	ModbusAgent::~ModbusAgent()
	{
		if (m_hasOtherBusiness)
		{
			TA_Base_Bus::BusinessManager::removeInstance();
		}

		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

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

		// clean up handle to ThreadPoolSingletonManager
		if (0 != m_threadPoolSingletonManager)
		{
			TA_Base_Core::ThreadPoolSingletonManager::removeInstance (m_threadPoolSingletonManager);
			m_threadPoolSingletonManager  = 0;
		}

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
	}

	std::vector<unsigned long> ModbusAgent::parseAddresses(std::string strAddressValues)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Parsing %s ...", strAddressValues.c_str());
		int pos = 0, index = 0;
		int value = 0;
		std::string valueSTR = "";
		std::vector<unsigned long> vAllValues;
		vAllValues.clear();
		while("" != strAddressValues)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Current List %s", strAddressValues.c_str());
			pos = strAddressValues.find ( ",", 0 );
			if ( 0 < pos )
			{
				valueSTR = (strAddressValues.substr( 0, pos )).c_str();
				value = atoi(valueSTR.c_str());
				if( value > 0)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Port added in List %s", valueSTR.c_str());
					vAllValues.push_back(value);
					index++;
				}
				else 
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Value configured is not greater than 0, List %s", strAddressValues.c_str());
				}
				strAddressValues = (strAddressValues.substr ( pos + 1, strAddressValues.length() )).c_str();
			}
			else if ( strAddressValues != "")
			{
				valueSTR = strAddressValues.c_str();
				value = atoi(valueSTR.c_str());
				if( value > 0)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Address added in List %s", valueSTR.c_str());
					vAllValues.push_back(value);
					index++;
					break;
				}
				else 
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Value configured is not greater than 0, List %s", strAddressValues.c_str());
				}
				strAddressValues = "";
			}
		} //while end

		return vAllValues;	
	}

	void ModbusAgent::loadAgentParamValues()
	{
        // if m_ModbusAgentPrimaryKey != 0, means the loadAgentParamValues() has been called, no need multip calls.
        if (0 < m_ModbusAgentPrimaryKey)
            return;
        
		// get configuration data for this EMS Agent
		// TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();
        // Becase m_genericAgent sitll be 0 in this time, it will get value after this function called by genericAgent, so we can't
        // get entityData via this way. And we get entityData through the way below.
        std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
        TA_Base_Core::IEntityDataPtr entityData(EntityAccessFactory::getInstance().getEntity(entityName));

		// dynamic cast the entity data to ModbusAgentEntityData
		TA_Base_Core::ModbusAgentEntityDataPtr ModbusAgentEntityData;
		ModbusAgentEntityData = 
			boost::dynamic_pointer_cast<TA_Base_Core::ModbusAgentEntityData>(entityData);

		// get the primary key of this ModbusAgent
		m_ModbusAgentPrimaryKey = entityData->getKey();
        
        
        
		// get the agent asset name
		m_agentAssetName = ModbusAgentEntityData->getAssetName();

		// get agent specific params
		m_bOverlapModbusRequest = ModbusAgentEntityData->getOverlapModbusRequest();
		m_bStandardModbusLengthLimit = ModbusAgentEntityData->getStandardModbusLengthLimit();
        m_iMaxModbusLength = ModbusAgentEntityData->getMaxModbusLength();
		m_iIntervalBetweenModbusTransaction = ModbusAgentEntityData->getIntervalBetweenModbusTransaction();

		// get the configured station name
		m_stationName = ModbusAgentEntityData->getStationName();
		m_iPollTimeout = ModbusAgentEntityData->getPollTimeout(); 

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"loadAgentParamValues finished. Set - "
				"m_ModbusAgentPrimaryKey = %d \n"
				"m_agentAssetName = %s \n"
				"m_bOverlapModbusRequest = %d \n"
				"m_bStandardModbusLengthLimit = %d \n"
				"m_iMaxModbusLength = %d \n"
				"m_iIntervalBetweenModbusTransaction = %d \n"
				"m_stationName = %s \n"
				"m_iPollTimeout = %d \n"
			, m_ModbusAgentPrimaryKey, m_agentAssetName.c_str(), m_bOverlapModbusRequest, m_bStandardModbusLengthLimit, m_iMaxModbusLength, m_iIntervalBetweenModbusTransaction, m_stationName.c_str(), m_iPollTimeout);
        
        // before get the values, we clear the vectors to make sure no data
        m_polling_di_start_address.clear();
		m_polling_di_end_address.clear();
		m_polling_ai_start_address.clear();
		m_polling_ai_end_address.clear();
		m_polling_mi_start_address.clear();
		m_polling_mi_end_address.clear();
		m_dpprocessors_start_ranges.clear();
		m_dpprocessors_end_ranges.clear();

		m_polling_di_start_address = parseAddresses(ModbusAgentEntityData->getPollingDiStartAddress());
		m_polling_di_end_address = parseAddresses(ModbusAgentEntityData->getPollingDiEndAddress());
		m_polling_di_timeout = ModbusAgentEntityData->getPollingDiTimeOut();
		m_polling_di_type = ModbusAgentEntityData->getPollingDiType();

		m_polling_ai_start_address = parseAddresses(ModbusAgentEntityData->getPollingAiStartAddress());
		m_polling_ai_end_address = parseAddresses(ModbusAgentEntityData->getPollingAiEndAddress());
		m_polling_ai_timeout = ModbusAgentEntityData->getPollingAiTimeOut();
		m_polling_ai_type = ModbusAgentEntityData->getPollingAiType();

		m_polling_mi_start_address = parseAddresses(ModbusAgentEntityData->getPollingMiStartAddress());
		m_polling_mi_end_address = parseAddresses(ModbusAgentEntityData->getPollingMiEndAddress());
		m_polling_mi_timeout = ModbusAgentEntityData->getPollingMiTimeOut();
		m_polling_mi_type = ModbusAgentEntityData->getPollingMiType();

		m_dpprocessors_start_ranges = parseAddresses(ModbusAgentEntityData->getDPProcessorsStartAddresses());
		m_dpprocessors_end_ranges = parseAddresses(ModbusAgentEntityData->getDPProcessorsEndAddresses());
		m_analogueDpUpdateInterval = ModbusAgentEntityData->getAnalogueDpUpdateInterval();

LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"DI start Address = %u \n"
				"DI end Address = %u \n"
				"DI Timeout = %d \n"
				"DI Type = %d \n"
				"AI start Address = %u \n"
				"AI end Address = %u \n"
				"AI Timeout = %d \n"
				"AI Type = %d \n"
				"MI start Address = %u \n"
				"MI end Address = %u \n"
				"MI Timeout = %d \n"
				"MI Type = %d \n"
				"DP Start size = %d"
				"DP End size = %d"
				"DP Analogue UPD Interval = %d"
			, m_polling_di_start_address.size(), m_polling_di_end_address.size(), m_polling_di_timeout, m_polling_di_type, m_polling_ai_start_address.size(), m_polling_ai_end_address.size(), m_polling_ai_timeout, m_polling_ai_type,
		m_polling_mi_start_address.size(), m_polling_mi_end_address.size(), m_polling_mi_timeout, m_polling_mi_type,
		m_dpprocessors_start_ranges.size(), m_dpprocessors_end_ranges.size(), m_analogueDpUpdateInterval);
	}

	void ModbusAgent::startProcessors()
	{
	}

	void ModbusAgent::stopProcessors()
	{
	}


	void ModbusAgent::createModbusWorkers()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"createModubsWorkers() called.");

		// if does not have the agent's pkey
		if (0 == m_ModbusAgentPrimaryKey)
		{
			// loadAgentParamValues();
            RTUManagerMap::iterator pos;
        	for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
            {

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUManager set m_agentAssetName params");
    						pos->second->setAgentAssetName(m_agentAssetName);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUManager set m_stationName params");
        					pos->second->setStationName(m_stationName);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUManager set m_bOverlapModbusRequest params");
    						pos->second->setOverlapModbusRequest(m_bOverlapModbusRequest);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUManager set m_bStandardModbusLengthLimit params");
        					pos->second->setStandardModbusLengthLimit(m_bStandardModbusLengthLimit);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUManager set m_iMaxModbusLength params");
    						pos->second->setMaxModbusLength(m_iMaxModbusLength);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUManager set m_iIntervalBetweenModbusTransaction params");
        					pos->second->setIntervalBetweenModbusTransaction(m_iIntervalBetweenModbusTransaction);
				/*LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUManager set m_iPollTimeout params");
    						pos->second->setPollTimeout(m_iPollTimeout);*/
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUManager for groupID %d, finished set params",pos->first );
			}
		}

        // create rtu workers
        RTUManagerMap::iterator pos;
        for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RTUManager_%d created PollingSchedulers.", pos->first);
			pos->second->createRTUWorkers();
			pos->second->setRTUStatusWorker();
			int index;
			// DI polling 
			if(m_polling_di_start_address.size() != m_polling_di_end_address.size())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Configuration is incorrect for DI polling schedulers (Unequal elements in Start & End list)");
			}
			for(index=0; index < m_polling_di_start_address.size()&& (m_polling_di_start_address.size() == m_polling_di_end_address.size()); index++)
			{
				if(m_polling_di_start_address[index] > m_polling_di_end_address[index])
					continue;
				(pos->second)->addPollingScheduler( m_polling_di_start_address[index], m_polling_di_end_address[index], m_polling_di_timeout, static_cast< TA_Base_Bus::ECommandType >(m_polling_di_type));
			}
			// AI polling 
			if(m_polling_ai_start_address.size() != m_polling_ai_end_address.size())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Configuration is incorrect for AI polling schedulers (Unequal elements in Start & End list)");
			}
			for(index=0; index < m_polling_ai_start_address.size()&& (m_polling_ai_start_address.size() == m_polling_ai_end_address.size()); index++)
			{
				if(m_polling_ai_start_address[index] > m_polling_ai_end_address[index])
					continue;
				(pos->second)->addPollingScheduler( m_polling_ai_start_address[index], m_polling_ai_end_address[index], m_polling_ai_timeout, static_cast< TA_Base_Bus::ECommandType >(m_polling_ai_type));
			}
			
			// MI polling 
			if(m_polling_mi_start_address.size() != m_polling_mi_end_address.size())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Configuration is incorrect for MI polling schedulers (Unequal elements in Start & End list)");
			}

			for(index=0; index < m_polling_mi_start_address.size()&& (m_polling_mi_start_address.size() == m_polling_mi_end_address.size()); index++)
			{
				if(m_polling_mi_start_address[index] > m_polling_mi_end_address[index])
					continue;
				(pos->second)->addPollingScheduler( m_polling_mi_start_address[index], m_polling_mi_end_address[index], m_polling_mi_timeout, static_cast< TA_Base_Bus::ECommandType >(m_polling_mi_type));
			}

			// DP processors
			for(index=0; index < m_dpprocessors_start_ranges.size()&&(m_dpprocessors_start_ranges.size() == m_dpprocessors_end_ranges.size()); index++)
			{
				if(m_dpprocessors_start_ranges[index] > m_dpprocessors_end_ranges[index])
					continue;
				std::stringstream ss;
				ss << "DPProcessor_" << (index+1);
				std::string name = ss.str();
				
				TA_Base_Bus::CommonDpProcessor *cdp = new TA_Base_Bus::CommonDpProcessor(name, m_dpprocessors_start_ranges[index], m_dpprocessors_end_ranges[index],0 ,false, m_analogueDpUpdateInterval );
				(pos->second)->addDpProcessor(cdp);
			}			

		}

        // add datapoint to rtu manager except virtual datapoint
        for (std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp = m_datapoints.begin(); itrDp != m_datapoints.end(); ++itrDp)
        {
            if (false == itrDp->second->isVirtual())
            {
            	RTUManagerMap::iterator pos;
                for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
				{
					bool ret = pos->second->addDataPoint(itrDp->second);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUManager addDataPoint address %s ", itrDp->second->getDataPointAddress().c_str(),ret==true?"success":"fail");
				}
            }
        }
    }


	TA_Base_Bus::IEntity* ModbusAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
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
            loadAgentParamValues();
			TA_Base_Core::RTUEntityData *rtuEntityData = dynamic_cast<TA_Base_Core::RTUEntityData*>(EntityData.get());
			int groupID = rtuEntityData->getGroupID();
			RTUManagerMap::iterator it;
			if( (it = m_rtuManagers.find(groupID) ) == m_rtuManagers.end())
			{				
                m_rtuManagers[groupID] = new TA_Base_Bus::StandardRTUManager(groupID, true, false, TA_Base_Bus::SINGLE_PRIMARY_RTU_PATTERN,
                                                m_bOverlapModbusRequest, m_bStandardModbusLengthLimit, m_iMaxModbusLength,
                                                m_iIntervalBetweenModbusTransaction);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "created RTUManager with groupID = %d, \
                                m_bOverlapModbusRequest = %d, m_bStandardModbusLengthLimit = %d, m_iMaxModbusLength = %d, \
                                m_iIntervalBetweenModbusTransaction = %d",groupID, m_bOverlapModbusRequest,
                                m_bStandardModbusLengthLimit, m_iMaxModbusLength, m_iIntervalBetweenModbusTransaction);
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Not created any RTUManager for groupID %d",groupID);
			}

        	m_rtuManagers[groupID]->createRTUEntity(EntityData);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "create RTUEntity with groupID = %d",groupID);
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
                
                m_dataNodes.push_back( dn );
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

	bool ModbusAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
											  const TA_Base_Core::SharedIEntityDataList& entityDataList,
											  TA_Base_Bus::IEntityList& createdEntities)
	{
		bool createdDnSpecificUtils = false;

        for ( TA_Base_Core::SharedIEntityDataList::const_iterator it = entityDataList.begin();
        it != entityDataList.end(); ++it )
        {
            std::string entityDataType = (*it)->getType();
            if ((entityDataType == TA_Base_Core::DataNodeEntityData::getStaticType()) ||
                (entityDataType == TA_Base_Core::DataPointEntityData::getStaticType()) )
            {
                m_scadaProxyFactory->registerEntityData(*it, entityDataType);
            }
        }
		for ( TA_Base_Core::SharedIEntityDataList::const_iterator iter = entityDataList.begin();
		iter != entityDataList.end(); ++iter )
		{
			// only datanodes need these things, so if there are no datanodes, why create them?
			if ( (false == createdDnSpecificUtils) &&
				((*iter)->getType() == TA_Base_Core::DataNodeEntityData::getStaticType()) )
			{
				// create an instance of DataNodeAccessFactory if not done already
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"Start of create DataNode Hierarchy Map" );
            
				TA_Base_Core::DataNodeAccessFactory::getInstance()->createDataNodeHierarchyMap(agentEntityData->getKey(), entityDataList);
            
				// initialise ScadaPersistence
				m_persistence = TA_Base_Bus::ScadaPersistencePtr( TA_Base_Bus::ScadaPersistence::createScadaPersistenceForAgent(TA_Base_Bus::ScadaPersistence::DATANODE, agentEntityData->getKey()) );
            
				createdDnSpecificUtils = true;
			}
        
			createdEntities.push_back( 
				TA_Base_Bus::IEntityList::value_type( (*iter)->getKey(), createEntity(*iter)) );
		}
    
		// return true to indicate not to call createEntity
		return true;
	}


	void ModbusAgent::entityCreationComplete()
	{
		// loading complete, free some memory
		TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();
	}

	void ModbusAgent::agentTerminate()
	{
 		if (m_hasOtherBusiness)
		{
			TA_Base_Bus::BusinessManager::getInstance().hostStop();
		}

		// make sure the processors are all terminated
		stopProcessors();
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
		m_scadaProxyFactory->setProxiesToMonitorMode();

    }


	void ModbusAgent::agentSetMonitor()
	{
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ModbusAgent is being set to monitor...");

        if (m_operationMode == TA_Base_Core::Monitor)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetMonitor() called when agent is already in monitor");
			return;
        }
        // set the flag to indicate the agent is now in Standby mode
		m_operationMode = TA_Base_Core::Monitor;
        
		if (m_hasOtherBusiness)
		{
			TA_Base_Bus::BusinessManager::getInstance().setToMonitor();
		}
        // set rtu manager to monitor mode
        
        RTUManagerMap::iterator pos;
        for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
		{
        	pos->second->setToMonitor();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RTUManager %d is set to monitor.",pos->first);
		}
		
		// if bulk interface is set up
		if (0 != m_dpBulkPolledInterface)
		{
			// stop receiving remote requests
			m_dpBulkPolledInterface->setToMonitor();
			m_bulkInterfaceStarted = false;
		}

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataPointBulkPolledInterface is set to monitor.");

        if ( NULL != m_dnBulkPolledInterface )
		{
			m_dnBulkPolledInterface->setToMonitor();
		}
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataNodeBulkPolledInterface is set to monitor.");
        // if bulk interface is set up
		if (0 != m_dpAccessInterface)
		{
			// stop receiving remote requests
			m_dpAccessInterface->setToMonitor();
		}

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataPointAccessInterface is set to monitor.");

        if (NULL != m_dnAccessInterface)
        {
			m_dnAccessInterface->setToMonitor();
		}
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataNodeAccessInterface is set to monitor.");
        // set all datanodes to monitor mode
		for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
		{
			(*itr)->setOperationMode( false );
		}

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datanode are set to monitor.");



        // set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ScadaProxyFactory is set to monitor.");
        
		// for each of the data point associated with this agent
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator it;
		for (it = m_datapoints.begin(); it != m_datapoints.end(); ++it)
		{
			// make the data point operated in Monitor mode
			it->second->setToMonitorMode();
		}

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datapoints are set to monitor.");
        
        // register this agent for Monitor messages
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
		registerForMonitorMessages();

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ModbusAgent for Station %s has been started up in MONITOR mode",
            m_stationName.c_str());
	}

    void ModbusAgent::agentSetControl()
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ModbusAgent is being set to control...");

        if (m_operationMode == TA_Base_Core::Control)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetControl() called when agent is already in control");
			return;
        }
		
        // set the flag to indicate the agent is now in Control mode
        m_operationMode = TA_Base_Core::Control;
        
        // for each of the data point associated with this agent
        std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp;
        for (itrDp = m_datapoints.begin(); itrDp != m_datapoints.end(); ++itrDp)
        {
            // make the data point operated in Control mode
            itrDp->second->setToControlMode();
        }
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datapoints are set to control.");
        
        // set the proxies to control mode
        m_scadaProxyFactory->setProxiesToControlMode();
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ScadaProxyFactory is set to control.");
        
        // if bulk interface is set up
        if (0 != m_dpBulkPolledInterface)
        {
            // start receiving remote requests
            m_dpBulkPolledInterface->setToControl();
            m_bulkInterfaceStarted = true;
        }
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataPointBulkPolledInterface is set to control.");
        
        // if bulk interface is set up
        if (0 != m_dpAccessInterface)
        {
            // start receiving remote requests
            m_dpAccessInterface->setToControl();
        }
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataPointAccessInterface is set to control.");
        
        // set all datanodes to control mode
        for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
        {
            (*itr)->setOperationMode( true );
        }
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datanode are set to control.");
        
        if ( NULL != m_dnBulkPolledInterface )
        {
            // start receiving remote requests
            m_dnBulkPolledInterface->setToControl();
        }
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataNodeBulkPolledInterface is set to control.");
        
        if (NULL != m_dnAccessInterface)
        {
            m_dnAccessInterface->setToControl();
        }
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataNodeAccessInterface is set to control.");
        
        // register this agent for Control messages
        //TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        registerForControlMessages();

       RTUManagerMap::iterator pos;
        for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
		{	
			pos->second->setToControl();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RTUManager %d is set to control.",pos->first);
		}

		if (m_hasOtherBusiness)
		{
			TA_Base_Bus::BusinessManager::getInstance().setToControl();
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ModbusAgent for Station %s has been started up in CONTROL mode",
        m_stationName.c_str());
    }
    

    void ModbusAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		unsigned long updateEventPKey = updateEvent.getKey();

		// if the update event is for ModbusAgent
		if (m_ModbusAgentPrimaryKey == updateEventPKey)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received config update for ModbusAgent...");

			// update ModbusAgent configuration
			updateModbusAgentConfiguration(updateEvent);
			return;
		}

		// if the update event is for RTU1
		bool isContinue = true;

		RTUManagerMap::iterator pos;
		for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
        {
			if( pos->second->updateRTUConfiguration(updateEvent) )
				isContinue = false;
		}
		if (isContinue == false )
		{
            // already processed, no need go further
			return;
		}

		// else update data point configuration
		updateDataPointConfiguration (updateEvent);
	}


	void ModbusAgent::startModbusAgent()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "startModbusAgent in..");		
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
		

		// start the processors to process data point events
		startProcessors();

		if (m_hasOtherBusiness)
		{
			TA_Base_Bus::BusinessManager::getInstance().hostStart();
		}

		// start the Generic Agent loop
		if (0 != m_genericAgent)
		{
			m_genericAgent->run();
		}
	}


	void ModbusAgent::submitAuditMessage (const TA_Base_Core::DescriptionParameters & description,
										const TA_Base_Core::MessageType & messageType)
	{
		try
		{
			if (0 == m_auditMessageSender)
			{
				m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::GenericAgentAudit::Context);
			}

			m_auditMessageSender->sendAuditMessage(messageType,				// message type
												   m_ModbusAgentPrimaryKey,		// entity key
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


	void ModbusAgent::registerForControlMessages()
	{
		// nothing to do at this stage
	}


	void ModbusAgent::registerForMonitorMessages()
	{
	}



	void ModbusAgent::updateModbusAgentConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		std::string msg ("");

		switch (updateEvent.getModifications())
		{
			// only if the ModbusAgent has been updated in the database
			case TA_Base_Core::Update:
			{
				// get the list of changed parameter from the config update
				const std::vector< std::string > * changes = &(updateEvent.getChangedParams());

				if ((0 != changes) && (false == changes->empty()))
				{
					// get configuration data for this EMS Agent
					TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();

					// dynamic cast the entity data to ModbusAgentEntityData
					TA_Base_Core::ModbusAgentEntityDataPtr ModbusAgentEntityData;
					ModbusAgentEntityData = 
						boost::dynamic_pointer_cast<TA_Base_Core::ModbusAgentEntityData>(entityData);

					// invalidate existing data so that new data is loaded from database
					ModbusAgentEntityData->invalidate();

					for (unsigned int i = 0; i < changes->size(); i++)
					{
						// RTU switching disabled
						if (TA_Base_Core::ModbusAgentEntityData::OVERLAPREQUEST == (*changes)[i])
						{
                            m_bOverlapModbusRequest = ModbusAgentEntityData->getOverlapModbusRequest();
							RTUManagerMap::iterator pos;
        					for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
							{
        						pos->second->setOverlapModbusRequest(m_bOverlapModbusRequest);
							}
						}
						// Proxy DataPoint Health Monitoring Period
						else if (TA_Base_Core::ModbusAgentEntityData::INTERVAL_BETWN_MODBUSTRX == (*changes)[i])
						{
                            m_iIntervalBetweenModbusTransaction = ModbusAgentEntityData->getIntervalBetweenModbusTransaction();
							RTUManagerMap::iterator pos;
        					for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
							{
        						pos->second->setIntervalBetweenModbusTransaction(m_iIntervalBetweenModbusTransaction);
							}
						}

                        // Analogue DataPoint Update Interval // TD16169
                        else if ( TA_Base_Core::ModbusAgentEntityData::MAX_MODBUS_LEN == (*changes)[i] )
                        {
                            m_iMaxModbusLength = ModbusAgentEntityData->getMaxModbusLength();
							RTUManagerMap::iterator pos;
        					for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
							{
        						pos->second->setMaxModbusLength(m_iMaxModbusLength);
							}
						}
                        else if ( TA_Base_Core::ModbusAgentEntityData::STD_MODBUS_LEN_LMT == (*changes)[i] )
                        {
                            m_bStandardModbusLengthLimit = ModbusAgentEntityData->getStandardModbusLengthLimit();
							RTUManagerMap::iterator pos;
        					for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
							{
        						pos->second->setStandardModbusLengthLimit(m_bStandardModbusLengthLimit);
							}
						}
						else
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Configuration update changes is not for ModbusAgentEntityData");
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


    void ModbusAgent::updateDataPointConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		unsigned long updateEventPKey = updateEvent.getKey();

		// find the DataPoint object whose entity key match with the
		// key associated with the received message
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator dpIter = m_datapoints.find (updateEventPKey);

		// if found one
		if (dpIter != m_datapoints.end())
		{
			TA_Base_Bus::DataPoint * dp = dpIter->second;

			RTUManagerMap::iterator pos;
			// get the type of updating event
			switch (updateEvent.getModifications())
			{
            case TA_Base_Core::Update:
                // pass the config update to the data point for updating
                dp->update(updateEvent);
                break;
            
            case TA_Base_Core::Delete:
        			for(pos = m_rtuManagers.begin(); pos !=  m_rtuManagers.end(); ++pos)
        				pos->second->removeDataPoint(dp);
                break;
                
            case TA_Base_Core::Create:
					for(pos = m_rtuManagers.begin(); pos !=  m_rtuManagers.end(); ++pos)
        				pos->second->addDataPoint(dp);
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
					RTUManagerMap::iterator pos;
        			for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
					{
						pos->second->removeDataPoint (dp);
						pos->second->addDataPoint (dp);
					}
					
				}
			}

			dp = 0;
		}
	}

	void ModbusAgent::notifyGroupOffline(const std::string& group)
	{
		// nothing to do at this stage
	}


	void ModbusAgent::notifyGroupOnline(const std::string& group)
	{
		// nothing to do at this stage
	}


	TA_Base_Bus::IEntity* ModbusAgent::createDataPointEntity (TA_Base_Core::IEntityDataPtr EntityData)
	{
		// cast the EntityData to DataPointEntityData
		TA_Base_Core::DataPointEntityDataPtr dataPointEntityData =
			boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData> (EntityData);

        TA_ASSERT(NULL != dataPointEntityData.get(), "dataPointEntityData is NULL in ModbusAgent::createDataPointEntity");

        // create a data point based on the specified entity data
		TA_Base_Bus::DataPoint * dp = m_dataPointFactory->createDataPoint(dataPointEntityData);

        // insert the newly created data point to internal list for further
        // referencing.
        if (NULL != dp)
        {
            m_datapoints[ dp->getEntityKey() ] = dp;
            //dp->setToMonitorMode(); // force datapoint to load config data
        }
    
		return dp;
	}


	TA_Base_Bus::DataPoint * ModbusAgent::getDataPoint (unsigned long entityKey)
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


    void ModbusAgent::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
	{
		// stop multi threads accessing the list of DataPoint objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataPointListtLock );

        dataPointList.clear();

        dataPointList = m_datapoints;
	}

    TA_Base_Bus::DataNode * ModbusAgent::getDataNode( unsigned long entityKey )
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

	void ModbusAgent::getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
    {
        TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
        dataNodeList.clear();
        for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
        {
            dataNodeList[ (*itr)->getEntityKey() ] = *itr;
        }        
    }


	TA_Base_Bus::DataPointFactory* ModbusAgent::getDataPointFactory()
	{
		TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");
	       
		return m_dataPointFactory;
	}
		
    void ModbusAgent::registerForStateUpdates()
    {
    }

	
    void ModbusAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
    }
}

