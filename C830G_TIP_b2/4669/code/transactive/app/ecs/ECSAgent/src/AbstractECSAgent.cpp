/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/AbstractECSAgent.cpp $
  * @author:  seanl
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2009/11/20 15:30:56 $
  * Last modified by:  $Author: builder $
  * 
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include <time.h>
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/generic_agent/src/IEntity.h"

#include "app/ecs/ECSAgent/src/AbstractECSAgent.h"
#include "app/ecs/ECSAgent/src/EcsZoneDataSource.h"
#include "app/ecs/ECSAgent/src/DataPointWriteQueueProcessor.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedMaps.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedMapsInitialiser.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfigInitialiser.h"
#include "app/ecs/ECSAgent/src/ItaEcsAuditing.h"
#include "app/ecs/ECSAgent/src/ItaEcsComms.h"
#include "app/ecs/ECSAgent/src/ItaEcsSynchronisation.h"
#include "app/ecs/ECSAgent/src/ItaEcsAlarms.h"
#include "app/ecs/ECSAgent/src/ItaEcsAgentCompleteState.h"
#include "app/ecs/ECSAgent/src/ItaEcsAgentSummary.h"
#include "app/ecs/ECSAgent/src/ItaEcsDatapoints.h"
#include "app/ecs/ECSAgent/src/EcsHelpers.h"

using TA_Base_Bus::IEntity;

//
// Constructor
//
// ExceptionChecked
AbstractECSAgent::AbstractECSAgent(ItaEcsAgentSummary& theSummary)

: m_genericAgent(NULL),
  m_operationMode(TA_Base_Core::NotApplicable),
  m_setupHasBeenCalled(false),
  m_cleanupHasBeenCalled(false),
  m_dataPointWriteQueueProcessor(NULL),
  m_ecsZoneDataSource(NULL),
  m_theSummary(theSummary),
  m_isOnStartup(true),
  m_dpBulkPolledInterface (NULL),
  m_dpAccessInterface (NULL),
  m_dnBulkPolledInterface(NULL),
  m_dnAccessInterface(NULL),
  m_scadaProxyFactory(NULL),
  m_summaryProcessor(NULL),	
  m_agentName(""),
  m_dataPointFactory(NULL)
{
	FUNCTION_ENTRY("Constructor");

    m_dataPoints.clear();
	m_dataNodes.clear();

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
AbstractECSAgent::~AbstractECSAgent()
{
	FUNCTION_ENTRY("Destructor");
    TA_ASSERT(m_cleanupHasBeenCalled, "ECSAgent::cleanup() must be called from the derived classes");

	FUNCTION_EXIT;
}

void AbstractECSAgent::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
{
	// stop multi threads accessing the list of DataPoint objects at the same time
	TA_Base_Core::ThreadGuard guard ( m_dataPointListLock );

	dataPointList.clear();

	std::vector< TA_Base_Bus::DataPoint* >::iterator itr;
	for(itr = m_dataPoints.begin(); itr != m_dataPoints.end(); itr++)
	{
		dataPointList[ (*itr)->getEntityKey() ] = *itr;
	}
}

// ExceptionChecked
void AbstractECSAgent::setup(int argc, char* argv[])
    // throw(TA_Core::GenericAgentException, TA_Core::DatabaseException, TA_Core::DataException)
{
	FUNCTION_ENTRY("setup");

    // This flag forces clients to call this method after base class construction and before
    // anything else.
    m_setupHasBeenCalled = true;

    m_dataPoints.clear();

    try
    {

        m_genericAgent = new TA_Base_Bus::GenericAgent(argc, argv, this);
        TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();
        m_agentName = entityData->getAgentName();

        //TD13744, register local entities asap.
        m_scadaProxyFactory->registerLocalEntities(m_agentName, m_genericAgent->getEntityMap());

        // create SummaryProcessor
        m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
        m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());
        
    }
    catch( const TA_Base_Core::InvalidECSConfigurationException& e )
    {
        // The InvalidECSConfigurationException should be raised as a unique alarm.  i.e. only
        // one of these alarms are displayed for each ECSAgent entity.  If we raise
        // a second InvalidECSConfigurationException alarm without closing the first one, only
        // the first one will be displayed.
        //
        // On startup, the AbstractECSAgent should clear this alarm.
        //
        // Consider the following scenario:
        // - InvalidECSConfigurationException raised for problem A.
        // - Agent exits.
        // - Operators make no changes.
        // - Agent executed.
        // - Previous alarm closed.
        // - InvalidECSConfigurationException raised for problem A.
        // - Agent exits.
        // - Operators corrects problem A.
        // - Agent executed.
        // - Previous alarm closed.
        // - InvalidECSConfigurationException raised for problem B.
        // - Agent exits.
        // - Operators corrects problem B.
        // - Agent executed.
        // - Previous alarm closed.
        // - Agent runs successfully.

        m_theSummary.getItaEcsAlarms().raiseEcsInvalidConfiguration(e.what());

        cleanup();
        // prevents run() being called by main
        throw(TA_Base_Core::GenericAgentException(TA_Base_Core::GenericAgentException::CONFIGURATION_ERROR, e.what()));

    }
    catch( const TA_Base_Core::TransactiveException& )
    {
        cleanup();
        throw;
    }
    catch(...)
    {
        cleanup();
        throw;
    }

	FUNCTION_EXIT;
}

