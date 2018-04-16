/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/StepImplementationFactory.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #37 $
  *
  *  Last modification: $DateTime: 2015/10/26 13:03:00 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */
#include <string>
#include <utility>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DllHandler.h"
#include "bus/response_plans/common/src/RpcAgency.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "app/response_plans/plan_step_plugin/common/src/BasicParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IStepComponent.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActiveStepMessage.h"
#include "app/response_plans/plan_agent/src/StepImplementationFactory.h"
#include "app/response_plans/plan_agent/src/StartStepComponent.h"
#include "app/response_plans/plan_agent/src/EndStepComponent.h"
#include "app/response_plans/plan_agent/src/AbortStepComponent.h"
#include "app/response_plans/plan_agent/src/RunPlanStepComponent.h"
#include "app/response_plans/plan_agent/src/TerminateStepComponent.h"
#include "app/response_plans/plan_agent/src/InclusiveGatewayStepComponent.h"
#include "app/response_plans/plan_agent/src/ExclusiveGatewayStepComponent.h"
#include "app/response_plans/plan_agent/src/ParallelGatewayStepComponent.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"

namespace TA_Base_App
{


    ActiveStepFactoryImpl::ActiveStepFactoryImpl():
        m_stepDllMap(),
        m_stepComponentMap()
    {
		std::map<unsigned long, TA_Base_Bus::StepTypeInfo> stepTypes;
		try
		{
			TA_Base_Bus::PlanDataReader::instance()->getStepTypes( stepTypes );
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed load step types info");
		}

        std::map<long, std::string> stepTypeDllNameMap;
		for(std::map<unsigned long, TA_Base_Bus::StepTypeInfo>::iterator it = stepTypes.begin(); it != stepTypes.end(); ++it)
		{
			const TA_Base_Bus::StepTypeInfo& refType = (*it).second;

            if ( 0 == refType.typeAccess )
            {
                continue;
            }

			if(!refType.serverDllName.empty())
			{
				stepTypeDllNameMap.insert(std::make_pair(refType.typeId, refType.serverDllName + ACE_DLL_SUFFIX));
			}
			else if(refType.typeName == "Start")
			{
				m_stepComponentMap[refType.typeId] = StartStepComponent::instance();
			}
			else if(refType.typeName == "End")
			{
				m_stepComponentMap[refType.typeId] = EndStepComponent::instance();
			}
			else if(refType.typeName == "Abort")
			{
				m_stepComponentMap[refType.typeId] = AbortStepComponent::instance();
			}
			else if(refType.typeName == "Run Plan")
			{
				m_stepComponentMap[refType.typeId] = RunPlanStepComponent::instance();
			}
			else if(refType.typeName == "Terminate Plan")
			{
				m_stepComponentMap[refType.typeId] = TerminateStepComponent::instance();
			}
			else if(refType.typeName == "Inclusive Gateway")
			{
				m_stepComponentMap[refType.typeId] = InclusiveGatewayStepComponent::instance();
			}
			else if(refType.typeName == "Exclusive Gateway")
			{
				m_stepComponentMap[refType.typeId] = ExclusiveGatewayStepComponent::instance();
			}
			else if(refType.typeName == "Parallel Gateway")
			{
				m_stepComponentMap[refType.typeId] = ParallelGatewayStepComponent::instance();
			}
		}

		std::map<long, std::string>::iterator mapIter = stepTypeDllNameMap.begin();

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin load dll...");

        for (;mapIter != stepTypeDllNameMap.end(); ++mapIter)
        {
			ACE_DLL dllLoader;
            int retval = dllLoader.open( mapIter->second.c_str() );
            if (0 != retval)
            {
				const char* error = dllLoader.error();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to open dll [%d, %s], error:%s", mapIter->first, mapIter->second.c_str(), NULL != error ? error : "");
				continue;
            }

            void *void_ptr = dllLoader.symbol("createStepComponent");

            GetStepComponentPtr stepComponentPtr = reinterpret_cast<GetStepComponentPtr>(reinterpret_cast<unsigned long>(void_ptr));
            if (NULL == stepComponentPtr)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Fail to obtain step creator ptr");
                continue;
            }

            TA_Base_Core::DllHandler dllHandler;
            TA_Base_App::IStepComponent* pStepComponent = stepComponentPtr();
			pStepComponent->init(dllHandler, *(AAMessageSender::instance()), TA_Base_Bus::RpcAgency::getInstance());
            m_stepComponentMap.insert( std::make_pair( mapIter->first, pStepComponent ) );
            m_stepDllMap.insert( std::make_pair( mapIter->first, dllLoader ) );
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finished load dll...");
    }

    ActiveStepFactoryImpl::~ActiveStepFactoryImpl()
    {
		for (std::map<long, ACE_DLL>::iterator itDllMap = m_stepDllMap.begin(); itDllMap != m_stepDllMap.end(); ++itDllMap)
		{
			itDllMap->second.close();
		}

        m_stepDllMap.clear();
    }

    ActiveStep* ActiveStepFactoryImpl::createStep( const BasicParameter& detail, IActivePlan& plan )
    {
        StepComponentMap::iterator creatorItr = m_stepComponentMap.find(detail.type);
        if ( m_stepComponentMap.end() != creatorItr )
        {
            return (creatorItr->second)->createActiveStep( detail, plan );
        }

        return NULL;
    }

    IActiveStepMessage* ActiveStepFactoryImpl::createStepMessage( const int stepType, const unsigned long ulStepNum, const std::vector<std::string>& parameters )
    {
        StepComponentMap::iterator creatorItr = m_stepComponentMap.find(stepType);
        if ( m_stepComponentMap.end() != creatorItr )
        {
            return (creatorItr->second)->createPlanStepMessage( ulStepNum, parameters );
        }

        return NULL;
    }

    void ActiveStepFactoryImpl::validateStepParameter( const int stepType, const std::vector<std::string>& parameters )
    {
        StepComponentMap::iterator validatorItr = m_stepComponentMap.find(stepType);
        if (m_stepComponentMap.end() != validatorItr)
        {
            (validatorItr->second)->validateStepParameter( parameters );
        }
    }

}
