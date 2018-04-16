/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ECSAgent/src/ItaEcsCachedConfig.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// ItaEcsCachedConfig.h: interface for the ItaEcsCachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ItaEcsCachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
#define AFX_ItaEcsCachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_

#include <string>
#include <vector>
#include "core/exceptions/src/ECSAgentExceptions.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "app/ecs/ECSAgent/src/EcsGlobals.h"


class ItaEcsCachedConfigObserver;

class ItaEcsCachedConfig  
{

public:

    
    // Observable descriptors

    typedef enum 
    {
        CONFIG_UNSURE,
        CONFIG_AUTO_MODE_CORBA_NAME,
        CONFIG_MODE_CONTROL_CORBA_NAME,
        CONFIG_MODE_IN_PROGRESS_CORBA_NAME,
        CONFIG_SELECTION_TIMEOUT_IN_MSECS,
        CONFIG_WORKER_THREADS_SLEEP_TIME_IN_MSECS
    } EItaEcsCachedConfigItemKey;

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
    virtual void attachObserver(ItaEcsCachedConfigObserver* observer, EItaEcsCachedConfigItemKey key)=0;


    /**
     * detachObserver
	 *
	 * Removes all instances of the ItaEcsCachedConfigObserver pointer in the observer collection.  The
     * observer will be removed for all keys.
     *
     */
    virtual void detachObserver(ItaEcsCachedConfigObserver* observer)=0;

    /**
     * updateInterestedObservers
	 *
	 * Calls the processItaEcsCachedConfigUpdate() method on all registered observers interested in the 
     * specified key.
     *
     * @param key used to call specific observers
     *
     */
    virtual void notifyInterestedObservers(EItaEcsCachedConfigItemKey key)=0;


    ///////////////////////////////////////////////////////////////////////////
    // GetMethods
    ///////////////////////////////////////////////////////////////////////////
    //
    // All getX methods can assert if they are not initialised, except for 
    // getAgentX methods which arew possibly called early for raising alarms.
    //
    ///////////////////////////////////////////////////////////////////////////

    // Common getMethods

    virtual bool                                getIsMaster()=0;
    virtual bool                                getIsInControl()=0;
    virtual unsigned long                       getAgentKey()=0;
    virtual std::string                         getAgentName()=0;
    virtual unsigned long                       getAgentTypeKey()=0;
    virtual unsigned long                       getAgentLocationKey()=0;
    virtual unsigned long                       getAgentSubsystemKey()=0;
    virtual bool                                getAgentInRuntime()=0;
    
    virtual unsigned long                       getWorkerThreadsSleepTimeInMSecs()=0;
    virtual unsigned long                       getMaxModeExecutionTimeInSecs()=0;
	
	virtual std::string							getAssetName()=0;

    // StationECSAgent getMethods

    virtual bool                                getIsOccOnline()=0;
    virtual std::string                         getOccGroupName()=0;
    virtual unsigned long                       getLocalATSAgentKey()=0;
    virtual std::string                         getLocalATSAgentName()=0;
    virtual unsigned long                       getSpareATSAgentKey()=0;
    virtual unsigned long                       getSpareATSAgentLocationKey()=0;
    virtual std::string                         getSpareATSAgentName() =0;
    virtual unsigned long                       getThresholdStallTimeInSecs()=0;
    virtual unsigned long                       getThresholdTemperature()=0;
    virtual unsigned long                       getTemperatureDeadbandLowerLimit()=0;
    virtual bool                                getCongestionUsesStallTime()=0;
    virtual bool                                getCongestionUsesTemperature()=0;
    
    virtual unsigned long                       getIscsAlivePollTimeInMSecs()=0;
    virtual unsigned long                       getNormalStationMode()=0;
    virtual TA_Base_Bus::DataPoint*             getMyMasterModeLockDataPoint()=0;
    virtual TA_Base_Bus::DataPoint*             getMyMasterModeLockControlDataPoint()=0;
    virtual TA_Base_Bus::DataPoint*             getMySelectedMasterModeDataPoint()=0;
    virtual TA_Base_Bus::DataPoint*             getMySelectedOverridingStationModeDataPoint()=0;
    virtual TA_Base_Bus::DataPoint*             getMyLastAppliedMasterModeDataPoint()=0;
    virtual TA_Base_Bus::DataPoint*             getMyLastAppliedStationModeDataPoint()=0;

    // NOTE! Deliberately kept as corbanames so that the client can register for state updates
    virtual TA_Base_Core::DataPointEntityDataPtr             getModeControlDpEntityData()=0;  
    virtual TA_Base_Core::DataPointEntityDataPtr             getModeInProgressDpEntityData()=0;  
    virtual TA_Base_Core::DataPointEntityDataPtr             getAutoModeDpEntityData()=0;  

    virtual TA_Base_Bus::DataPointProxySmartPtr & getModeStatusProxy()=0;
    virtual TA_Base_Bus::DataPointProxySmartPtr & getLocalModeProxy()=0;
    virtual TA_Base_Bus::DataPointProxySmartPtr & getIscsAliveProxy()=0; 
	
	virtual unsigned long getIscsEntityKey()=0;
	virtual std::string getIscsAgentName()=0;

    // MasterECSAgent getMethods

    virtual unsigned long                       getSelectionTimeoutInMSecs()=0;
    virtual unsigned long                       getNormalMasterMode()=0;
    virtual TA_Base_Bus::DataPoint*             getMyModeStatusDataPoint()=0;
    virtual TA_Base_Bus::DataPoint*             getMyModeInProgressDataPoint()=0;

    // Common

    
    // Station



};


class ItaEcsCachedConfigObserver
{
public:
    virtual void processItaEcsCachedConfigUpdate(ItaEcsCachedConfig::EItaEcsCachedConfigItemKey key) = 0;
};

#endif // !defined(AFX_ItaEcsCachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
