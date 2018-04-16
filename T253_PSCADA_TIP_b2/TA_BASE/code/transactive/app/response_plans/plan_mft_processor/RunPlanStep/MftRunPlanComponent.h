#ifndef __MFT_RUN_PLAN_COMPONENT_H_
#define __MFT_RUN_PLAN_COMPONENT_H_

#include "ace/RW_Mutex.h"
#include "ace/Singleton.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/EditorStepComponent.h"

namespace TA_Base_App
{
    class  MftRunPlanComponentImpl: public EditorStepComponent
    {
        
    public:
        friend class ACE_Singleton<MftRunPlanComponentImpl, ACE_Recursive_Thread_Mutex>;

		virtual PlanStep* createStep( IPlanNode* parent, BasicParameter &stepDetail );
		virtual PlanStep* createStep( IPlanNode* parent, const unsigned long stepId, const int stepType );

		virtual InteractiveStep* createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
			const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters );

        virtual IActivityComponent& getActivityComponent();

    private:
        MftRunPlanComponentImpl();
        ~MftRunPlanComponentImpl();
        MftRunPlanComponentImpl( const MftRunPlanComponentImpl& );
        MftRunPlanComponentImpl& operator=( const MftRunPlanComponentImpl& );

    };

    typedef ACE_Singleton<MftRunPlanComponentImpl, ACE_Recursive_Thread_Mutex> MftRunPlanComponent;
}

#endif