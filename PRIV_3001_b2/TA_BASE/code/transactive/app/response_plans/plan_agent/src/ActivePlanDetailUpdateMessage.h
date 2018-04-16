#ifndef ACTIVE_PLAN_DETAIL_UPDATE_HEADER_INCLUDED
#define ACTIVE_PLAN_DETAIL_UPDATE_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanDetailUpdateMessage.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  <description>
  */

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

    class ActivePlanDetailUpdateMessage : public IActivePlanMessage
    {
    //public methods
    public:
        ActivePlanDetailUpdateMessage(const TA_Base_Core::ActivePlanDetail& planDetail);

        virtual void execute( ActivePlan& activePlan ) const;
    
    private:
        ActivePlanDetailUpdateMessage& operator=( const ActivePlanDetailUpdateMessage& );
        ActivePlanDetailUpdateMessage( const ActivePlanDetailUpdateMessage& );

        TA_Base_Core::ActivePlanDetail m_detail;
    };

}

#endif //ACTIVE_PLAN_DETAIL_UPDATE_HEADER_INCLUDED
