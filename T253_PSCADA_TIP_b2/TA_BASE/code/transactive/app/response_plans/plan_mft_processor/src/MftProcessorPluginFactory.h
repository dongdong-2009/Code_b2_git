#ifndef MFT_PROCESSOR_IMPLEMENTATION_FACTORY_H
#define MFT_PROCESSOR_IMPLEMENTATION_FACTORY_H

#include <map>
#include "ace/Thread_Mutex.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IEditorStepComponent.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/InteractiveStep.h"

namespace TA_Base_App
{

typedef IEditorStepComponent* (*GetEditorStepComponentPtr) ();
typedef void (*DestroyEditorStepComponentPtr) ();
typedef std::map<long, IEditorStepComponent*> EditorStepComponentMap;

class MftProcessorPluginFactory
{
public:
	InteractiveStep* createExecutableInteractiveStep(const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
		const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters);

    static MftProcessorPluginFactory& getInstance();
    static void destroyInstance();

private:
    MftProcessorPluginFactory();
    virtual ~MftProcessorPluginFactory();
    MftProcessorPluginFactory( const MftProcessorPluginFactory& );
    MftProcessorPluginFactory& operator=( const MftProcessorPluginFactory& );

private:
    EditorStepComponentMap m_mapEditorStepComponents;

private:
    static MftProcessorPluginFactory* s_instance;
    static ACE_Thread_Mutex s_singletonLock;
};

}

#endif