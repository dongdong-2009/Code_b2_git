/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/ItaEcsCachedMaps.h $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2017/06/23 14:36:37 $
  * Last modified by:  $Author: dhanshri $
  *
  */
// ItaEcsCachedMaps.h: interface for the ItaEcsCachedMaps class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ItaEcsCachedMaps_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
#define AFX_ItaEcsCachedMaps_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_

#include <map>
#include <set>
#include <vector>
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/ecs/ecs_agent/IDL/src/CommonEcsAgentCorbaDef.h"
#include "app/ecs/ECSAgent/src/EcsGlobals.h"


class ItaEcsCachedMapsObserver;


class ItaEcsCachedMaps  
{

public:

    
    // Observable descriptors

    typedef enum 
    {
        MAP_UNSURE,
        MAP_ZONE_RECORDS,
        MAP_STATION_MODE_RECORDS,
        MAP_MASTER_MODE_RECORDS,
        MAP_LOCATION_NAMES,
        MAP_MASTER_MODE_LOCK_PROXIES,
        MAP_SELECTED_MASTER_MODE_PROXIES,
        MAP_SELECTED_OVERRIDING_STATION_MODE_PROXIES,
        MAP_LAST_APPLIED_MASTER_MODE_PROXIES,
        MAP_LAST_APPLIED_STATION_MODE_PROXIES,
        MAP_LOCAL_MODE_PROXIES,
        MAP_MY_ZONE_CONGESTION_DATAPOINTS,
        MAP_MY_ZONE_EMERGENCY_DATAPOINTS,
        //MAP_MY_ZONE_CONGESTION_APPLIED_DATAPOINTS, //Datapoint is removed
        MAP_MY_ZONE_EMERGENCY_APPLIED_DATAPOINTS,
        MAP_MY_ZONE_EVACUATION_SELECTED_DATAPOINTS,
        MAP_MY_ZONE_AIRFLOW_SELECTED_DATAPOINTS,
        MAP_MY_ZONE_EVACUATION_APPLIED_DATAPOINTS,
        MAP_MY_ZONE_AIRFLOW_APPLIED_DATAPOINTS,
        MAP_STATION_ECS_AGENT_CORBA_NAMES
    } EItaEcsCachedMapsItemKey;



    /**
     * attachObserver
	 *
	 * Adds the ItaEcsCachedMapsObserver instance to the collection of observers.  If the observer 
     * already exists for the key, no change will be made.
     *
     * @param observer pointer to the ItaEcsCachedMapsObserver instance
     *
     * @param key the EItaEcsCachedMapsItemKey the observer is interested in
     *
     */
    virtual void attachObserver(ItaEcsCachedMapsObserver* observer, EItaEcsCachedMapsItemKey key)=0;


    /**
     * detachObserver
	 *
	 * Removes all instances of the ItaEcsCachedMapsObserver pointer in the observer collection.  The
     * observer will be removed for all keys.
     *
     */
    virtual void detachObserver(ItaEcsCachedMapsObserver* observer)=0;

    /**
     * notifyInterestedObservers
	 *
	 * Calls the processItaEcsCachedMapsUpdate() method on all registered observers interested in the 
     * specified key.
     *
     * @param key used to call specific observers
     *
     */
    virtual void notifyInterestedObservers(EItaEcsCachedMapsItemKey key)=0;

   /**
     * getX methods for ZoneRecords map
	 *
	 * Methods to retrieve specific data from the map
     *
     * @throws CachedMappingNotFoundException if the mapping could not be found.
     *
     */

    virtual TA_IRS_Bus::EcZoneRecord getEcZoneRecord(unsigned long zoneId)=0;

    virtual std::string getTrack(unsigned long zoneId)=0;

    virtual std::string getEcsLocation(unsigned long zoneId)=0;

    virtual bool getIsIgnoringCongestion(unsigned long zoneId)=0;

    virtual std::vector<unsigned long> getAllZoneIds()=0;

    virtual bool isZoneIdInScope(unsigned long zoneId)=0;

    virtual unsigned long getZoneIdFromEquipmentEntityKey(unsigned long entityKey)=0;

	virtual bool checkIfEmergencyMasterMode(unsigned long mastermode)=0;
   /**
     * getX methods for StationModeRecords map
	 *
	 * Methods to retrieve specific data from the map
     *
     * @throws CachedMappingNotFoundException if the mapping could not be found.
     *
     */

	virtual void getEcStationModeRecords(TA_IRS_Bus::EcStationModeRecords& retVal)=0; 

    virtual TA_IRS_Bus::EcStationModeRecord getEcStationModeRecord(unsigned long key)=0;

	virtual void getEcStationModeRecordsForLocation(
		unsigned long locationKey, 
		TA_IRS_Bus::EcStationModeRecords& retVal)=0; 

    virtual void getEcStationModeRecord(
		unsigned long key, 
        unsigned long& locationKey,
        unsigned long& stationMode)=0;

    virtual unsigned long getStationModeFromStationState
        (const std::string& condition, const std::string& track)=0;

