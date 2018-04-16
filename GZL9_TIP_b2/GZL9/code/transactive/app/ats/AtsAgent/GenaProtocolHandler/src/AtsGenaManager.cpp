
#include "app/ats/AtsAgent/GenaProtocolHandler/src/AtsGenaManager.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaFactory.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaProtocolManager.h"


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include "bus/scada/ItaScada/src/CtaScada.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/DataProcessorManager.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsDataNotificationManager.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"
#include "bus/ats/ats_agent/CommonATS/src/AgentOperationMode.h"
#include "bus/ats/ats_agent/CommonATS/src/IProtocolManager.h"
#include "bus/ats/ats_agent/CommonATS/src/IAtsDataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataProcessor.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsDataNotificationManager.h"
#include "bus/protocol/Gena/src/IGENATopicDataStore.h"


#include "boost/algorithm/string.hpp"

#include <string>
#include <vector>


namespace TA_IRS_App
{
	AtsGenaManager::AtsGenaManager()
		: m_scada(NULL)
	{
		FUNCTION_ENTRY("AtsModBusManager");

		initialize();

		FUNCTION_EXIT;
	}

	AtsGenaManager::~AtsGenaManager()
	{
		FUNCTION_ENTRY("~AtsModBusManager");

		deInitialize();

		FUNCTION_EXIT;
	}

	void AtsGenaManager::deInitialize()
	{
		FUNCTION_ENTRY("deInitialize");

		//For Protocol Parser
		m_protocolManager->deRegisterProtocolParser(TA_IRS_Bus::eTRAIN);
		m_protocolManager->deRegisterProtocolParser(TA_IRS_Bus::ePA);
		m_protocolManager->deRegisterProtocolParser(TA_IRS_Bus::eSIG);
		m_protocolManager->deRegisterProtocolParser(TA_IRS_Bus::eBLK);
		m_protocolManager->deRegisterProtocolParser(TA_IRS_Bus::eESB);
		m_protocolManager->deRegisterProtocolParser(TA_IRS_Bus::eFG);
		m_protocolManager->deRegisterProtocolParser(TA_IRS_Bus::eSW);
		m_protocolManager->deRegisterProtocolParser(TA_IRS_Bus::ePIS);
		m_protocolManager->deRegisterProtocolParser(TA_IRS_Bus::ePlatform);
		m_protocolManager->deRegisterProtocolParser(TA_IRS_Bus::ePlatformDoor);

		//For Ats Data Processors
		m_DataProcessorManager->deRegisterAtsDataProcessor(TA_IRS_Bus::eTRAIN);
		m_DataProcessorManager->deRegisterAtsDataProcessor(TA_IRS_Bus::ePA);
		m_DataProcessorManager->deRegisterAtsDataProcessor(TA_IRS_Bus::eSIG);
		m_DataProcessorManager->deRegisterAtsDataProcessor(TA_IRS_Bus::eBLK);
		m_DataProcessorManager->deRegisterAtsDataProcessor(TA_IRS_Bus::eESB);
		m_DataProcessorManager->deRegisterAtsDataProcessor(TA_IRS_Bus::eFG);
		m_DataProcessorManager->deRegisterAtsDataProcessor(TA_IRS_Bus::eSW);
		m_DataProcessorManager->deRegisterAtsDataProcessor(TA_IRS_Bus::ePIS);
		m_DataProcessorManager->deRegisterAtsDataProcessor(TA_IRS_Bus::ePlatform);
		m_DataProcessorManager->deRegisterAtsDataProcessor(TA_IRS_Bus::ePlatformDoor);

		//For Data Dispatcher
		m_DataProcessorManager->deRegisterDataDispatch(TA_IRS_Bus::ePA);

		//For Data Synch
		m_DataProcessorManager->deRegisterDataSynch(TA_IRS_Bus::ePA);
		
		TA_Base_Bus::GENALibrary::getInstance().getClientInstance()->deRegisterConnStatusObserver(dynamic_cast<GenaProtocolManager*> (m_protocolManager));
		TA_Base_Bus::GENALibrary::getInstance().getClientInstance()->deRegisterDataStoreTopicObserver(dynamic_cast<GenaProtocolManager*> (m_protocolManager));
		TA_Base_Bus::GENALibrary::getInstance().releaseInstance();
	
		if (NULL != m_DataProcessorManager)
		{
			delete m_DataProcessorManager;
			m_DataProcessorManager = NULL;
		}

		if (NULL != m_scada)
		{
			delete m_scada;
			m_scada = NULL;
		}

		if(NULL != m_pPowerStatusDataStore)
		{
			delete m_pPowerStatusDataStore;
			m_pPowerStatusDataStore = NULL;
		}

		FUNCTION_EXIT;
	}

