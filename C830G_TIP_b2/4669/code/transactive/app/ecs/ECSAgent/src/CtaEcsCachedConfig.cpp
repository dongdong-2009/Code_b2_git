/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/CtaEcsCachedConfig.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/01/08 14:06:20 $
  * Last modified by:  $Author: builder $
  *
  */
// CtaEcsCachedConfig.cpp: implementation of the CtaEcsCachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include <string>
#include <vector>

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/data_access_interface/src/GlobalParameterAccessFactory.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/ecs/ecs_agent/IDL/src/CommonEcsAgentCorbaDef.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "app/ecs/ECSAgent/src/CtaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedMapsInitialiser.h"
#include "app/ecs/ECSAgent/src/EcsHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CtaEcsCachedConfig::CtaEcsCachedConfig(ItaEcsCachedMapsInitialiser& cachedMapsInitialiserInstance)
 : 
   // Common
   m_agentKey               (0),
   m_agentName              (""),
   m_agentTypeKey           (0),
   m_agentLocationKey       (0),
   m_agentSubsystemKey      (0),
   m_isOccOnline            (true),
   m_occGroupName           ("OCC"),
   m_agentInRuntime         (false),
   m_cachedMapsInitialiserInstance(cachedMapsInitialiserInstance),
   m_spareATSAgentKey       (0),
   m_spareATSAgentName      (""),
   m_assetName				("")

{
    FUNCTION_ENTRY("CtaEcsCachedConfig");
    //m_decisionComponentIds.clear();
    m_keyToObserversMap.clear();

    // Initialise map to flag all members (except for AgentX) as uninitialised

    setMemberInitialised(&m_agentKey);
    setMemberInitialised(&m_agentTypeKey);
    setMemberInitialised(&m_agentLocationKey);
    setMemberInitialised(&m_agentSubsystemKey);
    setMemberInitialised(&m_agentInRuntime);
    setMemberInitialised(&m_isOccOnline); // hack-currently never get callback for notifyGroupOnline/Offline
    setMemberInitialised(&m_occGroupName);

    // We don't need to explicitly set the other members to false as the 
    // default construction for the bool used when the map is accessed with an
    // unrecognised member pointer will always be false.

    // Refer to Google Groups: http://groups.google.com.au/groups?hl=en&lr=&ie=UTF-8&threadm=c1b53j%249sl%241%40newshispeed.ch&rnum=29&prev=/groups%3Fq%3D%2522default%2Bconstructor%2522%2Bbool%26hl%3Den%26lr%3D%26ie%3DUTF-8%26start%3D20%26sa%3DN
    FUNCTION_EXIT;
}

CtaEcsCachedConfig::~CtaEcsCachedConfig()
{
	FUNCTION_ENTRY("~CtaEcsCachedConfig");

    m_keyToObserversMap.clear();
    m_dataPoints.clear();

    FUNCTION_EXIT;
}



/**
 * attachObserver
 *
 * Adds the ItaEcsCachedConfigObserver instance to the collection of observers.  If the observer 
 * already exists for the key, no change will be made.
 *
 * @param observer pointer to the ItaEcsCachedConfigObserver instance
 *
 * @param key the EItaEcsCachedConfigItemKey the observer is interested in
 *
 */
void CtaEcsCachedConfig::attachObserver(ItaEcsCachedConfigObserver* observer, EItaEcsCachedConfigItemKey key)
{
	FUNCTION_ENTRY("attachObserver");

    // Search for all observers associated with the key
    std::pair<KeyToObserversMapIt, KeyToObserversMapIt> range =
        m_keyToObserversMap.equal_range (key);

    bool found(false);
    for ( ; range.first != range.second; ++range.first)
    {
        if ( (range.first)->second == observer)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForObserverMap );
        m_keyToObserversMap.insert(key2observer(key, observer));
    }
    FUNCTION_EXIT;
}


/**
 * detachObserver
 *
 * Removes all instances of the ItaEcsCachedConfigObserver pointer in the observer collection.  The
 * observer will be removed for all keys.
 *
 */
void CtaEcsCachedConfig::detachObserver(ItaEcsCachedConfigObserver* observer)
{
	FUNCTION_ENTRY("detachObserver");
    TA_Base_Core::ThreadGuard guard( m_lockForObserverMap );

    KeyToObserversMapIt it = m_keyToObserversMap.begin();
    while (it != m_keyToObserversMap.end())
    {
        if (it->second == observer)
        {
            m_keyToObserversMap.erase(it++);
        }
        else
        {
            ++it;
        }
    }
    FUNCTION_EXIT;
}

/**
 * notifyInterestedObservers
 *
 * Calls the processItaEcsCachedConfigUpdate() method on all registered observers interested in the 
 * specified key.  
 *
 * @param key used to call specific observers
 *
 */
void CtaEcsCachedConfig::notifyInterestedObservers(EItaEcsCachedConfigItemKey key)
{
	FUNCTION_ENTRY("notifyInterestedObservers");
    TA_Base_Core::ThreadGuard guard( m_lockForObserverMap );

    // Search for all observers associated with a key
    std::pair<KeyToObserversMapIt, KeyToObserversMapIt> range =
        m_keyToObserversMap.equal_range (key);

    for ( ; range.first != range.second; ++range.first)
    {
        try
        {
            (range.first)->second->processItaEcsCachedConfigUpdate(key);
        }
        catch(...)
        {
            // Do nothing and continue
        }
    }
    FUNCTION_EXIT;
}


boost::shared_ptr<TA_Base_Core::DataPointEntityData> CtaEcsCachedConfig::getEntityBySqlToken(const std::string& sqlToken)
    // throw InvalidECSConfigurationException
{
	FUNCTION_ENTRY("getEntityBySqlToken");
    TA_Base_Core::IEntityDataList entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocationWithNameLikeToken
        ( TA_Base_Core::DataPointEntityData::getStaticType(), 
          getAgentLocationKey(), 
          sqlToken );
    
    if ( entities.size() != 1 )
    {
        // Not found
        std::ostringstream msg;
        msg << "Could not resolve unique datapoint for sqlToken " 
            << sqlToken;
        TA_THROW(TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()));
    }

    TA_Base_Core::DataPointEntityData* dpEntity = dynamic_cast<TA_Base_Core::DataPointEntityData*>(entities.front());
    TA_ASSERT(dpEntity != NULL, "The defined entity is not datapoint")

    FUNCTION_EXIT;
    return boost::shared_ptr<TA_Base_Core::DataPointEntityData>(dpEntity);
}


