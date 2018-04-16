/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/CachedConfig.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// CachedConfig.h: interface for the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
#define AFX_CachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_

#include <string>
#include <map>
#include <list>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


namespace TA_Base_Bus
{
	class DataPoint;
}

namespace TA_IRS_App
{

class ICachedConfigObserver;

typedef std::map<unsigned short, TA_Base_Bus::DataPoint*> StationToFireDpsMap;
typedef StationToFireDpsMap::iterator StationToFireDpsMapIt;

typedef std::map<std::string, unsigned short> FireAbortNameToStationIdMap;
typedef FireAbortNameToStationIdMap::iterator FireAbortNameToStationIdMapIt;

class CachedConfig  
{

public:

    // Observable descriptors

    typedef enum 
    {
        CONFIG_PRIMARY_FEPC_ADDRESS,
        CONFIG_SECONDARY_FEPC_ADDRESS,
        CONFIG_PRIMARY_FEPC_PORT,
        CONFIG_SECONDARY_FEPC_PORT,
        CONFIG_SOCKET_TIMEOUT
    } ECachedConfigItemKey;


    /**
     * getInstance
	 *
	 * Returns an instance of the class
     *
     * @return the pointer to this singleton instance
     *
     */
	static CachedConfig* getInstance();

    /**
     * removeInstance
	 *
	 * Removes the instance of the class (if already created) and cleanup the members.  Primarily 
     * used upon program termination (e.g. from main()) so that Purify does not consider this class
     * and its members to be leaks.
     *
     */
    static void removeInstance();

    void setAgentKey(unsigned long param);
    // Removed setAgentName() as this is now taken from RunParams 
    void setAgentTypeKey(unsigned long param);
    void setAgentLocationKey(unsigned long param); 
    void setAgentSubsystemKey(unsigned long param);
    void setOccLocationKey(unsigned long param); 
    void setAgentInRuntime(bool param);
    void setAgentAssetName(std::string param);
    void setAgentName(std::string param);
	void setMonitorProcessKey(unsigned long param);

    void setLocalATSAgentKey(unsigned long param);
    void setLocalTrainAgentKey(unsigned long param); 
    void setFastPollRateInMSecs(unsigned long param);
    void setWorkerThreadsSleepTimeInMSecs(unsigned long param);
    void setSocketTimeoutInSecs(unsigned long param);
    void setRetries(unsigned long param);
    void setPrimaryFEPCAddress(const std::string& param); 
    void setSecondaryFEPCAddress(const std::string& param); 

    void setFireAlarmPollingIntervalInMillisecond(const unsigned long param);
    void setPasZonePollingIntervalInMillisecond(const unsigned long param);
    void setSystemFaultPollingIntervalInMillisecond(const unsigned long param);
    void setDvaLibPollingIntervalInMillisecond(const unsigned long param);
    void setPasDvaRecordingMaxDurationInSecond(const unsigned long param);

    // Converts from unsigned long to string
    void setPrimaryFEPCPort(unsigned long param);
    void setSecondaryFEPCPort(unsigned long param);

    void setIsMaster(bool param);
    void setIsDepot(bool param);

    // Set to null if the agent is unavailable
    void setLocalTrainAgentEntityName(const std::string& param);

    void setTisDisplayDurationInSecs(unsigned long param);
    void setKeepTerminatedBroadcastsInSecs(unsigned long param);
    void setMaxBroadcasts(unsigned long param);


    void setMajorSummaryAlarmDP(TA_Base_Bus::DataPoint* dp);
    void setMinorSummaryAlarmDP(TA_Base_Bus::DataPoint* dp);
    void setFireCountdownActiveDP(const StationToFireDpsMap& vecDps);
    void setFireCountDownOutputNames( const FireAbortNameToStationIdMap& mapNames );
    void setFireCountDownOutputToken( const std::string& strFireCountDownToken );

    void setTrainDvaMessageLibraryVersion(unsigned long param);

    void setPagingConsoles(const std::map<unsigned short, std::string>& pagingConsoleKeyList);

    // The standard entity details below should not assert / //throw since we may need to raise 
    // alarms with half-complete configuration.
    unsigned long       getAgentKey();
    std::string         getAgentName();
    unsigned long       getAgentTypeKey();
    unsigned long       getAgentLocationKey();
    unsigned long       getAgentSubsystemKey();
    unsigned long       getOccLocationKey();
    bool                getAgentInRuntime();
	std::string			getAgentAssetName();

    // These may assert / //throw.   
    unsigned long       getLocalATSAgentKey();
    unsigned long       getLocalTrainAgentKey();
    unsigned long       getFastPollRateInMSecs();
    unsigned long       getWorkerThreadsSleepTimeInMSecs();
    unsigned long       getSocketTimeoutInSecs();
    unsigned long       getRetries();

