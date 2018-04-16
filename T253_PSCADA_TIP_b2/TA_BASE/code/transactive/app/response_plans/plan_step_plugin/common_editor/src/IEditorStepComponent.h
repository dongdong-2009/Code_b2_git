#ifndef __I_MFT_STEP_COMPONENT_H_
#define __I_MFT_STEP_COMPONENT_H_

#include <string>
#include <vector>
#include <map>

namespace TA_Base_Core
{
    class IDllHandler;
}

namespace TA_Base_Bus
{
	class RpcAgency;
	struct StepShortcut;
}

class IActivityComponent;

namespace TA_Base_App
{
    class BasicParameter;
    class PlanStep;
    class IPlanNode;
    class InteractiveStep;
    class IPlanAgentAccess;
    class IMftPlanAgentAccess;
	class RightsManager;

    class IEditorStepComponent
    {
    public:
        virtual PlanStep* createStep( IPlanNode* parent, BasicParameter& stepDetail ) = 0;
        virtual PlanStep* createStep( IPlanNode* parent, const unsigned long stepId, const int stepType ) = 0;

		virtual InteractiveStep* createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
			const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters) = 0;

        virtual IActivityComponent& getActivityComponent() = 0;
        virtual void initLibrary( const int nTypeId, const int nAccess ) = 0;
        virtual void initLibrary( const int nTypeId, const int nAccess, const std::map<unsigned long, TA_Base_Bus::StepShortcut>& refShortcut ) = 0;
        virtual void initLibrary( const int nTypeId, const int nAccess, const TA_Base_Core::IDllHandler& handler, IPlanAgentAccess& refPlanAgentAccess, const std::map<unsigned long, TA_Base_Bus::StepShortcut>& refShortcut ) = 0;
        virtual void initLibrary( const int nTypeId, const int nAccess, const TA_Base_Core::IDllHandler& handler, IMftPlanAgentAccess& refPlanAgentAccess, RightsManager& refRightManager, TA_Base_Bus::RpcAgency& refRpcAgency ) = 0;
    };

    typedef IEditorStepComponent& (*GetMftStepComponentFuncPtr)();
}

#endif