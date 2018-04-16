/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/AbstractPAAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #18 $
  *
  * Last modification: $DateTime: 2017/12/14 09:52:30 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include "ace/Guard_T.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/DataPointStateUpdate_MessageTypes.h"
#include "core/message/types/PAAgentAlarms_MessageTypes.h"
#include "core/message/types/PAAgentAudit_MessageTypes.h"
#include "core/message/types/PAAgentStateUpdate_MessageTypes.h"
#include "core/message/types/PAAgentStateUpdateRequest_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/data_access_interface/entity_access/src/AbstractPAAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/PaFireCountdownAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/PaSysParamAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/PaPagingConsoleAccessFactory.h"
#include "core/threads/src/ThreadMemberFunction.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/threads/src/Thread.h"

#include "bus/generic_agent/src/IEntity.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/StationBroadcastProxy.h"
#include "app/pa/PAAgent/src/TrainBroadcastProxy.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasFireAlarmManager.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/PasPollingManager.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"
#include "app/pa/PAAgent/src/DvaMsgLib.h"
#include "app/pa/PAAgent/src/PasZonesState.h"
#include "app/pa/PAAgent/src/ScheduleDvaIdSupplier.h"
#include "app/pa/PAAgent/src/PasFaultStatus.h"
#include "app/pa/PAAgent/src/DataPointQualityController.h"

#include "app/pa/PAAgent/src/AbstractPAAgent.h"

typedef TA_Base_Core::ThreadedMemberFunction<TA_IRS_App::AbstractPAAgent> AbstractPAAgentThreadedMemberFunction;

namespace TA_IRS_App
{
//TD17452
    const std::string AbstractPAAgent::MONITORED_PROCESS_NAME_APPEND = "MonitoredProcess";
    const std::string AbstractPAAgent::AGENT_NAME_APPEND = "Agent";
//TD17452

//
// Constructor
//
// ExceptionChecked
AbstractPAAgent::AbstractPAAgent()
: m_genericAgent( NULL ),
  m_operationMode( TA_Base_Core::NotApplicable ),
  m_setupHasBeenCalled( false ),
  m_cleanupHasBeenCalled( false ),
  m_stateUpdateRequestSender( NULL ),
  m_stateUpdateSender( NULL ),
  m_pBroadcastManager( NULL ),
  m_pStnBroadcastProxy( NULL ),
  m_pTrnBroadcastProxy( NULL ),
  m_pPasConnectionAgency( NULL ),
  m_pPasPollingManager( NULL ),
  m_pDvaMsgLib( NULL ),
  m_pPasFireAlarmManager( NULL ),
  m_pPasFaultStatus( NULL ),
  m_pDataPointQualityController( NULL ),
  m_hasReceivedAndProcessedEntireAgentStateUpdate( false ),
  m_dpBulkPolledInterface ( NULL ),
  m_dpAccessInterface( NULL ),
  m_dnBulkPolledInterface( NULL ),
  m_dnAccessInterface( NULL ),
  m_scadaProxyFactory( NULL ),
  m_summaryProcessor( NULL ),	
  m_dataPointFactory( NULL )
{
    FUNCTION_ENTRY("Constructor");

	// get a handle to the ScadaProxyFactory
	m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
    m_dataPointFactory = new TA_Base_Bus::DataPointFactory(this);
	// set the proxies to monitor mode
	m_scadaProxyFactory->setProxiesToMonitorMode();

    FUNCTION_EXIT;
}


//
// Destructor
//
// ExceptionChecked
AbstractPAAgent::~AbstractPAAgent()
{
    FUNCTION_ENTRY("Destructor");

	if (!m_cleanupHasBeenCalled)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::cleanup() must be called from the derived classes" );
	}

    FUNCTION_EXIT;
}

void AbstractPAAgent::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
{
    // stop multi threads accessing the list of DataPoint objects at the same time
    ACE_Guard<ACE_RW_Mutex> guard( m_mtxDataPointListLock );

	dataPointList.clear();

	std::vector< TA_Base_Bus::DataPoint* >::iterator itr;
	for(itr = m_dataPoints.begin(); itr != m_dataPoints.end(); itr++)
	{
		dataPointList[ (*itr)->getEntityKey() ] = *itr;
	}
}

// ExceptionChecked
void AbstractPAAgent::setup(int argc, char* argv[])
    //throw(TA_Base_Core::GenericAgentException, TA_Base_Core::DatabaseException, TA_Base_Core::DataException)
{
    FUNCTION_ENTRY("setup");

    // This flag forces clients to call this method after base class construction and before
    // anything else.
    m_setupHasBeenCalled = true;

    try
    {
        if ( NULL == m_genericAgent )
        {
            m_genericAgent = new TA_Base_Bus::GenericAgent(argc, argv, this);
        }

        if ( NULL == m_stateUpdateRequestSender )
        {
            m_stateUpdateRequestSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender( TA_Base_Core::PAAgentStateUpdateRequest::Context );
        }

        if ( NULL == m_stateUpdateSender )
        {
            m_stateUpdateSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender( TA_Base_Core::PAAgentStateUpdate::Context );
        }

        TA_IRS_Bus::TrainAgentProxyFactory::getInstance().agentInitialise(m_genericAgent->getAgentEntityConfiguration());

        try
        {
            processConfiguration();
        }
		catch (TA_Base_Core::DataException& dae)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", dae.what());
		}
		catch (TA_Base_Core::DatabaseException& dbe)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", dbe.what());
		}
		catch (TA_Base_Core::InvalidPasConfigurationException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "InvalidPasConfigurationException", e.what());
		}
		catch (...)
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", " processConfiguration()-- Exception throw while process the configuration.");
        }

        initialiseAndValidateCachedMaps();

        m_pPasConnectionAgency = new PasConnectionAgency();
        m_pPasPollingManager = new PasPollingManager( *m_pPasConnectionAgency );
        PasTransactionManager::createInstance( *m_pPasConnectionAgency );
        m_pDvaMsgLib = new DvaMsgLib();
        m_pPasFireAlarmManager = new PasFireAlarmManager();
        m_pPasFaultStatus = new PasFaultStatus();
        PasZonesState::createInstance();
        ScheduleDvaIdSupplier::createInstance();
        m_pBroadcastManager = new BroadcastManager();
        m_pStnBroadcastProxy = new StationBroadcastProxy( *m_pBroadcastManager );
        m_pTrnBroadcastProxy = new TrainBroadcastProxy( *m_pBroadcastManager );

        m_pDataPointQualityController = new DataPointQualityController( m_mtxDataPointListLock, m_dataPoints );

        m_pPasConnectionAgency->registerConnectionStatusObserver( m_pBroadcastManager );
        m_pPasConnectionAgency->registerConnectionStatusObserver( m_pDataPointQualityController );
        m_pPasConnectionAgency->start();
        PasTransactionManager::getInstance().start();

        m_pPasPollingManager->registerPollingObserver( *m_pDvaMsgLib, 
            PAS_POLLING_DVA_LIB_COUNTER, PAS_POLLING_DVA_LIB_COUNTER );
        if ( CachedConfig::getInstance()->getIsMaster() )
        {
            m_pPasPollingManager->registerPollingObserver( *m_pPasFireAlarmManager, 
                PAS_POLLING_OCC_FIREALARM_START, PAS_POLLING_OCC_FIREALARM_END );
            m_pPasPollingManager->registerPollingObserver( *m_pPasFaultStatus,
                PAS_POLLING_SYSTEM_FAULT_START, PAS_POLLING_SYSTEM_FAULT_END );
        }
        else
        {
            m_pPasPollingManager->registerPollingObserver( *m_pPasFireAlarmManager, 
                PAS_POLLING_STN_FIREALARM, PAS_POLLING_STN_FIREALARM );
        }
        m_pPasPollingManager->registerPollingObserver( PasZonesState::getInstance(), 
            PAS_POLLING_ZONE_STATE_COUNTER, PAS_POLLING_ZONE_STATE_COUNTER );
        m_pPasPollingManager->registerPollingObserver( *m_pPasFaultStatus, 
            PAS_POLLING_FAULT_ZONE_COUNTER, PAS_POLLING_FAULT_ZONE_COUNTER );
        m_pPasPollingManager->registerPollingObserver( *m_pPasFaultStatus, 
            PAS_POLLING_SYSTEM_FAULT, PAS_POLLING_SYSTEM_FAULT );

        createSpecificComponents();

		// register the local entities into ScadaProxyFactory
		m_scadaProxyFactory->registerLocalEntities(m_entityData->getAgentName() , m_genericAgent->getEntityMap());
		// create SummaryProcessor
		m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());	

        refreshPasHardwareParameters();
    }
    catch( const TA_Base_Core::InvalidPasConfigurationException& e )
    {
        PasHelpers::getInstance()->raiseInvalidPAConfigurationAlarm(e.what());

        cleanup();
        // prevents run() being called by main
        TA_THROW(TA_Base_Core::GenericAgentException(TA_Base_Core::GenericAgentException::CONFIGURATION_ERROR, e.what()));
    }
    catch(...)
    {
        cleanup();
        throw;
    }

    FUNCTION_EXIT;
}


