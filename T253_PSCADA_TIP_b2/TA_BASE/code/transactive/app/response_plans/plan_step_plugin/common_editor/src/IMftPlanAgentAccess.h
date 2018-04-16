/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_step_plugin/common_active/src/IAAMessageSender.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2014/09/10 15:13:41 $
  *  Last modified by:  $Author: huangjian $
  *
  *  <description>
  */
#ifndef I_MFT_PLAN_AGENT_ACCESS_H
#define I_MFT_PLAN_AGENT_ACCESS_H

#include <string>
#include <vector>
#include "core/data_access_interface/PlanAgent/idl/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_App
{
    class IMftPlanAgentAccess
    {
    public:
        virtual std::string getNodePath( const unsigned long ulLocationKey, const unsigned long nodeId ) = 0;
        virtual unsigned long getStepPosition( const unsigned long ulLocationKey, const unsigned long ulPlanNodeId, const unsigned long ulStepId ) = 0;
        virtual void processStepResponse( const TA_Base_Core::ActivePlanId& activePlanId, TA_Base_Core::InteractiveStepData& stepDetail ) = 0;
    };

}

#endif // I_MFT_PLAN_AGENT_ACCESS_H