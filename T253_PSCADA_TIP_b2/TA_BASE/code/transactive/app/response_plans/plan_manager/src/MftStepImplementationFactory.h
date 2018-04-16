#ifndef MFT_STEP_IMPLEMENTATION_FACTORY_H
#define MFT_STEP_IMPLEMENTATION_FACTORY_H

#include <map>
#include "ace/RW_Mutex.h"
#include "ace/Singleton.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IEditorStepComponent.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityComponent.h"

namespace TA_Base_App
{
	enum EStepAccessType
	{
		ACCESS_NONE = 0,
		ACCESS_CLASSIC_ONLY,
		ACCESS_WORKFLOW_ONLY,
		ACCESS_ALL
	};

    typedef IEditorStepComponent* (*GetEditorStepComponentPtr) ();
    typedef void(*DestroyEditorStepComponentPtr) ();
    typedef std::map<long, IEditorStepComponent*> EditorStepComponentMap;

    class EditorStepFactoryImpl
    {
    public:
        friend class ACE_Singleton<EditorStepFactoryImpl, ACE_Recursive_Thread_Mutex>;

		PlanStep* createStep( IPlanNode* parent, BasicParameter& stepDetail );
        PlanStep* createStep( IPlanNode* parent, const int stepType, const unsigned long stepId );

        std::vector<IActivityComponent*> getStepTypes();

    private:
        EditorStepFactoryImpl();
        ~EditorStepFactoryImpl();
        EditorStepFactoryImpl( const EditorStepFactoryImpl& );
        EditorStepFactoryImpl& operator=( const EditorStepFactoryImpl& );

    private:
        EditorStepComponentMap m_mapEditorStepComponent;
    };

    typedef ACE_Singleton<EditorStepFactoryImpl, ACE_Recursive_Thread_Mutex> EditorStepFactory;
}

#endif