   /**
     * getX methods for MasterModeRecords map
	 *
	 * Methods to retrieve specific data from the map
     *
     * @throws CachedMappingNotFoundException if the mapping could not be found.
     *
     */

    virtual TA_IRS_Bus::EcMasterModeRecord getEcMasterModeRecord(unsigned long key)=0;

    virtual TA_IRS_Bus::EcMasterModeRecord getEcMasterModeRecordFromMasterMode(unsigned long masterMode)=0;

    virtual MasterModePair getMasterModesFromZoneId(unsigned long zoneId)=0;

    virtual unsigned long getZoneIdFromMasterMode(unsigned long masterMode)=0;

    virtual TA_IRS_Bus::EcStationModeKeys getEcStationModeKeysFromMasterMode(unsigned long masterMode)=0;

    virtual TA_IRS_Bus::EcStationModeRecords getEcStationModeRecordsFromMasterMode(unsigned long masterMode)=0;

   /**
     * getLocationName methods for LocationNames map
	 *
	 * Methods to retrieve specific data from the map
     *
	 * Returns "Unknown" if mapping annot be found
     */

    virtual std::string getLocationName(unsigned long locationKey)=0;

   /**
     * getLocationKey methods for LocationNames map
	 *
	 * Methods to retrieve specific data from the map
     *
     * @throws CachedMappingNotFoundException if the mapping could not be found.
     *
     */

    virtual unsigned long getLocationKey(const std::string& locationName)=0;


   /**
     * getX methods for MasterModeLockProxies map
	 *
	 * Methods to retrieve specific data from the map
     *
     * @throws CachedMappingNotFoundException if the mapping could not be found.
     *
     */

    virtual TA_Base_Bus::DataPointProxySmartPtr &getMasterModeLockProxy(unsigned long locationKey)=0;

    virtual TA_Base_Bus::DataPointProxySmartPtr &getSelectedMasterModeProxy(unsigned long locationKey)=0;

    virtual TA_Base_Bus::DataPointProxySmartPtr &getSelectedOverridingStationModeProxy(unsigned long locationKey)=0;

    virtual TA_Base_Bus::DataPointProxySmartPtr &getLastAppliedMasterModeProxy(unsigned long locationKey)=0;

    virtual TA_Base_Bus::DataPointProxySmartPtr &getLastAppliedStationModeProxy(unsigned long locationKey)=0;


   /**
     * getX methods for LocalModeProxies map
	 *
	 * Methods to retrieve specific data from the map
     *
     * @throws CachedMappingNotFoundException if the mapping could not be found.
     *
     */

    virtual TA_Base_Bus::DataPointProxySmartPtr& getLocalModeProxy(unsigned long locationKey)=0;


    /**
     * getX methods for MyZoneCongestionDataPoint map
     *
     * Methods to retrieve specific data from the map.  Note no exceptions are thrown
     *
     * @return NULL if datapoint cannot be found (out of scope)
     *
     */
    virtual TA_Base_Bus::DataPoint* getMyZoneCongestionDataPoint(unsigned long zoneId)=0;

    virtual TA_Base_Bus::DataPoint* getMyZoneEmergencyDataPoint(unsigned long zoneId)=0;

    //Datapoint no longer exists
    //virtual TA_Base_Bus::DataPoint* getMyZoneCongestionAppliedDataPoint(unsigned long zoneId)=0;

    virtual TA_Base_Bus::DataPoint* getMyZoneEmergencyAppliedDataPoint(unsigned long zoneId)=0;

    //Datapoint no longer exists
    //virtual TA_Base_Bus::DataPoint* getMyZoneCongestionAppliedDataPointForCongestionEntityKey(unsigned long entityKey)=0;

    virtual TA_Base_Bus::DataPoint* getMyZoneEvacuationSelectedDataPoint(unsigned long zoneId)=0;

    virtual TA_Base_Bus::DataPoint* getMyZoneAirflowSelectedDataPoint(unsigned long zoneId)=0;

    virtual TA_Base_Bus::DataPoint* getMyZoneEvacuationAppliedDataPoint(unsigned long zoneId)=0;

    virtual TA_Base_Bus::DataPoint* getMyZoneAirflowAppliedDataPoint(unsigned long zoneId)=0;



   /**
     * getX methods for MyZoneEmergencyDataPoint map
	 *
	 * Methods to retrieve specific data from the map
     *
     * @throws CachedMappingNotFoundException if the mapping could not be found.
     *
     */

    virtual TA_Base_Core::CorbaName getStationEcsAgentCorbaName(unsigned long locationKey)=0;

    virtual std::vector<unsigned long> getAllStationEcsAgentLocationKeys()=0;

	virtual std::map<unsigned long, unsigned long> getEmergencyEntityKeyToZoneId()=0;

};




class ItaEcsCachedMapsObserver
{
public:
    virtual void processItaEcsCachedMapsUpdate(ItaEcsCachedMaps::EItaEcsCachedMapsItemKey key) = 0;
};


#endif // !defined(AFX_ItaEcsCachedMaps_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)


