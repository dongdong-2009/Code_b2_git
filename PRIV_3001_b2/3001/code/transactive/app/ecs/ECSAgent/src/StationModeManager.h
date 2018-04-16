#ifndef StationModeManager_H
#define StationModeManager_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/StationModeManager.h $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2018/03/14 13:51:20 $
  * Last modified by : $Author: lim.cy $
  *
  *
  */

#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/ecs/ecs_agent/IDL/src/IStationEcsAgentCorbaDef.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/AbstractFailoverable.h"
#include "app/ecs/ECSAgent/src/StationModeExecutionWorker.h"

#include "bus/security/rights_library/src/RightsLibrary.h"	// CL-17274
#include "bus/security/access_control/actions/src/AccessControlledActions.h"  // CL-17274


class ItaEcsAuditing;
class ItaEcsAgentCompleteState;
class ItaEcsDatapoints;
class ItaEcsCachedConfig;




class StationModeManager : public virtual POA_TA_IRS_Bus::IStationEcsAgentCorbaDef,
                           public virtual TA_Base_Core::ServantBase,
                           public virtual AbstractFailoverable
{


public:

    StationModeManager( 
        ItaEcsCachedConfig& theConfig,
        ItaEcsAuditing& theAuditing,
        ItaEcsAgentCompleteState& theCompleteState,
		ItaEcsDatapoints& theDatapoints);//throw (TA_Core::GenericAgentException);

    virtual ~StationModeManager();

    // AbstractFailoverable overrides 
    virtual void setControl(bool isOnStartup=false);
    virtual void setMonitor(bool isOnStartup=false);

    // liuyu++ start/stop execution worker thread
    void start()
    {
        if (m_stationModeExecutionWorker.getCurrentState() != TA_Base_Core::Thread::THREAD_STATE_RUNNING)
        {
            m_stationModeExecutionWorker.start();
        }
    }

    void terminateAndWait()
    {
        m_stationModeExecutionWorker.terminateAndWait();
    }
    // ++liuyu start/stop execution worker thread
    
    // IDL Interface functions
    virtual void executeStationMode(CORBA::Long stationMode, CORBA::Boolean canOverrideStationModeInProgress, const char* sessionId);
    virtual void executeStationModeForMasterMode(CORBA::Long stationMode, CORBA::Long forMasterMode, const char* sessionId);
    virtual void setSelectedMasterModeDataPoint(CORBA::Long value);
    virtual void setSelectedOverridingStationModeDataPoint(CORBA::Long value);
    virtual void setLastAppliedMasterModeDataPoint(CORBA::Long value);
    virtual void setLastAppliedStationModeDataPoint(CORBA::Long value);
    virtual void setMasterModeLockDataPoint(CORBA::Boolean value);
	virtual CORBA::UShort getFlexibleTimeOutValue(); //TD16709

/*private:*/	//Why???
/*protected:*/  //Why???

	bool checkPermission(const char* sessionID);	// CL-17274++

	TA_Base_Bus::RightsLibrary* m_rightsMgr; // CL-17274++
    

protected:

    ItaEcsCachedConfig&                         m_theConfig;
    ItaEcsAuditing&                             m_theAuditing;
    ItaEcsAgentCompleteState&                   m_theCompleteState;
	ItaEcsDatapoints&							m_theDatapoints;
    TA_Base_Core::ReEntrantThreadLockable       m_lock;
    StationModeExecutionWorker                  m_stationModeExecutionWorker;        

};


#endif

