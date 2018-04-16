#ifndef __MFT_HANDLER_RUNPARAMS_COMPONENT_H_
#define __MFT_HANDLER_RUNPARAMS_COMPONENT_H_

#include "ace/RW_Mutex.h"
#include "ace/Singleton.h"

#include "app/response_plans/plan_step_plugin/common_editor/src/DllExport.h"

namespace TA_Base_Core
{
    class IDllHandler;
}

namespace TA_Base_App
{
    class IPlanAgentAccess;
    class IMftPlanAgentAccess;

    class PLAN_COMM_DLL PlanEditorStepInitComponentImpl
    {
        
    public:
        friend class ACE_Singleton<PlanEditorStepInitComponentImpl, ACE_Recursive_Thread_Mutex>;
		PlanEditorStepInitComponentImpl();
		virtual ~PlanEditorStepInitComponentImpl();
        virtual void initLibrary( const TA_Base_Core::IDllHandler& handler, IPlanAgentAccess& refPlanAgentAccess );
        virtual void initLibrary( const TA_Base_Core::IDllHandler& handler, IMftPlanAgentAccess& refPlanAgentAccess );

    private:
        PlanEditorStepInitComponentImpl( const PlanEditorStepInitComponentImpl& );
        PlanEditorStepInitComponentImpl& operator=( const PlanEditorStepInitComponentImpl& );

    protected:
        void initMiscellaneous( const TA_Base_Core::IDllHandler& handler );
        void initPlanAgentAccess( IPlanAgentAccess& refPlanAgentAccess );
        void initMftPlanAgentAccess( IMftPlanAgentAccess& refMftPlanAgentAccess );

    };

    typedef ACE_Singleton<PlanEditorStepInitComponentImpl, ACE_Recursive_Thread_Mutex> PlanEditorStepInitComponent;
}

#endif