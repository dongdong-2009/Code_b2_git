#ifndef ACTIVE_PLAN_LAUNCH_GUI_APP_STEP_HEADER_INCLUDED
#define ACTIVE_PLAN_LAUNCH_GUI_APP_STEP_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanLaunchGuiAppStepMessage.h $
  *  @author:  Bart Golab
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2014/10/24 16:32:43 $
  *  Last modified by:  $Author: huang.wenbo $
  *
  *  Generated when the result of a Launch GUI App interactive step execution request
  *  is received back from the Plan Manager.
  */

#include <string>
#include <vector>
#include "app/response_plans/plan_step_plugin/launch_gui_app/common/LaunchGuiResponseParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActiveStepMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

    class ActivePlanLaunchGuiAppStepMessage : public IActiveStepMessage
    {
    //public methods
    public:
        ActivePlanLaunchGuiAppStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData );
        virtual ~ActivePlanLaunchGuiAppStepMessage();
        
        virtual void execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const;
    private:
        ActivePlanLaunchGuiAppStepMessage& operator=( const ActivePlanLaunchGuiAppStepMessage& );
        ActivePlanLaunchGuiAppStepMessage( const ActivePlanLaunchGuiAppStepMessage& );
		unsigned long m_ulStepNum;
		LaunchGuiResponseParameter m_responseParam;
    };

}

#endif //ACTIVE_PLAN_LAUNCH_GUI_APP_STEP_HEADER_INCLUDED