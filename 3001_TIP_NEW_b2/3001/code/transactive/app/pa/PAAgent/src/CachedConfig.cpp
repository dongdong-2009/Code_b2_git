/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/CachedConfig.cpp $
  * @author:  Ripple
  * @version: $Revision: #7 $
  *
  * Last modification: $DateTime: 2016/11/30 11:21:38 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */
// CachedConfig.cpp: implementation of the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DataConversion.h"
#include "core/exceptions/src/PasExceptions.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/CachedConfig.h"

namespace TA_IRS_App
{

CachedConfig* CachedConfig::m_me = NULL;

TA_Base_Core::NonReEntrantThreadLockable CachedConfig::m_lockForConfigUpdate;
TA_Base_Core::NonReEntrantThreadLockable CachedConfig::m_lockForObserverMap;

// Note regarding locks
//
// Locks have been deliberately removed for FastPollRate and
// WorkerThreadsSleepTime for performance reasons.  Only a single
// thread will be updating the member variables and therefore it
// is relatively safe.  What may occasionally happen is that the
// members are read just as an online update is applied.  The worst
// that could happen is that the value becomes partially corrupted,
// the degree to which depends on the architecture.  As these
// particular parameters are read very frequently, the next time
// the member is read, the value will be correct.  The worst
// conceivable corruption would be a value almost double that
// intended.  One byte example
//
// Original value       00001111
// Intended new value   00010000
// Corrupted value      00011111
//
// So long as the corruption is temporary and does not cause the
// agent to crash it is acceptable.

//NonReEntrantThreadLockable CachedConfig::m_lockForFastPoll;
//NonReEntrantThreadLockable CachedConfig::m_lockForWorkerThreads;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CachedConfig::CachedConfig() : 
m_agentKey( TA_Base_Core::DatabaseKey::getInvalidKey() ),
m_agentName(""),
m_agentTypeKey( TA_Base_Core::DatabaseKey::getInvalidKey() ),
m_agentLocationKey( TA_Base_Core::DatabaseKey::getInvalidKey() ),
m_agentSubsystemKey( TA_Base_Core::DatabaseKey::getInvalidKey() ),
m_agentInRuntime(false),
m_assetName(""),
m_monitorProcessKey( TA_Base_Core::DatabaseKey::getInvalidKey() ),
m_localATSAgentKey( TA_Base_Core::DatabaseKey::getInvalidKey() ),
m_localTrainAgentKey( TA_Base_Core::DatabaseKey::getInvalidKey() ),
m_fastPollRateInMSecs(100),
m_workerThreadsSleepTimeInMSecs(100),
m_socketTimeoutInSecs(1000),
m_retries(3),
m_primaryFEPCAddress(""),
m_secondaryFEPCAddress(""),
m_primaryFEPCPort(""),
m_secondaryFEPCPort(""),
m_isMaster(false),
m_isDepot(false),
m_localTrainAgentEntityName(""),
m_tisDisplayDuration(0),
m_keepTerminatedBroadcastsInSecs(300),
m_maxBroadcasts(30),
m_majorSummaryAlarmDP(NULL),
m_minorSummaryAlarmDP(NULL),
m_fireCountdownActiveDP(),
m_strFireCountDownOutputToken(""),
m_trainDvaMessageLibraryVersion(0),
m_pagingConsoles(),
m_keyToObserversMap(),
m_fireAlarmPollingIntervalInMillisecond(500),
m_pasZonePollingIntervalInMillisecond(200),
m_systemFaultPollingIntervalInMillisecond(500),
m_dvaLibPollingIntervalInMillisecond(2000),
m_pasDvaRecordingMaxDurationInSecond(60)
{
}

CachedConfig::~CachedConfig()
{
    m_keyToObserversMap.clear();
}


CachedConfig* CachedConfig::getInstance()
{
	if ( 0 == m_me )
	{
		// Double checking to prevent multiple threads
		// creating multiple instances.

		TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

		if ( 0 == m_me )
		{
			m_me = new CachedConfig();
    	}
	}

	return m_me;
}

void CachedConfig::removeInstance( )
{
	//
	// Guard this to prevent multiple threads atempting
	// to delete/create simultaneously
	//
	TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
	if ( m_me != NULL )
	{
		delete m_me;
		m_me = NULL;
	}
}



/**
 * attachObserver
 *
 * Adds the ICachedConfigObserver instance to the collection of observers.  If the observer
 * already exists for the key, no change will be made.
 *
 * @param observer pointer to the ICachedConfigObserver instance
 *
 * @param key the ECachedConfigItemKey the observer is interested in
 *
 */
void CachedConfig::attachObserver(ICachedConfigObserver* observer, ECachedConfigItemKey key)
{

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
}


/**
 * detachObserver
 *
 * Removes all instances of the ICachedConfigObserver pointer in the observer collection.  The
 * observer will be removed for all keys.
 *
 */
void CachedConfig::detachObserver(ICachedConfigObserver* observer)
{
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
}

/**
 * notifyInterestedObservers
 *
 * Calls the processCachedConfigUpdate() method on all registered observers interested in the
 * specified key.  Implementors of ICachedConfigObserver MUST make sure that no exceptions
 * are unhandled.
 *
 * @param key used to call specific observers
 *
 */
void CachedConfig::notifyInterestedObservers(ECachedConfigItemKey key)
{
    TA_Base_Core::ThreadGuard guard( m_lockForObserverMap );

    // Search for all observers associated with a key
    std::pair<KeyToObserversMapIt, KeyToObserversMapIt> range =
        m_keyToObserversMap.equal_range (key);

    for ( ; range.first != range.second; ++range.first)
    {
        try
        {
            (range.first)->second->processCachedConfigUpdate(key);
        }
        catch(...)
        {
            // Do nothing and continue
        }
    }
}


void CachedConfig::setAgentKey(unsigned long param)
{
    if (( param == 0 ) || TA_Base_Core::DatabaseKey::isInvalidKey( param ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid PA Agent Key [%lu]", param );
        return;
    }

    m_agentKey = param;
}

void CachedConfig::setAgentTypeKey(unsigned long param)
{
    m_agentTypeKey = param;
}

void CachedConfig::setAgentLocationKey(unsigned long param) 
{
    if (( param == 0 ) || TA_Base_Core::DatabaseKey::isInvalidKey( param ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid Agent Location Key [%lu]", param );
        return;
    }

    m_agentLocationKey = param;
}

void CachedConfig::setAgentSubsystemKey(unsigned long param) 
{
    m_agentSubsystemKey = param;
}

void CachedConfig::setOccLocationKey(unsigned long param) 
{
    m_ulOccLocationKey = param;
}

void CachedConfig::setAgentInRuntime(bool param)
{
    m_agentInRuntime = param;
}

void CachedConfig::setAgentAssetName(std::string param)
{
    m_assetName = param.c_str();
}

void CachedConfig::setAgentName( std::string param )
{
    m_agentName = param.c_str();
}

void CachedConfig::setLocalATSAgentKey(unsigned long param) 
{
    if (( param == 0 ) || TA_Base_Core::DatabaseKey::isInvalidKey( param ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid ATS Agent Key [%lu]", param );
        return;
    }

    m_localATSAgentKey = param;
}

void CachedConfig::setLocalTrainAgentKey(unsigned long param) //throw(TA_Base_Core::InvalidPasConfigurationException)
{
    if (( param == 0 ) || TA_Base_Core::DatabaseKey::isInvalidKey( param ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid Train Agent Key [%lu]", param );
        return;
    }

    m_localTrainAgentKey = param;
}

void CachedConfig::setFastPollRateInMSecs(unsigned long param)
{
    m_fastPollRateInMSecs = param;
}

void CachedConfig::setWorkerThreadsSleepTimeInMSecs(unsigned long param)
{
    m_workerThreadsSleepTimeInMSecs = param;
}

void CachedConfig::setSocketTimeoutInSecs(unsigned long param)
{
    m_socketTimeoutInSecs = param;
    notifyInterestedObservers(CONFIG_SOCKET_TIMEOUT);
}

void CachedConfig::setRetries(unsigned long param)
{
    m_retries = param;
}

void CachedConfig::setPrimaryFEPCAddress(const std::string& param)
{
    if ( param.empty() )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "Empty Primary FEPC address" );
        return;
    }

    m_primaryFEPCAddress = param;
    notifyInterestedObservers(CONFIG_PRIMARY_FEPC_ADDRESS);
}

void CachedConfig::setMonitorProcessKey(unsigned long param)
{
    m_monitorProcessKey = param;
}

void CachedConfig::setSecondaryFEPCAddress(const std::string& param)
{
    if ( param.empty() )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "Empty Secondary FEPC address" );
        return;
    }

    m_secondaryFEPCAddress = param;
    notifyInterestedObservers(CONFIG_SECONDARY_FEPC_ADDRESS);
}

void CachedConfig::setPrimaryFEPCPort(unsigned long param)
{
    if ( param > 65535 )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid Primary FEPC Port [%lu]", param );
        return;
    }

