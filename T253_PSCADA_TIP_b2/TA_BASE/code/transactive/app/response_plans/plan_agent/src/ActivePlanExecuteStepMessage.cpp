/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanExecuteStepMessage.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanExecuteStepMessage.h"

namespace TA_Base_App
{
    ActivePlanExecuteStepMessage::ActivePlanExecuteStepMessage( const TA_Base_Core::ActiveStepDetail& detail ) :
    m_detail(detail),
    m_historyUpdate(),
    m_bUpdateHistory( false )
	{
	}
    
    ActivePlanExecuteStepMessage::ActivePlanExecuteStepMessage( const TA_Base_Core::ExecutionHistoryUpdate& historyUpdate ) :
    m_detail(),
    m_historyUpdate( historyUpdate ),
    m_bUpdateHistory( true )
    {
    }

	void ActivePlanExecuteStepMessage::execute( ActivePlan& activePlan ) const
	{
        if ( m_bUpdateHistory )
        {
            ActivePlanBulletinBoard::instance()->processExecutionHistoryUpdate( m_historyUpdate );

            for ( unsigned int unLoop = 0u; unLoop < m_historyUpdate.history.details.length(); ++unLoop )
            {
                const TA_Base_Core::ActiveStepDetail& refDetail = m_historyUpdate.history.details[unLoop];

                activePlan.stepStateUpdated( refDetail.id, refDetail.state );
            }
        }
        else
        {
            // Update all fields of ActivePlanDetail
            ActivePlanBulletinBoard::instance()->updateActiveStepDetail( activePlan.id(), m_detail );

            activePlan.stepStateUpdated( m_detail.id, m_detail.state );
        }
	}

} // TA_Base_App
