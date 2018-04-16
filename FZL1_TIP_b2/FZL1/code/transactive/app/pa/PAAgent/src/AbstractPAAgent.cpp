/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include <time.h>
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/DataPointStateUpdate_MessageTypes.h"
#include "core/message/types/PAAgentAlarms_MessageTypes.h"
#include "core/message/types/PAAgentAudit_MessageTypes.h"
#include "core/message/types/PAAgentStateUpdate_MessageTypes.h"
#include "core/message/types/PAAgentStateUpdateRequest_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/data_access_interface/entity_access/src/AbstractPAAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/threads/src/Thread.h"

#include "bus/generic_agent/src/IEntity.h"
#include "bus/generic_agent/src/GenericAgent.h"

#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"

#include "bus/pa/cachedmaps/src/CachedMaps.h"

#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/AbstractPAAgent.h"
#include "app/pa/PAAgent/src/FireAlarmSubscriber.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/PasHelpers.h"




using TA_Base_Bus::IEntity;
using namespace TA_Base_Core;
using namespace TA_IRS_Bus;


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
: m_genericAgent(NULL),
  m_operationMode(TA_Base_Core::NotApplicable),
  m_setupHasBeenCalled(false),
  m_cleanupHasBeenCalled(false),
  m_stateUpdateRequestSender(NULL),
  m_stateUpdateSender(NULL),
  m_broadcastManager(NULL),
  m_scrFireAlarm(NULL),
  m_hasReceivedAndProcessedEntireAgentStateUpdate(false)
{
    FUNCTION_ENTRY("Constructor");

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

        // The instance of the Pas Table Manager is Master/Station specific.
        IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();

        m_broadcastManager = new BroadcastManager();
        m_scrFireAlarm = new FireAlarmSubscriber();

        m_broadcastManager->activateServantWithName(CachedConfig::getInstance()->getAgentName());
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Registering PA Agent CORBA object: %s", CachedConfig::getInstance()->getAgentName().c_str());

        createSpecificComponents();
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

    if (NULL != m_scrFireAlarm)
    {
        delete m_scrFireAlarm;
        m_scrFireAlarm = NULL;
    }

    if (NULL != m_broadcastManager)
    {
        m_broadcastManager->deactivateAndDeleteServant();
        m_broadcastManager = NULL;
    }

    if (NULL != m_stateUpdateSender)
    {
        delete m_stateUpdateSender;
        m_stateUpdateSender = NULL;
    }

    if (NULL != m_stateUpdateRequestSender)
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
    AbstractBroadcastCircularList::removeInstance();
    CachedConfig::removeInstance();
    CachedMaps::removeInstance();
    AlarmHelperManager::getInstance().cleanUp();

    // The generic agent must not be deleted before the audit sender and DataPointProxy* as the
    // generic agent destructor cleans up all the StructuredEventSuppliers created for all message
    // senders.

    if (NULL != m_genericAgent)
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
	
	std::string::size_type pos = agentName.find_last_of(AGENT_NAME_APPEND);
    if (pos != std::string::npos)
	{
		std::string::size_type stringLength = pos + 1 - AGENT_NAME_APPEND.length();
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

    CachedConfig::getInstance()->setFasEntityName(commonPAAgentEntityData->getFasEntityName());

    // Determine special location flags:

    if (this->m_entityData->getLocation() == LocationAccessFactory::getInstance().getOccLocationKey())
    {
        CachedConfig::getInstance()->setIsMaster(true);
        CachedConfig::getInstance()->setIsDepot(false);
    }
    else
    {
        // Station or Depot
        CachedConfig::getInstance()->setIsMaster(false);

        // For GF, Depot location name "KCD" is modified to "DCC"
        // Currently, we do not need to identify whether the start location is deport or not , so we comments the following codes.
        //if (this->m_entityData->getLocation() == LocationAccessFactory::getInstance().getDepotLocationKey())
        //{
        //    // Depot
        //    CachedConfig::getInstance()->setIsDepot(true);
        //    CachedMaps::getInstance()->setOccDepotFlags(false, true);
        //}
        //else
        {
            // Standard station
            CachedConfig::getInstance()->setIsDepot(false);
        }
    }


    // Note the following can raise the TA_Base_Core::DatabaseException or TA_Base_Core::DataException.

    IEntityDataList allEntities;
    IEntityDataList::iterator it;

    // Locate the train agent key for subscribing to messages

    allEntities =
        EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( TrainAgentEntityData::getStaticType(), this->m_entityData->getLocation() );


    if ( allEntities.size() == 1 )
    {
        CachedConfig::getInstance()->setLocalTrainAgentKey(allEntities[0]->getKey());
        CachedConfig::getInstance()->setLocalTrainAgentEntityName(allEntities[0]->getName());
    }

    for (it = allEntities.begin(); it != allEntities.end(); ++it)
    {
        delete *it;
    }
    allEntities.clear();

    processSpecificConfiguration();

    FUNCTION_EXIT;
}

bool AbstractPAAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
                                          const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                          TA_Base_Bus::IEntityList& createdEntities)
{
    bool createdDnSpecificUtils = false;
    
    for ( TA_Base_Core::SharedIEntityDataList::const_iterator iter = entityDataList.begin();
    iter != entityDataList.end(); ++iter )
    {
        createdEntities.push_back( 
            TA_Base_Bus::IEntityList::value_type( (*iter)->getKey(), createEntity(*iter)) );
    }
    
    // return true to indicate not to call createEntity
    return true;
}


