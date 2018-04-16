/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\AtsModBusManager.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling and managing Protocol Handling, Data Notification and Data processing
  */

#include "app/ats/AtsAgent/ModBusProtocolHandler/src/AtsModBusManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/SIGRTUManager.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "boost/algorithm/string.hpp"
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/ModBusProtocolManager.h"
#include "bus/scada/ItaScada/src/CtaScada.h"
#include "core/utilities/src/RunParams.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/ModBusFactory.h"
#include "bus/ats/ats_agent/CommonATS/src/DataProcessorManager.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsDataNotificationManager.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"
#include "bus/ats/ats_agent/CommonATS/src/AgentOperationMode.h"

#include "bus/ats/ats_agent/CommonATS/src/IProtocolManager.h"
#include "bus/ats/ats_agent/CommonATS/src/IAtsDataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsDataNotificationManager.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include <string>
#include <vector>


namespace TA_IRS_App
{
	AtsModBusManager::AtsModBusManager()
		: m_rtuManager(NULL),
		  m_scada(NULL)
	{
		FUNCTION_ENTRY("AtsModBusManager");

		initialize();

		FUNCTION_EXIT;
	}

	AtsModBusManager::~AtsModBusManager()
	{
		FUNCTION_ENTRY("~AtsModBusManager");

		deInitialize();

		FUNCTION_EXIT;
	}

	void AtsModBusManager::deInitialize()
	{
		FUNCTION_ENTRY("deInitialize");

		//For Protocol Parser
		m_protocolManager->deRegisterProtocolParser(TA_IRS_Bus::ePA);
		m_protocolManager->deRegisterProtocolParser(TA_IRS_Bus::ePIDS);
		m_protocolManager->deRegisterProtocolParser(TA_IRS_Bus::eTRAIN);
		m_protocolManager->deRegisterProtocolParser(TA_IRS_Bus::ePIS);

		//For Data Sync
		m_DataProcessorManager->deRegisterDataSynch(TA_IRS_Bus::ePA);

		//For Data Dispatcher
		m_DataProcessorManager->deRegisterDataDispatch(TA_IRS_Bus::ePA);
		m_DataProcessorManager->deRegisterDataDispatch(TA_IRS_Bus::ePIDS);

		//For Ats Data Processors
		m_DataProcessorManager->deRegisterAtsDataProcessor(TA_IRS_Bus::ePA);
		m_DataProcessorManager->deRegisterAtsDataProcessor(TA_IRS_Bus::ePIDS);
		m_DataProcessorManager->deRegisterAtsDataProcessor(TA_IRS_Bus::eTRAIN);
		m_DataProcessorManager->deRegisterAtsDataProcessor(TA_IRS_Bus::ePIS);

		if (NULL != m_rtuManager)
		{
			delete m_rtuManager;
			m_rtuManager = NULL;
		}

		if (NULL != m_scada)
		{
			delete m_scada;
			m_scada = NULL;
		}

		if (NULL != m_DataProcessorManager)
		{
			delete m_DataProcessorManager;
			m_DataProcessorManager = NULL;
		}

		FUNCTION_EXIT;
	}

	void AtsModBusManager::initialize()
	{
		FUNCTION_ENTRY("initialize");

		m_rtuManager       = new SIGRTUManager;
		m_scada            = new TA_Base_Bus::CtaScada;
			
		FUNCTION_EXIT;
	}

	TA_Base_Bus::IEntity* AtsModBusManager::createEntity(TA_Base_Core::IEntityDataPtr entityData)
	{
		FUNCTION_ENTRY("createEntity");

		TA_Base_Bus::IEntity* entityRet = NULL;
		if (TA_Base_Core::RTUEntityData::getStaticType() == entityData->getType())
		{
			entityRet = m_rtuManager->createRTUEntity(entityData);
		}

		if ( TA_Base_Core::DataPointEntityData::getStaticType() == entityData->getType() )
		{
			TA_Base_Core::DataPointEntityDataPtr dataPointEntityData = 
				boost::dynamic_pointer_cast< TA_Base_Core::DataPointEntityData > ( entityData );

			TA_ASSERT(NULL != dataPointEntityData, "Unable to down cast entity data to datapoint entity data.");
			entityRet = m_scada->addDatapoint(entityData);

			TA_Base_Bus::DataPoint* dataPoint = dynamic_cast<TA_Base_Bus::DataPoint*>(entityRet);
			
			if ( NULL != dataPoint )
			{
				//Make sure that only non virtual datapoint is added in a cache
				if (false == dataPoint->isVirtual())
					m_dataPointCache.push_back(dataPoint);
			}
			
		}

		FUNCTION_EXIT;
		return entityRet;
	}

