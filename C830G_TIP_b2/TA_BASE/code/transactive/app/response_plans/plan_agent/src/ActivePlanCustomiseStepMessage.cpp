/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanCustomiseStepMessage.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2011/03/29 13:47:47 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

namespace TA_Base_App
{
    ActivePlanCustomiseStepMessage::ActivePlanCustomiseStepMessage( const TA_Base_Core::StepCustomisableDetail& detail ) :
    m_detail(detail),
    m_historyUpdate(),
    m_bUpdateHistory( false )
	{
	}
    
    ActivePlanCustomiseStepMessage::ActivePlanCustomiseStepMessage( const TA_Base_Core::CustomisationHistoryUpdate& historyUpdate ) :
    m_detail(),
    m_historyUpdate( historyUpdate ),
    m_bUpdateHistory( true )
    {
    }

	void ActivePlanCustomiseStepMessage::execute( ActivePlan& activePlan ) const
	{
// 		activePlan.customiseStep( m_detail );
        if ( m_bUpdateHistory )
        {
            activePlan.customiseSteps( m_historyUpdate.history.details );
        }
        else
        {
            activePlan.customiseStep( m_detail );
        }
	}

} // TA_Base_App