    unsigned long       getFireAlarmPollingIntervalInMillisecond();
    unsigned long       getPasZonePollingIntervalInMillisecond();
    unsigned long       getSystemFaultPollingIntervalInMillisecond();
    unsigned long       getDvaLibPollingIntervalInMillisecond();
    unsigned long       getPasDvaRecordingMaxDurationInSecond();

	unsigned long		getMonitorProcessKey();


    std::string         getPrimaryFEPCAddress();
    std::string         getSecondaryFEPCAddress();
    std::string         getPrimaryFEPCPort();
    std::string         getSecondaryFEPCPort();

    bool                getIsMaster();
    bool                getIsDepot();

    std::string         getLocalTrainAgentEntityName();

    bool                getIsLocalATSAgentConfigured();
    unsigned long       getTisDisplayDurationInSecs();
    unsigned long       getKeepTerminatedBroadcastsInSecs();
    unsigned long       getMaxBroadcasts();

    TA_Base_Bus::DataPoint*  getMajorSummaryAlarmDP();
    TA_Base_Bus::DataPoint*  getMinorSummaryAlarmDP();
    StationToFireDpsMap getFireCountdownActiveDPs();
    TA_Base_Bus::DataPoint* getFireCountdownActiveDP( unsigned short usStationId );
    unsigned short getStationIdFromFireAbortName( const std::string& strAbortName );
    std::string  getFireCountDownOutputToken();

    unsigned long       getTrainDvaMessageLibraryVersion();

    std::map<unsigned short, std::string>    getPagingConsoles();

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
    void attachObserver(ICachedConfigObserver* observer, ECachedConfigItemKey key);


    /**
     * detachObserver
	 *
	 * Removes all instances of the ICachedConfigObserver pointer in the observer collection.  The
     * observer will be removed for all keys.
     *
     */
    void detachObserver(ICachedConfigObserver* observer);

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
    void notifyInterestedObservers(ECachedConfigItemKey key);

private:
    CachedConfig();
	virtual ~CachedConfig();

   	static CachedConfig* m_me;

    static TA_Base_Core::NonReEntrantThreadLockable m_lockForConfigUpdate;
    //static TA_Base_Core::NonReEntrantThreadLockable m_lockForFastPoll;
    //static TA_Base_Core::NonReEntrantThreadLockable m_lockForWorkerThreads;
    static TA_Base_Core::NonReEntrantThreadLockable m_lockForObserverMap;

    // global parameters
    unsigned long m_agentKey;
    std::string m_agentName;
    unsigned long m_agentTypeKey;
    unsigned long m_agentLocationKey;
    unsigned long m_agentSubsystemKey;
    unsigned long m_ulOccLocationKey;
    bool                        m_agentInRuntime;

	std::string					m_assetName;
	//TD17452
	unsigned long				m_monitorProcessKey;

    // StationPAAgent and MasterPAAgent parameters
    unsigned long               m_localATSAgentKey;
    unsigned long               m_localTrainAgentKey;
	unsigned long               m_fastPollRateInMSecs;
    unsigned long               m_workerThreadsSleepTimeInMSecs;
    unsigned long               m_socketTimeoutInSecs;
    unsigned long               m_retries;
    unsigned long               m_fireAlarmPollingIntervalInMillisecond;
    unsigned long               m_pasZonePollingIntervalInMillisecond;
    unsigned long               m_systemFaultPollingIntervalInMillisecond;
    unsigned long               m_dvaLibPollingIntervalInMillisecond;
    unsigned long               m_pasDvaRecordingMaxDurationInSecond;
    std::string                 m_primaryFEPCAddress;
    std::string                 m_secondaryFEPCAddress;
    std::string                 m_primaryFEPCPort;
    std::string                 m_secondaryFEPCPort;

    bool                        m_isMaster;
    bool                        m_isDepot;

    std::string                 m_localTrainAgentEntityName;
    unsigned long               m_tisDisplayDuration;
    unsigned long               m_keepTerminatedBroadcastsInSecs;
    unsigned long               m_maxBroadcasts;

    TA_Base_Bus::DataPoint*          m_majorSummaryAlarmDP;
    TA_Base_Bus::DataPoint*          m_minorSummaryAlarmDP;
    StationToFireDpsMap m_fireCountdownActiveDP;
    FireAbortNameToStationIdMap m_mapFireAbortNames;
    std::string m_strFireCountDownOutputToken;

    unsigned long               m_trainDvaMessageLibraryVersion;

    // Paging console keys
    std::map<unsigned short, std::string>    m_pagingConsoles;

    // Observer maps    

    typedef std::multimap<ECachedConfigItemKey, ICachedConfigObserver*>  KeyToObserversMap;
    typedef KeyToObserversMap::iterator                                  KeyToObserversMapIt;
    typedef KeyToObserversMap::value_type                                key2observer;

    KeyToObserversMap   m_keyToObserversMap;
};


class ICachedConfigObserver
{
public:
    virtual void processCachedConfigUpdate(CachedConfig::ECachedConfigItemKey key) = 0;
};

}

#endif // !defined(AFX_CachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