// ExceptionChecked
void AbstractPAAgent::cleanup()
{
    FUNCTION_ENTRY("cleanup");

    // This flag forces clients to call this method before base class destruction.
    m_cleanupHasBeenCalled = true;

    // Destruction order is important here...

    destroySpecificComponents();

    // Unsubscribe to all messages (if not already).  Important to do this early before we start
    // deleting all the components (in particular the BroadcastManager).
    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
            dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this));

    if ( NULL != m_pPasConnectionAgency )
    {
        m_pPasConnectionAgency->clearObservers();
        // Reference by others, should be deleted at the end
    }

    if ( NULL != m_pDataPointQualityController )
    {
        delete m_pDataPointQualityController;
        m_pDataPointQualityController = NULL;
    }

    if ( NULL != m_pPasPollingManager )
    {
        try
        {
            m_pPasPollingManager->deregisterPollingObserver( PasZonesState::getInstance(), 
                PAS_POLLING_ZONE_STATE_COUNTER, PAS_POLLING_ZONE_STATE_COUNTER );
        }
        catch (...)
        {
        	// Ignore at exit
        }

        if ( NULL != m_pPasFaultStatus )
        {
            m_pPasPollingManager->deregisterPollingObserver( *m_pPasFaultStatus, 
                PAS_POLLING_FAULT_ZONE_COUNTER, PAS_POLLING_FAULT_ZONE_COUNTER );
        }


        if ( NULL != m_pPasFireAlarmManager )
        {
            if ( CachedConfig::getInstance()->getIsMaster() )
            {
                m_pPasPollingManager->deregisterPollingObserver( *m_pPasFireAlarmManager, 
                    PAS_POLLING_OCC_FIREALARM_START, PAS_POLLING_OCC_FIREALARM_END );
            }
            else
            {
                m_pPasPollingManager->deregisterPollingObserver( *m_pPasFireAlarmManager, 
                    PAS_POLLING_STN_FIREALARM, PAS_POLLING_STN_FIREALARM );
            }
        }

        if ( NULL != m_pDvaMsgLib )
        {
            m_pPasPollingManager->deregisterPollingObserver( *m_pDvaMsgLib, 
                PAS_POLLING_DVA_LIB_COUNTER, PAS_POLLING_DVA_LIB_COUNTER );
        }
    }

    if ( NULL != m_pBroadcastManager )
    {
        delete m_pBroadcastManager;
        m_pBroadcastManager = NULL;
    }

    ScheduleDvaIdSupplier::destoryInstance();
    PasZonesState::destoryInstance();

    if ( NULL != m_pPasFaultStatus )
    {
        delete m_pPasFaultStatus;
        m_pPasFaultStatus = NULL;
    }

    if ( NULL != m_pPasFireAlarmManager )
    {
        delete m_pPasFireAlarmManager;
        m_pDvaMsgLib = NULL;
    }

    if ( NULL != m_pDvaMsgLib )
    {
        delete m_pDvaMsgLib;
        m_pDvaMsgLib = NULL;
    }

    if ( NULL != m_pPasPollingManager )
    {
        delete m_pPasPollingManager;
        m_pPasPollingManager = NULL;
    }

    PasTransactionManager::destoryInstance();

    if ( NULL != m_pPasConnectionAgency )
    {
        delete m_pPasConnectionAgency;
        m_pPasConnectionAgency = NULL;
    }

    if ( NULL != m_pTrnBroadcastProxy )
    {
        delete m_pTrnBroadcastProxy;
        m_pTrnBroadcastProxy = NULL;
    }

    if ( NULL != m_pStnBroadcastProxy )
    {
        delete m_pStnBroadcastProxy;
        m_pStnBroadcastProxy = NULL;
    }

    // Deactivate the DataPoints' servant.  The GenericAgent
    // will delete the objects from memory space
    m_dataPoints.clear();

    if( m_stateUpdateSender )
    {
        delete m_stateUpdateSender;
        m_stateUpdateSender = NULL;
    }

    if( m_stateUpdateRequestSender )
    {
        delete m_stateUpdateRequestSender;
        m_stateUpdateRequestSender = NULL;
    }

    // Refer to Wierd Stuff 2 in main.cpp.
    // Remove the singletons here coz some of them can cause access violations if the generic agent
    // instance is removed first.  This is caused by dangling pointers after the event consumers
    // and event suppliers are deleted by the generic agent and subsubsequently referenced by the
    // singleton classes.
    //
    // Classes exhibiting this behavior include CachedMaps (due to the collection of DataPointProxy*)
    // and AlarmHelperManager.

    PasHelpers::removeInstance();
    CachedConfig::removeInstance();
    TA_IRS_Bus::CachedMaps::removeInstance();
    TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

    TA_IRS_Bus::TrainAgentProxyFactorySingleton::close();

    // if bulk interface is created
    if ( NULL != m_dpBulkPolledInterface )
    {
        delete m_dpBulkPolledInterface;
        m_dpBulkPolledInterface = NULL;
    }

    if ( NULL != m_dpAccessInterface )
    {
        delete m_dpAccessInterface;
        m_dpAccessInterface = NULL;
    }

    if ( NULL != m_dnBulkPolledInterface )
    {
        delete m_dnBulkPolledInterface;
        m_dnBulkPolledInterface = NULL;
    }

    if ( NULL != m_dnAccessInterface )
    {
        delete m_dnAccessInterface;
        m_dnAccessInterface = NULL;
    }

    if ( NULL != m_scadaProxyFactory )
    {
        // set the proxies to monitor mode
        m_scadaProxyFactory->setProxiesToMonitorMode();
    }

    // clean up handle to SummaryProcessor
    if ( NULL != m_summaryProcessor )
    {
        TA_Base_Bus::SummaryProcessor::removeInstance(m_summaryProcessor);
        m_summaryProcessor = NULL;
    }

    // remove handle to ScadaProxyFactory
    if ( NULL != m_scadaProxyFactory )
    {
        m_scadaProxyFactory = NULL;
        TA_Base_Bus::ScadaProxyFactory::removeInstance();
    }

    // The generic agent must not be deleted before the audit sender and DataPointProxy* as the
    // generic agent destructor cleans up all the StructuredEventSuppliers created for all message
    // senders.

    if( m_genericAgent)
    {
        m_genericAgent->deactivateAndDeleteServant();
        m_genericAgent = NULL;
    }

    FUNCTION_EXIT;
}