    m_primaryFEPCPort = TA_Base_Core::DataConversion::toString( param );
    notifyInterestedObservers(CONFIG_PRIMARY_FEPC_PORT);
}

void CachedConfig::setSecondaryFEPCPort(unsigned long param)
{
    if ( param > 65535 )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid Secondary FEPC Port [%lu]", param );
        return;
    }

    m_secondaryFEPCPort = TA_Base_Core::DataConversion::toString( param );
    notifyInterestedObservers(CONFIG_SECONDARY_FEPC_PORT);
}

void CachedConfig::setIsMaster(bool param)
{
    m_isMaster = param;
}

void CachedConfig::setIsDepot(bool param)
{
    m_isDepot = param;
}

void CachedConfig::setLocalTrainAgentEntityName(const std::string& param)
{
    m_localTrainAgentEntityName = param.c_str();
}

void CachedConfig::setTisDisplayDurationInSecs(unsigned long param)
{
    m_tisDisplayDuration = param;
}

void CachedConfig::setKeepTerminatedBroadcastsInSecs(unsigned long param)
{
    m_keepTerminatedBroadcastsInSecs = param;
}

void CachedConfig::setMaxBroadcasts(unsigned long param)
{
    m_maxBroadcasts = param;
}

void CachedConfig::setMajorSummaryAlarmDP(TA_Base_Bus::DataPoint* dp)
{
    if ( NULL == dp )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Invalid MajorSummaryAlarmDP - NULL pointer" );
        return;
    }

    m_majorSummaryAlarmDP = dp;
}

