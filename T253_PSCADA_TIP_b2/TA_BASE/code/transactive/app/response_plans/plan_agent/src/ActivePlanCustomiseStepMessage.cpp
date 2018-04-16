/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanCustomiseStepMessage.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2015/01/07 13:11:11 $
  *  Last modified by:  $Author: huangjian $
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanCustomiseStepMessage.h"

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
        if ( m_bUpdateHistory )
        {
            ActivePlanBulletinBoard::instance()->processCustomisationHistoryUpdate( m_historyUpdate );

            for ( unsigned int unLoop = 0u; unLoop < m_historyUpdate.history.details.length(); ++unLoop )
            {
                const TA_Base_Core::StepCustomisableDetail& refDetail = m_historyUpdate.history.details[unLoop];

                activePlan.customiseStep( refDetail );
            }
        }
        else
        {
            ActivePlanBulletinBoard::instance()->updateCustom(activePlan.id(), m_detail );

            activePlan.customiseStep( m_detail );

            std::string session = activePlan.session();
            const std::string strPlanName = activePlan.name();
            const unsigned long ulInstanceNum = activePlan.id().instance;
            const unsigned long ulStepNum = m_detail.id;
            AAMessageSender::instance()->submitAuditStepCustomize( session, strPlanName, ulInstanceNum, ulStepNum );

            //Send State Update message for this customization
            TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();

            pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

            TA_Base_Core::PlanCustomiseStepUpdate_var customiseStepUpdate = new TA_Base_Core::PlanCustomiseStepUpdate();
            customiseStepUpdate->plan = activePlan.id();
            customiseStepUpdate->detail = m_detail;
            pasUpdateData->update.customisedStep(customiseStepUpdate);
            ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
        }
	}

} // TA_Base_App
