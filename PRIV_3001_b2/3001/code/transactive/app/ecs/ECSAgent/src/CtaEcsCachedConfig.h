/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/CtaEcsCachedConfig.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// CtaEcsCachedConfig.h: interface for the CtaEcsCachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CtaEcsCachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
#define AFX_CtaEcsCachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_

#include <string>
#include <vector>

#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfigInitialiser.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/MasterECSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/StationECSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/exceptions/src/ECSAgentExceptions.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/naming/src/INamedObject.h"

class ItaEcsCachedMapsInitialiser;

namespace TA_Base_Core
{
	class StationECSAgentEntityData;
    class MasterECSAgentEntityData;
}

class CtaEcsCachedConfig : public virtual ItaEcsCachedConfig, public virtual  ItaEcsCachedConfigInitialiser
{

public:

    CtaEcsCachedConfig(ItaEcsCachedMapsInitialiser& cachedMapsInitialiserInstance);
	virtual ~CtaEcsCachedConfig();


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
    virtual void attachObserver(ItaEcsCachedConfigObserver* observer, EItaEcsCachedConfigItemKey key);


    /**
     * detachObserver
	 *
	 * Removes all instances of the ItaEcsCachedConfigObserver pointer in the observer collection.  The
     * observer will be removed for all keys.
     *
     */
    virtual void detachObserver(ItaEcsCachedConfigObserver* observer);

    /**
     * updateInterestedObservers
	 *
	 * Calls the processItaEcsCachedConfigUpdate() method on all registered observers interested in the 
     * specified key.
     *
     * @param key used to call specific observers
     *
     */
    virtual void notifyInterestedObservers(EItaEcsCachedConfigItemKey key);



    ///////////////////////////////////////////////////////////////////////////
    // SetMethods
    ///////////////////////////////////////////////////////////////////////////
    // All setX methods can throw(TA_Base_Core::InvalidECSConfigurationException);
    //
    // Removed setAgentName() as this is now taken from RunParams 
    ///////////////////////////////////////////////////////////////////////////

    virtual void initialiseAllConfigAndMaps(TA_Base_Core::IEntityDataPtr entityData, const DataPoints& datapoints);

    virtual void setEntityData(TA_Base_Core::IEntityDataPtr entityData);
    virtual void setDataPoints(const DataPoints& datapoints);

    virtual void processEntityData();
    virtual void processGlobalParameters();

    // Common
    virtual void setIsMaster(bool param);
    virtual void setIsInControl(bool param);
    virtual void setAgentInRuntime(bool param);

    virtual void setAgentKey(unsigned long param);
    virtual void setAgentTypeKey(unsigned long param);
    virtual void setAgentLocationKey(unsigned long param);
    virtual void setAgentSubsystemKey(unsigned long param);   

    // Station
    virtual void setIsOccOnline(bool param);
    virtual void setOccGroupName(const std::string& param);


    ///////////////////////////////////////////////////////////////////////////
    // GetMethods
    ///////////////////////////////////////////////////////////////////////////
    //
    // All getX methods can assert if they are not initialised, except for 
    // getAgentX methods which arew possibly called early for raising alarms.
    //
    ///////////////////////////////////////////////////////////////////////////

    // Common getMethods

    virtual bool                                getIsMaster();
    virtual bool                                getIsInControl();
    virtual unsigned long                       getAgentKey();
    virtual std::string                         getAgentName();
    virtual unsigned long                       getAgentTypeKey();
    virtual unsigned long                       getAgentLocationKey();
    virtual unsigned long                       getAgentSubsystemKey();
    virtual bool                                getAgentInRuntime();
    
    virtual unsigned long                       getWorkerThreadsSleepTimeInMSecs();
    virtual unsigned long                       getMaxModeExecutionTimeInSecs();
	
	virtual std::string							getAssetName();

    // StationECSAgent getMethods

