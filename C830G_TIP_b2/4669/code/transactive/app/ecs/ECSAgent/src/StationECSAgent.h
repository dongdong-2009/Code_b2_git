/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/StationECSAgent.h $
  * @author:   Andy Parker
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(StationStationECSAgent_85831B60_1CBB_43c8_A752_5EB30E0DE611__INCLUDED_)
#define StationStationECSAgent_85831B60_1CBB_43c8_A752_5EB30E0DE611__INCLUDED_


#include "app/ecs/ECSAgent/src/AbstractECSAgent.h"

class StationModeManager;
class AutoModeMonitor;
class IscsAliveWorker;
class ItaEcsAgentSummary;


class StationECSAgent : public virtual AbstractECSAgent
{
public:

    StationECSAgent(
        int argc, 
        char* argv[],
        ItaEcsAgentSummary& theSummary);

    virtual ~StationECSAgent();
    virtual void setupSpecifics();
        //throw(TA_Core::GenericAgentException, TA_Base_Core::InvalidECSConfigurationException, TA_Base_Core::CachedMappingNotFoundException);
protected:


    virtual void processSpecificConfiguration() ;
        //throw(TA_Base_Core::InvalidECSConfigurationException, TA_Core::DatabaseException, TA_Core::DataException);

    virtual void validateSpecificRuntimeConfiguration() ;
        //throw(TA_Base_Core::InvalidECSConfigurationException);

    virtual void cleanupSpecifics();

    virtual void stopSpecificThreads();
    // liuyu++ start master/station specific threads
    virtual void startSpecificThreads();
    // ++liuyu start master/station specific threads

    virtual void agentSetControlSpecifics();
    virtual void agentSetMonitorSpecifics();

    /** 
	  * notifyGroupOffline
	  *
	  * This request is passed on by GenericAgent when it receives a call to 
      * notifyGroupOffline from the Process Monitor.
      *
      * @param The group name
	  */
    virtual void notifyGroupOffline( const std::string& group );

	/** 
	  * notifyGroupOnline
	  *
	  * This request is passed on by GenericAgent when it receives a call to 
      * notifyGroupOnline from the Process Monitor.
      *
      * @param The group name
	  */
    virtual void notifyGroupOnline( const std::string& group );

    // Disable copy constructor and assignment operator
    StationECSAgent( const StationECSAgent& theAgent);
    StationECSAgent& operator=(const StationECSAgent &);

    // The Generic Agent
    StationModeManager*         m_stationModeManager;
	AutoModeMonitor*			m_autoModeMonitor;
    IscsAliveWorker*            m_iscsAliveWorker;

};
#endif // !defined(StationStationECSAgent_85831B60_1CBB_43c8_A752_5EB30E0DE611__INCLUDED_)
