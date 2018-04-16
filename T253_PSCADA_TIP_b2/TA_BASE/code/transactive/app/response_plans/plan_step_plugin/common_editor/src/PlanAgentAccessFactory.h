#ifndef __PLAN_AGENT_ACCESS_FACTORY_INCLUDE_H
#define __PLAN_AGENT_ACCESS_FACTORY_INCLUDE_H

#include "IPlanAgentAccess.h"

namespace TA_Base_App
{
    class PlanAgentAccessFactory
    {
    public:
        static IPlanAgentAccess& instance();
        static void init( IPlanAgentAccess& refPlanAgentAccess );

    private:
        PlanAgentAccessFactory();
        PlanAgentAccessFactory( const PlanAgentAccessFactory& fromHelper );
        PlanAgentAccessFactory& operator=(const PlanAgentAccessFactory& fromHelper);
        ~PlanAgentAccessFactory();

    private:
        static IPlanAgentAccess* s_pPlanAgentAccess;
   };
}


#endif