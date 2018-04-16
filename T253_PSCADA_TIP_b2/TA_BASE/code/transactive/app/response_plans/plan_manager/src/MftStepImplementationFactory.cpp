/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/StepImplementationFactory.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #6 $
  *
  *  Last modification: $DateTime: 2014/09/10 15:13:41 $
  *  Last modified by:  $Author: huangjian $
  *
  *  <description>
  */

#include "StdAfx.h"
#include <string>
#include <utility>
#include "ace/DLL.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DllHandler.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "app/response_plans/plan_step_plugin/common/src/BasicParameter.h"
#include "app/response_plans/plan_editor_plugin/workflow_editor/src/WorkflowEditor.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanAgentAccessFactory.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanEditorStepInitComponent.h"
#include "app/response_plans/plan_manager/src/MftStartStepComponent.h"
#include "app/response_plans/plan_manager/src/MftEndStepComponent.h"
#include "app/response_plans/plan_manager/src/MftAbortStepComponent.h"
#include "app/response_plans/plan_manager/src/MftRunPlanStepComponent.h"
#include "app/response_plans/plan_manager/src/MftTerminatePlanStepComponent.h"
#include "app/response_plans/plan_manager/src/MftInclusiveGatewayStepComponent.h"
#include "app/response_plans/plan_manager/src/MftExclusiveGatewayStepComponent.h"
#include "app/response_plans/plan_manager/src/MftParallelGatewayStepComponent.h"
#include "app/response_plans/plan_manager/src/PlanAgentAccess.h"
#include "app/response_plans/plan_manager/src/MftStepImplementationFactory.h"

namespace TA_Base_App
{
    EditorStepFactoryImpl::EditorStepFactoryImpl()
    {
        // Get Step type <-> dll name map from database
        std::map<unsigned long, TA_Base_Bus::StepTypeInfo> mapStepTypes;
		try
		{
			TA_Base_Bus::PlanDataReader::instance()->getStepTypes( mapStepTypes );
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed load step types info");
		}

		std::map<unsigned long, std::map<unsigned long, TA_Base_Bus::StepShortcut>> mapShortcuts;
		try
		{
			TA_Base_Bus::PlanDataReader::instance()->getStepShortcuts( mapShortcuts );
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed load step shortcuts");
		}

        TA_Base_Core::DllHandler dllHandler;
        PlanEditorStepInitComponent::instance()->initLibrary(dllHandler, PlanAgentAccess::getInstance());
        PlanAgentAccessFactory::init(PlanAgentAccess::getInstance());

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin load dll...");

        std::vector<IActivityComponent*> vecActivityComponents;
		for(std::map<unsigned long, TA_Base_Bus::StepTypeInfo>::iterator it = mapStepTypes.begin(); it != mapStepTypes.end(); ++it)
		{
			const TA_Base_Bus::StepTypeInfo& refType = it->second;

            if ( ACCESS_NONE == refType.typeAccess )
            {
                continue;
            }

            IEditorStepComponent* pStepComponent = NULL;

			if( !refType.clientDllName.empty() )
            {
                ACE_DLL dllLoader;
                std::string strDllName = refType.clientDllName + ACE_DLL_SUFFIX;
                int retval = dllLoader.open(strDllName.c_str(), 0, false);
                if ( retval != 0 )
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Fail to open dll");
                    continue;
                }

                void* void_ptr = dllLoader.symbol("createEditorStepComponent");
                GetEditorStepComponentPtr ptrEditorStepComponent = reinterpret_cast<GetEditorStepComponentPtr>(reinterpret_cast<unsigned long>(void_ptr));
                if (NULL == ptrEditorStepComponent)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Fail to obtain step creator ptr");
                }
                else
                {
                    pStepComponent = ptrEditorStepComponent();
                }
			}
			else if(refType.typeName == "Start")
            {
                pStepComponent = MftStartStepComponent::instance();
			}
			else if(refType.typeName == "End")
            {
                pStepComponent = MftEndStepComponent::instance();
			}
			else if(refType.typeName == "Abort")
            {
                pStepComponent = MftAbortStepComponent::instance();
			}
			else if(refType.typeName == "Run Plan")
            {
                pStepComponent = MftRunPlanStepComponent::instance();
			}
			else if(refType.typeName == "Terminate Plan")
            {
                pStepComponent = MftTerminatePlanStepComponent::instance();
			}
			else if(refType.typeName == "Inclusive Gateway")
            {
                pStepComponent = MftInclusiveGatewayStepComponent::instance();
			}
			else if(refType.typeName == "Exclusive Gateway")
            {
                pStepComponent = MftExclusiveGatewayStepComponent::instance();
            }
			else if(refType.typeName == "Parallel Gateway")
            {
                pStepComponent = MftParallelGatewayStepComponent::instance();
			}

            if ( NULL != pStepComponent )
            {
                m_mapEditorStepComponent[refType.typeId] = pStepComponent;
                pStepComponent->initLibrary( refType.typeId, refType.typeAccess, dllHandler, PlanAgentAccess::getInstance(), mapShortcuts[refType.typeId] );

                if (( ACCESS_WORKFLOW_ONLY == refType.typeAccess ) || ( ACCESS_ALL == refType.typeAccess ))
                {
                    vecActivityComponents.push_back( &(pStepComponent->getActivityComponent()) );
                }
            }
		}
        
        WorkflowEditor::plugActivities( "Plan Step", vecActivityComponents );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finished load dll..." );
    }

    EditorStepFactoryImpl::~EditorStepFactoryImpl()
    {
    }

    PlanStep* EditorStepFactoryImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
	{
		EditorStepComponentMap::iterator creatorItr = m_mapEditorStepComponent.find(stepDetail.type);
		if ( m_mapEditorStepComponent.end() != creatorItr )
		{
			return (creatorItr->second)->createStep(parent, stepDetail);
		}
		return NULL;
	}

    PlanStep* EditorStepFactoryImpl::createStep( IPlanNode* parent, const int stepType, const unsigned long stepId )
	{
		EditorStepComponentMap::iterator creatorItr = m_mapEditorStepComponent.find(stepType);
		if ( m_mapEditorStepComponent.end() != creatorItr )
		{
			return (creatorItr->second)->createStep(parent, stepId, stepType);
		}
		return NULL;
	}

	std::vector<IActivityComponent*> EditorStepFactoryImpl::getStepTypes()
	{
        static std::vector<IActivityComponent*> s_vecActivities;

        if ( s_vecActivities.empty() )
        {
            for (EditorStepComponentMap::iterator itLoop = m_mapEditorStepComponent.begin(); m_mapEditorStepComponent.end() != itLoop; ++itLoop )
            {
                s_vecActivities.push_back( &((itLoop->second)->getActivityComponent()) );
            }
        }

        return s_vecActivities;
	}
}