// ExceptionChecked
DataPoints CtaEcsCachedConfig::getDataPointsByToken(const std::string& token)
    // throw InvalidECSConfigurationException
{
    FUNCTION_ENTRY("getDataPointsByToken");

    DataPoints matches;
    // Search for string matches
    DataPointsIt it = m_dataPoints.begin();
    for (; it!=m_dataPoints.end(); ++it)
    {
        std::string name = (*it)->getDataPointName();

        if (name.find(token) != std::string::npos)
        {

            // Ex: to ignore "..EmergencyApplied" if token is "Emergency"
            // Assuming the token is the last string in the name
            if ((name.find(token) + token.length()) == name.length())
            {
                matches.push_back(*it);
            }
        }
    }

    if(matches.empty())
    {
        // Not found
        std::ostringstream msg;
        msg << "Could not resolve any datapoints for token " 
            << token;
        TA_THROW(TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()));    
    }

    FUNCTION_EXIT;
    return matches;
}

TA_Base_Bus::DataPoint* CtaEcsCachedConfig::getDataPointByToken(const std::string& token)
    // throw InvalidECSConfigurationException
{
    FUNCTION_ENTRY("getDataPointByToken");

    TA_Base_Bus::DataPoint* dataPoint = NULL;

    DataPoints matches;
    // Search for string matches
    DataPointsIt it = m_dataPoints.begin();
    for (; it!=m_dataPoints.end(); ++it)
    {
        std::string name = (*it)->getDataPointName();

        if (name.find(token) != std::string::npos)
        {
            if (!dataPoint)
            {
                dataPoint = (*it);
                // continue on for validation
            }
            else
            {
                // Already found, therefore we have found a duplicate
                std::ostringstream msg;
                msg << "Could not resolve unique datapoint for token " 
                    << token;
                TA_THROW(TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()));
            }           
        }
    }

    if(!dataPoint)
    {
        // Not found
        std::ostringstream msg;
        msg << "Could not resolve unique datapoint for token " 
            << token;
        TA_THROW(TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()));
    }     

    FUNCTION_EXIT;
    return dataPoint;
}