	void AtsGenaManager::initialize()
	{
		FUNCTION_ENTRY("initialize");

		//create power status data store
		m_pPowerStatusDataStore	= new PowerStatusDataStore;
		
		//create CtaScada
		m_scada	= new TA_Base_Bus::CtaScada;
			
		FUNCTION_EXIT;
	}

	TA_Base_Bus::IEntity* AtsGenaManager::createEntity(TA_Base_Core::IEntityDataPtr entityData)
	{
		FUNCTION_ENTRY("createEntity");

		TA_Base_Bus::IEntity* entityRet = NULL;

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
		return NULL;
	}

    void AtsGenaManager::addAllDataPoints(std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList)
    {
        FUNCTION_ENTRY("AtsGenaManager::addAllDataPoints");
        
        m_scada->addAllDatapoints(dataPointList);
        
        std::map < unsigned long, TA_Base_Bus::DataPoint *>::iterator it;
        for (it = dataPointList.begin(); it != dataPointList.end(); it++)
        {
            TA_Base_Bus::DataPoint * ptrDp = it->second;
            if (false == ptrDp->isVirtual())
            {
                m_dataPointCache.push_back(ptrDp);
            }
        }
        
        FUNCTION_EXIT("AtsGenaManager::addAllDataPoints");
    }
    
	void AtsGenaManager::initializeData()
	{
		FUNCTION_ENTRY("initializeData");

		//Initialize Gena library
		TA_Base_Bus::GENALibrary::getInstance().initialize();
		
		//Gena Server register Power Section Status Data Store
		TA_Base_Bus::GENALibrary::getInstance().getServerInstance()->registerDataStore(m_pPowerStatusDataStore);

		//Gena Client register Protocol Manager for updating notification
		TA_Base_Bus::GENALibrary::getInstance().getClientInstance()->registerDataStoreTopicObserver(dynamic_cast<GenaProtocolManager*> (m_protocolManager));
		//Gena Client register Protocol Manager for server status observer
		TA_Base_Bus::GENALibrary::getInstance().getClientInstance()->registerConnStatusObserver(dynamic_cast<GenaProtocolManager*> (m_protocolManager));

		//Initialize CtaScada
		initializeScada();

		//Initialize DataCache for power, train location, PA, PIS, Signal, Block, Esb, Switch, Floodgate data
		TA_IRS_Bus::DataCache::getInstance()->initialize(m_scada,m_atsEntityData, TA_IRS_Bus::eTrainIDSingle);
		TA_IRS_Bus::DataCache::getInstance()->registerPowerStatusObserver(m_pPowerStatusDataStore);

		m_protocolManager->setAtsDataNotification((TA_IRS_Bus::IAtsDataNotification*)m_dataNotification);
		//Register Protocol Parsers
		m_protocolManager->registerProtocolParser(TA_IRS_Bus::eTRAIN,GenaFactory::getInstance()->createParser(TA_IRS_Bus::eTRAIN));
		m_protocolManager->registerProtocolParser(TA_IRS_Bus::ePA,GenaFactory::getInstance()->createParser(TA_IRS_Bus::ePA));
		m_protocolManager->registerProtocolParser(TA_IRS_Bus::eSIG,GenaFactory::getInstance()->createParser(TA_IRS_Bus::eSIG));
		m_protocolManager->registerProtocolParser(TA_IRS_Bus::eBLK,GenaFactory::getInstance()->createParser(TA_IRS_Bus::eBLK));
		m_protocolManager->registerProtocolParser(TA_IRS_Bus::eESB,GenaFactory::getInstance()->createParser(TA_IRS_Bus::eESB));
		m_protocolManager->registerProtocolParser(TA_IRS_Bus::eSW,GenaFactory::getInstance()->createParser(TA_IRS_Bus::eSW));
		m_protocolManager->registerProtocolParser(TA_IRS_Bus::eFG,GenaFactory::getInstance()->createParser(TA_IRS_Bus::eFG));
		m_protocolManager->registerProtocolParser(TA_IRS_Bus::ePIS,GenaFactory::getInstance()->createParser(TA_IRS_Bus::ePIS));
		m_protocolManager->registerProtocolParser(TA_IRS_Bus::ePlatform,GenaFactory::getInstance()->createParser(TA_IRS_Bus::ePlatform));
		m_protocolManager->registerProtocolParser(TA_IRS_Bus::ePlatformDoor,GenaFactory::getInstance()->createParser(TA_IRS_Bus::ePlatformDoor));
		//Register for Ats Data Processors
		m_DataProcessorManager->registerAtsDataProcessor(TA_IRS_Bus::eTRAIN, GenaFactory::getInstance()->createAtsDataProcessor(TA_IRS_Bus::eTRAIN,m_atsEntityData));
		m_DataProcessorManager->registerAtsDataProcessor(TA_IRS_Bus::ePA, GenaFactory::getInstance()->createAtsDataProcessor(TA_IRS_Bus::ePA,m_atsEntityData));
		m_DataProcessorManager->registerAtsDataProcessor(TA_IRS_Bus::eSIG, GenaFactory::getInstance()->createAtsDataProcessor(TA_IRS_Bus::eSIG,m_atsEntityData));
		m_DataProcessorManager->registerAtsDataProcessor(TA_IRS_Bus::eBLK, GenaFactory::getInstance()->createAtsDataProcessor(TA_IRS_Bus::eBLK,m_atsEntityData));
		m_DataProcessorManager->registerAtsDataProcessor(TA_IRS_Bus::eESB, GenaFactory::getInstance()->createAtsDataProcessor(TA_IRS_Bus::eESB,m_atsEntityData));
		m_DataProcessorManager->registerAtsDataProcessor(TA_IRS_Bus::eSW, GenaFactory::getInstance()->createAtsDataProcessor(TA_IRS_Bus::eSW,m_atsEntityData));
		m_DataProcessorManager->registerAtsDataProcessor(TA_IRS_Bus::eFG, GenaFactory::getInstance()->createAtsDataProcessor(TA_IRS_Bus::eFG,m_atsEntityData));
		m_DataProcessorManager->registerAtsDataProcessor(TA_IRS_Bus::ePIS, GenaFactory::getInstance()->createAtsDataProcessor(TA_IRS_Bus::ePIS,m_atsEntityData));
		m_DataProcessorManager->registerAtsDataProcessor(TA_IRS_Bus::ePlatform, GenaFactory::getInstance()->createAtsDataProcessor(TA_IRS_Bus::ePlatform,m_atsEntityData));
		m_DataProcessorManager->registerAtsDataProcessor(TA_IRS_Bus::ePlatformDoor, GenaFactory::getInstance()->createAtsDataProcessor(TA_IRS_Bus::ePlatformDoor,m_atsEntityData));
		//Register Data Sync
		m_DataProcessorManager->registerDataSynch(TA_IRS_Bus::ePA,GenaFactory::getInstance()->createDataSynch(TA_IRS_Bus::ePA,m_atsEntityData));
		//Register Data Dispatch
		m_DataProcessorManager->registerDataDispatch(TA_IRS_Bus::ePA, GenaFactory::getInstance()->createDispatcher(TA_IRS_Bus::ePA));
		
		FUNCTION_EXIT;
	}