    virtual bool                                getIsOccOnline();
    virtual std::string                         getOccGroupName();
    virtual unsigned long                       getLocalATSAgentKey();
    virtual std::string                         getLocalATSAgentName();
    virtual unsigned long                       getSpareATSAgentKey();
    virtual unsigned long                       getSpareATSAgentLocationKey();
    virtual std::string                         getSpareATSAgentName();
    virtual unsigned long                       getThresholdStallTimeInSecs();
    virtual unsigned long                       getThresholdTemperature();
    virtual unsigned long                       getTemperatureDeadbandLowerLimit();
    virtual bool                                getCongestionUsesStallTime();
    virtual bool                                getCongestionUsesTemperature();
    
    virtual unsigned long                       getIscsAlivePollTimeInMSecs();
    virtual unsigned long                       getNormalStationMode();
    virtual TA_Base_Bus::DataPoint*             getMyMasterModeLockDataPoint();
    virtual TA_Base_Bus::DataPoint*             getMyMasterModeLockControlDataPoint();
    virtual TA_Base_Bus::DataPoint*             getMySelectedMasterModeDataPoint();
    virtual TA_Base_Bus::DataPoint*             getMySelectedOverridingStationModeDataPoint();
    virtual TA_Base_Bus::DataPoint*             getMyLastAppliedMasterModeDataPoint();
    virtual TA_Base_Bus::DataPoint*             getMyLastAppliedStationModeDataPoint();

    virtual unsigned long getModeControlDpEntityKey();  
    virtual unsigned long getModeInProgressDpEntityKey();  
    virtual unsigned long getAutoModeDpEntityKey();  

    virtual TA_Base_Bus::DataPointProxySmartPtr &getModeStatusProxy();
    virtual TA_Base_Bus::DataPointProxySmartPtr &getLocalModeProxy();
    virtual TA_Base_Bus::DataPointProxySmartPtr &getIscsAliveProxy();   

	virtual unsigned long getIscsEntityKey();
	virtual std::string getIscsAgentName();

    // MasterECSAgent getMethods

    virtual unsigned long                       getSelectionTimeoutInMSecs();
    virtual unsigned long                       getNormalMasterMode();
    virtual TA_Base_Bus::DataPoint*             getMyModeStatusDataPoint();
    virtual TA_Base_Bus::DataPoint*             getMyModeInProgressDataPoint();

    // Common

    
    // Station


private:

    void initialiseCachedMaps();

    // Common 

    void processWorkerThreadsSleepTimeInMSecs();
    void processMaxModeExecutionTimeInSecs();
    void processMyZonesCongestionToken();
    void processMyZonesEmergencyToken(); 
    void processLocalATSAgentKey();
    void processSpareATSAgentKey();
	void processAssetName();

    // StationECSAgent

    void processIscsAlivePollTimeInSecs();
    void processNormalStationMode();
    
    void processMyMasterModeLockToken();
    void processMyMasterModeLockControlToken();
    void processMySelectedMasterModeToken();
    void processMySelectedOverridingStationModeToken();
    void processMyLastAppliedMasterModeToken();
    void processMyLastAppliedStationModeToken();

    void processModeStatusSqlToken();
    void processModeControlSqlToken();
    void processModeInProgressSqlToken();
    void processLocalModeSqlToken();
    void processIscsAliveSqlToken();   
    void processAutoModeSqlToken();  


    // MasterECSAgent

    void processSelectionTimeoutInSecs();
    void processNormalMasterMode();

    //void processMyZonesCongestionAppliedToken(); //Datapoint is removed
    void processMyZonesEmergencyAppliedToken();
    void processMyZonesEvacuationSelectedToken();
    void processMyZonesAirflowSelectedToken();
    void processMyZonesEvacuationAppliedToken();
    void processMyZonesAirflowAppliedToken();
    void processMyModeStatusToken();
    void processMyModeInProgressToken();

    void processMasterModeLocksSqlToken();
    void processSelectedMasterModesSqlToken();
    void processSelectedOverridingStationModesSqlToken();
    void processLastAppliedMasterModesSqlToken();
    void processLastAppliedStationModesSqlToken();
    void processLocalModesSqlToken();


    // Global Parameters

    void processThresholdStallTimeInSecs();
    void processThresholdTemperature();
    void processTemperatureDeadband();
    void processCongestionUsesStallTime();
    void processCongestionUsesTemperature();