// ExceptionChecked
void AbstractPAAgent::processConfiguration()
    //throw(TA_Base_Core::InvalidPasConfigurationException, TA_Base_Core::DatabaseException, TA_Base_Core::DataException)
{
    FUNCTION_ENTRY("processConfiguration");

	if (!m_setupHasBeenCalled)
	{
	    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::setup() must be called from the derived classes" );
	    return;
	}
	if (NULL == m_genericAgent)
	{
	    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_genericAgent is NULL" );
	    return;
	}

    m_entityData = m_genericAgent->getAgentEntityConfiguration();

    if (NULL == m_entityData.get())
    {
        TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Unrecognised EntityData"));
    }

	std::string agentName = m_entityData->getName();
	
	int pos = agentName.find_last_of(AGENT_NAME_APPEND);
	if (pos != -1)
	{
		int stringLength = pos + 1 - AGENT_NAME_APPEND.length();
		agentName = agentName.substr(0,stringLength);
    }
	std::string monitoredProcessName = agentName + MONITORED_PROCESS_NAME_APPEND;
	
	try
	{
		TA_Base_Core::IEntityData *entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(monitoredProcessName);

		if (entityData != NULL)
		{
			CachedConfig::getInstance()->setMonitorProcessKey(entityData->getKey());
		}

		delete entityData;
		entityData = NULL;
	}
	catch(...)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"No data found for monitor process %s", monitoredProcessName.c_str() );
	}

    CachedConfig::getInstance()->setAgentName(m_entityData->getAgentName());
    CachedConfig::getInstance()->setAgentKey(m_entityData->getKey());

    // liuyu++ to closeInvalidPAConfigurationAlarm need to set agent location key first

    CachedConfig::getInstance()->setAgentTypeKey(m_entityData->getTypeKey());
    CachedConfig::getInstance()->setAgentLocationKey(m_entityData->getLocation());
    CachedConfig::getInstance()->setAgentSubsystemKey(m_entityData->getSubsystem());

    // We want to close the InvalidPasConfigurationException as soon as we can send another.  Please refer
    // to comments in the Constructor.
    PasHelpers::getInstance()->closeInvalidPAConfigurationAlarm();
    // ++liuyu

    if (!TA_Base_Core::RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ))
    {
        // Assumed to be set by Process Management.  Just in case, we set this for rights libraries
        // to prevent asserts.
        std::stringstream stmLocationKey;
        stmLocationKey << m_entityData->getLocation();
        TA_Base_Core::RunParams::getInstance().set( RPARAM_LOCATIONKEY, stmLocationKey.str().c_str() );
    }

    TA_Base_Core::AbstractPAAgentEntityDataPtr commonPAAgentEntityData = 
        boost::dynamic_pointer_cast<TA_Base_Core::AbstractPAAgentEntityData>( m_genericAgent->getAgentEntityConfiguration() );

    if (NULL==commonPAAgentEntityData.get())
    {
        TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Entity is not configured as a PAAgent entity type"));
    }

    CachedConfig::getInstance()->setAgentAssetName(commonPAAgentEntityData->getAssetName());
    CachedConfig::getInstance()->setFastPollRateInMSecs(commonPAAgentEntityData->getFastPollRateInMSecs());
    CachedConfig::getInstance()->setWorkerThreadsSleepTimeInMSecs(commonPAAgentEntityData->getWorkerThreadsSleepTimeInMSecs());
    CachedConfig::getInstance()->setSocketTimeoutInSecs(commonPAAgentEntityData->getSocketTimeoutInSecs());
    CachedConfig::getInstance()->setTisDisplayDurationInSecs(commonPAAgentEntityData->getTisDisplayDurationInSecs());
    CachedConfig::getInstance()->setKeepTerminatedBroadcastsInSecs(commonPAAgentEntityData->getKeepTerminatedBroadcastsInSecs());
    CachedConfig::getInstance()->setMaxBroadcasts(commonPAAgentEntityData->getMaxBroadcasts());
    CachedConfig::getInstance()->setRetries(commonPAAgentEntityData->getRetries());
    CachedConfig::getInstance()->setPrimaryFEPCAddress(commonPAAgentEntityData->getPrimaryFEPCAddress());
    CachedConfig::getInstance()->setSecondaryFEPCAddress(commonPAAgentEntityData->getSecondaryFEPCAddress());
    CachedConfig::getInstance()->setPrimaryFEPCPort(commonPAAgentEntityData->getPrimaryFEPCPort());
    CachedConfig::getInstance()->setSecondaryFEPCPort(commonPAAgentEntityData->getSecondaryFEPCPort());
	CachedConfig::getInstance()->setAgentAssetName(commonPAAgentEntityData->getAssetName());
    CachedConfig::getInstance()->setFireAlarmPollingIntervalInMillisecond(commonPAAgentEntityData->getFireAlarmPollingIntervalInMillisecond());
    CachedConfig::getInstance()->setPasZonePollingIntervalInMillisecond(commonPAAgentEntityData->getPasZonePollingIntervalInMillisecond());
    CachedConfig::getInstance()->setSystemFaultPollingIntervalInMillisecond(commonPAAgentEntityData->getSystemFaultPollingIntervalInMillisecond());
    CachedConfig::getInstance()->setDvaLibPollingIntervalInMillisecond(commonPAAgentEntityData->getDvaLibPollingIntervalInMillisecond());
    CachedConfig::getInstance()->setPasDvaRecordingMaxDurationInSecond(commonPAAgentEntityData->getPasDvaRecordingMaxDurationInSecond());

    // Determine special location flags:

    const unsigned long ulOccLocationKey = TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey();
    CachedConfig::getInstance()->setOccLocationKey( ulOccLocationKey );

    TA_Base_Core::ILocation::ELocationType eLocationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(m_entityData->getLocation());

	if ( TA_Base_Core::ILocation::OCC == eLocationType )
    {
        CachedConfig::getInstance()->setIsMaster(true);
        CachedConfig::getInstance()->setIsDepot(false);
    }
    else
    {
        // Station or Depot
        CachedConfig::getInstance()->setIsMaster(false);

		if ( TA_Base_Core::ILocation::DPT == eLocationType )
        {
            // Depot
            CachedConfig::getInstance()->setIsDepot(true);
        }
        else
        {
            // Standard station
            CachedConfig::getInstance()->setIsDepot(false);
        }
    }


    // Note the following can raise the TA_Base_Core::DatabaseException or TA_Base_Core::DataException.

    TA_Base_Core::IEntityDataList allEntities;

    // Locate the train agent key for subscribing to messages
    allEntities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( 
        TA_Base_Core::TrainAgentEntityData::getStaticType(), m_entityData->getLocation() );

    if ( allEntities.size() != 1 )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Train agent is not configured, Train broadcasts not possible" );
    }
    else
    {
        CachedConfig::getInstance()->setLocalTrainAgentKey(allEntities[0]->getKey());
        CachedConfig::getInstance()->setLocalTrainAgentEntityName(allEntities[0]->getName());
    }

    TA_Base_Core::IEntityDataList::iterator it;
    for (TA_Base_Core::IEntityDataList::iterator it = allEntities.begin(); it != allEntities.end(); ++it)
    {
        delete *it;
    }
    allEntities.clear();


    // Setup cached datapoints

    // Verify expected datapoint names
    DataPoints datapoints;

    datapoints = getDataPointsByToken(commonPAAgentEntityData->getMyMajorSummaryAlarmToken());
    if(datapoints.size() != 1)
    {
        std::ostringstream msg;
        msg << "Failed to locate unique major summary alarm child entity datapoint ("
            << commonPAAgentEntityData->getMyMajorSummaryAlarmToken() << ")";
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "[NoData] %s", msg.str().c_str() );
    }
    else
    {
        CachedConfig::getInstance()->setMajorSummaryAlarmDP(datapoints.front());
    }

    datapoints = getDataPointsByToken(commonPAAgentEntityData->getMyMinorSummaryAlarmToken());
    if(datapoints.size() != 1)
    {
        std::ostringstream msg;
        msg << "Failed to locate unique minor summary alarm child entity datapoint ("
            << commonPAAgentEntityData->getMyMinorSummaryAlarmToken() << ")";
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "[NoData] %s", msg.str().c_str() );
    }
    else
    {
        CachedConfig::getInstance()->setMinorSummaryAlarmDP(datapoints.front());
    }

	datapoints = getDataPointsByToken(commonPAAgentEntityData->getMyFireCountdownActiveToken());
	if(0 == datapoints.size())
	{
		std::ostringstream msg;
		msg << "Failed to locate fire countdown active child entity datapoint ("
            << commonPAAgentEntityData->getMyFireCountdownActiveToken() << ")";
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "[NoData] %s", msg.str().c_str() );
	}
	else
	{
        std::map<std::string, unsigned short> mapFireTimers;
        if ( CachedConfig::getInstance()->getIsMaster() )
        {
            try
            {
                mapFireTimers = TA_Base_Core::PaFireCountdownAccessFactory::getInstance().getOccPaFireTimers();
            }
            catch ( ... )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get Occ Fire Timers" );
            }
        }
        else
        {
            try
            {
                mapFireTimers = TA_Base_Core::PaFireCountdownAccessFactory::getInstance().getStnPaFireTimers( 
                    CachedConfig::getInstance()->getAgentLocationKey() );
            }
            catch ( ... )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get Stn Fire Timers" );
            }
        }

        std::map<unsigned short, TA_Base_Bus::DataPoint*> mapFireTimerDps;
        std::string strDpName = EMPTY_STRING;
        for ( DataPointsIt itLoop = datapoints.begin(); datapoints.end() != itLoop; ++itLoop )
        {
            if ( NULL == *itLoop )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "NULL DataPoint pointer" );
                continue;
            }

            strDpName = (*itLoop)->getDataPointName();
            std::map<std::string, unsigned short>::iterator itFound = mapFireTimers.find( strDpName );
            if ( mapFireTimers.end() == itFound )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, 
                    "[NoData] PA Fire Timer %s is not configured in database Table PA_FIRE_COUNTDOWN_TIMER", strDpName.c_str() );
                continue;
            }

            mapFireTimerDps.insert( std::map<unsigned short, TA_Base_Bus::DataPoint*>::value_type(itFound->second, *itLoop) );
        }

		CachedConfig::getInstance()->setFireCountdownActiveDP( mapFireTimerDps );
	}

    std::string strToken = commonPAAgentEntityData->getMyFireCountdownActiveOutputToken();
    CachedConfig::getInstance()->setFireCountDownOutputToken( strToken );

    std::map<std::string, unsigned short> mapFireAborts;
    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        try
        {
            mapFireAborts = TA_Base_Core::PaFireCountdownAccessFactory::getInstance().getOccPaFireAborts();
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get Occ Fire Aborts" );
        }
    }
    else
    {
        try
        {
            mapFireAborts = TA_Base_Core::PaFireCountdownAccessFactory::getInstance().getStnPaFireAborts( 
                CachedConfig::getInstance()->getAgentLocationKey() );
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get Occ Fire Aborts" );
        }
    }

    CachedConfig::getInstance()->setFireCountDownOutputNames( mapFireAborts );

    // load the paging console Id's
    // Get Paging Consoles from database
    std::map<unsigned short, std::string> pagingConsoles;
    try
    {
        pagingConsoles = TA_Base_Core::PaPagingConsoleAccessFactory::getInstance().getPaPagingConsolesByLocationKey( 
            CachedConfig::getInstance()->getAgentLocationKey() );
    }
    catch (TA_Base_Core::DataException& dae)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DataException", dae.what());
    }
    catch (...)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get pa paging console ids by locationkey = %ld", CachedConfig::getInstance()->getAgentLocationKey());
    }

    CachedConfig::getInstance()->setPagingConsoles(pagingConsoles);

    processSpecificConfiguration();

    FUNCTION_EXIT;
}


