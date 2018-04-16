/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/CtaEcsDatapoints.h $
  * @author:  seanl
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  */

#if !defined(CtaEcsDatapoints_INCLUDED)
#define CtaEcsDatapoints_INCLUDED

#include <set>

#include "app/ecs/ECSAgent/src/ItaEcsCachedMaps.h"
#include "app/ecs/ECSAgent/src/AbstractFailoverable.h"
#include "app/ecs/ECSAgent/src/ItaEcsDatapoints.h"

#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"


class ItaEcsCachedConfig;


class CtaEcsDatapoints : public virtual AbstractFailoverable, 
                         public virtual ItaEcsDatapoints,
                         public ItaEcsCachedMapsObserver,
						 public TA_Base_Bus::IEntityUpdateEventProcessor

{

public:

// Common ECS datapoint access

	
    virtual unsigned long getCongestion(unsigned long zoneId);
    virtual void setCongestion(unsigned long zoneId, unsigned long value, bool fInit = false);
    virtual std::set<unsigned long> getZonesInCongestion();
	virtual void attachCongestionObserver(ICongestionObserver* observer);
	virtual void detachCongestionObserver(ICongestionObserver* observer);

    virtual bool getEmergency(unsigned long zoneId);// throws InvalidECSConfigurationException
    virtual void setEmergency(unsigned long zoneId, bool value, bool fInit = false);
    virtual std::set<unsigned long> getZonesInEmergency();
	virtual void attachEmergencyObserver(IEmergencyObserver* observer);
	virtual void detachEmergencyObserver(IEmergencyObserver* observer);


// Station ECS Agent datapoint access

    virtual bool getMasterModeLocked();
    virtual bool getLocalMode();
    virtual unsigned long getCurrentStationMode();

    virtual void setLastAppliedMasterMode(long value, bool fInit = false);
    virtual void setLastAppliedStationMode(long value, bool fInit = false);
    virtual void setSelectedMasterMode(long value, bool fInit = false);
    virtual void setSelectedOverridingStationMode(long value, bool fInit = false);

    virtual void setMasterModeLock(bool value, bool fInit=false);
    virtual void setMasterModeLockControl(bool value, bool fInit=false);

    virtual void setIscsAlive(unsigned long value);

// Master ECS Agent datapoint access

    virtual void setExternalMasterModeLock(unsigned long locationKey, bool value);
    virtual void setExternalLastAppliedMasterMode(unsigned long locationKey, long  value);
    virtual void setExternalLastAppliedStationMode(unsigned long locationKey, long  value);
    virtual void setExternalSelectedMasterMode(unsigned long locationKey, long  value);
    //virtual void setExternalSelectedMasterMode(unsigned long locationKey, long  value);
    virtual void setExternalSelectedOverridingStationMode(unsigned long locationKey, long  value);
    //virtual void setExternalSelectedOverridingStationMode(unsigned long locationKey, long  value);

	virtual bool getExternalStationInLocalMode(unsigned long locationKey);

	virtual void setMasterModeInProgress(unsigned long value, bool fInit = false);
    virtual unsigned long getMasterModeInProgress();

    // CurrentMasterMode == MasterModeStatus
    virtual unsigned long getCurrentMasterMode();
    virtual void setCurrentMasterMode(unsigned long value, bool fInit = false);
    
    //Datapoint is removed
    //virtual void setCongestionApplied(unsigned long zoneId, bool value, bool fInit = false);
	//virtual void setCongestionAppliedGivenEntityKey( unsigned long entityKey, bool value );

	virtual bool getEmergencyApplied(unsigned long zoneId);
    virtual void setEmergencyApplied(unsigned long zoneId, bool value, bool fInit = false);
    virtual void setEvacuationSelected(unsigned long zoneId, unsigned long value, bool fInit = false);
    virtual void setAirflowSelected(unsigned long zoneId, unsigned long value, bool fInit = false);
    virtual void setEvacuationApplied(unsigned long zoneId, unsigned long value, bool fInit = false);
    virtual void setAirflowApplied(unsigned long zoneId, unsigned long value, bool fInit = false);


public:


    CtaEcsDatapoints(
        ItaEcsCachedMaps& theMaps,
        ItaEcsCachedConfig& theConfig);

    virtual ~CtaEcsDatapoints();

    virtual void initialise();

    // AbstractFailoverable overrides
    virtual void setControl(bool isOnStartup=false);
    virtual void setMonitor(bool isOnStartup=false);

    //virtual void processItaEcsCachedConfigUpdate(ItaEcsCachedConfig::EItaEcsCachedConfigItemKey key);
    virtual void processItaEcsCachedMapsUpdate(ItaEcsCachedMaps::EItaEcsCachedMapsItemKey key);

private:

    void resetDataPoints();

    unsigned long getIntegerDataPoint(TA_Base_Bus::DataPoint* dp, const std::string& descriptionForDebug); // throws EcsOperationModeException, InvalidECSConfigurationException
    void setIntegerDataPoint(TA_Base_Bus::DataPoint* dp, unsigned long value, const std::string& descriptionForDebug, bool fInit = false);
	void setIntegerDataPoint(TA_Base_Bus::DataPoint* dp, long value, const std::string& descriptionForDebug, bool fInit = false);
    
    bool getBooleanDataPoint(TA_Base_Bus::DataPoint* dp, const std::string& descriptionForDebug); // throws EcsOperationModeException, InvalidECSConfigurationException
    void setBooleanDataPoint(TA_Base_Bus::DataPoint* dp, bool value, const std::string& descriptionForDebug, bool fInit = false);
    void setBooleanProxy(TA_Base_Bus::DataPointProxySmartPtr &proxy, bool value, const std::string& descriptionForDebug, bool fInit = false);

	void setIntegerProxy(TA_Base_Bus::DataPointProxySmartPtr &proxy, unsigned long value, const std::string& descriptionForDebug);
    void setIntegerProxy(TA_Base_Bus::DataPointProxySmartPtr &proxy, long value, const std::string& descriptionForDebug);

    virtual void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

	//zn++ - when station ecs agent startup, set forced value instead
	void setForcedIntegerDataPoint(TA_Base_Bus::DataPoint* dp, long value, const std::string& descriptionForDebug);
	//++zn
    


private: 

	typedef std::set<IEmergencyObserver*>           EmergencyObservers;
	typedef EmergencyObservers::iterator			EmergencyObserversIt;
	typedef std::set<ICongestionObserver*>          CongestionObservers;
	typedef CongestionObservers::iterator			CongestionObserversIt;

	struct DataPointObserverData
	{
		DataPointObserverData(unsigned long ecsZoneId, TA_Base_Bus::DataPointProxySmartPtr *proxy)
			: m_ecsZoneId(ecsZoneId), m_proxy(proxy)
		{}
        DataPointObserverData()
			: m_ecsZoneId(TA_IRS_Bus::ECS_NONE_OR_INVALID)
		{}

		unsigned long								m_ecsZoneId;
		TA_Base_Bus::DataPointProxySmartPtr			* m_proxy;
	};

	typedef std::map<unsigned long, DataPointObserverData>	KeyToObserverDataMap;
	typedef KeyToObserverDataMap::iterator			KeyToObserverDataMapIt;



    TA_Base_Core::ReEntrantThreadLockable			m_lockForConfigUpdates;
    TA_Base_Core::ReEntrantThreadLockable			m_lockForObservers;
	
    ItaEcsCachedConfig&								m_theConfig;
    ItaEcsCachedMaps&								m_theMaps;

    std::vector<unsigned long>						m_allZoneIds;

	EmergencyObservers								m_emergencyObservers;
	CongestionObservers								m_congestionObservers;
	KeyToObserverDataMap							m_emergencyObserverData;
	KeyToObserverDataMap							m_congestionObserverData;

	TA_Base_Bus::ScadaProxyFactory*					m_proxyFactory;	

};


#endif // !defined(CtaEcsDatapoints_INCLUDED)
