
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DllHandler.h"
#include "core/corba/src/CorbaUtil.h"
#include "bus/response_plans/common/src/RpcAgency.h"
#include "bus/response_plans/PlanDataAccess/src/PlanStructures.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanAgentAccessFactory.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MftPlanAgentAccessFactory.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/EditorStepComponent.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/RightsManager.h"

namespace TA_Base_App
{

EditorStepComponent::EditorStepComponent() : 
m_eType( 0 )
{
}


EditorStepComponent::~EditorStepComponent()
{
}

void EditorStepComponent::initMiscellaneous( const TA_Base_Core::IDllHandler& handler )
{
    TA_Base_Core::CorbaUtil::setInstance( handler.getCorbaUtil() );
	TA_Base_Core::DebugUtil::setInstance( handler.getDebugUtil() );

    TA_Base_Core::RunParams& dllRunParams = TA_Base_Core::RunParams::getInstance();
    TA_Base_Core::RunParams::ParamVector paraVec = handler.getRunParams();
    TA_Base_Core::RunParams::ParamVector::iterator it = paraVec.end();
    for ( it = paraVec.begin(); it != paraVec.end(); it++ )
    {
        if ( !dllRunParams.isSet( it->name.c_str() ) )
        {
            dllRunParams.set( it->name.c_str(), it->value.c_str() );
        }
    }
}

void EditorStepComponent::initPlanAgentAccess( IPlanAgentAccess& refPlanAgentAccess )
{
    PlanAgentAccessFactory::init( refPlanAgentAccess );
}

void EditorStepComponent::initMftPlanAgentAccess( IMftPlanAgentAccess& refMftPlanAgentAccess )
{
    MftPlanAgentAccessFactory::init( refMftPlanAgentAccess );
}

void EditorStepComponent::initRightsManager( RightsManager& refRightsManager )
{
	RightsManager::setInstance( refRightsManager );
}

void EditorStepComponent::initRpcAgency( TA_Base_Bus::RpcAgency& refRpcAgency )
{
    TA_Base_Bus::RpcAgency::setInstance( refRpcAgency );
}

void EditorStepComponent::initLibrary( const int nTypeId, const int nAccess )
{
    m_eType = nTypeId;
    m_eAccess = nAccess;
}

void EditorStepComponent::initLibrary( const int nTypeId, const int nAccess, const std::map<unsigned long, TA_Base_Bus::StepShortcut>& refShortcut )
{
    m_eType = nTypeId;
    m_eAccess = nAccess;
	initShortcuts(refShortcut);
}

void EditorStepComponent::initLibrary( const int nTypeId, const int nAccess, const TA_Base_Core::IDllHandler& handler, IPlanAgentAccess& refPlanAgentAccess, const std::map<unsigned long, TA_Base_Bus::StepShortcut>& refShortcut )
{
    m_eType = nTypeId;
    m_eAccess = nAccess;
	initShortcuts(refShortcut);
    initMiscellaneous( handler );
    initPlanAgentAccess( refPlanAgentAccess );
}

void EditorStepComponent::initLibrary( const int nTypeId, const int nAccess, const TA_Base_Core::IDllHandler& handler, IMftPlanAgentAccess& refPlanAgentAccess, RightsManager& refRightsManager, TA_Base_Bus::RpcAgency& refRpcAgency )
{
    m_eType = nTypeId;
    m_eAccess = nAccess;
    initMiscellaneous( handler );
    initMftPlanAgentAccess( refPlanAgentAccess );
	initRightsManager( refRightsManager );
}

void EditorStepComponent::initShortcuts( const std::map<unsigned long, TA_Base_Bus::StepShortcut>& refShortcut )
{
	m_vecShortcuts.clear();

	for (std::map<unsigned long, TA_Base_Bus::StepShortcut>::const_iterator itLoop = refShortcut.begin(); refShortcut.end() != itLoop; ++itLoop)
	{
		m_vecShortcuts.push_back( std::make_pair( itLoop->second.icon, itLoop->second.shortcuts ));
	}
}

std::vector<std::pair<std::string, std::vector<int>>>& EditorStepComponent::workflowShortcuts()
{
	return m_vecShortcuts;
}

}