void AbstractECSAgent::setupSpecifics()
{
    FUNCTION_ENTRY("setupSpecifics");
    try
    {

		// wenching++
		// bug 775 (TD1838)
		m_theSummary.getItaEcsAgentCompleteState().setDataPoints(m_dataPoints);
		// ++wenching
		// bug 775 (TD1838)

        m_dataPointWriteQueueProcessor = new DataPointWriteQueueProcessor(m_theSummary.getItaEcsCachedConfig());

        processConfiguration();
        validateRuntimeConfiguration();

		m_ecsZoneDataSource = new EcsZoneDataSource(
			m_theSummary.getItaEcsDatapoints(),
            m_theSummary.getItaEcsCachedMaps(), 
			m_theSummary.getItaEcsCachedConfig());

        TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::GLOBAL_PARAMETERS,*this);
    }
    catch( const TA_Base_Core::InvalidECSConfigurationException& e )
    {
        // The InvalidECSConfigurationException should be raised as a unique alarm.  i.e. only
        // one of these alarms are displayed for each ECSAgent entity.  If we raise
        // a second InvalidECSConfigurationException alarm without closing the first one, only
        // the first one will be displayed.
        //
        // On startup, the AbstractECSAgent should clear this alarm.
        //
        // Consider the following scenario:
        // - InvalidECSConfigurationException raised for problem A.
        // - Agent exits.
        // - Operators make no changes.
        // - Agent executed.
        // - Previous alarm closed.
        // - InvalidECSConfigurationException raised for problem A.
        // - Agent exits.
        // - Operators corrects problem A.
        // - Agent executed.
        // - Previous alarm closed.
        // - InvalidECSConfigurationException raised for problem B.
        // - Agent exits.
        // - Operators corrects problem B.
        // - Agent executed.
        // - Previous alarm closed.
        // - Agent runs successfully.

        m_theSummary.getItaEcsAlarms().raiseEcsInvalidConfiguration(e.what());

        cleanup();
        // prevents run() being called by main
        throw(TA_Base_Core::GenericAgentException(TA_Base_Core::GenericAgentException::CONFIGURATION_ERROR, e.what()));
    }
    catch(...)
    {
        cleanup();
        throw;
    }
    FUNCTION_EXIT;
}
// ExceptionChecked
void AbstractECSAgent::cleanup()
{
   	FUNCTION_ENTRY("cleanup");


    // This flag forces clients to call this method before base class destruction.
    m_cleanupHasBeenCalled = true;

    // Destruction order is important here...

    cleanupSpecifics();

    m_theSummary.cleanup();

	// Deactivate the DataPoints' servant.  The GenericAgent
	// will delete the objects from memory space
    m_dataPoints.clear();

   	TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::GLOBAL_PARAMETERS,*this);

    if (m_ecsZoneDataSource)
    {
        delete m_ecsZoneDataSource;
        m_ecsZoneDataSource = NULL;
    }

    if (m_dataPointWriteQueueProcessor)
    {
        delete m_dataPointWriteQueueProcessor;
        m_dataPointWriteQueueProcessor = NULL;
    }

    // if bulk interface is created
	if (m_dpBulkPolledInterface != NULL)
	{
        delete m_dpBulkPolledInterface;
        m_dpBulkPolledInterface = NULL;
	}
    if (m_dpAccessInterface != NULL)
    {
        delete m_dpAccessInterface;
        m_dpAccessInterface = NULL;
    }
	if (m_dnBulkPolledInterface != NULL)
	{
        delete m_dnBulkPolledInterface;
        m_dnBulkPolledInterface = NULL;
	}
    if (m_dnAccessInterface != NULL)
    {
        delete m_dnAccessInterface;
        m_dnAccessInterface = NULL;
    }

    if( m_genericAgent)
    {
        m_genericAgent->deactivateAndDeleteServant();
        m_genericAgent = NULL;
    }

	// set the proxies to monitor mode
	m_scadaProxyFactory->setProxiesToMonitorMode();
	
	// clean up handle to SummaryProcessor
	if ( NULL != m_summaryProcessor )
	{
		TA_Base_Bus::SummaryProcessor::removeInstance(m_summaryProcessor);
		m_summaryProcessor = NULL;
	}
	
	// remove handle to ScadaProxyFactory
	TA_Base_Bus::ScadaProxyFactory::removeInstance();
	m_scadaProxyFactory = NULL;

	FUNCTION_EXIT;
}