void CtaEcsCachedConfig::initialiseAllConfigAndMaps(TA_Base_Core::IEntityDataPtr entityData, const DataPoints& datapoints)
{
	FUNCTION_ENTRY("initialiseAllConfigAndMaps");
    setEntityData(entityData);
    setDataPoints(datapoints);

    initialiseCachedMaps();
    processEntityData();
    processGlobalParameters();
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setEntityData(TA_Base_Core::IEntityDataPtr entityData)
{
	FUNCTION_ENTRY("setEntityData");
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

        m_masterEntityData = boost::dynamic_pointer_cast<TA_Base_Core::MasterECSAgentEntityData>(entityData);
        m_stationEntityData = boost::dynamic_pointer_cast<TA_Base_Core::StationECSAgentEntityData>(entityData);

        setIsMaster(m_masterEntityData.get()!=NULL);
    }

    // Process these immediately so we can raise alarms properly.
    
    setAgentKey(entityData->getKey());
    setAgentTypeKey(entityData->getTypeKey());
    setAgentLocationKey(entityData->getLocation());
    setAgentSubsystemKey(entityData->getSubsystem());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setDataPoints(const DataPoints& datapoints)
{
	FUNCTION_ENTRY("setDataPoints");
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

    m_dataPoints = datapoints;
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processGlobalParameters()
{
	FUNCTION_ENTRY("processGlobalParameters");
    processThresholdStallTimeInSecs();
    processThresholdTemperature();
    processTemperatureDeadband();
    processCongestionUsesStallTime();
    processCongestionUsesTemperature();
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processEntityData()
{
	FUNCTION_ENTRY("processEntityData");

    processWorkerThreadsSleepTimeInMSecs();
    processMaxModeExecutionTimeInSecs();
    processSpareATSAgentKey();
    processLocalATSAgentKey();
	processAssetName();

    if (getIsMaster())
    {
        processSelectionTimeoutInSecs();
        processNormalMasterMode();

        processMyZonesCongestionToken();
        processMyZonesEmergencyToken();
        //processMyZonesCongestionAppliedToken(); // Datapoint is removed
        processMyZonesEmergencyAppliedToken();
        processMyZonesEvacuationSelectedToken();
        processMyZonesAirflowSelectedToken();
        processMyZonesEvacuationAppliedToken();
        processMyZonesAirflowAppliedToken();
        processMyModeStatusToken();
        processMyModeInProgressToken();

        processMasterModeLocksSqlToken();
        processSelectedMasterModesSqlToken();
        processSelectedOverridingStationModesSqlToken();
        processLastAppliedMasterModesSqlToken();
        processLastAppliedStationModesSqlToken();
        processLocalModesSqlToken();
    }
    else // Station
    {
        processNormalStationMode();
        processIscsAlivePollTimeInSecs();
    
        processMyZonesCongestionToken();
        processMyZonesEmergencyToken();
        processMyMasterModeLockToken();
        processMyMasterModeLockControlToken();
        processMySelectedMasterModeToken();
        processMySelectedOverridingStationModeToken();
        processMyLastAppliedMasterModeToken();
        processMyLastAppliedStationModeToken();

        processModeStatusSqlToken();
        processModeControlSqlToken();
        processModeInProgressSqlToken();
        processIscsAliveSqlToken();   
        processAutoModeSqlToken();   
        processLocalModeSqlToken();
    }
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::initialiseCachedMaps()
{
	FUNCTION_ENTRY("initialiseCachedMaps");
    m_cachedMapsInitialiserInstance.refreshLocationNames();
    m_cachedMapsInitialiserInstance.refreshZoneRecords();

    if (getIsMaster())
    {       
        m_cachedMapsInitialiserInstance.refreshStationModeRecords();
        m_cachedMapsInitialiserInstance.refreshMasterModeRecords();
        m_cachedMapsInitialiserInstance.refreshStationEcsAgentCorbaNames();
    }
    else // Station
    {
        m_cachedMapsInitialiserInstance.refreshStationModeRecords(getAgentLocationKey());

        // can use congestion/emergency datapoint to fetch the zone ids
        std::string  entityParameterValue = m_stationEntityData->getMyZonesCongestionToken();
        m_cachedMapsInitialiserInstance.refreshZoneIdInScope(getDataPointsByToken(entityParameterValue));
    }
    FUNCTION_EXIT;
}



///////////////////////////////////////////////////////////////////////////
// Global Parameters 
///////////////////////////////////////////////////////////////////////////


void CtaEcsCachedConfig::processThresholdStallTimeInSecs()
{
	FUNCTION_ENTRY("processThresholdStallTimeInSecs");
    std::string globalParameterValue = 
        TA_Base_Core::GlobalParameterAccessFactory::getInstance().getGlobalParameterValueFromNameAndSubsystem
        (TA_IRS_Bus::ECS_GLOBAL_PARAMETER_THRESHOLD_STALL_TIME, getAgentSubsystemKey());
    
    unsigned long convertedValue = atol(globalParameterValue.c_str());

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_thresholdStallTimeInSecs = convertedValue;
    setMemberInitialised(&m_thresholdStallTimeInSecs);
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processThresholdTemperature()
{
	FUNCTION_ENTRY("processThresholdTemperature");
    std::string globalParameterValue = 
        TA_Base_Core::GlobalParameterAccessFactory::getInstance().getGlobalParameterValueFromNameAndSubsystem
        (TA_IRS_Bus::ECS_GLOBAL_PARAMETER_THRESHOLD_TEMPERATURE, getAgentSubsystemKey());
    
    unsigned long convertedValue = atol(globalParameterValue.c_str());

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_thresholdTemperature = convertedValue;
    setMemberInitialised(&m_thresholdTemperature);
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processTemperatureDeadband()
{
	FUNCTION_ENTRY("processTemperatureDeadband");
    std::string globalParameterValue = 
        TA_Base_Core::GlobalParameterAccessFactory::getInstance().getGlobalParameterValueFromNameAndSubsystem
        (TA_IRS_Bus::ECS_GLOBAL_PARAMETER_TEMPERATURE_DEADBAND, getAgentSubsystemKey());
    
    unsigned long convertedValue = atol(globalParameterValue.c_str());
    unsigned long thresholdTemperature = getThresholdTemperature();
    
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_temperatureDeadbandLowerLimit = thresholdTemperature - convertedValue;
    setMemberInitialised(&m_temperatureDeadbandLowerLimit);

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processCongestionUsesStallTime()
{
	FUNCTION_ENTRY("processCongestionUsesStallTime");
    std::string globalParameterValue = 
        TA_Base_Core::GlobalParameterAccessFactory::getInstance().getGlobalParameterValueFromNameAndSubsystem
        (TA_IRS_Bus::ECS_GLOBAL_PARAMETER_CONGESTION_USES_STALL_TIME, getAgentSubsystemKey());

    //Implicitly threadsafe: TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_congestionUsesStallTime = EcsHelpers::isTrue(globalParameterValue.c_str());
    setMemberInitialised(&m_congestionUsesStallTime);
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processCongestionUsesTemperature()
{
	FUNCTION_ENTRY("processCongestionUsesTemperature");
    std::string globalParameterValue = 
        TA_Base_Core::GlobalParameterAccessFactory::getInstance().getGlobalParameterValueFromNameAndSubsystem
        (TA_IRS_Bus::ECS_GLOBAL_PARAMETER_CONGESTION_USES_TEMPERATURE, getAgentSubsystemKey());

    //Implicitly threadsafe: TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_congestionUsesTemperature = EcsHelpers::isTrue(globalParameterValue.c_str());
    setMemberInitialised(&m_congestionUsesTemperature);
    FUNCTION_EXIT;
}



///////////////////////////////////////////////////////////////////////////
// Common ECSAgent entity parameters
///////////////////////////////////////////////////////////////////////////

void CtaEcsCachedConfig::processWorkerThreadsSleepTimeInMSecs()
{
	FUNCTION_ENTRY("processWorkerThreadsSleepTimeInMSecs");
    unsigned long entityParameterValue;
    if (m_masterEntityData)
    {
        entityParameterValue = m_masterEntityData->getWorkerThreadsSleepTimeInMSecs();
    }
    else if (m_stationEntityData)
    {
        entityParameterValue = m_stationEntityData->getWorkerThreadsSleepTimeInMSecs();
    }
    else
    {
        TA_ASSERT(false, "CtaEcsCachedConfig not initialised with EntityData");
    }



    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

    if (isMemberInitialised(&m_workerThreadsSleepTimeInMSecs) && 
        m_workerThreadsSleepTimeInMSecs == entityParameterValue)
    {
        // No change
		FUNCTION_EXIT;
        return;
    }

    m_workerThreadsSleepTimeInMSecs = entityParameterValue;
    setMemberInitialised(&m_workerThreadsSleepTimeInMSecs);
    notifyInterestedObservers(CONFIG_WORKER_THREADS_SLEEP_TIME_IN_MSECS);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter WorkerThreadsSleepTimeInMSecs=%ld", m_workerThreadsSleepTimeInMSecs);
    FUNCTION_EXIT;
}


void CtaEcsCachedConfig::processMaxModeExecutionTimeInSecs()
{
	FUNCTION_ENTRY("processMaxModeExecutionTimeInSecs");
    unsigned long entityParameterValue;
    if (m_masterEntityData)
    {
        entityParameterValue = m_masterEntityData->getMaxModeExecutionTimeInSecs();
    }
    else if (m_stationEntityData)
    {
        entityParameterValue = m_stationEntityData->getMaxModeExecutionTimeInSecs();
    }
    else
    {
        TA_ASSERT(false, "CtaEcsCachedConfig not initialised with EntityData");
    }



    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

    if (isMemberInitialised(&m_maxModeExecutionTimeInSecs) && 
        m_maxModeExecutionTimeInSecs == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_maxModeExecutionTimeInSecs = entityParameterValue;
    setMemberInitialised(&m_maxModeExecutionTimeInSecs);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MaxModeExecutionTimeInSecs=%ld", m_maxModeExecutionTimeInSecs);
    FUNCTION_EXIT;
}


void CtaEcsCachedConfig::processMyZonesCongestionToken()
{
	FUNCTION_ENTRY("processMyZonesCongestionToken");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue;

    if (m_masterEntityData)
    {
        entityParameterValue = m_masterEntityData->getMyZonesCongestionToken();
    }
    else if (m_stationEntityData)
    {
        entityParameterValue = m_stationEntityData->getMyZonesCongestionToken();
    }
    else
    {
        TA_ASSERT(false, "CtaEcsCachedConfig not initialised with EntityData");
    }


    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_cachedMapsInitialiserInstance.refreshMyZoneCongestionDataPoints(getDataPointsByToken(entityParameterValue));

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesCongestionToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyZonesEmergencyToken()
{
	FUNCTION_ENTRY("processMyZonesEmergencyToken");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue;

    if (m_masterEntityData)
    {
        entityParameterValue = m_masterEntityData->getMyZonesEmergencyToken();
    }
    else if (m_stationEntityData)
    {
        entityParameterValue = m_stationEntityData->getMyZonesEmergencyToken();
    }
    else
    {
        TA_ASSERT(false, "CtaEcsCachedConfig not initialised with EntityData");
    }


    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_cachedMapsInitialiserInstance.refreshMyZoneEmergencyDataPoints(getDataPointsByToken(entityParameterValue));

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesEmergencyToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}




///////////////////////////////////////////////////////////////////////////
// StationECSAgent entity parameters
///////////////////////////////////////////////////////////////////////////

void CtaEcsCachedConfig::processNormalStationMode()
{
	FUNCTION_ENTRY("processNormalStationMode");
    unsigned long entityParameterValue;
    if (m_stationEntityData)
    {
        entityParameterValue = m_stationEntityData->getNormalStationMode();
    }
    else
    {
        TA_ASSERT(false, "CtaEcsCachedConfig not initialised with EntityData");
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

    if (isMemberInitialised(&m_normalStationMode) && 
        m_normalStationMode == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_normalStationMode = entityParameterValue;
    setMemberInitialised(&m_normalStationMode);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter NormalStationMode=%ld", m_normalStationMode);
    FUNCTION_EXIT;
}


void CtaEcsCachedConfig::processIscsAlivePollTimeInSecs()
{
	FUNCTION_ENTRY("processIscsAlivePollTimeInSecs");
    unsigned long entityParameterValue;
    if (m_stationEntityData)
    {
        entityParameterValue = m_stationEntityData->getIscsAlivePollTimeInSecs();
    }
    else
    {
        TA_ASSERT(false, "CtaEcsCachedConfig not initialised with EntityData");
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

    if (isMemberInitialised(&m_iscsAlivePollTimeInMSecs) && 
        m_iscsAlivePollTimeInMSecs == entityParameterValue*1000)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_iscsAlivePollTimeInMSecs = entityParameterValue*1000;
    setMemberInitialised(&m_iscsAlivePollTimeInMSecs);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter IscsAlivePollTimeInSecs=%ld", entityParameterValue);
    FUNCTION_EXIT;
}


void CtaEcsCachedConfig::processMyMasterModeLockToken()
{
	FUNCTION_ENTRY("processMyMasterModeLockToken");
    TA_ASSERT(m_stationEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getMyMasterModeLockToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        TA_THROW( TA_Base_Core::InvalidECSConfigurationException("Invalid MyMasterModeLockDataPoint set") );
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_myMasterModeLockDataPoint = datapoint;
    lastEntityParameterValue = entityParameterValue;
    setMemberInitialised(&m_myMasterModeLockDataPoint);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyMasterModeLockToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

  
void CtaEcsCachedConfig::processMyMasterModeLockControlToken()
{
	FUNCTION_ENTRY("processMyMasterModeLockControlToken");
    TA_ASSERT(m_stationEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getMyMasterModeLockControlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        TA_THROW( TA_Base_Core::InvalidECSConfigurationException("Invalid MyMasterModeLockControlToken set") );
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_myMasterModeLockControlDataPoint = datapoint;
    lastEntityParameterValue = entityParameterValue;
    setMemberInitialised(&m_myMasterModeLockControlDataPoint);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyMasterModeLockControlToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMySelectedMasterModeToken()
{
	FUNCTION_ENTRY("processMySelectedMasterModeToken");
    TA_ASSERT(m_stationEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getMySelectedMasterModeToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        TA_THROW( TA_Base_Core::InvalidECSConfigurationException("Invalid MySelectedMasterModeToken set") );
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_mySelectedMasterModeDataPoint = datapoint;
    lastEntityParameterValue = entityParameterValue;
    setMemberInitialised(&m_mySelectedMasterModeDataPoint);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MySelectedMasterModeToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMySelectedOverridingStationModeToken()
{
	FUNCTION_ENTRY("processMySelectedOverridingStationModeToken");
    TA_ASSERT(m_stationEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getMySelectedOverridingStationModeToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        TA_THROW( TA_Base_Core::InvalidECSConfigurationException("Invalid MySelectedOverridingStationModeToken set") );
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_mySelectedOverridingStationModeDataPoint = datapoint;
    lastEntityParameterValue = entityParameterValue;
    setMemberInitialised(&m_mySelectedOverridingStationModeDataPoint);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MySelectedOverridingStationModeToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyLastAppliedMasterModeToken()
{
	FUNCTION_ENTRY("processMyLastAppliedMasterModeToken");
    TA_ASSERT(m_stationEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getMyLastAppliedMasterModeToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        TA_THROW( TA_Base_Core::InvalidECSConfigurationException("Invalid MyLastAppliedMasterModeToken set") );
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_myLastAppliedMasterModeDataPoint = datapoint;
    lastEntityParameterValue = entityParameterValue;
    setMemberInitialised(&m_myLastAppliedMasterModeDataPoint);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyLastAppliedMasterModeToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyLastAppliedStationModeToken()
{
	FUNCTION_ENTRY("processMyLastAppliedStationModeToken");
    TA_ASSERT(m_stationEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getMyLastAppliedStationModeToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        TA_THROW( TA_Base_Core::InvalidECSConfigurationException("Invalid MyLastAppliedStationModeToken set") );
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_myLastAppliedStationModeDataPoint = datapoint;
    lastEntityParameterValue = entityParameterValue;
    setMemberInitialised(&m_myLastAppliedStationModeDataPoint);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyLastAppliedStationModeToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processModeStatusSqlToken()
{
	FUNCTION_ENTRY("processModeStatusSqlToken");
    TA_ASSERT(m_stationEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getModeStatusSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    boost::shared_ptr<TA_Base_Core::DataPointEntityData> param = getEntityBySqlToken(entityParameterValue);

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(param, m_nullEventProcessor, m_modeStatusProxy);
        
    lastEntityParameterValue = entityParameterValue;
    setMemberInitialised(&m_modeStatusProxy);
    notifyInterestedObservers(CONFIG_UNSURE);
    
    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter ModeStatusSqlToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processLocalModeSqlToken()
{
	FUNCTION_ENTRY("processLocalModeSqlToken");
    TA_ASSERT(m_stationEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getLocalModeSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    boost::shared_ptr<TA_Base_Core::DataPointEntityData> param = getEntityBySqlToken(entityParameterValue);

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(param, m_nullEventProcessor, m_localModeProxy);

    lastEntityParameterValue = entityParameterValue;
    setMemberInitialised(&m_localModeProxy);
    notifyInterestedObservers(CONFIG_UNSURE);
    
    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter LocalModeSqlToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processIscsAliveSqlToken()
{
	FUNCTION_ENTRY("processIscsAliveSqlToken");
    TA_ASSERT(m_stationEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getIscsAliveSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    boost::shared_ptr<TA_Base_Core::DataPointEntityData> param = getEntityBySqlToken(entityParameterValue);

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(param, m_nullEventProcessor, m_iscsAliveProxy);

    lastEntityParameterValue = entityParameterValue;
    setMemberInitialised(&m_iscsAliveProxy);
    notifyInterestedObservers(CONFIG_UNSURE);
    
    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter IscsAliveSqlToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processModeControlSqlToken()
{
	FUNCTION_ENTRY("processModeControlSqlToken");
    TA_ASSERT(m_stationEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getModeControlSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_modeControlDpEntity = getEntityBySqlToken(entityParameterValue);

    lastEntityParameterValue = entityParameterValue;
    setMemberInitialised(&m_modeControlDpEntity);
    notifyInterestedObservers(CONFIG_MODE_CONTROL_CORBA_NAME);
    
    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter ModeControlSqlToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processModeInProgressSqlToken()
{
	FUNCTION_ENTRY("processModeInProgressSqlToken");
    TA_ASSERT(m_stationEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getModeInProgressSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_modeInProgressDpEntity = getEntityBySqlToken(entityParameterValue);

    lastEntityParameterValue = entityParameterValue;
    setMemberInitialised(&m_modeInProgressDpEntity);
    notifyInterestedObservers(CONFIG_MODE_IN_PROGRESS_CORBA_NAME);
    
    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter ModeInProgressSqlToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processAutoModeSqlToken()
{
	FUNCTION_ENTRY("processAutoModeSqlToken");
    TA_ASSERT(m_stationEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getAutoModeSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_autoModeDpEntity = getEntityBySqlToken(entityParameterValue);

    lastEntityParameterValue = entityParameterValue;
    setMemberInitialised(&m_autoModeDpEntity);
    notifyInterestedObservers(CONFIG_AUTO_MODE_CORBA_NAME);
    
    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter AutoModeSqlToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}


///////////////////////////////////////////////////////////////////////////
// MasterECSAgent entity parameters
///////////////////////////////////////////////////////////////////////////


void CtaEcsCachedConfig::processSelectionTimeoutInSecs()
{
	FUNCTION_ENTRY("processSelectionTimeoutInSecs");
    unsigned long entityParameterValue;
    if (m_masterEntityData)
    {
        entityParameterValue = m_masterEntityData->getSelectionTimeoutInSecs();
    }
    else
    {
        TA_ASSERT(false, "CtaEcsCachedConfig not initialised with EntityData");
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

    if (isMemberInitialised(&m_selectionTimeoutInMSecs) && 
        m_selectionTimeoutInMSecs == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_selectionTimeoutInMSecs = entityParameterValue*1000;
    setMemberInitialised(&m_selectionTimeoutInMSecs);
    notifyInterestedObservers(CONFIG_SELECTION_TIMEOUT_IN_MSECS);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter SelectionTimeoutInSecs=%ld", entityParameterValue);
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processNormalMasterMode()
{
	FUNCTION_ENTRY("processNormalMasterMode");
    unsigned long entityParameterValue;
    if (m_masterEntityData)
    {
        entityParameterValue = m_masterEntityData->getNormalMasterMode();
    }
    else
    {
        TA_ASSERT(false, "CtaEcsCachedConfig not initialised with EntityData");
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

    if (isMemberInitialised(&m_normalMasterMode) && 
        m_normalMasterMode == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_normalMasterMode = entityParameterValue;
    setMemberInitialised(&m_normalMasterMode);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter NormalMasterMode=%ld", m_normalMasterMode);
    FUNCTION_EXIT;
}

//Datapoint is removed
/*
void CtaEcsCachedConfig::processMyZonesCongestionAppliedToken()
{
	FUNCTION_ENTRY("processMyZonesCongestionAppliedToken");
    TA_ASSERT(m_masterEntityData, "CtaEcsCachedConfig not initialised with EntityData");

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyZonesCongestionAppliedToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_cachedMapsInitialiserInstance.refreshMyZoneCongestionAppliedDataPoints(getDataPointsByToken(entityParameterValue));

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesCongestionAppliedToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}
*/

void CtaEcsCachedConfig::processMyZonesEmergencyAppliedToken()
{
	FUNCTION_ENTRY("processMyZonesEmergencyAppliedToken");
    TA_ASSERT(m_masterEntityData, "CtaEcsCachedConfig not initialised with EntityData");

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyZonesEmergencyAppliedToken(); 

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_cachedMapsInitialiserInstance.refreshMyZoneEmergencyAppliedDataPoints(getDataPointsByToken(entityParameterValue));

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesEmergencyAppliedToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyZonesEvacuationSelectedToken()
{
	FUNCTION_ENTRY("processMyZonesEvacuationSelectedToken");
    TA_ASSERT(m_masterEntityData, "CtaEcsCachedConfig not initialised with EntityData");

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyZonesEvacuationSelectedToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_cachedMapsInitialiserInstance.refreshMyZoneEvacuationSelectedDataPoints(getDataPointsByToken(entityParameterValue));

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesEvacuationSelectedToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyZonesAirflowSelectedToken()
{
	FUNCTION_ENTRY("processMyZonesAirflowSelectedToken");
    TA_ASSERT(m_masterEntityData, "CtaEcsCachedConfig not initialised with EntityData");

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyZonesAirflowSelectedToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_cachedMapsInitialiserInstance.refreshMyZoneAirflowSelectedDataPoints(getDataPointsByToken(entityParameterValue));

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesAirflowSelectedToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyZonesEvacuationAppliedToken()
{
	FUNCTION_ENTRY("processMyZonesEvacuationAppliedToken");
    TA_ASSERT(m_masterEntityData, "CtaEcsCachedConfig not initialised with EntityData");

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyZonesEvacuationAppliedToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_cachedMapsInitialiserInstance.refreshMyZoneEvacuationAppliedDataPoints(getDataPointsByToken(entityParameterValue));

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesEvacuationAppliedToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyZonesAirflowAppliedToken()
{
	FUNCTION_ENTRY("processMyZonesAirflowAppliedToken");
    TA_ASSERT(m_masterEntityData, "CtaEcsCachedConfig not initialised with EntityData");

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyZonesAirflowAppliedToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_cachedMapsInitialiserInstance.refreshMyZoneAirflowAppliedDataPoints(getDataPointsByToken(entityParameterValue));

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesAirflowAppliedToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}



void CtaEcsCachedConfig::processMyModeStatusToken()
{
	FUNCTION_ENTRY("processMyModeStatusToken");
    TA_ASSERT(m_masterEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyModeStatusToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        TA_THROW( TA_Base_Core::InvalidECSConfigurationException("Invalid MyModeStatusToken set") );
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_myModeStatusDataPoint = datapoint;
    lastEntityParameterValue = entityParameterValue;
    setMemberInitialised(&m_myModeStatusDataPoint);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyModeStatusToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyModeInProgressToken()
{
	FUNCTION_ENTRY("processMyModeInProgressToken");
    TA_ASSERT(m_masterEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyModeInProgressToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        TA_THROW( TA_Base_Core::InvalidECSConfigurationException("Invalid MyModeInProgressToken set") );
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_myModeInProgressDataPoint = datapoint;
    lastEntityParameterValue = entityParameterValue;
    setMemberInitialised(&m_myModeInProgressDataPoint);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyModeInProgressToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}


void CtaEcsCachedConfig::processMasterModeLocksSqlToken()
{
	FUNCTION_ENTRY("processMasterModeLocksSqlToken");
    TA_ASSERT(m_masterEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMasterModeLocksSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_cachedMapsInitialiserInstance.refreshMasterModeLockProxies(entityParameterValue);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MasterModeLocksSqlToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processSelectedMasterModesSqlToken()
{
	FUNCTION_ENTRY("processSelectedMasterModesSqlToken");
    TA_ASSERT(m_masterEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getSelectedMasterModesSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_cachedMapsInitialiserInstance.refreshSelectedMasterModeProxies(entityParameterValue);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter SelectedMasterModesSqlToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processSelectedOverridingStationModesSqlToken()
{
	FUNCTION_ENTRY("processSelectedOverridingStationModesSqlToken");
    TA_ASSERT(m_masterEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getSelectedOverridingStationModesSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_cachedMapsInitialiserInstance.refreshSelectedOverridingStationModeProxies(entityParameterValue);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter SelectedOverridingStationModesSqlToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processLastAppliedMasterModesSqlToken()
{
	FUNCTION_ENTRY("processLastAppliedMasterModesSqlToken");
    TA_ASSERT(m_masterEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getLastAppliedMasterModesSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_cachedMapsInitialiserInstance.refreshLastAppliedMasterModeProxies(entityParameterValue);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter LastAppliedMasterModesSqlToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processLastAppliedStationModesSqlToken()
{
	FUNCTION_ENTRY("processLastAppliedStationModesSqlToken");
    TA_ASSERT(m_masterEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getLastAppliedStationModesSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_cachedMapsInitialiserInstance.refreshLastAppliedStationModeProxies(entityParameterValue);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter LastAppliedStationModesSqlToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processLocalModesSqlToken()
{
	FUNCTION_ENTRY("processLocalModesSqlToken");
    TA_ASSERT(m_masterEntityData , "CtaEcsCachedConfig not initialised with EntityData");
    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getLocalModesSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    m_cachedMapsInitialiserInstance.refreshLocalModeProxies(entityParameterValue);

	//TODO: need to verify with PMSAgent
	setMemberInitialised(&m_localModeProxy);
	notifyInterestedObservers(CONFIG_UNSURE);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter LocalModesSqlToken=%s", entityParameterValue.c_str());
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setIsMaster(bool param)
{
	FUNCTION_ENTRY("setIsMaster");
    // Implicitly threadsafe
    m_isMaster = param;
    setMemberInitialised(&m_isMaster);
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setIsInControl(bool param)
{
	FUNCTION_ENTRY("setIsInControl");
    // Implicitly threadsafe
    m_isInControl = param;
    setMemberInitialised(&m_isInControl);
    FUNCTION_EXIT;
}


void CtaEcsCachedConfig::setAgentKey(unsigned long param)
{
	FUNCTION_ENTRY("setAgentKey");
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_agentKey = param;
    setMemberInitialised(&m_agentKey);
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setAgentTypeKey(unsigned long param)
{
	FUNCTION_ENTRY("setAgentTypeKey");
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_agentTypeKey = param;
    setMemberInitialised(&m_agentTypeKey);
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setAgentLocationKey(unsigned long param) 
//throw(TA_Base_Core::InvalidECSConfigurationException)
{
	FUNCTION_ENTRY("setAgentLocationKey");
    if (param == 0 || param == -1)
    {
        TA_THROW( TA_Base_Core::InvalidECSConfigurationException("Invalid LocationKey set") );
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_agentLocationKey = param;
    setMemberInitialised(&m_agentLocationKey);
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setAgentSubsystemKey(unsigned long param) 
//throw(TA_Base_Core::InvalidECSConfigurationException)
{
	FUNCTION_ENTRY("setAgentSubsystemKey");
    if ( param == -1)
    {
        TA_THROW( TA_Base_Core::InvalidECSConfigurationException("Invalid SubsystemKey set") );
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_agentSubsystemKey = param;
    setMemberInitialised(&m_agentSubsystemKey);
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setAgentInRuntime(bool param)
{
	FUNCTION_ENTRY("setAgentInRuntime");
    //Implicitly threadsafe: TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_agentInRuntime = param;
    setMemberInitialised(&m_agentInRuntime);
    FUNCTION_EXIT;
}
    
void CtaEcsCachedConfig::setIsOccOnline(bool param)
{
	FUNCTION_ENTRY("setIsOccOnline");
    //Implicitly threadsafe: TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_isOccOnline = param;
    setMemberInitialised(&m_isOccOnline);
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setOccGroupName(const std::string& param) //throw(TA_Base_Core::InvalidECSConfigurationException)
{
	FUNCTION_ENTRY("setOccGroupName");
    if (param == "")
    {
        std::ostringstream msg;
        msg << "Invalid ECS Agent parameter.  "
            << "OccGroupName=" << param;

        TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_occGroupName = param;
    setMemberInitialised(&m_occGroupName);
    FUNCTION_EXIT;
}


// Station or Master
void CtaEcsCachedConfig::processLocalATSAgentKey()
{
	FUNCTION_ENTRY("processLocalATSAgentKey");

    TA_Base_Core::IEntityDataList entityDataList;

    if (m_masterEntityData)
    {
        entityDataList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation
            ( TA_Base_Core::AtsAgentEntityData::getStaticType(), m_masterEntityData->getLocation() );
    }
    else if (m_stationEntityData)
    {
        entityDataList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation
            ( TA_Base_Core::AtsAgentEntityData::getStaticType(), m_stationEntityData->getLocation() );
    }
    else
    {
        TA_ASSERT(false, "CtaEcsCachedConfig not initialised with EntityData");
    }

    if (entityDataList.size()!=1)
    {
        // Clean up first
        for (TA_Base_Core::IEntityDataList::iterator it = entityDataList.begin(); it != entityDataList.end(); ++it)
        {
            delete *it;
        }
        TA_THROW(TA_Base_Core::InvalidECSConfigurationException("Could not resolve a unique ATS Agent entity key for this location"));
    }

    TA_Base_Core::IEntityData* atsEntityData = entityDataList[0];
    unsigned long atsAgentKey = atsEntityData->getKey();
    std::string atsAgentName = atsEntityData->getName();

    // clean up entityDataList
    delete atsEntityData;
    atsEntityData = NULL;
    entityDataList.clear();

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_localATSAgentKey = atsAgentKey;
    m_localATSAgentName = atsAgentName;
    setMemberInitialised(&m_localATSAgentKey);
    setMemberInitialised(&m_localATSAgentName);
    FUNCTION_EXIT;
}

// Station or Master
void CtaEcsCachedConfig::processAssetName()
{
	FUNCTION_ENTRY("processAssetName");

    if (m_masterEntityData)
    {
		m_assetName = m_masterEntityData->getAssetName();
    }
    else if (m_stationEntityData)
    {
		m_assetName = m_stationEntityData->getAssetName();
	}
    else
    {
        TA_ASSERT(false, "CtaEcsCachedConfig not initialised with asset name");
    }

    setMemberInitialised(&m_assetName);
    
    FUNCTION_EXIT;
}


// Station or Master
void CtaEcsCachedConfig::processSpareATSAgentKey()
{
	FUNCTION_ENTRY("processSpareATSAgentKey");

    TA_Base_Core::IEntityData* atsEntityData = NULL;

    if (m_masterEntityData)
    {
        if(m_masterEntityData->getSpareAtsAgentName().empty() == false)
        {
            atsEntityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_masterEntityData->getSpareAtsAgentName());
        }
    }
    else if (m_stationEntityData)
    {
        if(m_stationEntityData->getSpareAtsAgentName().empty() == false)
        {
            atsEntityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_stationEntityData->getSpareAtsAgentName());
        }
    }
    else
    {
        TA_ASSERT(false, "CtaEcsCachedConfig not initialised with EntityData");
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    if(atsEntityData != NULL)
    {
        m_spareATSAgentKey = atsEntityData->getKey();
        m_spareATSAgentName = atsEntityData->getName();
        m_spareATSAgentLocationKey = atsEntityData->getLocation();

        // clean up entityDataList
        delete atsEntityData;
        atsEntityData = NULL;
    }
    setMemberInitialised(&m_spareATSAgentKey);
    setMemberInitialised(&m_spareATSAgentName);
    setMemberInitialised(&m_spareATSAgentLocationKey);
    
    FUNCTION_EXIT;
}


///////////////////////////////////////////////////////////////////////////
// Common GetMethods
///////////////////////////////////////////////////////////////////////////

bool CtaEcsCachedConfig::getIsMaster()
{
    // Implicitly threadsafe
    TA_ASSERT(isMemberInitialised(&m_isMaster), "m_isMaster not set");
    return m_isMaster;
}

bool CtaEcsCachedConfig::getIsInControl()
{
    // Implicitly threadsafe
    TA_ASSERT(isMemberInitialised(&m_isInControl), "m_isInControl not set");
    return m_isInControl;
}

unsigned long CtaEcsCachedConfig::getAgentKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_agentKey), "m_agentKey not set");
    return m_agentKey;
}

std::string CtaEcsCachedConfig::getAssetName()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_assetName), "m_assetName not set");
    return m_assetName;
}

std::string  CtaEcsCachedConfig::getAgentName()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

    if (!isMemberInitialised(&m_agentName))
    {
        m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
        if (m_agentName.size()!=0)
        {
            setMemberInitialised(&m_agentName);
        }
    }
    return m_agentName;
}

unsigned long CtaEcsCachedConfig::getAgentTypeKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_agentTypeKey), "m_agentTypeKey not set");
    return m_agentTypeKey;
}

unsigned long CtaEcsCachedConfig::getAgentLocationKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_agentLocationKey), "m_agentLocationKey not set");
    return m_agentLocationKey;
}

unsigned long CtaEcsCachedConfig::getAgentSubsystemKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_agentSubsystemKey), "m_agentSubsystemKey not set");
    return m_agentSubsystemKey;
}

bool CtaEcsCachedConfig::getAgentInRuntime()
{
    //Implicitly threadsafe
    TA_ASSERT(isMemberInitialised(&m_agentInRuntime), "m_agentInRuntime not set");
    return m_agentInRuntime;
}

unsigned long CtaEcsCachedConfig::getWorkerThreadsSleepTimeInMSecs()
{
    //Threadsafe enough - can live with temporary data corruption
    TA_ASSERT(isMemberInitialised(&m_workerThreadsSleepTimeInMSecs), "m_workerThreadsSleepTimeInMSecs not set");
    return m_workerThreadsSleepTimeInMSecs;
}


unsigned long CtaEcsCachedConfig::getMaxModeExecutionTimeInSecs()   
{
    //Threadsafe enough - can live with temporary data corruption
    TA_ASSERT(isMemberInitialised(&m_maxModeExecutionTimeInSecs), "m_maxModeExecutionTimeInSecs not set");
    return m_maxModeExecutionTimeInSecs;
}




///////////////////////////////////////////////////////////////////////////
// StationEcsAgent GetMethods
///////////////////////////////////////////////////////////////////////////

bool CtaEcsCachedConfig::getIsOccOnline()
{
    //Implicitly threadsafe: TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_isOccOnline), "m_isOccOnline not set");
    return m_isOccOnline;
}

std::string CtaEcsCachedConfig::getOccGroupName()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_occGroupName), "m_occGroupName not set");
    return m_occGroupName;
}

unsigned long CtaEcsCachedConfig::getLocalATSAgentKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_localATSAgentKey), "m_localATSAgentKey not set");
    return m_localATSAgentKey;
}

std::string CtaEcsCachedConfig::getLocalATSAgentName()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_localATSAgentName), "m_localATSAgentName not set");
    return m_localATSAgentName;
}

unsigned long CtaEcsCachedConfig::getSpareATSAgentKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_spareATSAgentKey), "m_spareATSAgentKey not set");
    return m_spareATSAgentKey;
}

std::string CtaEcsCachedConfig::getSpareATSAgentName()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_spareATSAgentName), "m_spareATSAgentName not set");
    return m_spareATSAgentName;
}

unsigned long CtaEcsCachedConfig::getSpareATSAgentLocationKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_spareATSAgentLocationKey), "m_spareATSAgentLocationKey not set");
    return m_spareATSAgentLocationKey;
}

unsigned long CtaEcsCachedConfig::getThresholdStallTimeInSecs()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_thresholdStallTimeInSecs), "m_thresholdStallTimeInSecs not set");
    return m_thresholdStallTimeInSecs;
}


unsigned long CtaEcsCachedConfig::getThresholdTemperature()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_thresholdTemperature), "m_thresholdTemperature not set");
    return m_thresholdTemperature;
}


unsigned long CtaEcsCachedConfig::getTemperatureDeadbandLowerLimit()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_temperatureDeadbandLowerLimit), "m_temperatureDeadbandLowerLimit not set");
    return m_temperatureDeadbandLowerLimit;
}

bool CtaEcsCachedConfig::getCongestionUsesStallTime()
{
    //Implicitly threadsafe: TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_congestionUsesStallTime), "m_congestionUsesStallTime not set");
    return m_congestionUsesStallTime;
}

bool CtaEcsCachedConfig::getCongestionUsesTemperature()
{
    //Implicitly threadsafe: TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_congestionUsesTemperature), "m_congestionUsesTemperature not set");
    return m_congestionUsesTemperature;
}

unsigned long CtaEcsCachedConfig::getIscsAlivePollTimeInMSecs()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_iscsAlivePollTimeInMSecs), "m_iscsAlivePollTimeInMSecs not set");
    return m_iscsAlivePollTimeInMSecs;
}

unsigned long CtaEcsCachedConfig::getNormalStationMode()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_normalStationMode), "m_normalStationMode not set");
    return m_normalStationMode;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMyMasterModeLockDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_myMasterModeLockDataPoint), "m_myMasterModeLockDataPoint not set");
    return m_myMasterModeLockDataPoint;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMyMasterModeLockControlDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_myMasterModeLockControlDataPoint), "m_myMasterModeLockControlDataPoint not set");
    return m_myMasterModeLockControlDataPoint;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMySelectedMasterModeDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_mySelectedMasterModeDataPoint), "m_mySelectedMasterModeDataPoint not set");
    return m_mySelectedMasterModeDataPoint;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMySelectedOverridingStationModeDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_mySelectedOverridingStationModeDataPoint), "m_mySelectedOverridingStationModeDataPoint not set");
    return m_mySelectedOverridingStationModeDataPoint;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMyLastAppliedMasterModeDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_myLastAppliedMasterModeDataPoint), "m_myLastAppliedMasterModeDataPoint not set");
    return m_myLastAppliedMasterModeDataPoint;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMyLastAppliedStationModeDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_myLastAppliedStationModeDataPoint), "m_myLastAppliedStationModeDataPoint not set");
    return m_myLastAppliedStationModeDataPoint;
}

TA_Base_Bus::DataPointProxySmartPtr&  CtaEcsCachedConfig::getModeStatusProxy()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_modeStatusProxy), "m_modeStatusProxy not set");
    return m_modeStatusProxy;
}

TA_Base_Bus::DataPointProxySmartPtr&  CtaEcsCachedConfig::getLocalModeProxy()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_localModeProxy), "m_localModeProxy not set");
    return m_localModeProxy;
}

