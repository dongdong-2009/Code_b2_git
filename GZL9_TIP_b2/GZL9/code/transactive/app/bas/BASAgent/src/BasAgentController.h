/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/BasAgentController.h $
  * @author:	Huang Jian
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2017/05/29 19:57:02 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *
  */
#if !defined(APP_BAS_BASAgent_BASAgentController_H)
#define APP_BAS_BASAgent_BASAgentController_H


#include "bus/bas/bas_agent/IDL/src/IStationBasAgentCorbaDef.h"
#include "bus/scada/scada_agent/src/ScadaAgentUser.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/data_access_interface/entity_access/src/StationBASAgentEntityData.h"
#include "core/corba/src/ServantBase.h"


namespace TA_IRS_App
{
    class BASAgent;
    class IStationModeExecutionProxy;
    class ITimetableExecutionProxy;
    class PeriodicThread;

    class BasAgentController :
        public virtual POA_TA_IRS_Bus::IStationBasAgentCorbaDef,
        public virtual TA_Base_Core::ServantBase
    {
    public:
        BasAgentController(TA_IRS_App::BASAgent& paraGenericAgentUser, IStationModeExecutionProxy& paraStationModeExecutionProxy, ITimetableExecutionProxy& paraTimetableExecutionProxy);
        virtual ~BasAgentController();
        

        // IDL Interface functions
        virtual void executeStationMode(CORBA::Long subsystemKey, CORBA::Long stationMode, CORBA::Boolean canOverrideStationModeInProgress, const char* sessionId);
        virtual void executeTimeTable(TA_IRS_Bus::IStationBasAgentCorbaDef::ETimeTableType timeTableType, CORBA::Long tableKey, const char* sessionID);

        void startupBASFunctions();

	private:
        BasAgentController(const BasAgentController&);
        BasAgentController& operator=(const BasAgentController&);

        void checkPermission(long actionKey, const std::string& sessionId);

        IStationModeExecutionProxy& 	m_stationModeExecutionProxy;
        ITimetableExecutionProxy& 		m_timetableExecutionProxy;

        PeriodicThread*					m_schedule;
        TA_IRS_App::BASAgent&			m_genericAgentUser;

        TA_Base_Bus::RightsLibrary*		m_rightsMgr;
    };
}

#endif
