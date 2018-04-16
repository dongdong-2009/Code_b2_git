
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanAgentAccessFactory.h"


namespace TA_Base_App
{
    IPlanAgentAccess* PlanAgentAccessFactory::s_pPlanAgentAccess = NULL;

    PlanAgentAccessFactory::PlanAgentAccessFactory()
    {
    }

    PlanAgentAccessFactory::~PlanAgentAccessFactory()
    {
    }

    void PlanAgentAccessFactory::init( IPlanAgentAccess& refPlanAgentAccess )
    {
        s_pPlanAgentAccess = &refPlanAgentAccess;
    }

    IPlanAgentAccess& PlanAgentAccessFactory::instance()
    {
        if ( NULL == s_pPlanAgentAccess )
        {

        }

        return *s_pPlanAgentAccess;
    }
}