TA_Base_Bus::DataPointProxySmartPtr&  CtaEcsCachedConfig::getIscsAliveProxy()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_iscsAliveProxy), "m_iscsAliveProxy not set");
    return m_iscsAliveProxy;
}

unsigned long CtaEcsCachedConfig::getIscsEntityKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

	static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getIscsAliveSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
        return 0;
    }

    boost::shared_ptr<TA_Base_Core::DataPointEntityData> param = getEntityBySqlToken(entityParameterValue);	

	return param->getKey();
}

std::string CtaEcsCachedConfig::getIscsAgentName()
{
	TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

	static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getIscsAliveSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
        return 0;
    }

    boost::shared_ptr<TA_Base_Core::DataPointEntityData> param = getEntityBySqlToken(entityParameterValue);	

	return param->getAgentName();
}

TA_Base_Core::DataPointEntityDataPtr CtaEcsCachedConfig::getModeControlDpEntityData()
{
    TA_Base_Core::ThreadGuard guard(m_lockForConfigUpdate);
    TA_ASSERT(isMemberInitialised(&m_modeControlDpEntity), "m_modeControlDpEntity not set");

    return m_modeControlDpEntity;
}

TA_Base_Core::DataPointEntityDataPtr CtaEcsCachedConfig::getModeInProgressDpEntityData()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_modeInProgressDpEntity), "m_modeInProgressDpEntity not set");

    return m_modeInProgressDpEntity;
}

TA_Base_Core::DataPointEntityDataPtr CtaEcsCachedConfig::getAutoModeDpEntityData()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_autoModeDpEntity), "m_autoModeDpEntity not set");

    return m_autoModeDpEntity;
}





///////////////////////////////////////////////////////////////////////////
// MasterEcsAgent GetMethods
///////////////////////////////////////////////////////////////////////////

unsigned long  CtaEcsCachedConfig::getSelectionTimeoutInMSecs()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_selectionTimeoutInMSecs), "m_selectionTimeoutInMSecs not set");
    return m_selectionTimeoutInMSecs;
}

unsigned long  CtaEcsCachedConfig::getNormalMasterMode()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_normalMasterMode), "m_normalMasterMode not set");
    return m_normalMasterMode;
}


TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMyModeStatusDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_myModeStatusDataPoint), "m_myModeStatusDataPoint not set");
    return m_myModeStatusDataPoint;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMyModeInProgressDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    TA_ASSERT(isMemberInitialised(&m_myModeInProgressDataPoint), "m_myModeInProgressDataPoint not set");
    return m_myModeInProgressDataPoint;
}





