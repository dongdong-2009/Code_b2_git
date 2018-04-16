
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/IDllHandler.h"
#include "core/corba/src/CorbaUtil.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanAgentAccessFactory.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MftPlanAgentAccessFactory.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanEditorStepInitComponent.h"

namespace TA_Base_App
{

    PlanEditorStepInitComponentImpl::PlanEditorStepInitComponentImpl()
    {

    }

    PlanEditorStepInitComponentImpl::~PlanEditorStepInitComponentImpl()
    {

    }


    void PlanEditorStepInitComponentImpl::initLibrary( const TA_Base_Core::IDllHandler& handler, IPlanAgentAccess& refPlanAgentAccess )
    {
        initMiscellaneous( handler );
        initPlanAgentAccess( refPlanAgentAccess );
    }

    void PlanEditorStepInitComponentImpl::initLibrary( const TA_Base_Core::IDllHandler& handler, IMftPlanAgentAccess& refPlanAgentAccess )
    {
        initMiscellaneous( handler );
        initMftPlanAgentAccess( refPlanAgentAccess );
    }

	void PlanEditorStepInitComponentImpl::initMiscellaneous( const TA_Base_Core::IDllHandler& handler )
	{
		TA_Base_Core::CorbaUtil::setInstance( handler.getCorbaUtil() );
		TA_Base_Core::DebugUtil::setInstance( handler.getDebugUtil() );

		TA_Base_Core::RunParams& dllRunParams = TA_Base_Core::RunParams::getInstance();
		TA_Base_Core::RunParams::ParamVector paraVec = handler.getRunParams();
		TA_Base_Core::RunParams::ParamVector::iterator it = paraVec.end();
		for( it = paraVec.begin(); it != paraVec.end(); it++ )
		{
			if( !dllRunParams.isSet( it->name.c_str() ) )
			{
				dllRunParams.set( it->name.c_str(), it->value.c_str() );
			}
		}
    }

    void PlanEditorStepInitComponentImpl::initPlanAgentAccess( IPlanAgentAccess& refPlanAgentAccess )
    {
        PlanAgentAccessFactory::init( refPlanAgentAccess );
    }

    void PlanEditorStepInitComponentImpl::initMftPlanAgentAccess( IMftPlanAgentAccess& refMftPlanAgentAccess )
    {
        MftPlanAgentAccessFactory::init( refMftPlanAgentAccess );
    }
}