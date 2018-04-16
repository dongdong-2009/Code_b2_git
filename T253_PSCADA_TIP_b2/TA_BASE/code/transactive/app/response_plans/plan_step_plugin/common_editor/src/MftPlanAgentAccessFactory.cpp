#include "app/response_plans/plan_step_plugin/common_editor/src/MftPlanAgentAccessFactory.h"


namespace TA_Base_App
{
	IMftPlanAgentAccess* MftPlanAgentAccessFactory::s_pMftPlanAgentAccess = NULL;

    MftPlanAgentAccessFactory::MftPlanAgentAccessFactory()
    {
    }

    MftPlanAgentAccessFactory::~MftPlanAgentAccessFactory()
    {
    }

	void MftPlanAgentAccessFactory::init( IMftPlanAgentAccess& refMftPlanAgentAccess )
	{
		s_pMftPlanAgentAccess = &refMftPlanAgentAccess;
	}

	IMftPlanAgentAccess& MftPlanAgentAccessFactory::instance()
	{
		if ( NULL == s_pMftPlanAgentAccess )
		{

		}

		return *s_pMftPlanAgentAccess;
	}
}