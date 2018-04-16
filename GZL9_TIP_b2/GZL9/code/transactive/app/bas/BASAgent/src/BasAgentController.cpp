/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/BasAgentController.cpp $
  * @author:	Huang Jian
  * @version:	$Revision: #3 $
  *
  * Last modification:	$DateTime: 2017/05/29 19:57:02 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *
  */
#include "app/bas/BASAgent/src/BasAgentController.h"
#include "app/bas/BASAgent/src/IStationModeExecutionProxy.h"
#include "app/bas/BASAgent/src/ITimetableExecutionProxy.h"
#include "app/bas/BASAgent/src/TimetableManager.h"
#include "app/bas/BASAgent/src/ModeExecutionException.h"
#include "app/bas/BASAgent/src/BASAgent.h"
#include "app/bas/BASAgent/src/FireTriggerBasEquipmentManager.h"
#include "app/bas/BASAgent/src/BasCachedConfig.h"
#include "app/bas/BASAgent/src/PeriodicThread.h"

#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/scada/ModbusHelper/src/ModbusHelperFactory.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/security/rights_library/src/RightsLibrary.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/PMSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/data_access_interface/entity_access/src/StationSystemEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/bas/src/SubsystemAccessFactory.h"
#include "core/utilities/src/RunParams.h"

namespace TA_IRS_App
{

    BasAgentController::BasAgentController( TA_IRS_App::BASAgent& paraGenericAgentUser, TA_IRS_App::IStationModeExecutionProxy& paraStationModeExecutionProxy, TA_IRS_App::ITimetableExecutionProxy& paraTimetableExecutionProxy):
    m_genericAgentUser(paraGenericAgentUser),
    m_stationModeExecutionProxy(paraStationModeExecutionProxy),
    m_timetableExecutionProxy(paraTimetableExecutionProxy),
    m_rightsMgr(NULL)
    {
        TA_Base_Bus::RightsLibraryFactory rlf;
        m_rightsMgr = rlf.buildRightsLibrary();
        if ( NULL == m_rightsMgr )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Rights library not created.");
        }
    }

    BasAgentController::~BasAgentController()
    {
        if (m_rightsMgr != NULL)
        {
        	delete m_rightsMgr;
        	m_rightsMgr = NULL;
        }
    }

    void BasAgentController::executeStationMode( CORBA::Long subsystemKey, CORBA::Long stationMode, CORBA::Boolean canOverrideStationModeInProgress, const char* sessionId )
    {
        m_genericAgentUser.checkOperationMode();
        
        checkPermission(TA_Base_Bus::aca_SET_MODE_ECS, sessionId);
        
        m_stationModeExecutionProxy.executeStationMode(subsystemKey, stationMode, canOverrideStationModeInProgress, sessionId);
    }

    void BasAgentController::executeTimeTable( TA_IRS_Bus::IStationBasAgentCorbaDef::ETimeTableType timeTableType, CORBA::Long tableKey, const char* sessionID )
    {
        m_genericAgentUser.checkOperationMode();

        checkPermission(TA_Base_Bus::aca_SET_STATION_TIMETABLE_ECS, sessionID);

        m_timetableExecutionProxy.executeTimeTable(timeTableType, tableKey, sessionID, BasCachedConfig::getInstance().getLocationKey());
    }

    void BasAgentController::checkPermission(long actionKey, const std::string& sessionId)
    {
        bool actionAllowed = false;
        std::string reason = "";

        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        long entityKey = TA_IRS_App::BasCachedConfig::getInstance().getEntityKey();

        try
        {
            actionAllowed = m_rightsMgr->isActionPermittedOnResource(
                sessionId, entityKey, 
                actionKey, 
                reason, decisionModule);

        }
        catch (...)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception caught when checking the action permission for BasAgent" );

            actionAllowed = false;
        }  

        if (!actionAllowed)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Access denied, action key: %d", actionKey);
            throw TA_Base_Core::AccessDeniedException("No rights to do this operation.");

        }
    }
}
