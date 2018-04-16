/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanStepFailedMessage.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanStepFailedMessage.h"

namespace TA_Base_App
{

ActivePlanStepFailedMessage::ActivePlanStepFailedMessage( const std::string& reason, const unsigned long ulStepNum, const bool bIgnoreFailure ) : 
m_reason( reason.c_str() ),
m_ulStepNumber( ulStepNum ),
m_bIgnoreFailure( bIgnoreFailure ),
m_pActiveStepMsg( NULL )
{
}

ActivePlanStepFailedMessage::ActivePlanStepFailedMessage( IActiveStepMessage* pStepMsg ) :
m_reason( "" ),
m_ulStepNumber( 0u ),
m_bIgnoreFailure( false ),
m_pActiveStepMsg( pStepMsg )
{
}

ActivePlanStepFailedMessage::~ActivePlanStepFailedMessage()
{
    if ( NULL != m_pActiveStepMsg )
    {
        delete m_pActiveStepMsg;
        m_pActiveStepMsg = NULL;
    }
}

void ActivePlanStepFailedMessage::execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const
{
    if ( ActivePlanBulletinBoard::instance()->isControlMode() )
    {
        const TA_Base_Core::ActivePlanId& refPlanId = activePlan.id();
        const std::string strPlanName = activePlan.name();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Step %d of Plan %s instance %d failed - %s", 
            m_ulStepNumber, strPlanName.c_str(), refPlanId.instance, m_reason.c_str() );

        std::string alarmId = "";
        std::string strSession = activePlan.session();

        if (ActivePlanBulletinBoard::instance()->isManualPlan(refPlanId))
        {
            AAMessageSender::instance()->submitAuditStepExecutedFailure( strSession, strPlanName, m_reason, refPlanId.instance, m_ulStepNumber );
        }
        else if ( !m_bIgnoreFailure )
        {
            alarmId = AAMessageSender::instance()->raiseAlarmStepExecutedFailure( strPlanName, m_reason, refPlanId.instance, m_ulStepNumber );
            ActivePlanBulletinBoard::instance()->setAssociatedAlarmId(refPlanId, m_ulStepNumber, alarmId.c_str ());
        }
    }
}

void ActivePlanStepFailedMessage::execute( ActivePlan& activePlan ) const
{
    if ( NULL != m_pActiveStepMsg )
    {
        m_pActiveStepMsg->execute( activePlan, *(AAMessageSender::instance()) );
    }
    else
    {
        execute( activePlan, *(AAMessageSender::instance()) );
    }
}

} // TA_Base_App
