/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/MasterECSAgent.h $
  * @author:   Andy Parker
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#if !defined(MasterECSAgent_85831B60_1CBB_43c8_A752_5EB30E0DE611__INCLUDED_)
#define MasterECSAgent_85831B60_1CBB_43c8_A752_5EB30E0DE611__INCLUDED_


#include "app/ecs/ECSAgent/src/AbstractECSAgent.h"
#include "app/ecs/ECSAgent/src/EcsZoneDataSource.h"

#include "core/alarm/src/AlarmReceiver.h"

class EmergencyProposalEngine;
class CongestionProposalEngine;
class MasterModeManager;
class ItaEcsAgentSummary;
class EmergencyNormalisationMonitor;



class MasterECSAgent : public virtual AbstractECSAgent,
					   public TA_Base_Core::AlarmReceiver
{
public:

    MasterECSAgent(int argc, char* argv[], ItaEcsAgentSummary& theSummary);

    virtual ~MasterECSAgent();
    
    virtual void setupSpecifics();
        //throw(TA_Core::GenericAgentException, TA_Base_Core::InvalidECSConfigurationException, TA_Base_Core::CachedMappingNotFoundException);

	//
	// receiveAlarmMessage
	//
	// Receive alarm state updates. i.e in this case, close messages.
	//
	virtual void receiveAlarmMessage( const TA_Base_Core::AlarmMessageCorbaDef& alarmMessage );

	//
	// subscribeForAlarmUpdates
	//
	// Subscribes for alarm updates i.e close messages.
	//
	void subscribeForAlarmUpdates();


	//
	// unsubscribeForAlarmUpdates
	//
	// Unsubscribes for alarm updates.
	//
	void unsubscribeForAlarmUpdates();


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

private:
    // Disable copy constructor and assignment operator
    MasterECSAgent( const MasterECSAgent& theAgent);
    MasterECSAgent& operator=(const MasterECSAgent &);

    void agentSetControlSpecificsInThread();//limin, for failover
    EmergencyProposalEngine*		m_emergencyProposalEngine;
    CongestionProposalEngine*		m_congestionProposalEngine;
    MasterModeManager*				m_masterModeManager;

	EmergencyNormalisationMonitor*	m_emergencyNormalisationMonitor;
};

#endif // !defined(MasterECSAgent_A7EE4E27_1FF9_4bb8_BBB7_6B1F2C675F52__INCLUDED_)