void CachedConfig::setMinorSummaryAlarmDP(TA_Base_Bus::DataPoint* dp)
{
    if ( NULL == dp )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Invalid MinorSummaryAlarmDP - NULL pointer" );
        return;
    }

    m_minorSummaryAlarmDP = dp;
}

void CachedConfig::setFireCountdownActiveDP(const StationToFireDpsMap& mapDatapoints)
{
    if ( mapDatapoints.empty() )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Invalid FireCountdownActiveDP - empty map" );
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_fireCountdownActiveDP = mapDatapoints;
}

void CachedConfig::setFireCountDownOutputNames( const FireAbortNameToStationIdMap& mapNames )
{
    if ( mapNames.empty() )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Invalid input parameter - empty map" );
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_mapFireAbortNames = mapNames;
}

void CachedConfig::setFireCountDownOutputToken( const std::string& strFireCountDownToken )
{
    if ( strFireCountDownToken.empty() )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Invalid FireCountDownOutputToken - empty string" );
        return;
    }

    m_strFireCountDownOutputToken = strFireCountDownToken;
}

void CachedConfig::setTrainDvaMessageLibraryVersion(unsigned long param)
{
    m_trainDvaMessageLibraryVersion = param;
}

void CachedConfig::setPagingConsoles( const std::map<unsigned short, std::string>& pagingConsoles )
{
    m_pagingConsoles = pagingConsoles;
}

