/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/CtaEcsCachedMaps.h $
  * @author:  Ripple
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2017/06/23 14:36:37 $
  * Last modified by:  $Author: dhanshri $
  *
  */
// CtaEcsCachedMaps.h: interface for the CtaEcsCachedMaps class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CtaEcsCachedMaps_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
#define AFX_CtaEcsCachedMaps_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_

#include <map>
#include <set>
#include <vector>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "app/ecs/ECSAgent/src/CorbaNameWrapperForMaps.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedMaps.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedMapsInitialiser.h"

class CtaEcsCachedMaps : public virtual ItaEcsCachedMaps, public virtual ItaEcsCachedMapsInitialiser
{

private:

    struct EcsZoneDataPoints
    {
        typedef enum
        {
            CONGESTION = 0,
            EMERGENCY = 1,
            //CONGESTION_APPLIED  =2,
            EMERGENCY_APPLIED   =3,
            EVACUATION_SELECTED =4,
            AIRFLOW_SELECTED    =5,
            EVACUATION_APPLIED  =6,
            AIRFLOW_APPLIED     =7,
            TOTAL_ITEMS         =8
        } EcsZoneDataPointTypes;

        EcsZoneDataPoints()
        { 
            m_dataPoints.reserve(TOTAL_ITEMS);
            m_dataPoints.assign(TOTAL_ITEMS,NULL);
        }

        TA_Base_Bus::DataPoint* getDataPoint(EcsZoneDataPointTypes dataPointType)
        {
            return m_dataPoints[dataPointType];
        }

        void setDataPoint(EcsZoneDataPointTypes dataPointType, TA_Base_Bus::DataPoint* dataPoint)
        {
            m_dataPoints[dataPointType]=dataPoint;
        }

        DataPoints  m_dataPoints;
    };


public:

    CtaEcsCachedMaps();
	virtual ~CtaEcsCachedMaps();

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
    virtual void attachObserver(ItaEcsCachedMapsObserver* observer, EItaEcsCachedMapsItemKey key);


    /**
     * detachObserver
	 *
	 * Removes all instances of the ItaEcsCachedMapsObserver pointer in the observer collection.  The
     * observer will be removed for all keys.
     *
     */
    virtual void detachObserver(ItaEcsCachedMapsObserver* observer);

    /**
     * notifyInterestedObservers
	 *
	 * Calls the processCtaEcsCachedMapsUpdate() method on all registered observers interested in the 
     * specified key.
     *
     * @param key used to call specific observers
     *
     */
    virtual void notifyInterestedObservers(EItaEcsCachedMapsItemKey key);


    /**
     * refreshX methods
	 *
	 * Methods to refresh/initialise the contents of the maps
     *
     * @throws DatabaseException 
     *         If there is a problem establishing a connection with the database
     *         or if an error is encountered while retrieving data.
     * @throws DataException      
     *         If the data cannot be converted to the required format or if the 
     *         wrong ammount of data is retrieved.
     * @throws InvalidECSConfigurationException 
     *         If a single zoneId is mapped to more than one record (i.e. 
     *         zoneIds are not unique).  This indicates invalid configuration 
     *         in the databases.
     */

    // Reads all zoneId records
    virtual void refreshZoneRecords();

    // specify locationKey to refresh for a given location only
    virtual void refreshStationModeRecords(unsigned long locationKey=-1); 

    virtual void refreshMasterModeRecords();
    
    virtual void refreshLocationNames();
    
    virtual void refreshMasterModeLockProxies(const std::string& sqlToken);
    
    virtual void refreshSelectedMasterModeProxies(const std::string& sqlToken);
    
    virtual void refreshSelectedOverridingStationModeProxies(const std::string& sqlToken);
    
    virtual void refreshLastAppliedMasterModeProxies(const std::string& sqlToken);
    
    virtual void refreshLastAppliedStationModeProxies(const std::string& sqlToken);
    
    virtual void refreshLocalModeProxies(const std::string& sqlToken);