// ExceptionChecked
DataPoints AbstractPAAgent::getDataPointsByToken(const std::string& token)
{
    FUNCTION_ENTRY("getPaAgentDataPointsByToken");

    DataPoints matches;
    // Search for string matches
    DataPointsIt it = m_dataPoints.begin();
    for (; it!=m_dataPoints.end(); ++it)
    {
        std::string name = (*it)->getDataPointName();

        if (name.find(token) != std::string::npos)
        {
            matches.push_back(*it);
        }
    }

    FUNCTION_EXIT;
    return matches;
}


bool AbstractPAAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
                                          const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                          TA_Base_Bus::IEntityList& createdEntities)
{
    bool createdDnSpecificUtils = false;
    
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


void AbstractPAAgent::entityCreationComplete()
{
    // loading complete, free some memory
    TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();
}



//
// createEntity
//
// ExceptionChecked
TA_Base_Bus::IEntity* AbstractPAAgent::createEntity(TA_Base_Core::IEntityDataPtr ptrEntityData)
    //throw(TA_Base_Core::InvalidPasConfigurationException)
{
	FUNCTION_ENTRY("createEntity");

    //
    // Note: the objects of interface IEntityData and IEntity are not owned
    // by this specific agent, but by the GenericAgent.  As such no object
    // clean up shall be done with regards to these objects.
    //

    TA_Base_Bus::IEntity * theEntity = 0;
    std::string entityDataType = ptrEntityData->getType();

    // if the specified entity is of the type DataPoint
    if ( TA_Base_Core::DataPointEntityData::getStaticType() == entityDataType )
    {
        // create and return the DataPoint entity
        FUNCTION_EXIT;
        return createAndInitialiseDataPointEntity ( ptrEntityData );
    }
	else if(entityDataType == TA_Base_Core::DataNodeEntityData::getStaticType())
	{
		TA_Base_Bus::DataNode* dn = NULL;
		try
		{
            dn = new TA_Base_Bus::DataNode (boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(ptrEntityData), m_persistence);
			
			m_dataNodes.push_back( dn );
			theEntity = dn;
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating DataNode %s (%lu)",
				ptrEntityData->getName().c_str(), ptrEntityData->getKey() );
			
			// To ensure the generic agent is constructed even if one DataNode creation fails
		}
	}
    // otherwise don't care
    else
    {
        // do nothing
    }

    FUNCTION_EXIT;
    //return NULL;
	return theEntity;
}

// ExceptionChecked
TA_Base_Bus::IEntity* AbstractPAAgent::createAndInitialiseDataPointEntity ( TA_Base_Core::IEntityDataPtr EntityData )
    //throw(TA_Base_Core::InvalidPasConfigurationException)
{
    FUNCTION_ENTRY("createDataPointEntity");

    // cast the EntityData to DataPointEntityData
    TA_Base_Core::DataPointEntityDataPtr dataPointEntityData =
        boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData>( EntityData );

    TA_Base_Bus::DataPoint* theEntity = m_dataPointFactory->createDataPoint( dataPointEntityData );

    if ( NULL == theEntity )
    {
        std::ostringstream msg;
        msg << "Child entity datapoint " << dataPointEntityData->getName()
            << " has not configured correctly and cannot be created";
        TA_THROW(TA_Base_Core::InvalidPasConfigurationException(msg.str().c_str()));
    }

    m_dataPoints.push_back(theEntity);

    FUNCTION_EXIT;
    return theEntity;
}



//
// agentTerminate
//
// ExceptionChecked
void AbstractPAAgent::agentTerminate()
{
     FUNCTION_ENTRY("agentTerminate");
	 if (!m_setupHasBeenCalled)
	 {
		 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::setup() must be called from the derived classes" );
		 return;
	 }

    stopSpecificComponents();

    if (( NULL != m_pPasFireAlarmManager ) && ( m_pPasFireAlarmManager->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING ))
    {
        m_pPasFireAlarmManager->terminateAndWait();
    }

    if ( NULL != m_pBroadcastManager )
    {
        m_pBroadcastManager->enable( false );
    }

    PasTransactionManager::getInstance().enable( false );

    if ( NULL != m_pPasPollingManager )
    {
        m_pPasPollingManager->enable( false );
    }

    if ( NULL != m_pPasConnectionAgency )
    {
        m_pPasConnectionAgency->enable( false );
    }

    if ( NULL != m_pDataPointQualityController )
    {
        m_pDataPointQualityController->enable( false );
    }

    // if bulk interface is created
    if ( NULL != m_dpBulkPolledInterface )
    {
		delete m_dpBulkPolledInterface;
	    m_dpBulkPolledInterface = NULL;
    }
    if ( NULL != m_dpAccessInterface )
    {
		delete m_dpAccessInterface;
	    m_dpAccessInterface = NULL;
    }
	if ( NULL != m_dnBulkPolledInterface )
    {
		delete m_dnBulkPolledInterface;
		m_dnBulkPolledInterface = NULL;
    }
    if ( NULL != m_dnAccessInterface )
    {
		delete m_dnAccessInterface;
		m_dnAccessInterface = NULL;
    }

    FUNCTION_EXIT;
}


//
// agentSetMonitor
//
// ExceptionChecked
void AbstractPAAgent::agentSetMonitor()
{
    FUNCTION_ENTRY("agentSetMonitor");

    TA_THREADGUARD( m_lockForOperationModes );

    if (m_operationMode == TA_Base_Core::Monitor)
    {
        return;
    }

    m_operationMode=TA_Base_Core::Monitor;

    agentSetMonitorSpecifics();

    if (( NULL != m_pPasFireAlarmManager ) && ( m_pPasFireAlarmManager->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING ))
    {
        m_pPasFireAlarmManager->terminateAndWait();
    }

    //limin++--, failover
    AbstractPAAgentThreadedMemberFunction& thrd = AbstractPAAgentThreadedMemberFunction::alloc( *this );
    thrd.add( &AbstractPAAgent::registerForMonitorMessages );
    thrd.start();
    //registerForMonitorMessages();
    //limin++--, failover

    // if bulk interface is set up
    if ( NULL != m_dpBulkPolledInterface )
    {
	    // stop receiving remote requests
	    m_dpBulkPolledInterface->setToMonitor();
    }
    if ( NULL != m_dpAccessInterface )
    {
	    // stop receiving remote requests
	    m_dpAccessInterface->setToMonitor();
    }
	if ( NULL != m_dnBulkPolledInterface )
    {
		// stop receiving remote requests
		m_dnBulkPolledInterface->setToMonitor();
    }
    if ( NULL != m_dnAccessInterface )
    {
		// stop receiving remote requests
		m_dnAccessInterface->setToMonitor();
    }

    // set the proxies to monitor mode
    m_scadaProxyFactory->setProxiesToMonitorMode();

    // For each of the data points created for this agent, set the data point to monitor mode
    for ( DataPointsIt itPoint = m_dataPoints.begin(); itPoint!=m_dataPoints.end(); ++itPoint )
    {
        if ( NULL == *itPoint )
        {
            continue;
        }
        (*itPoint)->setToMonitorMode();
    }

    // set all datanodes to monitor mode
    for ( DataNodeList::iterator itNode = m_dataNodes.begin(); itNode != m_dataNodes.end(); ++itNode )
    {
        if ( NULL == *itNode )
        {
            continue;
        }
        (*itNode)->setOperationMode( false );
    }

    if ( NULL != m_pPasConnectionAgency )
    {
        m_pPasConnectionAgency->enable( false );
    }

    if ( NULL != m_pPasFireAlarmManager )
    {
        m_pPasFireAlarmManager->enable( false );
    }

    PasZonesState::getInstance().enable( false );
    ScheduleDvaIdSupplier::getInstance().enable( false );

    if ( NULL != m_pPasFaultStatus )
    {
        m_pPasFaultStatus->enable( false );
    }

    if ( NULL != m_pBroadcastManager )
    {
        m_pBroadcastManager->enable( false );
    }

    PasTransactionManager::getInstance().enable( false );

    if ( NULL != m_pPasPollingManager )
    {
        m_pPasPollingManager->enable( false );
    }

    if ( NULL != m_pDataPointQualityController )
    {
        m_pDataPointQualityController->enable( false );
    }

    FUNCTION_EXIT;
}