unsigned long CachedConfig::getAgentKey()
{
    return m_agentKey;
}

unsigned long CachedConfig::getMonitorProcessKey()
{
    return m_monitorProcessKey;
}

std::string CachedConfig::getAgentName()
{
    return m_agentName;
}

std::string CachedConfig::getAgentAssetName()
{
    return m_assetName;
}

unsigned long CachedConfig::getAgentTypeKey()
{
    return m_agentTypeKey;
}

unsigned long CachedConfig::getAgentLocationKey()
{
    return m_agentLocationKey;
}

unsigned long CachedConfig::getAgentSubsystemKey()
{
    return m_agentSubsystemKey;
}

unsigned long CachedConfig::getOccLocationKey()
{
    return m_ulOccLocationKey;
}

unsigned long CachedConfig::getLocalATSAgentKey()
{
    return m_localATSAgentKey;
}

unsigned long CachedConfig::getLocalTrainAgentKey()
{
    return m_localTrainAgentKey;
}

unsigned long CachedConfig::getFastPollRateInMSecs()
{
    return m_fastPollRateInMSecs;
}

unsigned long CachedConfig::getWorkerThreadsSleepTimeInMSecs()
{
    return m_workerThreadsSleepTimeInMSecs;
}

unsigned long CachedConfig::getSocketTimeoutInSecs()
{
    return m_socketTimeoutInSecs;
}

unsigned long CachedConfig::getRetries()
{
    return m_retries;
}

std::string CachedConfig::getPrimaryFEPCAddress()
{
    return m_primaryFEPCAddress;
}

std::string CachedConfig::getSecondaryFEPCAddress()
{
    return m_secondaryFEPCAddress;
}

std::string CachedConfig::getPrimaryFEPCPort()
{
    return m_primaryFEPCPort;
}

std::string CachedConfig::getSecondaryFEPCPort()
{
    return m_secondaryFEPCPort;
}

bool CachedConfig::getIsMaster()
{
    return m_isMaster;
}

bool CachedConfig::getIsDepot()
{
    return m_isDepot;
}

std::string CachedConfig::getLocalTrainAgentEntityName()
{
    return m_localTrainAgentEntityName;
}

bool CachedConfig::getIsLocalATSAgentConfigured()
{
    // We test against the location key because it is faster compared to string comparisons.
    return TA_Base_Core::DatabaseKey::isValidKey( m_localATSAgentKey );
}

unsigned long CachedConfig::getTisDisplayDurationInSecs()
{
    return m_tisDisplayDuration;
}

unsigned long CachedConfig::getKeepTerminatedBroadcastsInSecs()
{
    return m_keepTerminatedBroadcastsInSecs;
}

unsigned long CachedConfig::getMaxBroadcasts()
{
    return m_maxBroadcasts;
}

TA_Base_Bus::DataPoint* CachedConfig::getMajorSummaryAlarmDP()
{
    if ( NULL == m_minorSummaryAlarmDP )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[NoData] MajorSummaryAlarm entity is not configured in database" );
    }

    return m_majorSummaryAlarmDP;
}

TA_Base_Bus::DataPoint* CachedConfig::getMinorSummaryAlarmDP()
{
    if ( NULL == m_minorSummaryAlarmDP )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[NoData] MinorSummaryAlarm entity is not configured in database" );
    }

    return m_minorSummaryAlarmDP;
}

StationToFireDpsMap CachedConfig::getFireCountdownActiveDPs()
{
    if ( m_fireCountdownActiveDP.empty() )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[NoData] FireCountdownActive entity is not configured in database" );
    }

    return m_fireCountdownActiveDP;
}

TA_Base_Bus::DataPoint* CachedConfig::getFireCountdownActiveDP( unsigned short usStationId )
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

    StationToFireDpsMapIt itFound = m_fireCountdownActiveDP.find( usStationId );
    if ( m_fireCountdownActiveDP.end() == itFound )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[NoData] FireCountdownActive entity for station %d is not configured in database", usStationId );
        return NULL;
    }

    return itFound->second;
}