// ExceptionChecked
void AbstractECSAgent::processConfiguration()
    // throw(TA_Base_Core::InvalidECSConfigurationException, TA_Core::DatabaseException, TA_Core::DataException)
{
 	FUNCTION_ENTRY("processConfiguration");
    TA_ASSERT(m_setupHasBeenCalled, "ECSAgent::setup() must be called from the derived classes");
    TA_ASSERT(m_genericAgent, "m_genericAgent is NULL");

    TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();

    if (NULL == entityData.get())
    {
        TA_THROW(TA_Base_Core::InvalidECSConfigurationException("Unrecognised EntityData"));
    }

    m_theSummary.getItaEcsCachedConfigInitialiser().initialiseAllConfigAndMaps(entityData,m_dataPoints);
    m_theSummary.getItaEcsDatapoints().initialise();


	if (!TA_Base_Core::RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ))
    {
        // Assumed to be set by Process Management.  Just in case, we set this for rights libraries
        // to prevent asserts.
		std::ostringstream locationKey;
		locationKey << entityData->getLocation();
        TA_Base_Core::RunParams::getInstance().set(RPARAM_LOCATIONKEY, locationKey.str().c_str());
	}

    FUNCTION_EXIT;
}

// ExceptionChecked
void AbstractECSAgent::validateRuntimeConfiguration() 
   // throw(TA_Base_Core::InvalidECSConfigurationException)
{
 	FUNCTION_ENTRY("validateRuntimeConfiguration");
    TA_ASSERT(m_setupHasBeenCalled, "ECSAgent::setup() must be called from the derived classes");

    m_theSummary.getItaEcsCachedMapsInitialiser().validateAllInitialisedMaps();

    validateSpecificRuntimeConfiguration();

    FUNCTION_EXIT;
}


bool AbstractECSAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
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


void AbstractECSAgent::entityCreationComplete()
{
    // loading complete, free some memory
    TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();
}