//
// agentSetControl
//
// ExceptionChecked
void AbstractPAAgent::agentSetControl()
{
    FUNCTION_ENTRY("agentSetControl");

    TA_THREADGUARD( m_lockForOperationModes );

    if (m_operationMode == TA_Base_Core::Control)
    {
        return;
    }

    m_operationMode=TA_Base_Core::Control;

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "setModeChangingFlag:true");
	m_pBroadcastManager->setModeChangingFlag(true);

    AbstractPAAgentThreadedMemberFunction& thrd = AbstractPAAgentThreadedMemberFunction::alloc( *this );
    thrd.add( &AbstractPAAgent::registerForControlMessages );
    thrd.start();

	// set all DataNodes to control mode
	for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
    {
        if ( NULL == *itr )
        {
            continue;
        }
		(*itr)->setOperationMode( true );
	}

	// For each of the data points created for this agent, set the data point to control mode
    for ( DataPointsIt it = m_dataPoints.begin(); it!=m_dataPoints.end(); ++it)
    {
        if ( NULL == *it )
        {
            continue;
        }
        (*it)->setToControlMode();
    }

    // set the proxies to control mode
    m_scadaProxyFactory->setProxiesToControlMode();	

    if (( NULL != m_pPasFireAlarmManager ) && ( m_pPasFireAlarmManager->getCurrentState() != TA_Base_Core::Thread::THREAD_STATE_RUNNING ))
    {
        m_pPasFireAlarmManager->start();
    }

    agentSetControlSpecifics();

    // if bulk interface is set up
    if ( NULL != m_dpBulkPolledInterface )
    {
	    // start receiving remote requests
	    m_dpBulkPolledInterface->setToControl();
    }
    if ( NULL != m_dpAccessInterface )
    {
	    // start receiving remote requests
	    m_dpAccessInterface->setToControl();
    }
	// if bulk interface is set up
    if ( NULL != m_dnBulkPolledInterface )
    {
		// start receiving remote requests
		m_dnBulkPolledInterface->setToControl();
    }
    if ( NULL != m_dnAccessInterface )
    {
		// start receiving remote requests
		m_dnAccessInterface->setToControl();
    }

    if ( NULL != m_pDataPointQualityController )
    {
        m_pDataPointQualityController->enable();
    }

    if ( NULL != m_pPasPollingManager )
    {
        m_pPasPollingManager->enable();
    }

    PasTransactionManager::getInstance().enable();

    if ( NULL != m_pBroadcastManager )
    {
        m_pBroadcastManager->enable();
    }

    if ( NULL != m_pPasFireAlarmManager )
    {
        m_pPasFireAlarmManager->enable();
    }

    ScheduleDvaIdSupplier::getInstance().enable();
    PasZonesState::getInstance().enable();

    if ( NULL != m_pPasFaultStatus )
    {
        m_pPasFaultStatus->enable();
    }

    if ( NULL != m_pPasConnectionAgency )
    {
        m_pPasConnectionAgency->enable();
    }
	
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "setModeChangingFlag:false");
	m_pBroadcastManager->setModeChangingFlag(false);

    FUNCTION_EXIT;
}

//
// notifyGroupOffline
//
void AbstractPAAgent::notifyGroupOffline( const std::string& group )
{
    FUNCTION_ENTRY("notifyGroupOffline");

    FUNCTION_EXIT;
}

//
// notifyGroupOnline
//
void AbstractPAAgent::notifyGroupOnline( const std::string& group )
{
    FUNCTION_ENTRY("notifyGroupOnline");

    FUNCTION_EXIT;
}


//
// registerForControlMessages
//
void AbstractPAAgent::registerForControlMessages()
{
    FUNCTION_ENTRY("registerForControlMessages");

    // Unsubscribe if already subscribed...
    //limin--, failover issue
    //MessageSubscriptionManager::getInstance().unsubscribeToMessages(
    //        dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<CommsMessageCorbaDef>*>(this));
    //--limin

    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
            dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this));

    // Subscribe to receive the PAAgentStateUpdateRequest messages
    // This is received when the monitor instance starts up
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
        TA_Base_Core::PAAgentStateUpdateRequest::PAAgentStateUpdateRequest,
        dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this),
        CachedConfig::getInstance()->getAgentName());

    FUNCTION_EXIT;
}


//
// registerForMonitorMessages
//
void AbstractPAAgent::registerForMonitorMessages()
{
     FUNCTION_ENTRY("registerForMonitorMessages");

    // Unsubscribe if already subscribed...
    //limin--, failover issue
    //MessageSubscriptionManager::getInstance().unsubscribeToMessages(
    //        dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<CommsMessageCorbaDef>*>(this));
    //--limin

    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
            dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this));

    // Subscribe to receive the PAAgentStateUpdate messages
    // This is received in response to a sending of an PAAgentStateUpdateRequest message
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
        TA_Base_Core::PAAgentStateUpdate::Context,
        dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this),
        CachedConfig::getInstance()->getAgentName());

    FUNCTION_EXIT;
}


//
// registerForStateUpdates
//
void AbstractPAAgent::registerForStateUpdates()
{
     FUNCTION_ENTRY("registerForStateUpdates");

    if( 0 == TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
    {
        //limin++--, failover
        AbstractPAAgentThreadedMemberFunction& thrd = AbstractPAAgentThreadedMemberFunction::alloc( *this );
        thrd.add( &AbstractPAAgent::registerForControlMessages );
        thrd.start();

        //registerForControlMessages();
        //limin++--, failover
    }
    else
    {
        //limin++--, failover
        AbstractPAAgentThreadedMemberFunction& thrd = AbstractPAAgentThreadedMemberFunction::alloc( *this );
        thrd.add( &AbstractPAAgent::registerForMonitorMessages );
        thrd.start();
        
        //registerForMonitorMessages();
        //limin++--, failover
    }

    FUNCTION_EXIT;
}


//
// processOnlineUpdate
//
// ExceptionChecked
void AbstractPAAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
    FUNCTION_ENTRY("processOnlineUpdate");

    unsigned long updateEventPKey = updateEvent.getKey();

    if ( CachedConfig::getInstance()->getAgentKey() == updateEventPKey )
    {
        // update PA Agent configuration
        // Can only be an Update
        updatePAAgentConfiguration ( updateEvent );

        return;
    }
    else
    {
        // Must be for the child entities (datapoints)
        // Can be an Update / Create / Delete modification.
        updateDataPointConfiguration ( updateEvent );
    }

    FUNCTION_EXIT;
}

// ExceptionChecked
void AbstractPAAgent::updateDataPointConfiguration ( const TA_Base_Core::ConfigUpdateDetails & updateEvent )
{
    // No audit messages are raised from here - If a datapoint is being updated then it will
    // raise its own audit message.

     FUNCTION_ENTRY("updateDataPointConfiguration");

    try
    {
        TA_Base_Bus::DataPoint* theDataPoint = 0;
        unsigned long updateEventPKey = updateEvent.getKey();

        DataPointsIt it = m_dataPoints.begin();
        for (; it!=m_dataPoints.end(); ++it)
        {
			if (NULL == *it)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DataPoint* in m_dataPoints is NULL" );
				continue;
			}
            if ((*it)->getEntityKey() == updateEventPKey)
            {
                theDataPoint = (*it);
                break;
            }
        }

        if (theDataPoint)
        {
            // get the type of updating event
            switch ( updateEvent.getModifications() )
            {
                case TA_Base_Core::Update:
                {
                    // pass the config update to the data point for updating
                    theDataPoint->update ( updateEvent );

                    // exit switch statement
                    break;
                }
                case TA_Base_Core::Delete:
                case TA_Base_Core::Create:
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Received online update to create/delete a datapoint %s. The online creation/deletion of datapoints is not supported",
                        theDataPoint->getDataPointName().c_str());
                }
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Online update for unrecognised child entity pKey=%d ignored",
                    updateEvent.getKey() );
        }
    } // try block
    catch(TA_Base_Core::DataException)
    {
        // This means that the new configuration cannot be interpreted properly from
        // entity data.  These values will be ignored for the life of the application.

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Online update of datapoint pKey=%d failed - invalid configuration",
                    updateEvent.getKey() );
    }
    catch(TA_Base_Core::DatabaseException&)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Online update of datapoint pKey=%d failed - loss of database",
                    updateEvent.getKey() );
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Online update of datapoint pKey=%d failed",
                    updateEvent.getKey() );
    }

    FUNCTION_EXIT;
}

