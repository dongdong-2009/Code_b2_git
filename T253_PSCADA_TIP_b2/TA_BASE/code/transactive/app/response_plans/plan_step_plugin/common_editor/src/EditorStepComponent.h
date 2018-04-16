#ifndef MFTSTEPCOMPONENT_INCLUDE_H
#define MFTSTEPCOMPONENT_INCLUDE_H

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/InteractiveStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IEditorStepComponent.h"

namespace TA_Base_App
{

class EditorStepComponent : public IEditorStepComponent
{
public:
    EditorStepComponent();
    virtual ~EditorStepComponent();

    virtual void initLibrary( const int nTypeId, const int nAccess );
    virtual void initLibrary( const int nTypeId, const int nAccess, const std::map<unsigned long, TA_Base_Bus::StepShortcut>& refShortcut );
    virtual void initLibrary( const int nTypeId, const int nAccess, const TA_Base_Core::IDllHandler& handler, IPlanAgentAccess& refPlanAgentAccess, const std::map<unsigned long, TA_Base_Bus::StepShortcut>& refShortcut );
    virtual void initLibrary( const int nTypeId, const int nAccess, const TA_Base_Core::IDllHandler& handler, IMftPlanAgentAccess& refPlanAgentAccess, RightsManager& refRightsManager, TA_Base_Bus::RpcAgency& refRpcAgency );

	std::vector<std::pair<std::string, std::vector<int>>>& workflowShortcuts();

protected:
    void initMiscellaneous( const TA_Base_Core::IDllHandler& handler );
    void initPlanAgentAccess( IPlanAgentAccess& refPlanAgentAccess );
    void initMftPlanAgentAccess( IMftPlanAgentAccess& refMftPlanAgentAccess );
    void initRightsManager( RightsManager& refRightsManager );
    void initRpcAgency( TA_Base_Bus::RpcAgency& refRpcAgency );
	void initShortcuts( const std::map<unsigned long, TA_Base_Bus::StepShortcut>& refShortcut );

protected:
    int m_eType;
    int m_eAccess;
	std::vector<std::pair<std::string, std::vector<int>>> m_vecShortcuts;
};

}

#endif