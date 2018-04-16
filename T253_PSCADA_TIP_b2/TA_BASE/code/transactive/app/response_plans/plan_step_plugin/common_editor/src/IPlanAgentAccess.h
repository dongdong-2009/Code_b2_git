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
#ifndef I_PLAN_AGENT_ACCESS_H
#define I_PLAN_AGENT_ACCESS_H

#include <string>
#include <vector>

namespace TA_Base_App
{
    class IPlanAgentAccess
    {
    public:
        virtual std::vector<std::string> getStepParameters(const unsigned long stepId) = 0;
		virtual std::string getNodePath(const unsigned long nodeId) = 0;
    };

}

#endif // I_PLAN_AGENT_ACCESS_H