/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/StationECSAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include "app/ecs/ECSAgent/src/EcsZoneDataSource.h"
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
#include "app/ecs/ECSAgent/src/StationECSAgent.h"
#include "app/ecs/ECSAgent/src/IscsAliveWorker.h"
#include "app/ecs/ECSAgent/src/DataPointWriteQueueProcessor.h"
#include "app/ecs/ECSAgent/src/StationModeManager.h"
#include "app/ecs/ECSAgent/src/AutoModeMonitor.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Bus::IEntity;


//
// Constructor
//
// ExceptionChecked
StationECSAgent::StationECSAgent(
    int argc, 
    char* argv[],
    ItaEcsAgentSummary& theSummary)
: AbstractECSAgent(theSummary),
  m_stationModeManager(NULL),
  m_autoModeMonitor(NULL),
  m_iscsAliveWorker(NULL)
{
	FUNCTION_ENTRY("Constructor");

    setup(argc,argv);

	FUNCTION_EXIT;
}


//
// Destructor
//
// ExceptionChecked
StationECSAgent::~StationECSAgent()
{
	FUNCTION_ENTRY("Destructor");

    cleanup();

	FUNCTION_EXIT;
}

//
// notifyGroupOffline
//
void StationECSAgent::notifyGroupOffline( const std::string& group )
{
 	FUNCTION_ENTRY("notifyGroupOffline");

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "20364 notifyGroupOffline" );

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, 
        "Group %s is offline", group.c_str());
    
    // ToDo - concerns about starting up when group is Offline

	if( m_theSummary.getItaEcsCachedConfig().getOccGroupName() == group )
	{
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, 
            "Notify cached config initializer to offline");

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "20364 change Occ Offline" );
        
        m_theSummary.getItaEcsCachedConfigInitialiser().setIsOccOnline(false);
		m_ecsZoneDataSource->updateAllEcsZoneDataPoints();
	}

    FUNCTION_EXIT;
}

//
// notifyGroupOnline
//
void StationECSAgent::notifyGroupOnline( const std::string& group )
{
 	FUNCTION_ENTRY("notifyGroupOnline");

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "20364 notifyGroupOnline" );

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, 
        "Group %s is online", group.c_str());

    // ToDo - concerns about starting up when group is Offline

	if( m_theSummary.getItaEcsCachedConfig().getOccGroupName() == group )
	{
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, 
            "Notify cached config initializer to online");

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "20364 change Occ Online" );
        
        m_theSummary.getItaEcsCachedConfigInitialiser().setIsOccOnline(true);
	}

    FUNCTION_EXIT;
}

// ExceptionChecked
void StationECSAgent::setupSpecifics()
    //throw(TA_Core::GenericAgentException, TA_Base_Core::InvalidECSConfigurationException, TA_Base_Core::CachedMappingNotFoundException)
{
    FUNCTION_ENTRY("setupSpecifics");
    AbstractECSAgent::setupSpecifics();

    //TA_ASSERT(m_dataPointWriteQueueProcessor, "m_dataPointWriteQueueProcessor is NULL");
	if (NULL == m_dataPointWriteQueueProcessor)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,"m_dataPointWriteQueueProcessor is NULL" );
		return;
	}
   
	try
	{
		m_iscsAliveWorker = new IscsAliveWorker( 
			m_theSummary.getItaEcsCachedConfig(), 
			m_theSummary.getItaEcsDatapoints()); 

		m_stationModeManager = new StationModeManager( 
			m_theSummary.getItaEcsCachedConfig() , 
			m_theSummary.getItaEcsAuditing(), 
			m_theSummary.getItaEcsAgentCompleteState(), 
			m_theSummary.getItaEcsDatapoints());
	}
	catch ( ... )
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Unknown Exception");		
	}


	m_autoModeMonitor = new AutoModeMonitor(
		m_theSummary.getItaEcsCachedConfig(), 
		m_theSummary.getItaEcsDatapoints()); 
	
    m_dataPointWriteQueueProcessor->registerOiAssociation(
        m_theSummary.getItaEcsCachedConfig().getMyMasterModeLockControlDataPoint(), 
        m_theSummary.getItaEcsCachedConfig().getMyMasterModeLockDataPoint());
    
    
    FUNCTION_EXIT;
}

// ExceptionChecked
void StationECSAgent::cleanupSpecifics()
{
    FUNCTION_ENTRY("cleanupSpecifics");
    //TA_ASSERT(m_dataPointWriteQueueProcessor, "m_dataPointWriteQueueProcessor is NULL");
	if (NULL == m_dataPointWriteQueueProcessor)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,"m_dataPointWriteQueueProcessor is NULL" );
		return;
	}

    m_dataPointWriteQueueProcessor->deregisterOiAssociation(
        m_theSummary.getItaEcsCachedConfig().getMyMasterModeLockControlDataPoint());
    

    if( m_stationModeManager)
    {
        delete m_stationModeManager;
        m_stationModeManager = NULL;
    }

    if( m_autoModeMonitor)
    {
        delete m_autoModeMonitor;
        m_autoModeMonitor = NULL;
    }

    if( m_iscsAliveWorker)
    {
        delete m_iscsAliveWorker;
        m_iscsAliveWorker = NULL;
    }

    FUNCTION_EXIT;
}

// ExceptionChecked
void StationECSAgent::processSpecificConfiguration()
    //throw(TA_Base_Core::InvalidECSConfigurationException, TA_Core::DatabaseException, TA_Core::DataException)
{
    FUNCTION_ENTRY("processSpecificConfiguration");

    FUNCTION_EXIT;
}


// ExceptionChecked
void StationECSAgent::validateSpecificRuntimeConfiguration()
    //throw(TA_Base_Core::InvalidECSConfigurationException)
{
    FUNCTION_ENTRY("validateSpecificRuntimeConfiguration");

    FUNCTION_EXIT;
}
void StationECSAgent::stopSpecificThreads()
{ 
    m_iscsAliveWorker->terminateAndWait();
    m_stationModeManager->terminateAndWait();
}
// liuyu++ start master/station specific threads
void StationECSAgent::startSpecificThreads()
{
    m_iscsAliveWorker->start();
    m_stationModeManager->start();
}
// ++liuyu start master/station specific threads


void StationECSAgent::agentSetControlSpecifics()
{
    FUNCTION_ENTRY("agentSetControlSpecifics");

    m_iscsAliveWorker->setControl(m_isOnStartup);
    m_autoModeMonitor->setControl(m_isOnStartup);
    m_stationModeManager->setControl(m_isOnStartup);

	FUNCTION_EXIT;
}

void StationECSAgent::agentSetMonitorSpecifics()
{
    FUNCTION_ENTRY("agentSetMonitorSpecifics");

    //stopSpecificThreads(); // moved to AbstraceEcsAgent

    m_stationModeManager->setMonitor(m_isOnStartup);
    m_autoModeMonitor->setMonitor(m_isOnStartup);
    m_iscsAliveWorker->setMonitor(m_isOnStartup);

	FUNCTION_EXIT;

}