void AbstractPAAgent::entityCreationComplete()
{
}



//
// createEntity
//
// ExceptionChecked
IEntity* AbstractPAAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
    //throw(TA_Base_Core::InvalidPasConfigurationException)
{
	FUNCTION_ENTRY("createEntity");
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");

    FUNCTION_EXIT;
    return NULL;
}

//
// agentTerminate
//
// ExceptionChecked
void AbstractPAAgent::agentTerminate()
{
     FUNCTION_ENTRY("agentTerminate");
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");

    stopSpecificComponents();

    m_broadcastManager->disable();
    m_scrFireAlarm->enable(false);
    m_scrFireAlarm->notifyQuit();

    FUNCTION_EXIT;
}


//
// agentSetMonitor
//
// ExceptionChecked
void AbstractPAAgent::agentSetMonitor()
{
     FUNCTION_ENTRY("agentSetMonitor");
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");

    ThreadGuard guard( m_lockForOperationModes );

    if (m_operationMode == TA_Base_Core::Monitor)
    {
        return;
    }

    m_operationMode=TA_Base_Core::Monitor;

    agentSetMonitorSpecifics();

    registerForMonitorMessages();

    this->m_broadcastManager->disable();

    FUNCTION_EXIT;
}


//
// agentSetControl
//
// ExceptionChecked
void AbstractPAAgent::agentSetControl()
{
     FUNCTION_ENTRY("agentSetControl");
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");

    ThreadGuard guard( m_lockForOperationModes );

    if (m_operationMode == TA_Base_Core::Control)
    {
        return;
    }

    m_operationMode=TA_Base_Core::Control;

	m_broadcastManager->setModeChangingFlag(true);

    m_broadcastManager->enable();
    m_scrFireAlarm->enable();

    registerForControlMessages();

    agentSetControlSpecifics();

	m_broadcastManager->setModeChangingFlag(false);

    FUNCTION_EXIT;
}

//
// notifyGroupOffline
//
void AbstractPAAgent::notifyGroupOffline( const std::string& group )
{
     FUNCTION_ENTRY("notifyGroupOffline");

    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");
    // TODO: Degraded mode operations

    FUNCTION_EXIT;
}

//
// notifyGroupOnline
//
void AbstractPAAgent::notifyGroupOnline( const std::string& group )
{
     FUNCTION_ENTRY("notifyGroupOnline");

    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");
    // TODO: Degraded mode operations

    FUNCTION_EXIT;
}


//
// registerForControlMessages
//
void AbstractPAAgent::registerForControlMessages()
{
     FUNCTION_ENTRY("registerForControlMessages");

    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");

    // Unsubscribe if already subscribed...
    //limin--, failover issue
    //MessageSubscriptionManager::getInstance().unsubscribeToMessages(
    //        dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<CommsMessageCorbaDef>*>(this));
    //--limin

    MessageSubscriptionManager::getInstance().unsubscribeToMessages(
            dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<StateUpdateMessageCorbaDef>*>(this));

    // Subscribe to receive the PAAgentStateUpdateRequest messages
    // This is received when the monitor instance starts up

    MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
        PAAgentStateUpdateRequest::PAAgentStateUpdateRequest,
        dynamic_cast<SpecialisedMessageSubscriber<StateUpdateMessageCorbaDef>*>(this),
        CachedConfig::getInstance()->getAgentName());

    FUNCTION_EXIT;
}


