/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanDetail.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2017/12/12 14:18:48 $
  *  Last modified by:  $Author: hoa.le $
  *
  *  Implementation of PlanAccessFactory.
  *  TODO: put hard coded column names and SQL in a config file or in the data
  *  access classes outlined in the DataAccessInterface usage guide.
  */

#include "ace/OS.h"

#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "app/response_plans/plan_agent/src/PlanAgentUser.h"
#include "app/response_plans/plan_agent/src/PlanDetail.h"


namespace TA_Base_App
{

PlanDetail::PlanDetail( const PlanDetail& rhs ) :
detail( rhs.detail ),
alarmID( rhs.alarmID.c_str() ),
userKey( rhs.userKey ),
userLocation( rhs.userLocation ),
userWorkstation( rhs.userWorkstation ),
exeHistoryMap( rhs.exeHistoryMap ),
custHistoryMap( rhs.custHistoryMap ),
lastModified( rhs.lastModified )
{
}

PlanDetail::PlanDetail( const std::string& session, const unsigned long triggerEntity, const unsigned long userKey, const unsigned long userLocation,
    const unsigned long userWrokstation, const unsigned long updateId, const std::string& owner, const TA_Base_Core::ActivePlanId& refActivePlan, 
    const bool bPending, const std::string& remark, const bool manExec, const std::string& assocAlarmId ) :
detail(),
alarmID( "" ),
userKey( userKey ),
userLocation( userLocation ),
userWorkstation( userWrokstation ),
exeHistoryMap(),
custHistoryMap(),
lastModified( ACE_OS::time() )
{
    detail.plan.plan = refActivePlan.plan;
    detail.plan.instance = refActivePlan.instance;
    detail.plan.location = refActivePlan.location;
    detail.triggerEntity = triggerEntity;
    detail.path = TA_Base_Bus::PlanDataReader::instance()->getNodePath(refActivePlan.plan).c_str();
    detail.owner = owner.c_str();
    detail.activeUpdateId = updateId;
    detail.activePlanState = TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE;
    detail.remark = remark.c_str();
    detail.session = session.c_str();
    detail.manuallyExecuted = manExec;
    detail.assocAlarmId = assocAlarmId.c_str();
    detail.timer = 0u;
    detail.lastStateTransitionTime = lastModified;
    detail.instantiationTime = lastModified;
}

bool PlanDetail::set( const int state )
{
	if( state != detail.activePlanState )
	{
		detail.activePlanState = static_cast<TA_Base_Core::EActivePlanState>(state);
        detail.lastStateTransitionTime = ACE_OS::time();
		lastModified = ACE_OS::time();

		return true;
	}

	return false;
}

bool PlanDetail::isNonActive() const
{
    switch( detail.activePlanState )
    {
    case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
    case TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE:
        return true;
        //break;

    default:
        break;
    }

    return false;
}

bool PlanDetail::isActive() const
{
    switch( detail.activePlanState )
    {
    case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
    case TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE:
        return true;
        //break;

    default:
        break;
    }

    return false;
}

} // TA_Base_App
