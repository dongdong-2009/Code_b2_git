/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanStepResponseMessage.cpp $
  *  @author:  Bart Golab
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2014/07/10 16:23:57 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Generated when the result of a Run Plan interactive step execution request
  *  is received back from the Plan Manager.
  */

#include "app/response_plans/plan_step_plugin/common_active/src/IActiveStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/ActivePlanStepResponseMessage.h"

namespace TA_Base_App
{
    ActivePlanStepResponseMessage::ActivePlanStepResponseMessage( IActiveStepMessage* pStepMessage ) :
    m_pStepMessage( pStepMessage )
    {
    }

    ActivePlanStepResponseMessage::~ActivePlanStepResponseMessage()
    {
        delete m_pStepMessage;
        m_pStepMessage = NULL;
    }

    void ActivePlanStepResponseMessage::execute( ActivePlan& activePlan ) const
    {
        if ( NULL == m_pStepMessage )
        {
            return;
        }

        m_pStepMessage->execute( activePlan, *(AAMessageSender::instance()) );
    }
}