//
// createEntity
//
// ExceptionChecked
IEntity* AbstractECSAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
    // throw(TA_Base_Core::InvalidECSConfigurationException)
{
 	FUNCTION_ENTRY("createEntity");
    TA_ASSERT(m_setupHasBeenCalled, "ECSAgent::setup() must be called from the derived classes");

    //
	// Note: the objects of interface IEntityData and IEntity are not owned
	// by this specific agent, but by the GenericAgent.  As such no object
	// clean up shall be done with regards to these objects.
	//

	IEntity * theEntity = 0;
	std::string entityDataType = EntityData->getType();

	// if the specified entity is of the type DataPoint
	if ( TA_Base_Core::DataPointEntityData::getStaticType() == entityDataType )
	{
		// create and return the DataPoint entity
       	FUNCTION_EXIT;
		return createDataPointEntity ( EntityData );
	}
	else if(entityDataType == TA_Base_Core::DataNodeEntityData::getStaticType())
	{
		TA_Base_Bus::DataNode* dn = NULL;
		try
		{
            dn = new TA_Base_Bus::DataNode (boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(EntityData), m_persistence);
			
			m_dataNodes.push_back( dn );
			theEntity = dn;
			return theEntity;	// CL-20756 zn++: someone actually miss this line of code.
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
				EntityData->getName().c_str(), EntityData->getKey() );
			
			// To ensure the generic agent is constructed even if one DataNode creation fails
		}
	}
	// otherwise don't care
	else
	{
		// do nothing
	}

	FUNCTION_EXIT;
    return NULL;
}

// ExceptionChecked
IEntity* AbstractECSAgent::createDataPointEntity ( TA_Base_Core::IEntityDataPtr EntityData )
    // throw(TA_Base_Core::InvalidECSConfigurationException)
{
 	FUNCTION_ENTRY("createDataPointEntity");
    TA_ASSERT(m_setupHasBeenCalled, "ECSAgent::setup() must be called from the derived classes");

    // cast the EntityData to DataPointEntityData
	TA_Base_Core::DataPointEntityDataPtr dataPointEntityData =
        boost::dynamic_pointer_cast< TA_Base_Core::DataPointEntityData > ( EntityData );

	TA_Base_Bus::DataPoint* theEntity = m_dataPointFactory->createDataPoint ( dataPointEntityData );

    if (!theEntity)
    {
        std::ostringstream msg;
        msg << "Child entity datapoint " << dataPointEntityData->getName()
            << " has not configured correctly and cannot be created";
        TA_THROW(TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()));
    }

    m_dataPoints.push_back(theEntity);

    FUNCTION_EXIT;
    return theEntity;
}

//
// agentTerminate
//
// ExceptionChecked
void AbstractECSAgent::agentTerminate()
{
 	FUNCTION_ENTRY("agentTerminate");
    TA_ASSERT(m_setupHasBeenCalled, "ECSAgent::setup() must be called from the derived classes");
    TA_ASSERT(m_dataPointWriteQueueProcessor, "m_dataPointWriteQueueProcessor is NULL");

    stopSpecificThreads();

    // Stop common threads for DataPointCorbaDef_Impl
    m_dataPointWriteQueueProcessor->terminateAndWait();

    // if bulk interface is created
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


	FUNCTION_EXIT;
}