// ExceptionChecked
void AbstractPAAgent::updatePAAgentConfiguration ( const TA_Base_Core::ConfigUpdateDetails & updateEvent)
{
    FUNCTION_ENTRY("updatePAAgentConfiguration");

    if ( updateEvent.getModifications() != TA_Base_Core::Update )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unexpected Delete/Create modification ignored" );
        return;
    }

    // get configuration data for this PA Agent
    TA_Base_Core::AbstractPAAgentEntityDataPtr entityData = 
        boost::dynamic_pointer_cast<TA_Base_Core::AbstractPAAgentEntityData>(m_genericAgent->getAgentEntityConfiguration());

	if (NULL == entityData.get())
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unexpected change in entityData class since initialisation" );
		return;
	}

    // force a reread of the configuration
    entityData->invalidate();

    // Note that all parameters are either accessed directly out of CachedConfig
    // or automatically notify ICachedConfigObservers to take up the new values.
    // Therefore there is no need to explicitly call classes throughout the PA
    // Agent to reinitialise states.
    const std::vector<std::string> changes = updateEvent.getChangedParams();

    for ( unsigned int i = 0; i < changes.size(); i++ )
    {
        try
        {
            if (changes[i] == TA_Base_Core::AbstractPAAgentEntityData::FAST_POLL_RATE)
            {
                CachedConfig::getInstance()->setFastPollRateInMSecs(entityData->getFastPollRateInMSecs());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("FastPollRateInMSecs");
            }
            else if (changes[i] == TA_Base_Core::AbstractPAAgentEntityData::WORKER_THREADS_SLEEP_TIME)
            {
                CachedConfig::getInstance()->setWorkerThreadsSleepTimeInMSecs(entityData->getWorkerThreadsSleepTimeInMSecs());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("WorkerThreadsSleepTimeInMSecs");
            }
            else if (changes[i] == TA_Base_Core::AbstractPAAgentEntityData::SOCKET_TIMEOUT)
            {
                CachedConfig::getInstance()->setSocketTimeoutInSecs(entityData->getSocketTimeoutInSecs());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("SocketTimeoutInSecs");
            }
            else if (changes[i] == TA_Base_Core::AbstractPAAgentEntityData::TIS_DISPLAY_DURATION)
            {
                CachedConfig::getInstance()->setTisDisplayDurationInSecs(entityData->getTisDisplayDurationInSecs());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("TisDisplayDurationInSecs");
            }
            else if (changes[i] == TA_Base_Core::AbstractPAAgentEntityData::KEEP_TERMINATED_BROADCASTS)
            {
                CachedConfig::getInstance()->setKeepTerminatedBroadcastsInSecs(entityData->getKeepTerminatedBroadcastsInSecs());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("KeepTerminatedBroadcastsInSecs");
            }
            else if (changes[i] == TA_Base_Core::AbstractPAAgentEntityData::RETRIES)
            {
                CachedConfig::getInstance()->setRetries(entityData->getRetries());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("Retries");
            }
            else if (changes[i] == TA_Base_Core::AbstractPAAgentEntityData::PRIMARY_FEPC_ADDRESS)
            {
                CachedConfig::getInstance()->setPrimaryFEPCAddress(entityData->getPrimaryFEPCAddress());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("PrimaryFEPCAddress");
            }
            else if (changes[i] == TA_Base_Core::AbstractPAAgentEntityData::SECONDARY_FEPC_ADDRESS)
            {
                CachedConfig::getInstance()->setSecondaryFEPCAddress(entityData->getSecondaryFEPCAddress());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("SecondaryFEPCAddress");
            }
            else if (changes[i] == TA_Base_Core::AbstractPAAgentEntityData::PRIMARY_FEPC_PORT)
            {
                CachedConfig::getInstance()->setPrimaryFEPCPort(entityData->getPrimaryFEPCPort());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("PrimaryFEPCPort");
            }
            else if (changes[i] == TA_Base_Core::AbstractPAAgentEntityData::SECONDARY_FEPC_PORT)
            {
                CachedConfig::getInstance()->setSecondaryFEPCPort(entityData->getSecondaryFEPCPort());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("SecondaryFEPCPort");
            }
            else if (changes[i] == TA_Base_Core::AbstractPAAgentEntityData::MAX_BROADCASTS)
            {
                CachedConfig::getInstance()->setMaxBroadcasts(entityData->getMaxBroadcasts());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("MaxBroadcasts");
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Online update of parameter %s not supported",
                     changes[i].c_str() ); //TD14337 yezh++
            }

        } // try block
        catch(TA_Base_Core::InvalidPasConfigurationException)
        {
            // This means that the CachedConfig has detected invalid values and has not
            // overwritten them yet.  These values will be ignored for the life of the
            // application.
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Online update of parameter %s failed - invalid configuration",
                     changes[i].c_str() ); //TD14337 yezh++
        }
        catch(TA_Base_Core::DataException)
        {
            // This means that the new configuration cannot be interpreted properly from
            // entity data.  These values will be ignored for the life of the application.
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Online update of parameter %s failed - invalid configuration",
                     changes[i].c_str() ); //TD14337 yezh++
        }
        catch(TA_Base_Core::DatabaseException&)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Online update of parameter %s failed - loss of database",
                     changes[i].c_str() ); //TD14337 yezh++
        }
        catch(...)
        {
            // Not expecting anything else
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Online update of parameter %s failed",
                     changes[i].c_str() ); //TD14337 yezh++
        }
    } // for loop


    FUNCTION_EXIT;
}


//
// run
//
// ExceptionChecked
void AbstractPAAgent::run()
{
     FUNCTION_ENTRY("run");
	 if (!m_setupHasBeenCalled)
	 {
		 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::setup() must be called from the derived classes" );
		 return;
	 }
	 if (NULL == m_genericAgent)
	 {
		 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_genericAgent is NULL" );
		 return;
	 }

    // check the runtime parameter OperationMode
    std::string operationMode ( "" );
    operationMode = TA_Base_Core::RunParams::getInstance().get ( "OperationMode" );

    // if defined as "Control"
    if ( RPARAM_CONTROL == operationMode )
    {
        agentSetControl();
        startingInControlMode();
    }
    else if ( RPARAM_MONITOR == operationMode )
    {
        agentSetMonitor();
        startingInMonitorMode();
    }

    CachedConfig::getInstance()->setAgentInRuntime(true);

    
	// create an instance of the class DataPointAgentCorbaDef_Impl
	m_dpBulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl ( m_entityData->getName(), this );

	if ( operationMode == RPARAM_CONTROL )
	{
		// start receiving remote requests
		m_dpBulkPolledInterface->setToControl();
	}
	else
	{
		// stop receiving remote requests
		m_dpBulkPolledInterface->setToMonitor();
	}

	m_dpAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl ( m_entityData->getName(), this );

	if ( operationMode == RPARAM_CONTROL )
	{
		// start receiving remote requests
		m_dpAccessInterface->setToControl();
	}
	else
	{
		// stop receiving remote requests
		m_dpAccessInterface->setToMonitor();
	}

	m_dnBulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl ( m_entityData->getName(), this );
	
	if ( operationMode == RPARAM_CONTROL )
	{
		// start receiving remote requests
		m_dnBulkPolledInterface->setToControl();
	}
	else
	{
		// stop receiving remote requests
		m_dnBulkPolledInterface->setToMonitor();
	}
		
	m_dnAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl ( m_entityData->getName(), this );
	
	if ( operationMode == RPARAM_CONTROL )
	{
		// start receiving remote requests
		m_dnAccessInterface->setToControl();
	}
	else
	{
		// stop receiving remote requests
		m_dnAccessInterface->setToMonitor();
	}

    m_genericAgent->run();


    FUNCTION_EXIT;
}

// ExceptionChecked
void AbstractPAAgent::killMeAndRaiseAlarm()
{
     FUNCTION_ENTRY("killMeAndRaiseAlarm");
	 if (!m_setupHasBeenCalled)
	 {
		 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::setup() must be called from the derived classes" );
		 return;
	 }
	 if (NULL == m_genericAgent)
	 {
		 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_genericAgent is NULL" );
		 return;
	 }

    PasHelpers::getInstance()->raiseInvalidPAConfigurationAlarm("PAAgent in runtime operation will be terminated - inconsistent PA configuration");

    m_genericAgent->onTerminate();

    FUNCTION_EXIT;
}

