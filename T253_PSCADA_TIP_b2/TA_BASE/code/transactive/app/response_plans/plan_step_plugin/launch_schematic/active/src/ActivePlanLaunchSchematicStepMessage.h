#ifndef ACTIVE_PLAN_LAUNCH_SCHEMATIC_STEP_MSG_HEADER_INCLUDED
#define ACTIVE_PLAN_LAUNCH_SCHEMATIC_STEP_MSG_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanLaunchSchematicStepMessage.h $
  *  @author:  Bart Golab
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2014/10/24 16:32:43 $
  *  Last modified by:  $Author: huang.wenbo $
  *
  *  Generated when the result of a Launch Schematic interactive step execution request
  *  is received back from the Plan Manager.
  */
#include <vector>
#include "app/response_plans/plan_step_plugin/common_active/src/IActiveStepMessage.h"
#include "app/response_plans/plan_step_plugin/launch_schematic/common/LaunchSchematicResponseParameter.h"

namespace TA_Base_App
{
    class ActivePlan;

    class ActivePlanLaunchSchematicStepMessage : public IActiveStepMessage
    {
    //public methods
    public:
        ActivePlanLaunchSchematicStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData );
        virtual ~ActivePlanLaunchSchematicStepMessage();
        
        virtual void execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const;
    private:
        ActivePlanLaunchSchematicStepMessage& operator=( const ActivePlanLaunchSchematicStepMessage& );
        ActivePlanLaunchSchematicStepMessage( const ActivePlanLaunchSchematicStepMessage& );
		unsigned long m_ulStepNum;
		LaunchSchematicResponseParameter m_responseParam;
    };

}

#endif // ACTIVE_PLAN_LAUNCH_SCHEMATIC_STEP_MSG_HEADER_INCLUDED