//
// registerForMonitorMessages
//
void AbstractPAAgent::registerForMonitorMessages()
{
     FUNCTION_ENTRY("registerForMonitorMessages");

    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");

    // Unsubscribe if already subscribed...
    //limin--, failover issue
    //MessageSubscriptionManager::getInstance().unsubscribeToMessages(
    //        dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<CommsMessageCorbaDef>*>(this));
    //--limin

    MessageSubscriptionManager::getInstance().unsubscribeToMessages(
            dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<StateUpdateMessageCorbaDef>*>(this));

    // Subscribe to receive the PAAgentStateUpdate messages
    // This is received in response to a sending of an PAAgentStateUpdateRequest message

    MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
        PAAgentStateUpdate::Context,
        dynamic_cast<SpecialisedMessageSubscriber<StateUpdateMessageCorbaDef>*>(this),
        CachedConfig::getInstance()->getAgentName());

    FUNCTION_EXIT;
}


//
// registerForStateUpdates
//
void AbstractPAAgent::registerForStateUpdates()
{
     FUNCTION_ENTRY("registerForStateUpdates");
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");

    FUNCTION_EXIT;
}

//
// processOnlineUpdate
//
// ExceptionChecked
void AbstractPAAgent::processOnlineUpdate(const ConfigUpdateDetails& updateEvent)
{
     FUNCTION_ENTRY("processOnlineUpdate");
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");

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
        // Do nothing?
    }

    FUNCTION_EXIT;
}

// ExceptionChecked
void AbstractPAAgent::updatePAAgentConfiguration ( const TA_Base_Core::ConfigUpdateDetails & updateEvent)
{
    FUNCTION_ENTRY("updatePAAgentConfiguration");

    if ( updateEvent.getModifications() != TA_Base_Core::Update )
    {
        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugWarn,
            "Unexpected Delete/Create modification ignored" );
        return;
    }

    // get the list of changed parameter from the config update
    const std::vector< std::string > changes = updateEvent.getChangedParams();
    TA_ASSERT( changes.size() > 0, "Empty changes should have been prevented by GenericAgent");

    // get configuration data for this PA Agent
    AbstractPAAgentEntityDataPtr entityData = boost::dynamic_pointer_cast<AbstractPAAgentEntityData>
    ( m_genericAgent->getAgentEntityConfiguration() );

    TA_ASSERT(entityData.get(), "Unexpected change in entityData class since initialisation");

    // force a reread of the configuration
    entityData->invalidate();

    // Note that all parameters are either accessed directly out of CachedConfig
    // or automatically notify ICachedConfigObservers to take up the new values.
    // Therefore there is no need to explicitly call classes throughout the PA
    // Agent to reinitialise states.

    for ( unsigned int i = 0; i < changes.size(); i++ )
    {
        try
        {
            if (changes[i] == AbstractPAAgentEntityData::FAST_POLL_RATE)
            {
                CachedConfig::getInstance()->setFastPollRateInMSecs(entityData->getFastPollRateInMSecs());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("FastPollRateInMSecs");
            }
            else if (changes[i] == AbstractPAAgentEntityData::WORKER_THREADS_SLEEP_TIME)
            {
                CachedConfig::getInstance()->setWorkerThreadsSleepTimeInMSecs(entityData->getWorkerThreadsSleepTimeInMSecs());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("WorkerThreadsSleepTimeInMSecs");
            }
            else if (changes[i] == AbstractPAAgentEntityData::SOCKET_TIMEOUT)
            {
                CachedConfig::getInstance()->setSocketTimeoutInSecs(entityData->getSocketTimeoutInSecs());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("SocketTimeoutInSecs");
            }
            else if (changes[i] == AbstractPAAgentEntityData::TIS_DISPLAY_DURATION)
            {
                CachedConfig::getInstance()->setTisDisplayDurationInSecs(entityData->getTisDisplayDurationInSecs());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("TisDisplayDurationInSecs");
            }
            else if (changes[i] == AbstractPAAgentEntityData::KEEP_TERMINATED_BROADCASTS)
            {
                CachedConfig::getInstance()->setKeepTerminatedBroadcastsInSecs(entityData->getKeepTerminatedBroadcastsInSecs());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("KeepTerminatedBroadcastsInSecs");
            }
            else if (changes[i] == AbstractPAAgentEntityData::RETRIES)
            {
                CachedConfig::getInstance()->setRetries(entityData->getRetries());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("Retries");
            }
            else if (changes[i] == AbstractPAAgentEntityData::PRIMARY_FEPC_ADDRESS)
            {
                CachedConfig::getInstance()->setPrimaryFEPCAddress(entityData->getPrimaryFEPCAddress());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("PrimaryFEPCAddress");
            }
            else if (changes[i] == AbstractPAAgentEntityData::SECONDARY_FEPC_ADDRESS)
            {
                CachedConfig::getInstance()->setSecondaryFEPCAddress(entityData->getSecondaryFEPCAddress());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("SecondaryFEPCAddress");
            }
            else if (changes[i] == AbstractPAAgentEntityData::PRIMARY_FEPC_PORT)
            {
                CachedConfig::getInstance()->setPrimaryFEPCPort(entityData->getPrimaryFEPCPort());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("PrimaryFEPCPort");
            }
            else if (changes[i] == AbstractPAAgentEntityData::SECONDARY_FEPC_PORT)
            {
                CachedConfig::getInstance()->setSecondaryFEPCPort(entityData->getSecondaryFEPCPort());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("SecondaryFEPCPort");
            }
            else if (changes[i] == AbstractPAAgentEntityData::MAX_BROADCASTS)
            {
                CachedConfig::getInstance()->setMaxBroadcasts(entityData->getMaxBroadcasts());
                PasHelpers::getInstance()->submitPAAgentOnlineUpdateAuditMessage("MaxBroadcasts");
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Online update of parameter %s not supported",
                     changes[i].c_str() ); //TD14337 yezh++
            }

        } // try block
        catch(TA_Base_Core::InvalidPasConfigurationException)
        {
            // This means that the CachedConfig has detected invalid values and has not
            // overwritten them yet.  These values will be ignored for the life of the
            // application.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Online update of parameter %s failed - invalid configuration",
                     changes[i].c_str() ); //TD14337 yezh++
        }
        catch(TA_Base_Core::DataException)
        {
            // This means that the new configuration cannot be interpreted properly from
            // entity data.  These values will be ignored for the life of the application.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Online update of parameter %s failed - invalid configuration",
                     changes[i].c_str() ); //TD14337 yezh++
        }
        catch(TA_Base_Core::DatabaseException&)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Online update of parameter %s failed - loss of database",
                     changes[i].c_str() ); //TD14337 yezh++
        }
        catch(...)
        {
            // Not expecting anything else
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
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
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");
    TA_ASSERT(m_genericAgent, "m_genericAgent is NULL");

    // check the runtime parameter OperationMode
    std::string operationMode ( "" );
    operationMode = RunParams::getInstance().get ( "OperationMode" );

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

    m_genericAgent->run();

    FUNCTION_EXIT;
}