void AbstractPAAgent::startingInMonitorMode()
{
    // Trigger the synchronsation of all datapoint states with those in the Control Agent instance
    // if it is already running

    CORBA::Any data;

    m_stateUpdateRequestSender->sendStateUpdateMessage(
        TA_Base_Core::PAAgentStateUpdateRequest::PAAgentStateUpdateRequest,
        CachedConfig::getInstance()->getAgentKey(),  // Not used
        CachedConfig::getInstance()->getAgentName(),
        data );                                      // Not used

    // Must wait until we have received the response from the Control Agent
    // otherwise we will end up unsynchronised.  
    while (!m_hasReceivedAndProcessedEntireAgentStateUpdate)
    {
        // Wait forever.  On the off chance that the Control Agent is unavailable
        // the System Controller will restart this agent.  
        TA_Base_Core::Thread::sleep ( CachedConfig::getInstance()->getWorkerThreadsSleepTimeInMSecs() );

        if (m_operationMode == TA_Base_Core::Control)
        {
            // This is for the scenario where we have started in Monitor Mode and are stuck
            // waiting forever for the entire state update to be processed.  If in the 
            // meantime, we have been forced into Control mode, we ensure that we break
            // out of that loop so that we can enter runtime and therefore later terminate.
            // We can assume that theres no point trying to get the state update as the 
            // Control agent is most likely to be down.

            break;
        }
    }
}

void AbstractPAAgent::startingInControlMode()
{
    // Reset any alarms
	if ( NULL != m_pBroadcastManager )
	{
		m_pBroadcastManager->restoreScheduledDva();
	}
}

// ExceptionChecked
void AbstractPAAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
{
     FUNCTION_ENTRY("receiveSpecialisedMessage");
	 if (!m_setupHasBeenCalled)
	 {
		 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::setup() must be called from the derived classes" );
		 return;
	 }

    if ( NULL == m_pBroadcastManager )
    {
        // BroadcastManager not created yet.  This can happen since the GenericAgent doAgentSetup()
        // subscribes to the messages in the constructor of Generic GUI yet this is done before
        // BroadcastManager, or anything PA Agent related has constructed.
        return;
    }

    std::string messageTypeKey(message.messageTypeKey);
    if (messageTypeKey == TA_Base_Core::PAAgentStateUpdate::PAAgentStateUpdate.getTypeKey() )
    {
        processPAAgentStateUpdate(message);
    }
    else if (messageTypeKey == TA_Base_Core::PAAgentStateUpdate::PAAgentEntireAgentStateUpdate.getTypeKey() )
    {
        processPAAgentEntireAgentStateUpdate(message);
    }
    else if (messageTypeKey == TA_Base_Core::PAAgentStateUpdate::PAAgentNewBroadcastUpdate.getTypeKey() )
    {
        processPAAgentNewBroadcastUpdate(message);
    }
    else if (messageTypeKey == TA_Base_Core::PAAgentStateUpdate::PAAgentRemovedBroadcastUpdate.getTypeKey() )
    {
        processPAAgentRemovedBroadcastUpdate(message);
    }
    else if (messageTypeKey == TA_Base_Core::PAAgentStateUpdate::PAAgentRenamedBroadcastUpdate.getTypeKey() )
    {
        processPAAgentRenamedBroadcastUpdate(message);
    }
    else if (messageTypeKey == TA_Base_Core::PAAgentStateUpdate::PAAgentBroadcastParameterUpdate.getTypeKey() )
    {
        processPAAgentBroadcastParameterUpdate(message);
    }
    else if (messageTypeKey == TA_Base_Core::PAAgentStateUpdate::PAAgentBroadcastProgressUpdate.getTypeKey() )
    {
        processPAAgentBroadcastProgressUpdate(message);
    }
    else if (messageTypeKey == TA_Base_Core::PAAgentStateUpdate::PAAgentPasParamUpdate.getTypeKey() )
    {
        processPAAgentPasParamUpdate(message);
    }
    else if (messageTypeKey == TA_Base_Core::PAAgentStateUpdate::PAAgentAvailableScheduleDvaIdUpdate.getTypeKey() )
    {
        processPAAgentPasScheduleDvaIdUpdate(message);
    }
    else if (messageTypeKey == TA_Base_Core::PAAgentStateUpdateRequest::PAAgentStateUpdateRequest.getTypeKey() )
    {
        processPAAgentStateUpdateRequest();
    }
    else
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, "Unexpected message received");
    }

    FUNCTION_EXIT;
}



// Called by another thread - need to ensure threadsafety of the datapoint container
// Note that we deliberately avoid sending the DataPointStateUpdateMessage because this
// would affect other clients such as the OPC Bridge.  Instead we send the PA Agent specific
// message.

// ExceptionChecked
void AbstractPAAgent::processPAAgentStateUpdateRequest()
{
    FUNCTION_ENTRY("processPAAgentStateUpdateRequest");

	if (!m_setupHasBeenCalled)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::setup() must be called from the derived classes" );
		return;
	}
	if (NULL == m_stateUpdateSender)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_stateUpdateSender is NULL" );
		return;
	}
	if (NULL == m_pBroadcastManager)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_pBroadcastManager is NULL" );
		return;
	}

    try
    {
        TA_Base_Bus::IPAAgentCorbaDef::CompleteAgentState* pAgentState = 
            new TA_Base_Bus::IPAAgentCorbaDef::CompleteAgentState();

        // Broadcast PAAgentEntireAgentStateUpdate messages for all broadcasts
        m_pBroadcastManager->getCompleteBroadcastStateSeqCopy( pAgentState->broadcastStates );
        // All available schedule DVA event Ids
        pAgentState->bAvailableIdInitialized = m_pBroadcastManager->getCompleteAvaliableEventIds( pAgentState->availableIds );

        CORBA::Any data;
        data <<= pAgentState;

        m_stateUpdateSender->sendStateUpdateMessage(
            TA_Base_Core::PAAgentStateUpdate::PAAgentEntireAgentStateUpdate,
            0,
            CachedConfig::getInstance()->getAgentName(),
            data );
    }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugFatal, "Exception thrown while processing event");
    }

    FUNCTION_EXIT;
}

void AbstractPAAgent::processPAAgentPasParamUpdate( const TA_Base_Core::StateUpdateMessageCorbaDef& message )
{
    FUNCTION_ENTRY("processPAAgentPasParamUpdate");

    if ( NULL == m_pDvaMsgLib )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DvaMsgLib is not created yet when receive state update message");
        return;
    }

    TA_Base_Bus::IPAAgentCorbaDef::PaSysParameter* updateData = NULL;
    try
    {
        // if successfully to extract the stateUpdateInfo
        message.stateUpdateInfo >>= updateData;
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "Exception thrown while processing state update");
    }

    if ( NULL == updateData )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Update data is not PaSysParameterSeq, stop process state update");
        return;
    }

    std::string strParamName = updateData->strName.in();
    std::string strParamValue = updateData->strValue.in();

    if ( 0 == strParamName.compare( TA_Base_Core::PAS_PARAM_NAME::DVA_LIB_COUNTER_VALUE  ))
    {
        m_pDvaMsgLib->setDvaMsgLibCounter( strParamValue );
    }

    FUNCTION_EXIT;
}

//////////////////////////////////////////////////////////
// The following is taken from the VirtualDatapointAgent
//////////////////////////////////////////////////////////



// Called by another thread - need to ensure threadsafety of the datapoint container
//
// ExceptionChecked
void AbstractPAAgent::processPAAgentStateUpdate( const TA_Base_Core::StateUpdateMessageCorbaDef& message )
{
    FUNCTION_ENTRY("processPAAgentStateUpdateRequest");
	if (!m_setupHasBeenCalled)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::setup() must be called from the derived classes" );
		return;
	}
	if (NULL == m_pBroadcastManager)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_pBroadcastManager is NULL" );
		return;
	}
    // Depreciated

    FUNCTION_EXIT;
}