    // Pre: refreshZoneRecords() called
    virtual void refreshMyZoneCongestionDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints);

    // Pre: refreshZoneRecords() called
    virtual void refreshMyZoneEmergencyDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints);

    // Datapoint is removed
    // Pre: refreshZoneRecords() called
    //virtual void refreshMyZoneCongestionAppliedDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints);
    
    // Pre: refreshZoneRecords() called
    virtual void refreshMyZoneEmergencyAppliedDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints);

    // Pre: refreshZoneRecords() called
    virtual void refreshMyZoneEvacuationSelectedDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints);

    // Pre: refreshZoneRecords() called
    virtual void refreshMyZoneAirflowSelectedDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints);

    // Pre: refreshZoneRecords() called
    virtual void refreshMyZoneEvacuationAppliedDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints);

    // Pre: refreshZoneRecords() called
    virtual void refreshMyZoneAirflowAppliedDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints);

    virtual void refreshStationEcsAgentCorbaNames();
 
    virtual void refreshZoneIdInScope(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints);
    /**
     * validateX methods
	 *
	 * Verification methods to validate loaded data after all required maps have
     * refreshed.
     *
     * @throws InvalidECSConfigurationException if data is invalid
     *
     */

    virtual void validateAllInitializedMaps(); 

   /**
     * getX methods for ZoneRecords map
	 *
	 * Methods to retrieve specific data from the map
     *
     * @throws CachedMappingNotFoundException if the mapping could not be found.
     *
     */
	virtual bool checkIfEmergencyMasterMode(unsigned long mastermode);

    virtual TA_IRS_Bus::EcZoneRecord getEcZoneRecord(unsigned long zoneId);

    virtual std::string getTrack(unsigned long zoneId);

    virtual std::string getEcsLocation(unsigned long zoneId);

    virtual bool getIsIgnoringCongestion(unsigned long zoneId);

    virtual std::vector<unsigned long> getAllZoneIds();

    virtual bool isZoneIdInScope(unsigned long zoneId);

    virtual unsigned long getZoneIdFromEquipmentEntityKey(unsigned long entityKey);


   /**
     * getX methods for StationModeRecords map
	 *
	 * Methods to retrieve specific data from the map
     *
     * @throws CachedMappingNotFoundException if the mapping could not be found.
     *
     */

	
	virtual void getEcStationModeRecords(TA_IRS_Bus::EcStationModeRecords& retVal); 

    virtual TA_IRS_Bus::EcStationModeRecord getEcStationModeRecord(unsigned long key);

	virtual void getEcStationModeRecordsForLocation(
		unsigned long locationKey, 
		TA_IRS_Bus::EcStationModeRecords& retVal); 

    virtual void getEcStationModeRecord(
		unsigned long key, 
        unsigned long& locationKey,
        unsigned long& stationMode);

    virtual unsigned long getStationModeFromStationState
        (const std::string& condition, const std::string& track);

   /**
     * getX methods for MasterModeRecords map
	 *
	 * Methods to retrieve specific data from the map
     *
     * @throws CachedMappingNotFoundException if the mapping could not be found.
     *
     */

    virtual TA_IRS_Bus::EcMasterModeRecord getEcMasterModeRecord(unsigned long key);

    virtual TA_IRS_Bus::EcMasterModeRecord getEcMasterModeRecordFromMasterMode(unsigned long masterMode);

    virtual MasterModePair getMasterModesFromZoneId(unsigned long zoneId);

    virtual unsigned long getZoneIdFromMasterMode(unsigned long masterMode);

    virtual TA_IRS_Bus::EcStationModeKeys getEcStationModeKeysFromMasterMode(unsigned long masterMode);

    virtual TA_IRS_Bus::EcStationModeRecords getEcStationModeRecordsFromMasterMode(unsigned long masterMode);

   /**
     * getLocationName methods for LocationNames map
	 *
	 * Methods to retrieve specific data from the map
     *
	 * Returns "Unknown" if mapping annot be found
     */

    virtual std::string getLocationName(unsigned long locationKey);

   /**
     * getLocationKey methods for LocationNames map
	 *
	 * Methods to retrieve specific data from the map
     *
     * @throws CachedMappingNotFoundException if the mapping could not be found.
     *
     */

    virtual unsigned long getLocationKey(const std::string& locationName);


   /**
     * getX methods for MasterModeLockProxies map
	 *
	 * Methods to retrieve specific data from the map
     *
     * @throws CachedMappingNotFoundException if the mapping could not be found.
     *
     */

    virtual TA_Base_Bus::DataPointProxySmartPtr & getMasterModeLockProxy(unsigned long locationKey);

    virtual TA_Base_Bus::DataPointProxySmartPtr & getSelectedMasterModeProxy(unsigned long locationKey);

    virtual TA_Base_Bus::DataPointProxySmartPtr &getSelectedOverridingStationModeProxy(unsigned long locationKey);

    virtual TA_Base_Bus::DataPointProxySmartPtr & getLastAppliedMasterModeProxy(unsigned long locationKey);

    virtual TA_Base_Bus::DataPointProxySmartPtr & getLastAppliedStationModeProxy(unsigned long locationKey);


   /**
     * getX methods for LocalModeProxies map
	 *
	 * Methods to retrieve specific data from the map
     *
     * @throws CachedMappingNotFoundException if the mapping could not be found.
     *
     */

    virtual TA_Base_Bus::DataPointProxySmartPtr &getLocalModeProxy(unsigned long locationKey);


    /**
     * getX methods for MyZoneCongestionDataPoint map
     *
     * Methods to retrieve specific data from the map.  Note no exceptions are thrown
     *
     * @return NULL if datapoint cannot be found (out of scope)
     *
     */
    virtual TA_Base_Bus::DataPoint* getMyZoneCongestionDataPoint(unsigned long zoneId);

    virtual TA_Base_Bus::DataPoint* getMyZoneEmergencyDataPoint(unsigned long zoneId);

    //Datapoint no longer exists
    //virtual TA_Base_Bus::DataPoint* getMyZoneCongestionAppliedDataPoint(unsigned long zoneId);

    virtual TA_Base_Bus::DataPoint* getMyZoneEmergencyAppliedDataPoint(unsigned long zoneId);

    //Datapoint no longer exists
    //virtual TA_Base_Bus::DataPoint* getMyZoneCongestionAppliedDataPointForCongestionEntityKey(unsigned long entityKey);

    virtual TA_Base_Bus::DataPoint* getMyZoneEvacuationSelectedDataPoint(unsigned long zoneId);

    virtual TA_Base_Bus::DataPoint* getMyZoneAirflowSelectedDataPoint(unsigned long zoneId);

    virtual TA_Base_Bus::DataPoint* getMyZoneEvacuationAppliedDataPoint(unsigned long zoneId);

    virtual TA_Base_Bus::DataPoint* getMyZoneAirflowAppliedDataPoint(unsigned long zoneId);



   /**
     * getX methods for MyZoneEmergencyDataPoint map
	 *
	 * Methods to retrieve specific data from the map
     *
     * @throws CachedMappingNotFoundException if the mapping could not be found.
     *
     */

    virtual TA_Base_Core::CorbaName getStationEcsAgentCorbaName(unsigned long locationKey);

    virtual std::vector<unsigned long> getAllStationEcsAgentLocationKeys();

	virtual std::map<unsigned long, unsigned long> getEmergencyEntityKeyToZoneId();


