#ifndef MasterModeManager_H
#define MasterModeManager_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/MasterModeManager.h $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  *
  */

#include <set>

#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/AbstractFailoverable.h"
#include "app/ecs/ECSAgent/src/MasterModeExecutionWorker.h"
#include "app/ecs/ECSAgent/src/EcsArrowManager.h"
#include "app/ecs/ECSAgent/src/MasterModeDataPointWorkQueue.h"

#include "bus/ecs/ecs_agent/IDL/src/IMasterEcsAgentCorbaDef.h"

#include "core/exceptions/src/GenericAgentException.h"
#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/SingletonTimerUtil.h"


class ItaEcsCachedMaps;
class ItaEcsAuditing;
class ItaEcsAlarms;
class ItaEcsAgentCompleteState;
class ItaEcsDatapoints;


// Basic interface into EcsArrowManager
class ImmmArrowAccess
{
public: 

    virtual void setAppliedMasterMode(unsigned long masterMode) = 0;
	virtual void setSelectedMasterMode(unsigned long masterMode) = 0;
};




//TD4303, use the timer utility in core library, rather than a special timer class
class MasterModeManager : public virtual POA_TA_IRS_Bus::IMasterEcsAgentCorbaDef,
                          public virtual TA_Base_Core::ServantBase,
                          public virtual AbstractFailoverable,
                          public virtual ImmmArrowAccess,
                          public TA_Base_Core::ITimeoutCallback,
						  public virtual IEmergencyObserver
{

public:

    MasterModeManager(
        ItaEcsCachedMaps& theMaps,
        ItaEcsCachedConfig& theConfig,
        ItaEcsAuditing& theAuditing,
        ItaEcsAlarms& theAlarms,
        ItaEcsAgentCompleteState& theCompleteState,
		ItaEcsDatapoints& theDatapoints); //throw (TA_Core::GenericAgentException);

    virtual ~MasterModeManager();

    // AbstractFailoverable overrides
    virtual void setControl(bool isOnStartup=false);
    virtual void setMonitor(bool isOnStartup=false);

	virtual void processEmergencyUpdate(unsigned long zoneId, bool value);

    virtual void setAppliedMasterMode(unsigned long masterMode)
    {
        m_ecsArrowManager.setAppliedMasterMode(masterMode);
    }

	virtual void setSelectedMasterMode(unsigned long masterMode)
    {
        m_ecsArrowManager.setSelectedMasterMode(masterMode);
    }

    // liuyu++ start/stop execution worker thread
    void start()
    {
        if (m_masterModeExecutionWorker.getCurrentState() != TA_Base_Core::Thread::THREAD_STATE_RUNNING)
        {
            m_masterModeExecutionWorker.start();
        }
    }

    void terminateAndWait()
    {
        m_masterModeExecutionWorker.terminateAndWait();
    }
    // ++liuyu start/stop execution worker thread

	//TD16709
	void processExternalStationDp(std::map<unsigned long, long>& externalStationMode,
								  std::map<unsigned long, long>& externalMasterMode,
								  ACE_Barrier& barrier);



    // IDL Interface functions
    virtual void login(long zoneId, const char* sessionId);
    virtual void logout(const char* sessionId);
    virtual void executeMasterMode(CORBA::Long masterMode, CORBA::Boolean canOverrideMasterModeInProgress, const char* sessionId);
    virtual void selectMasterMode(CORBA::Long masterMode, const char* sessionId);
    virtual TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* getAllProposedModes();
	virtual CORBA::UShort getFlexibleTimeOutValue(); //TD16709
    
    //Implement for ITimeoutCallback;
    void timerExpired(long timerId, void* userData);

	std::set<unsigned long> getStationsLocationByZone(unsigned long zoneId);

protected:

    std::list<unsigned long> getStationsInLocalMode(unsigned long masterMode);

    std::string toCommaSeparatedLocations(const std::list<unsigned long>& locationKeys);



protected:

    typedef std::multiset<std::string>                  SessionIds;
    typedef SessionIds::iterator                        SessionIdsIt;

    ItaEcsCachedMaps&                                   m_theMaps;
    ItaEcsCachedConfig&                                 m_theConfig;
    ItaEcsAuditing&                                     m_theAuditing;
    ItaEcsAlarms&                                       m_theAlarms;
    ItaEcsAgentCompleteState&                           m_theCompleteState;
	ItaEcsDatapoints&									m_theDatapoints;

    TA_Base_Core::ReEntrantThreadLockable               m_lock;
    MasterModeExecutionWorker                           m_masterModeExecutionWorker;
    EcsArrowManager                                     m_ecsArrowManager;
    SessionIds                                          m_activeSessionIds;
    TA_Base_Core::SingletonTimerUtil&		            m_timerUtility;

	// TD16709
	MasterModeDataPointWorkQueueManager*				m_workQueueManager;
	std::map<unsigned long, long>						m_externalStationMode;
	std::map<unsigned long, long>						m_externalMasterMode;
	std::map<unsigned long, bool>						m_emergencyStatus;

};


#endif