    // Helpers

    DataPoints getDataPointsByToken(const std::string& token);
        //throw(TA_Base_Core::InvalidECSConfigurationException);
    TA_Base_Bus::DataPoint* getDataPointByToken(const std::string& token);
        //throw(TA_Base_Core::InvalidECSConfigurationException);

    boost::shared_ptr<TA_Base_Core::DataPointEntityData> getEntityBySqlToken(const std::string& sqlToken);
        //throw(TA_Base_Core::InvalidECSConfigurationException);

    ItaEcsCachedMapsInitialiser&                m_cachedMapsInitialiserInstance;

    TA_Base_Core::ReEntrantThreadLockable    m_lockForConfigUpdate;
    TA_Base_Core::ReEntrantThreadLockable    m_lockForObserverMap;
    NullEventProcessor                          m_nullEventProcessor;

    bool                                m_isMaster;
    bool                                m_isInControl;   
    unsigned long                       m_agentKey;
    std::string                         m_agentName;
    unsigned long                       m_agentTypeKey;
    unsigned long                       m_agentLocationKey;
    unsigned long                       m_agentSubsystemKey;
    bool                                m_agentInRuntime;

	std::string							m_assetName;

    bool                                m_isOccOnline;
    std::string                         m_occGroupName;
    unsigned long                       m_localATSAgentKey;
    std::string                         m_localATSAgentName;
    unsigned long                       m_spareATSAgentKey;
    std::string                         m_spareATSAgentName;
    unsigned long                       m_spareATSAgentLocationKey;
    unsigned long                       m_thresholdStallTimeInSecs;
    unsigned long                       m_thresholdTemperature;
    unsigned long                       m_temperatureDeadbandLowerLimit;
    bool                                m_congestionUsesStallTime;
    bool                                m_congestionUsesTemperature;

    // Common EntityParameters
    unsigned long                       m_workerThreadsSleepTimeInMSecs;
    unsigned long                       m_maxModeExecutionTimeInSecs;

    // StationECSAgent EntityParameters
    unsigned long                       m_normalStationMode;
    unsigned long                       m_iscsAlivePollTimeInMSecs;

    TA_Base_Bus::DataPoint*                     m_myMasterModeLockDataPoint;
    TA_Base_Bus::DataPoint*                     m_myMasterModeLockControlDataPoint;
    TA_Base_Bus::DataPoint*                     m_mySelectedMasterModeDataPoint;
    TA_Base_Bus::DataPoint*                     m_mySelectedOverridingStationModeDataPoint;
    TA_Base_Bus::DataPoint*                     m_myLastAppliedMasterModeDataPoint;
    TA_Base_Bus::DataPoint*                     m_myLastAppliedStationModeDataPoint;
    TA_Base_Bus::DataPoint*                     m_myModeStatusDataPoint;
    TA_Base_Bus::DataPoint*                     m_myModeInProgressDataPoint;

    TA_Base_Bus::DataPointProxySmartPtr         m_iscsAliveProxy;
    TA_Base_Bus::DataPointProxySmartPtr         m_modeStatusProxy;
    TA_Base_Bus::DataPointProxySmartPtr         m_localModeProxy;   

    unsigned long m_modeControlDpEntity;
    unsigned long m_modeInProgressDpEntity;
    unsigned long m_autoModeDpEntity;
    
    // MasterECSAgent EntityParameters
    unsigned long                      m_selectionTimeoutInMSecs;
    unsigned long                      m_normalMasterMode;

    TA_Base_Core::StationECSAgentEntityDataPtr    m_stationEntityData;
    TA_Base_Core::MasterECSAgentEntityDataPtr     m_masterEntityData;
    DataPoints									  m_dataPoints;

    // Observer maps
    typedef std::multimap<EItaEcsCachedConfigItemKey, ItaEcsCachedConfigObserver*> KeyToObserversMap;
    typedef KeyToObserversMap::iterator                                 KeyToObserversMapIt;
    typedef KeyToObserversMap::value_type                               key2observer;

    KeyToObserversMap   m_keyToObserversMap;
};

#endif // !defined(AFX_CtaEcsCachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
