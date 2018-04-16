/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/MftProcessorPluginFactory.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #11 $
  *
  *  Last modification: $DateTime: 2015/10/26 13:03:00 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */
#include <string>
#include <utility>
#include "ace/DLL.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DllHandler.h"
#include "bus/response_plans/common/src/RpcAgency.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "app/response_plans/plan_mft_processor/src/PlanAgentAccess.h"
#include "app/response_plans/plan_mft_processor/src/MftProcessorPluginFactory.h"
#include "app/response_plans/plan_mft_processor/RunPlanStep/MftRunPlanComponent.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/RightsManager.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanEditorStepInitComponent.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MftPlanAgentAccessFactory.h"

namespace TA_Base_App
{
    MftProcessorPluginFactory* MftProcessorPluginFactory::s_instance = NULL;
    ACE_Thread_Mutex MftProcessorPluginFactory::s_singletonLock;

    MftProcessorPluginFactory& MftProcessorPluginFactory::getInstance()
    {
        FUNCTION_ENTRY("getInstance");

        if ( NULL == s_instance )
        {
            ACE_Guard<ACE_Thread_Mutex> guard( s_singletonLock );

            if ( NULL == s_instance )
            {
                s_instance = new MftProcessorPluginFactory();
            }
        }

        FUNCTION_EXIT;
        return *s_instance;
    }

    void MftProcessorPluginFactory::destroyInstance()
    {
        FUNCTION_ENTRY("destroyInstance");

        ACE_Guard<ACE_Thread_Mutex> guard( s_singletonLock );

        if ( NULL != s_instance )
        {
            delete s_instance;

            s_instance = NULL;
        }

        FUNCTION_EXIT;
    }

    MftProcessorPluginFactory::MftProcessorPluginFactory()
    {
        // Get Step type <-> dll name map from database

		std::map<unsigned long, TA_Base_Bus::StepTypeInfo> stepTypes;
		try
		{
			TA_Base_Bus::PlanDataReader::instance()->getStepTypes( stepTypes );
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed load step types info");
        }

        TA_Base_Core::DllHandler dllHandler;
        PlanEditorStepInitComponent::instance()->initLibrary(dllHandler, PlanAgentAccess::getInstance());
        MftPlanAgentAccessFactory::init(PlanAgentAccess::getInstance());

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin load dll...");

		for(std::map<unsigned long, TA_Base_Bus::StepTypeInfo>::iterator it = stepTypes.begin(); it != stepTypes.end(); ++it)
		{
			const TA_Base_Bus::StepTypeInfo& refTypeInfo = (*it).second;
            if (!refTypeInfo.clientDllName.empty())
			{
                std::string strDllName = refTypeInfo.clientDllName + ACE_DLL_SUFFIX;
			    ACE_DLL dllLoader;
                int retval = dllLoader.open(strDllName.c_str(), 0, false);
                if ( retval != 0 )
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Fail to open dll");
				    continue;
                }

			    void *void_ptr = dllLoader.symbol("createEditorStepComponent");
                GetEditorStepComponentPtr mftStepComponentPtr = reinterpret_cast<GetEditorStepComponentPtr>(reinterpret_cast<unsigned long>(void_ptr));
			    if ( NULL == mftStepComponentPtr )
			    {
				    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Fail to obtain step creator ptr");
			    }
			    else
			    {
				    TA_Base_App::IEditorStepComponent* pStepComponent= mftStepComponentPtr();
				    pStepComponent->initLibrary( refTypeInfo.typeId, refTypeInfo.typeAccess, dllHandler, PlanAgentAccess::getInstance(), RightsManager::getInstance(), TA_Base_Bus::RpcAgency::getInstance() );
                    m_mapEditorStepComponents.insert(std::make_pair(refTypeInfo.typeId, pStepComponent));
			    }
			}
            else if (refTypeInfo.typeName == "Run Plan")
			{
                m_mapEditorStepComponents[refTypeInfo.typeId] = MftRunPlanComponent::instance();
                MftRunPlanComponent::instance()->initLibrary(refTypeInfo.typeId, refTypeInfo.typeAccess);
			}
		}

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finished load dll...");
    }

    MftProcessorPluginFactory::~MftProcessorPluginFactory()
    {
        m_mapEditorStepComponents.clear();

        MftRunPlanComponent::close();
        TA_Base_Bus::PlanDataReader::close();
    }

	InteractiveStep* MftProcessorPluginFactory::createExecutableInteractiveStep(const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
		const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters)
	{
		EditorStepComponentMap::iterator creatorItr = m_mapEditorStepComponents.find(eType);
		if ( m_mapEditorStepComponents.end() != creatorItr )
		{
			return (creatorItr->second)->createExecutableStep(unId,eType,ulLocation,ulPlanId,ulInstanceNum,ulStepNum,vecParameters);
		}

		return NULL;
	}

}