	void AtsModBusManager::initializeData()
	{
		FUNCTION_ENTRY("initializeData");

		initializeRTUManager();
		initializeScada();
		TA_IRS_Bus::DataCache::getInstance()->initialize(m_scada,m_atsEntityData);

		//Register Data Sync
		m_DataProcessorManager->registerDataSynch(TA_IRS_Bus::ePA,ModBusFactory::getInstance()->createDataSynch(TA_IRS_Bus::ePA,m_atsEntityData));

		//Register Data Dispatch
		m_DataProcessorManager->registerDataDispatch(TA_IRS_Bus::ePA,ModBusFactory::getInstance()->createDispatcher(TA_IRS_Bus::ePA));
		m_DataProcessorManager->registerDataDispatch(TA_IRS_Bus::ePIDS,ModBusFactory::getInstance()->createDispatcher(TA_IRS_Bus::ePIDS));
		m_protocolManager->setAtsDataNotification((TA_IRS_Bus::IAtsDataNotification*)m_dataNotification);

		//Register Protocol Parsers
		m_protocolManager->registerProtocolParser(TA_IRS_Bus::ePA, ModBusFactory::getInstance()->createParser(TA_IRS_Bus::ePA,m_atsEntityData));
		m_protocolManager->registerProtocolParser(TA_IRS_Bus::ePIDS,ModBusFactory::getInstance()->createParser(TA_IRS_Bus::ePIDS,m_atsEntityData));
		m_protocolManager->registerProtocolParser(TA_IRS_Bus::eTRAIN,ModBusFactory::getInstance()->createParser(TA_IRS_Bus::eTRAIN,m_atsEntityData));
		m_protocolManager->registerProtocolParser(TA_IRS_Bus::ePIS,ModBusFactory::getInstance()->createParser(TA_IRS_Bus::ePIS,m_atsEntityData));

		//Register for Ats Data Processors
		m_DataProcessorManager->registerAtsDataProcessor(TA_IRS_Bus::ePA, ModBusFactory::getInstance()->createAtsDataProcessor(TA_IRS_Bus::ePA,m_atsEntityData));
		m_DataProcessorManager->registerAtsDataProcessor(TA_IRS_Bus::ePIDS, ModBusFactory::getInstance()->createAtsDataProcessor(TA_IRS_Bus::ePIDS,m_atsEntityData));
		m_DataProcessorManager->registerAtsDataProcessor(TA_IRS_Bus::eTRAIN, ModBusFactory::getInstance()->createAtsDataProcessor(TA_IRS_Bus::eTRAIN,m_atsEntityData));
		m_DataProcessorManager->registerAtsDataProcessor(TA_IRS_Bus::ePIS, ModBusFactory::getInstance()->createAtsDataProcessor(TA_IRS_Bus::ePIS,m_atsEntityData));
		
		FUNCTION_EXIT;
	}

	void AtsModBusManager::initializeRTUManager()
	{
		FUNCTION_ENTRY("initializeRTUManager");

		std::string assetName = m_atsEntityData->getAssetName();
		m_rtuManager->setAgentAssetName(assetName);
		m_rtuManager->createRTUWorkers();
		SIGRTUManager* rtuManager = dynamic_cast<SIGRTUManager*>(m_rtuManager);
		rtuManager->setRTUStatusWorker();
		rtuManager->setPollingScheduler();

		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RTU workers successfully created.");

		// add station systems to rtu manager
		std::vector<TA_Base_Core::IEntityData*> pmsAgentChildrenEntities;
		pmsAgentChildrenEntities = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf (m_atsEntityData->getKey());

		std::vector<TA_Base_Bus::DataPoint*>::iterator iterDatapoint = m_dataPointCache.begin();
		for (iterDatapoint; iterDatapoint != m_dataPointCache.end(); iterDatapoint++)
		{
			//Filter Train and Platform informations. These will not be included in the RTU Manager.
			if (!checkTrainAddressInfo((*iterDatapoint)->getDataPointAddress()))
			{
				m_rtuManager->addDataPoint(*iterDatapoint);
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Add datapoint to RTU Manager, Name : %s",
					(*iterDatapoint)->getDataPointName().c_str());
			}					
		}