protected:


    typedef std::map<unsigned long, TA_IRS_Bus::EcZoneRecord>        
        ZoneIdToEcZoneRecordMap;

    typedef std::map<unsigned long, TA_IRS_Bus::EcStationModeRecord> 
        StationModeKeyToEcStationModeRecordMap;

    typedef std::map<unsigned long, TA_IRS_Bus::EcMasterModeRecord> 
        MasterModeKeyToEcMasterModeRecordMap;

    typedef std::map<unsigned long, std::string>                
        LocationKeyToNameMap;

    typedef std::map<unsigned long, CorbaNameWrapperForMaps>                
        LocationKeyToCorbaNameMap;

    typedef std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>                
        LocationKeyToProxyMap;

    typedef std::map<unsigned long, CorbaNameWrapperForMaps>
        ZoneIdToCorbaNameMap;

    typedef std::map<unsigned long, EcsZoneDataPoints>
        ZoneIdToEcsZoneDataPointsMap;


    /**
     * prevalidateX helper methods
	 *
	 * Methods to validate data as records are loaded before the final 
     * validation stages.
     *
     * @throws InvalidECSConfigurationException invalid configuration detected 
     *         in the databases.
     */

    bool isZoneRecordValid(const TA_IRS_Bus::EcZoneRecord& record);

    bool isStationModeRecordValid(const TA_IRS_Bus::EcStationModeRecord& record);

    /**
     * refreshProxiesHelper
	 *
	 * Common code for refreshing a given map with the Proxies of the datapoints
     * retrieved by the sqlToken 
     *
     * @throws InvalidECSConfigurationException invalid configuration detected 
     *         in the databases.
     */

    void refreshLocationCorbaNamesHelper( const std::string& sqlToken, 
                                          LocationKeyToCorbaNameMap& theMap, 
                                          TA_Base_Core::ReEntrantThreadLockable& theMapLock,
                                          const std::string& entityTypeName,
                                          EItaEcsCachedMapsItemKey notificationMessage,
                                          const std::string& objectLabelForErrorMessages);

    void refreshLocationProxiesHelper( const std::string& sqlToken, 
                                       LocationKeyToProxyMap& theMap, 
                                       TA_Base_Core::ReEntrantThreadLockable& theMapLock,
                                       const std::string& entityTypeName,
                                       EItaEcsCachedMapsItemKey notificationMessage,
                                       const std::string& objectLabelForErrorMessages);

    void refreshMyZoneDataPointsHelper( const std::vector<TA_Base_Bus::DataPoint*>& dataPoints, 
                                        EcsZoneDataPoints::EcsZoneDataPointTypes dataPointType, 
                                        EItaEcsCachedMapsItemKey notificationMessage,
                                        const std::string& objectLabelForErrorMessages);

    void validateZoneRecords();
    void validateStationModeRecords(); 
    void validateMasterModeRecords();
    void validateLocationNames();
    void validateMasterModeLockProxies();
    void validateSelectedMasterModeProxies();
    void validateSelectedOverridingStationModeProxies();
    void validateLastAppliedMasterModeProxies();
    void validateLastAppliedStationModeProxies();
    void validateLocalModeProxies();
    void validateMyZoneDataPoints();
    void validateStationEcsAgentCorbaNames();

    TA_Base_Core::ReEntrantThreadLockable    m_lockForObserverMap;
    NullEventProcessor                          m_nullEventProcessor;

    ZoneIdToEcZoneRecordMap                     m_zoneRecords;
    StationModeKeyToEcStationModeRecordMap      m_stationModeRecords;
    MasterModeKeyToEcMasterModeRecordMap        m_masterModeRecords;
	std::vector<unsigned long>					m_masterModesForEmergency;
    LocationKeyToNameMap                        m_locationNames;
    LocationKeyToProxyMap                       m_masterModeLockProxies;
    LocationKeyToProxyMap                       m_selectedMasterModeProxies;
    LocationKeyToProxyMap                       m_selectedOverridingStationModeProxies;
    LocationKeyToProxyMap                       m_lastAppliedMasterModeProxies;
    LocationKeyToProxyMap                       m_lastAppliedStationModeProxies;
    LocationKeyToProxyMap                       m_localModeProxies;
    ZoneIdToEcsZoneDataPointsMap                m_myZoneDataPoints;
    LocationKeyToCorbaNameMap                   m_stationEcsAgentCorbaNames;

	// Used to remove emergency proposal when alarm closed manually
	std::map<unsigned long, unsigned long>		m_emergencyKeyToZoneId;

    // Members used to keep track of initialising parameters.  Set and reset
    // by the refreshX methods
    std::vector<unsigned long>                  m_zoneIdScope;
    
    // Synchronisation locks for each of the maps
    TA_Base_Core::ReEntrantThreadLockable    m_zoneRecordsLock;
    TA_Base_Core::ReEntrantThreadLockable    m_zoneIdScopeLock;
    TA_Base_Core::ReEntrantThreadLockable    m_stationModeRecordsLock;
    TA_Base_Core::ReEntrantThreadLockable    m_masterModeRecordsLock;
    TA_Base_Core::ReEntrantThreadLockable    m_locationNamesLock;
    TA_Base_Core::ReEntrantThreadLockable    m_masterModeLockProxiesLock;
    TA_Base_Core::ReEntrantThreadLockable    m_selectedMasterModeProxiesLock;
    TA_Base_Core::ReEntrantThreadLockable    m_selectedOverridingStationModeProxiesLock;
    TA_Base_Core::ReEntrantThreadLockable    m_lastAppliedMasterModeProxiesLock;
    TA_Base_Core::ReEntrantThreadLockable    m_lastAppliedStationModeProxiesLock;
    TA_Base_Core::ReEntrantThreadLockable    m_localModeProxiesLock;
    TA_Base_Core::ReEntrantThreadLockable    m_myZoneDataPointsLock;
    TA_Base_Core::ReEntrantThreadLockable    m_stationEcsAgentCorbaNamesLock;
    

    // Observer maps
    typedef std::multimap<EItaEcsCachedMapsItemKey, ItaEcsCachedMapsObserver*> KeyToObserversMap;
    typedef KeyToObserversMap::iterator KeyToObserversMapIt;
    typedef KeyToObserversMap::value_type key2observer;

    KeyToObserversMap                           m_keyToObserversMap;

	TA_Base_Bus::ScadaProxyFactory*				m_proxyFactory;	


};


#endif // !defined(AFX_CtaEcsCachedMaps_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)