unsigned short CachedConfig::getStationIdFromFireAbortName( const std::string& strAbortName )
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

    FireAbortNameToStationIdMapIt itFound = m_mapFireAbortNames.find( strAbortName );
    if ( m_mapFireAbortNames.end() == itFound )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[NoData] FireCountdownAbort entity %s is not configured in database", strAbortName.c_str() );
        return 0u;
    }

    return itFound->second;
}

std::string CachedConfig::getFireCountDownOutputToken()
{
    if ( m_strFireCountDownOutputToken.empty() )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[NoData] FireCountdownOutputName is not configured in database" );
    }

    return m_strFireCountDownOutputToken;
}

unsigned long CachedConfig::getTrainDvaMessageLibraryVersion()
{
    return m_trainDvaMessageLibraryVersion;
}

std::map<unsigned short, std::string> CachedConfig::getPagingConsoles()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

    if ( m_pagingConsoles.empty() )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[NoData] PagingConsoleIds is not configured in database" );
    }
    
    return m_pagingConsoles;
}

void CachedConfig::setFireAlarmPollingIntervalInMillisecond( const unsigned long param )
{
    m_fireAlarmPollingIntervalInMillisecond = param;
}

void CachedConfig::setPasZonePollingIntervalInMillisecond( const unsigned long param )
{
    m_pasZonePollingIntervalInMillisecond = param;
}

void CachedConfig::setSystemFaultPollingIntervalInMillisecond( const unsigned long param )
{
    m_systemFaultPollingIntervalInMillisecond = param;
}

void CachedConfig::setDvaLibPollingIntervalInMillisecond( const unsigned long param )
{
    m_dvaLibPollingIntervalInMillisecond = param;
}

void CachedConfig::setPasDvaRecordingMaxDurationInSecond( const unsigned long param )
{
    m_pasDvaRecordingMaxDurationInSecond = param;
}

unsigned long CachedConfig::getFireAlarmPollingIntervalInMillisecond()
{
    if ( 0u == m_fireAlarmPollingIntervalInMillisecond )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "[NoData] AlarmPollingIntervalInMillisecond is not configured in database" );
        return PAS_POLLING_FIREALARM_INTERVAL;
    }

    return m_fireAlarmPollingIntervalInMillisecond;
}

unsigned long CachedConfig::getPasZonePollingIntervalInMillisecond()
{
    if ( 0u == m_pasZonePollingIntervalInMillisecond )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "[NoData] ZonePollingIntervalInMillisecond is not configured in database" );
        return PAS_POLLING_ZONE_COUNTER_INTERVAL;
    }

    return m_pasZonePollingIntervalInMillisecond;
}

unsigned long CachedConfig::getSystemFaultPollingIntervalInMillisecond()
{
    if ( 0u == m_systemFaultPollingIntervalInMillisecond )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "[NoData] systemFaultPollingIntervalInMillisecond is not configured in database" );
        return PAS_POLLING_SYSTEM_FAULT_INTERVAL;
    }

    return m_systemFaultPollingIntervalInMillisecond;
}

unsigned long CachedConfig::getDvaLibPollingIntervalInMillisecond()
{
    if ( 0u == m_dvaLibPollingIntervalInMillisecond )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "[NoData] DvaLibPollingIntervalInMillisecond is not configured in database" );
        return PAS_POLLING_DVA_LIB_COUNTER_INTERVAL;
    }

    return m_dvaLibPollingIntervalInMillisecond;
}

unsigned long CachedConfig::getPasDvaRecordingMaxDurationInSecond()
{
    if ( 0u == m_pasDvaRecordingMaxDurationInSecond )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "[NoData] DvaRecordingMaxDurationInSecond is not configured in database" );
        return PAS_MAX_DVA_RECORDING_DURATION;
    }

    return m_pasDvaRecordingMaxDurationInSecond;
}

}