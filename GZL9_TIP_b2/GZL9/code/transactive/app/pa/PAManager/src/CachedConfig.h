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
// CachedConfig.h: interface for the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
#define AFX_CachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_

#include "core/exceptions/src/PASExceptions.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"

class ICachedConfigObserver;

class CachedConfig  
: 
public TA_Base_Core::RunParamUser
{

public:

    // Observable descriptors

    typedef enum 
    {
        KEY_LOCAL_PA_AGENT_KEY,
        KEY_LOCAL_PA_AGENT_CORBA_NAME,
        KEY_LOCAL_PA_AGENT_NAME
    } ECachedConfigItemKey;

    typedef enum 
    {
        LOCATION_TYPE_INVALID,
        LOCATION_TYPE_OCC,
        LOCATION_TYPE_DEPOT,
        LOCATION_TYPE_STATION
    } ELocationType;

    /*
     * onRunParamChange (interface implementation)
     *
     * @see RunParamUser::onRunParamChange for more details
     *
     */
	virtual void onRunParamChange(const std::string& name, const std::string& value);

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

    void setEntityKey(unsigned long param);
    void setEntityName(const std::string& param);
    void setEntityTypeKey(unsigned long param);
    void setEntityLocationKey(unsigned long param) throw(TA_Base_Core::InvalidPasConfigurationException);
    void setEntitySubsystemKey(unsigned long param) throw(TA_Base_Core::InvalidPasConfigurationException);
    void setPABackEntityKey( unsigned long ulKey );

    /**
     * setSessionId
     *
     * Sets the session Id, and in doing so sets some internal data related
     *  to the session Id (via database access)
     *     
     */
    void setSessionId(const std::string& sessionId);

    void setLocationType(ELocationType param);

    // The standard entity details below should not assert / throw since we may need to raise 
    // alarms with half-complete configuration.
    unsigned long       getEntityKey();
    std::string         getEntityName();
    unsigned long       getEntityTypeKey();
    unsigned long       getEntityLocationKey();
    unsigned long       getEntitySubsystemKey();
    unsigned long       getPABackEntityKey();
    std::string         getSessionId();

    /**
     * getLocalConsoleRecordingSourceId
     *
     * @pre setSessionId has been called (this is point at which this 
     *          information loaded)
     */
    unsigned long       getLocalConsoleRecordingSourceId();

    /**
     * getLocalConsolePlaybackZoneId
     *
     * @pre setSessionId has been called (this is point at which this 
     *          information loaded)
     */
    unsigned long       getLocalConsolePlaybackZoneId();

    void                setPhysicalLocationKey(unsigned long key);
    unsigned long       getPhysicalLocationKey();
    
    ELocationType       getLocationType();

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
     * isAtOcc
     *
     * @return true if this PAManager is operating from OCC
     *
     */
    bool isAtOcc();

    /**
     * isDepotLocation
     *
     * @return true if the input location key corresponds to the depot location
     *
     * @param locationKey the location key of the area we're checking to see if it's the depot
     *
     */
    bool isDepotLocation(unsigned long locationKey);

    /**
     * getDepotLocatonKey
     *
     * @return the location key of the depot station
     * @exception ValueNotSetException if unable to fetch data
     */
    unsigned long getDepotLocationKey();
 
    void registerRunParamUser();

private:

    CachedConfig();
	virtual ~CachedConfig();

   	static CachedConfig* m_me;

    static TA_Base_Core::NonReEntrantThreadLockable m_lockForConfigUpdate;
    static TA_Base_Core::NonReEntrantThreadLockable m_lockForObserverMap;

    // global parameters
    unsigned long       m_entityKey;
    std::string         m_entityName;
    unsigned long       m_entityTypeKey;
    TA_Base_Core::PrimitiveWrapper<unsigned long> m_entityLocationKey;
    unsigned long       m_entitySubsystemKey;

    unsigned long       m_physicalLocationKey;

    unsigned long       m_pagingConsoleSourceId;
    unsigned long       m_pagingConsoleZoneId;

    unsigned long       m_ulPABackEntityKey;

    ELocationType       m_locationType;
    
    // The Session (or GUI) identifier for this operator terminal
    std::string         m_sessionId;
    
    typedef std::multimap<ECachedConfigItemKey, ICachedConfigObserver*>  KeyToObserversMap;
    typedef KeyToObserversMap::iterator                                  KeyToObserversMapIt;
    typedef KeyToObserversMap::value_type                                key2observer;

    KeyToObserversMap   m_keyToObserversMap;

    // Caches the depot location key
    TA_Base_Core::PrimitiveWrapper<unsigned long> m_depotLocationKey;
};

#endif // !defined(AFX_CachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