 	TA_IRS_Bus::IProtocolManager* AtsGenaManager::createProtocolManager()
	{
		FUNCTION_ENTRY("createProtocolManager");

		TA_IRS_Bus::IProtocolManager* protocolManager = NULL;
		protocolManager = new GenaProtocolManager(m_scada);

		FUNCTION_EXIT;
		return protocolManager;
	}

	TA_IRS_Bus::IAtsDataNotification* AtsGenaManager::createDataNotification()
	{
		FUNCTION_ENTRY("createDataNotification");

		TA_IRS_Bus::IAtsDataNotification* dataNotification = NULL;
		dataNotification = new TA_IRS_Bus::AtsDataNotificationManager((TA_IRS_Bus::IDataUpdate*)m_DataProcessorManager);

		FUNCTION_EXIT;
		return dataNotification;
	}

	TA_IRS_Bus::IDataProcessor* AtsGenaManager::createDataProcessor()
	{
		FUNCTION_ENTRY("createDataProcessor");

		TA_IRS_Bus::IDataProcessor* dataProcessor = NULL;
		dataProcessor      = new TA_IRS_Bus::DataProcessorManager(m_atsEntityData);

		FUNCTION_EXIT;
		return dataProcessor;
	}

	void AtsGenaManager::setOperationMode(TA_Base_Core::EOperationMode eMode)
	{
		FUNCTION_ENTRY("setOperationMode");

		if (TA_Base_Core::Control == eMode)
		{
			m_scada->controlDatapoints(true);
			TA_IRS_Bus::AgentOperationMode::getInstance().setToControlMode();
			//Start Gena Service
			TA_Base_Bus::GENALibrary::getInstance().startService();
			m_protocolManager->setOperationMode(eMode);
			TA_IRS_Bus::DataCache::getInstance()->setOperationMode(eMode);

		}
		else if (TA_Base_Core::Monitor == eMode)
		{			
			
			TA_Base_Bus::GENALibrary::getInstance().stopService();
			m_protocolManager->setOperationMode(eMode);
			TA_IRS_Bus::DataCache::getInstance()->setOperationMode(eMode);

			m_scada->controlDatapoints(false);
			TA_IRS_Bus::AgentOperationMode::getInstance().setToMonitorMode();
			//Stop Gena Service
		}
		

		FUNCTION_EXIT;
	}

	void AtsGenaManager::initializeScada()
	{
		FUNCTION_ENTRY("initializeScada");

		// std::string entityname = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
		// m_scada->setupScadaInterface(entityname);

		FUNCTION_EXIT;
	}
}