		FUNCTION_EXIT;
	}

	bool AtsModBusManager::checkTrainAddressInfo(const std::string& sourceAddress)
	{
		FUNCTION_ENTRY("checkTrainAddressInfo");

		bool reHasAddress = false;

		std::vector<std::string> SearchKeys;
		SearchKeys.push_back(m_atsEntityData->getTrainServiceNumberAddress());
		SearchKeys.push_back(m_atsEntityData->getTrainDirectionAddress());
		SearchKeys.push_back(m_atsEntityData->getStopFlagAddress());
		SearchKeys.push_back(m_atsEntityData->getTrainPisDpAddress());


		//Check if the source has the address name based on the key
		std::vector<std::string>::iterator iterkey = SearchKeys.begin();
		for(iterkey; iterkey != SearchKeys.end(); iterkey++)
		{
			std::string keyAddress = *iterkey;
			boost::to_upper(keyAddress);
			std::size_t found = sourceAddress.find(keyAddress);
			if ( found != std::string::npos )
			{
				//Address found
				reHasAddress = true;
				break;
			}
		}

		FUNCTION_EXIT;
		return reHasAddress;
	}

 	TA_IRS_Bus::IProtocolManager* AtsModBusManager::createProtocolManager()
	{
		FUNCTION_ENTRY("createProtocolManager");

		TA_IRS_Bus::IProtocolManager* protocolManager = NULL;
		protocolManager = new ModBusProtocolManager(m_atsEntityData,(TA_Base_Bus::IProcessDataBlock&)*m_rtuManager);

		FUNCTION_EXIT;
		return protocolManager;
	}

	TA_IRS_Bus::IAtsDataNotification* AtsModBusManager::createDataNotification()
	{
		FUNCTION_ENTRY("createDataNotification");

		TA_IRS_Bus::IAtsDataNotification* dataNotification = NULL;
		dataNotification = new TA_IRS_Bus::AtsDataNotificationManager((TA_IRS_Bus::IDataUpdate*)m_DataProcessorManager);

		FUNCTION_EXIT;
		return dataNotification;
	}

	TA_IRS_Bus::IDataProcessor* AtsModBusManager::createDataProcessor()
	{
		FUNCTION_ENTRY("createDataProcessor");

		TA_IRS_Bus::IDataProcessor* dataProcessor = NULL;
		dataProcessor      = new TA_IRS_Bus::DataProcessorManager(m_atsEntityData);

		FUNCTION_EXIT;
		return dataProcessor;
	}

	void AtsModBusManager::setOperationMode(TA_Base_Core::EOperationMode eMode)
	{
		FUNCTION_ENTRY("setOperationMode");

		if (TA_Base_Core::Control == eMode)
		{
			m_scada->controlDatapoints(true);
			m_rtuManager->setToControl();
			TA_IRS_Bus::AgentOperationMode::getInstance().setToControlMode();
		}
		else if (TA_Base_Core::Monitor == eMode)
		{
			m_rtuManager->setToMonitor();
			m_scada->controlDatapoints(false);
			TA_IRS_Bus::AgentOperationMode::getInstance().setToMonitorMode();
		}

		m_protocolManager->setOperationMode(eMode);
		TA_IRS_Bus::DataCache::getInstance()->setOperationMode(eMode);

		FUNCTION_EXIT;
	}

	void AtsModBusManager::initializeScada()
	{
		FUNCTION_ENTRY("initializeScada");

		std::string entityname = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
		m_scada->setupScadaInterface(entityname);

		FUNCTION_EXIT;
	}
}