//
// agentSetMonitor
//
// ExceptionChecked
void AbstractECSAgent::agentSetMonitor()
{
 	FUNCTION_ENTRY("agentSetMonitor");
    TA_ASSERT(m_setupHasBeenCalled, "ECSAgent::setup() must be called from the derived classes");

    TA_Base_Core::ThreadGuard guard( m_lockForOperationModes );

    if (m_operationMode == TA_Base_Core::Monitor)
    {
        return;
    }

    m_operationMode=TA_Base_Core::Monitor;

	// For each of the data points created for this agent, set the data point to monitor mode
	DataPointsIt it = m_dataPoints.begin();
	for ( ; it!=m_dataPoints.end(); ++it)
    {
        TA_ASSERT(*it, "DataPoint* in m_dataPoints is NULL");
		(*it)->setToMonitorMode();
	}

	// set all datanodes to monitor mode
	for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
	{
		(*itr)->setOperationMode( false );
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "AbstractECSAgent::agentSetMonitor() - finished set dp and dn to monitor");
	
	// set the proxies to monitor mode
	m_scadaProxyFactory->setProxiesToMonitorMode();

    if (m_dpBulkPolledInterface  != NULL)
	{
		m_dpBulkPolledInterface->setToMonitor();
	}
    if (m_dpAccessInterface != NULL)
    {
        m_dpAccessInterface->setToMonitor();
    }
	if (m_dnBulkPolledInterface != NULL)
	{
		m_dnBulkPolledInterface->setToMonitor();
	}
    if (m_dnAccessInterface != NULL)
    {
        m_dnAccessInterface->setToMonitor();
    }

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "AbstractECSAgent::agentSetMonitor() - starts set ecs specifics");

	m_ecsZoneDataSource->setMonitor(m_isOnStartup);

	m_theSummary.setMonitor(m_isOnStartup);

	agentSetMonitorSpecifics();

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Operating in Monitor mode");

	FUNCTION_EXIT;
}


//
// agentSetControl
//
// ExceptionChecked
void AbstractECSAgent::agentSetControl()
{
 	FUNCTION_ENTRY("agentSetControl");
    TA_ASSERT(m_setupHasBeenCalled, "ECSAgent::setup() must be called from the derived classes");

    TA_Base_Core::ThreadGuard guard( m_lockForOperationModes );

    if (m_operationMode == TA_Base_Core::Control)
    {
        return;
    }

    m_operationMode=TA_Base_Core::Control;

   	// For each of the data points created for this agent, set the data point to control mode
	DataPointsIt it = m_dataPoints.begin();
	for ( ; it!=m_dataPoints.end(); ++it)
    {
        TA_ASSERT(*it, "DataPoint* in m_dataPoints is NULL");
		(*it)->setToControlMode();
	}

	// set all datanodes to control mode
	for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
	{
		(*itr)->setOperationMode( true );
	}

	// set the proxies to control mode
	m_scadaProxyFactory->setProxiesToControlMode();	

    if (m_dpBulkPolledInterface  != NULL)
	{
		m_dpBulkPolledInterface->setToControl();
	}

    if (m_dpAccessInterface != NULL)
    {
        m_dpAccessInterface->setToControl();
    }

	if (m_dnBulkPolledInterface != NULL)
	{
		m_dnBulkPolledInterface->setToControl();
	}

    if (m_dnAccessInterface != NULL)
    {
        m_dnAccessInterface->setToControl();
    }

	agentSetControlSpecifics();

	m_theSummary.setControl(m_isOnStartup);

	m_ecsZoneDataSource->setControl(m_isOnStartup);
    
    if (m_isOnStartup)
    {
        // Close existing alarms 
        m_theSummary.getItaEcsAlarms().closeEcsInvalidConfiguration(true); // isForced=true
    } // (m_isOnStartup)

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Operating in Control mode");

	FUNCTION_EXIT;
}

//
// notifyGroupOffline
//
void AbstractECSAgent::notifyGroupOffline( const std::string& group )
{
 	FUNCTION_ENTRY("notifyGroupOffline");

    TA_ASSERT(m_setupHasBeenCalled, "ECSAgent::setup() must be called from the derived classes");
    notifyGroupOffline(group); 

    FUNCTION_EXIT;
}

//
// notifyGroupOnline
//
void AbstractECSAgent::notifyGroupOnline( const std::string& group )
{
 	FUNCTION_ENTRY("notifyGroupOnline");

    TA_ASSERT(m_setupHasBeenCalled, "ECSAgent::setup() must be called from the derived classes");
    notifyGroupOnline(group);

    FUNCTION_EXIT;
}



