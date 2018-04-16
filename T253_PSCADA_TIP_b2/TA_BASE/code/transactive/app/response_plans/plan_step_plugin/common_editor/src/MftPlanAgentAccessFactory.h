#ifndef __MFT_PLAN_AGENT_ACCESS_FACTORY_INCLUDE_H
#define __MFT_PLAN_AGENT_ACCESS_FACTORY_INCLUDE_H

#include "app/response_plans/plan_step_plugin/common_editor/src/IMftPlanAgentAccess.h"

namespace TA_Base_App
{
    class MftPlanAgentAccessFactory
    {
    public:
		static IMftPlanAgentAccess& instance();
		static void init( IMftPlanAgentAccess& refMftPlanAgentAccess );

    private:
        MftPlanAgentAccessFactory();
        MftPlanAgentAccessFactory( const MftPlanAgentAccessFactory& fromHelper );
        MftPlanAgentAccessFactory& operator=(const MftPlanAgentAccessFactory& fromHelper);
        ~MftPlanAgentAccessFactory();

    private:
		static IMftPlanAgentAccess* s_pMftPlanAgentAccess;
    };
}


#endif