// ExceptionChecked
void AbstractPAAgent::killMeAndRaiseAlarm()
{
     FUNCTION_ENTRY("killMeAndRaiseAlarm");
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");
    TA_ASSERT(m_genericAgent, "m_genericAgent is NULL");

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
        PAAgentStateUpdateRequest::PAAgentStateUpdateRequest,
        CachedConfig::getInstance()->getAgentKey(),  // Not used
        CachedConfig::getInstance()->getAgentName(),
        data );                                      // Not used


    // Must wait until we have received the response from the Control Agent
    // otherwise we will end up unsynchronised.  
    while (!m_hasReceivedAndProcessedEntireAgentStateUpdate)
    {
        // Wait forever.  On the off chance that the Control Agent is unavailable
        // the System Controller will restart this agent.  
        Thread::sleep ( CachedConfig::getInstance()->getWorkerThreadsSleepTimeInMSecs() );

        
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
}


// ExceptionChecked
void AbstractPAAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
{
     FUNCTION_ENTRY("receiveSpecialisedMessage");
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");

    if (m_broadcastManager==NULL)
    {
        // BroadcastManager not created yet.  This can happen since the GenericAgent doAgentSetup()
        // subscribes to the messages in the constructor of Generic GUI yet this is done before
        // BroadcastManager, or anything PA Agent related has constructed.
        return;
    }

    std::string messageTypeKey(message.messageTypeKey);
    if (messageTypeKey == PAAgentStateUpdate::PAAgentEntireAgentStateUpdate.getTypeKey() )
    {
        processPAAgentEntireAgentStateUpdate(message);
    }
    else if (messageTypeKey == PAAgentStateUpdate::PAAgentNewBroadcastUpdate.getTypeKey() )
    {
        processPAAgentNewBroadcastUpdate(message);
    }
    else if (messageTypeKey == PAAgentStateUpdate::PAAgentRemovedBroadcastUpdate.getTypeKey() )
    {
        processPAAgentRemovedBroadcastUpdate(message);
    }
    else if (messageTypeKey == PAAgentStateUpdate::PAAgentRenamedBroadcastUpdate.getTypeKey() )
    {
        processPAAgentRenamedBroadcastUpdate(message);
    }
    else if (messageTypeKey == PAAgentStateUpdate::PAAgentBroadcastParameterUpdate.getTypeKey() )
    {
        processPAAgentBroadcastParameterUpdate(message);
    }
    else if (messageTypeKey == PAAgentStateUpdate::PAAgentBroadcastProgressUpdate.getTypeKey() )
    {
        processPAAgentBroadcastProgressUpdate(message);
    }
    else if (messageTypeKey == PAAgentStateUpdateRequest::PAAgentStateUpdateRequest.getTypeKey() )
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

    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");
    TA_ASSERT(m_stateUpdateSender, "m_stateUpdateSender is NULL");
    TA_ASSERT(m_broadcastManager, "m_broadcastManager is NULL");


    try
    {
        // Broadcast PAAgentEntireAgentStateUpdate messages for all broadcasts
        TA_Base_Bus::IPAAgentCorbaDef::CompleteBroadcastStateSeq* disposableUpdateData =
            m_broadcastManager->getCompleteBroadcastStateSeqCopy();

        CORBA::Any data;
        data <<= disposableUpdateData;

        m_stateUpdateSender->sendStateUpdateMessage(
            PAAgentStateUpdate::PAAgentEntireAgentStateUpdate,
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

//////////////////////////////////////////////////////////
// The following is taken from the VirtualDatapointAgent
//////////////////////////////////////////////////////////



// Called by another thread - need to ensure threadsafety of the datapoint container
//
// ExceptionChecked

void AbstractPAAgent::processPAAgentEntireAgentStateUpdate( const TA_Base_Core::StateUpdateMessageCorbaDef& message )
{
    FUNCTION_ENTRY("processPAAgentEntireAgentStateUpdate");
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");
    TA_ASSERT(m_broadcastManager, "m_broadcastManager is NULL");

    try
    {
        TA_Base_Bus::IPAAgentCorbaDef::CompleteBroadcastStateSeq* updateData = NULL;

        // if successfull to extract the stateUpdateInfo
        if ( 0 != ( message.stateUpdateInfo >>= updateData ) )
        {
            m_broadcastManager->processPAAgentEntireAgentStateUpdate( updateData );
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
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");
    TA_ASSERT(m_broadcastManager, "m_broadcastManager is NULL");

    try
    {
        TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* updateBroadcastData = 0;
        TA_Base_Bus::IPAAgentCorbaDef::TriggeredBroadcastConfig* updateTriggeredData =0;

        // if successfull to extract the stateUpdateInfo
        if ( 0 != ( message.stateUpdateInfo >>= updateBroadcastData ) )
        {
            m_broadcastManager->processPAAgentNewBroadcastUpdate( updateBroadcastData );
        }
        else if ( 0 != ( message.stateUpdateInfo >>= updateTriggeredData ))
        {
            m_broadcastManager->processPAAgentNewBroadcastUpdate( updateTriggeredData );
        }

        updateBroadcastData = 0;
        updateTriggeredData =0;
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
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");
    TA_ASSERT(m_broadcastManager, "m_broadcastManager is NULL");

    try
    {
        CORBA::ULong ulScheduleKey= 0;

        // if successfully to extract the stateUpdateInfo
        if ( 0 != ( message.stateUpdateInfo >>= ulScheduleKey ) )
        {
            m_broadcastManager->processPAAgentRemovedBroadcastUpdate( static_cast<unsigned long>(ulScheduleKey) );
        }
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
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");
    TA_ASSERT(m_broadcastManager, "m_broadcastManager is NULL");

    try
    {
        TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData* updateData = NULL;

        // if successfull to extract the stateUpdateInfo
        if ( 0 != ( message.stateUpdateInfo >>= updateData ) )
        {
            m_broadcastManager->processPAAgentRenamedBroadcastUpdate( updateData );
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
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");
    TA_ASSERT(m_broadcastManager, "m_broadcastManager is NULL");

    try
    {
        TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter* updateData = NULL;

        // if successfull to extract the stateUpdateInfo
        if ( 0 != ( message.stateUpdateInfo >>= updateData ) )
        {
            m_broadcastManager->processPAAgentBroadcastParameterUpdate( updateData );
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
    TA_ASSERT(m_setupHasBeenCalled, "PAAgent::setup() must be called from the derived classes");
    TA_ASSERT(m_broadcastManager, "m_broadcastManager is NULL");

    try
    {
        TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* updateData = NULL;

        // if successfull to extract the stateUpdateInfo
        if ( 0 != ( message.stateUpdateInfo >>= updateData ) )
        {
            m_broadcastManager->processPAAgentBroadcastProgressUpdate( updateData );
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

} // namespace TA_IRS_App