//
// registerForStateUpdates
//
void AbstractECSAgent::registerForStateUpdates()
{
 	FUNCTION_ENTRY("registerForStateUpdates");

    // BUG FIX TD 1494 *******************************************************************
    // The problem was that the Agent's location key was requested from the CachedConfig
    // before it was set, and also the CachedMaps ASSERT was removed because of startup
    // alarming sending scenarios and so the call to subscribeToCommsMessage in
    // registerForMonitorMessages() would assert with a locationKey set to 0.  This only
    // occurs when the Agent starts up in monitor mode.
    //
    // For the ECS Agent (in monitor mode), we register to Comms messages for
    // state updates, unfortunately, we need to have available the location
    // key at this time but there is no way of obtaining it from the entity
    // data inside the generic agent unless we create a new one ourselves.
    // The reason is that the Generic Agent is pointer member is still null
    // when we get here.
    //
    // Instead we will not call registerForMonitorMessages() /
    // registerForControlMessages() at this point, but instead rely upon a later
    // call to agentSetControl() / agentSetMonitor() in the run() method.
    // This way, we delay the subscription of update/comms messages until we
    // definitely have the locationKey available.
    // ******************************************************************* BUG FIX TD 1494

    FUNCTION_EXIT;
}





//
// processOnlineUpdate
//
// ExceptionChecked
void AbstractECSAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
 	FUNCTION_ENTRY("processOnlineUpdate");
    TA_ASSERT(m_setupHasBeenCalled, "ECSAgent::setup() must be called from the derived classes");



    unsigned long updateEventPKey = updateEvent.getKey();

	if ( m_theSummary.getItaEcsCachedConfig().getAgentKey() == updateEventPKey )
	{
		// update ECS Agent configuration
        // Can only be an Update

        // get configuration data for this ECS Agent
        TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();

        TA_ASSERT(entityData.get()!=NULL, "Unexpected change in entityData class since initialisation");
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "Processing ECSAgent online update");

        // force a reread of the configuration
        entityData->invalidate();

        try
        {
            m_theSummary.getItaEcsCachedConfigInitialiser().processEntityData();
        } // try block
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            // Serious misconfiguration
            killMeAndRaiseAlarm();
        }
        catch(TA_Base_Core::InvalidECSConfigurationException& e)
        {
            m_theSummary.getItaEcsAlarms().raiseEcsInvalidConfiguration(e.what());
            // Continue with previous configuration.  These exceptions are thrown when
            // trying to change a CachedConfig parameter.
        }
        catch(...)
        {
            // We can expect any one of the following exceptions, which reflect bad configuration:
            //
            //   DataException
            //   DatabaseException
            //        
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
                "OnlineUpdate processing failed");
        }

		return;
	}
	else
	{
//		// Must be for the child entities (datapoints)
//        // Can be an Update / Create / Delete modification.
//        updateDataPointConfiguration ( updateEvent );
	}

    FUNCTION_EXIT;
}

// ExceptionChecked
void AbstractECSAgent::updateDataPointConfiguration ( const TA_Base_Core::ConfigUpdateDetails & updateEvent )
{
 	FUNCTION_ENTRY("updateDataPointConfiguration");
    TA_ASSERT(m_setupHasBeenCalled, "ECSAgent::setup() must be called from the derived classes");

    try
    {
	    TA_Base_Bus::DataPoint* theDataPoint = 0;
	    unsigned long updateEventPKey = updateEvent.getKey();

        DataPointsIt it = m_dataPoints.begin();
        for (; it!=m_dataPoints.end(); ++it)
        {
            TA_ASSERT(*it, "DataPoint* in m_dataPoints is NULL");
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
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, 
                        "Online creation/deletion of datapoints are not supported");
                }
		    }
        }
        else
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, 
                "Online update for unrecognised child entity ignored");
        }
    } // try block
    catch(...)
    {
        // We can expect any one of the following exceptions, which reflect bad configuration:
        //
        //   DataException
        //   DatabaseException
        //

        killMeAndRaiseAlarm();
    }

    FUNCTION_EXIT;
}