void AbstractPAAgent::processPAAgentEntireAgentStateUpdate( const TA_Base_Core::StateUpdateMessageCorbaDef& message )
{
    FUNCTION_ENTRY("processPAAgentEntireAgentStateUpdate");
	if (!m_setupHasBeenCalled)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::setup() must be called from the derived classes" );
		return;
	}
	if (NULL == m_pBroadcastManager)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_pBroadcastManager is NULL" );
		return;
	}

    try
    {
        TA_Base_Bus::IPAAgentCorbaDef::CompleteAgentState* updateData = NULL;

        // if successfull to extract the stateUpdateInfo
        if (( 0 != ( message.stateUpdateInfo >>= updateData )) && ( NULL != updateData ))
        {
            m_pBroadcastManager->processPAAgentEntireAgentStateUpdate( *updateData );
        }

        updateData = NULL;
    }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugFatal,
            "Exception thrown while processing state update");
    }

    m_hasReceivedAndProcessedEntireAgentStateUpdate = true;

    FUNCTION_EXIT;
}

void AbstractPAAgent::processPAAgentNewBroadcastUpdate( const TA_Base_Core::StateUpdateMessageCorbaDef& message )
{
    FUNCTION_ENTRY("processPAAgentNewBroadcastUpdate");
	if (!m_setupHasBeenCalled)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::setup() must be called from the derived classes" );
		return;
	}
	if (NULL == m_pBroadcastManager)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_pBroadcastManager is NULL" );
		return;
	}

    try
    {
        TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* updateData = NULL;

        // if successfull to extract the stateUpdateInfo
        if (( 0 != ( message.stateUpdateInfo >>= updateData )) && ( NULL != updateData ))
        {
            m_pBroadcastManager->processPAAgentNewBroadcastUpdate( *updateData );
        }

        updateData = NULL;
       }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugFatal,
            "Exception thrown while processing state update");
    }

    FUNCTION_EXIT;
}

void AbstractPAAgent::processPAAgentRemovedBroadcastUpdate( const TA_Base_Core::StateUpdateMessageCorbaDef& message )
{
    FUNCTION_ENTRY("processPAAgentRemovedBroadcastUpdate");
	if (!m_setupHasBeenCalled)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::setup() must be called from the derived classes" );
		return;
	}
	if (NULL == m_pBroadcastManager)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_pBroadcastManager is NULL" );
		return;
	}

    try
    {
        const char* updateData = NULL;

        // if successfull to extract the stateUpdateInfo
        if (( 0 != ( message.stateUpdateInfo >>= CORBA::Any::to_string( updateData, 0 ))) && ( NULL != updateData ))
        {
            m_pBroadcastManager->processPAAgentRemovedBroadcastUpdate( updateData );
        }

        updateData = NULL;
       }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugFatal,
            "Exception thrown while processing state update");
    }

    FUNCTION_EXIT;
}

void AbstractPAAgent::processPAAgentRenamedBroadcastUpdate( const TA_Base_Core::StateUpdateMessageCorbaDef& message )
{
    FUNCTION_ENTRY("processPAAgentRenamedBroadcastUpdate");
	if (!m_setupHasBeenCalled)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::setup() must be called from the derived classes" );
		return;
	}
	if (NULL == m_pBroadcastManager)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_pBroadcastManager is NULL" );
		return;
	}

    try
    {
        TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData* updateData = NULL;

        // if successfull to extract the stateUpdateInfo
        if (( 0 != ( message.stateUpdateInfo >>= updateData )) && ( NULL != updateData ))
        {
            m_pBroadcastManager->processPAAgentRenamedBroadcastUpdate( *updateData );
        }

        updateData = NULL;
       }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugFatal,
            "Exception thrown while processing state update");
    }

    FUNCTION_EXIT;
}

void AbstractPAAgent::processPAAgentBroadcastParameterUpdate( const TA_Base_Core::StateUpdateMessageCorbaDef& message )
{
    FUNCTION_ENTRY("processPAAgentBroadcastParameterUpdate");
	if (!m_setupHasBeenCalled)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::setup() must be called from the derived classes" );
		return;
	}
	if (NULL == m_pBroadcastManager)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_pBroadcastManager is NULL" );
		return;
	}

    try
    {
        TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter* updateData = NULL;

        // if successfull to extract the stateUpdateInfo
        if (( 0 != ( message.stateUpdateInfo >>= updateData )) && ( NULL != updateData ))
        {
            m_pBroadcastManager->processPAAgentBroadcastParameterUpdate( *updateData );
        }

        updateData = NULL;
    }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugFatal,
            "Exception thrown while processing state update");
    }

    FUNCTION_EXIT;
}

void AbstractPAAgent::processPAAgentBroadcastProgressUpdate( const TA_Base_Core::StateUpdateMessageCorbaDef& message )
{
    FUNCTION_ENTRY("processPAAgentBroadcastProgressUpdate");
	if (!m_setupHasBeenCalled)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::setup() must be called from the derived classes" );
		return;
	}
	if (NULL == m_pBroadcastManager)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_pBroadcastManager is NULL" );
		return;
	}

    try
    {
        TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* updateData = NULL;

        // if successfull to extract the stateUpdateInfo
        if (( 0 != ( message.stateUpdateInfo >>= updateData )) && ( NULL != updateData ))
        {
            m_pBroadcastManager->processPAAgentBroadcastProgressUpdate( *updateData );
        }

        updateData = NULL;
       }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugFatal,
            "Exception thrown while processing state update");
    }

    FUNCTION_EXIT;
}

void AbstractPAAgent::processPAAgentPasScheduleDvaIdUpdate( const TA_Base_Core::StateUpdateMessageCorbaDef& message )
{
    FUNCTION_ENTRY("processPAAgentPasScheduleDvaIdUpdate");
	if (!m_setupHasBeenCalled)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAAgent::setup() must be called from the derived classes" );
		return;
	}
	if (NULL == m_pBroadcastManager)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_pBroadcastManager is NULL" );
		return;
	}

    try
    {
        TA_Base_Bus::IPAAgentCorbaDef::ScheduleDvaIds* updateData = NULL;

        // if successfull to extract the stateUpdateInfo
        if (( 0 != ( message.stateUpdateInfo >>= updateData )) && ( NULL != updateData ))
        {
            m_pBroadcastManager->setCompleteAvaliableEventIds( *updateData );
        }

        updateData = NULL;
    }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugFatal,
            "Exception thrown while processing state update");
    }

    FUNCTION_EXIT;
}

TA_Base_Bus::DataPoint * AbstractPAAgent::getDataPoint ( unsigned long entityKey )
{
	TA_Base_Bus::DataPoint* theDataPoint = NULL;

	try
	{
		// Find datapoint
		const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap = m_genericAgent->getEntityMap();
		std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator it = entityMap->find(entityKey);

		if (it != entityMap->end())
		{		
			theDataPoint = dynamic_cast<TA_Base_Bus::DataPoint*>((*it).second);
		}
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "getDataPoint() - Exception thrown while retrieving datapoint");
	}

	return theDataPoint;
}

TA_Base_Bus::DataPointFactory* AbstractPAAgent::getDataPointFactory()
{
    return m_dataPointFactory;
}

TA_Base_Bus::DataNode * AbstractPAAgent::getDataNode( unsigned long entityKey )
{
	// stop multi threads accessing the list of DataNode objects at the same time
	ACE_Guard<ACE_RW_Mutex> guard( m_mtxDataNodeListLock );

	TA_Base_Bus::DataNode * dn = NULL;

	const std::map<unsigned long, TA_Base_Bus::IEntity*>* dataNodes = m_genericAgent->getEntityMap();
	std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator itr = dataNodes->find( entityKey );
	
	if ( itr != dataNodes->end() )
	{
		dn = dynamic_cast<TA_Base_Bus::DataNode *>(itr->second);
		
		if ( NULL == dn )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not convert entity %lu to a DataNode", entityKey );
		}
	}
	
	return dn;
}

void AbstractPAAgent::getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
{
    ACE_Guard<ACE_RW_Mutex> guard( m_mtxDataNodeListLock );

	dataNodeList.clear();
	for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
	{
		dataNodeList[ (*itr)->getEntityKey() ] = *itr;
    }
}

void AbstractPAAgent::refreshPasHardwareParameters()
{
    std::map<std::string, std::string> mapParamValues;

    try
    {
        mapParamValues = TA_Base_Core::PaSysParamAccessFactory::getInstance().getPaSysParamsByLocationKey( 
            CachedConfig::getInstance()->getAgentLocationKey() );
    }
    catch ( std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
            "Caught unknown exception when PasTransactionManager running" );
    }

    if ( mapParamValues.empty() )
    {
        return;
    }

    std::map<std::string, std::string>::iterator itFoundParam;
    itFoundParam = mapParamValues.find( TA_Base_Core::PAS_PARAM_NAME::DVA_LIB_COUNTER_VALUE );
    if ( mapParamValues.end() != itFoundParam )
    {
        if ( NULL != m_pDvaMsgLib )
        {
            m_pDvaMsgLib->setDvaMsgLibCounter( itFoundParam->second );
        }
    }
}

} // namespace TA_IRS_App