//
// run
//
// ExceptionChecked
void AbstractECSAgent::run()
{
 	FUNCTION_ENTRY("run");
    TA_ASSERT(m_setupHasBeenCalled, "ECSAgent::setup() must be called from the derived classes");
    TA_ASSERT(m_genericAgent, "m_genericAgent is NULL");
    TA_ASSERT(m_dataPointWriteQueueProcessor, "m_dataPointWriteQueueProcessor is NULL");


    // start common threads for DataPointCorbaDef_Impl
    if (m_dataPointWriteQueueProcessor->getCurrentState() != TA_Base_Core::Thread::THREAD_STATE_RUNNING)
    {
        m_dataPointWriteQueueProcessor->start();
    }

	// if bulk interface is setup
	m_dpBulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl ( m_agentName, this );
    m_dpAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl(m_agentName, this);
	m_dnBulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl ( m_agentName, this );
    m_dnAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl(m_agentName, this);

    // check the runtime parameter OperationMode
	std::string operationMode ( "" );
	operationMode = TA_Base_Core::RunParams::getInstance().get ( RPARAM_OPERATIONMODE );

	// if defined as "Control"
	if ( ( 0 < operationMode.length() ) && ( RPARAM_CONTROL == operationMode ) )
	{
		agentSetControl();
	}
	else
	{
		agentSetMonitor();
	}

    // liuyu++ start agent specific threads
    startSpecificThreads();
    // ++liuyu start agent specific threads

    m_theSummary.getItaEcsCachedConfigInitialiser().setAgentInRuntime(true);
    m_isOnStartup = false;

    m_genericAgent->run();

    m_theSummary.getItaEcsCachedConfigInitialiser().setAgentInRuntime(false);


	FUNCTION_EXIT;
}

// ExceptionChecked
void AbstractECSAgent::killMeAndRaiseAlarm()
{
 	FUNCTION_ENTRY("killMeAndRaiseAlarm");
    TA_ASSERT(m_setupHasBeenCalled, "ECSAgent::setup() must be called from the derived classes");
    TA_ASSERT(m_genericAgent, "m_genericAgent is NULL");

    m_theSummary.getItaEcsAlarms().raiseEcsInvalidConfiguration(
        "ECSAgent in runtime operation will be terminated due to inconsistent ECS configuration");

    m_genericAgent->onTerminate();

    FUNCTION_EXIT;
}

TA_Base_Bus::DataPoint* AbstractECSAgent::getDataPoint ( unsigned long entityKey )
{
	// find if there is a data point whose pkey matches with the specified entity key
    DataPointsIt it = m_dataPoints.begin();
    for (; it!=m_dataPoints.end(); ++it)
    {
        TA_ASSERT(*it, "DataPoint* in m_dataPoints is NULL");
        if ((*it)->getEntityKey() == entityKey)
        {
            return (*it);
        }
    }

    // Not found
    return NULL;
}

TA_Base_Bus::DataPointFactory* AbstractECSAgent::getDataPointFactory()
{
    TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");
    
    return m_dataPointFactory;
}


void AbstractECSAgent::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
    if (updateEvent.getModifications() == TA_Base_Core::Update)
	{
        if (updateEvent.getType() == TA_Base_Core::GLOBAL_PARAMETERS)
        {
            m_theSummary.getItaEcsCachedConfigInitialiser().processGlobalParameters();
        }
    }
}

TA_Base_Bus::DataNode * AbstractECSAgent::getDataNode( unsigned long entityKey )
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

void AbstractECSAgent::getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
{
	TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
	
	dataNodeList.clear();
	for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
	{
		dataNodeList[ (*itr)->getEntityKey() ] = *itr;
	}        
}
