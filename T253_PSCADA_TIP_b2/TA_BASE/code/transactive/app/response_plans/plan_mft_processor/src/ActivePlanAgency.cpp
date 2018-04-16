/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/ActivePlanAgency.cpp $
  * @author:  Huang Jian
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2017/12/12 14:18:48 $
  * Last modified by:  $Author: hoa.le $
  *
  */

#include "ace/Guard_T.h"

#include "core/utilities/src/RunParams.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "bus/response_plans/common/src/RpcAgency.h"
#include "bus/response_plans/plan_agent/src/SortableActiveStepId.h"
#include "app/response_plans/plan_mft_processor/src/ActivePlanAgency.h"
#include "app/response_plans/plan_mft_processor/src/PlanAgentUpdateProcessor.h"
#include "app/response_plans/plan_mft_processor/src/MftProcessorPluginFactory.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/InteractiveStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/RightsManager.h"

namespace TA_Base_App
{
    ActivePlanAgency* ActivePlanAgency::s_instance = NULL;
    ACE_Thread_Mutex ActivePlanAgency::s_singletonLock;
    
    ActivePlanAgency& ActivePlanAgency::getInstance()
    {
        FUNCTION_ENTRY("getInstance");

        if ( NULL == s_instance )
        {
            ACE_Guard<ACE_Thread_Mutex> guard( s_singletonLock );

            if ( NULL == s_instance )
            {
                s_instance = new ActivePlanAgency();
            }
        }

        FUNCTION_EXIT;
        return *s_instance;
    }

    void ActivePlanAgency::destroyInstance()
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

    ActivePlanAgency::ActivePlanAgency() :
    m_mtxStepsLock(),
    m_activePlanStepMap(),
    m_planAgent(NULL)
    {
    }

    ActivePlanAgency::~ActivePlanAgency()
    {
        uninit();
    }

    void ActivePlanAgency::init()
    {
        RightsManager::getInstance().init();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "LOCATIONKEY:%d", RightsManager::getInstance().locationId() );

        if( m_planAgent == NULL )
        {
            m_planAgent = new TA_Base_Bus::PlanAgentLibrary( RightsManager::getInstance().locationId() );
        }

        PlanAgentUpdateProcessor::getInstance();
        TA_Base_Bus::RpcAgency::createInstance( RightsManager::getInstance().locationId(), RightsManager::getInstance().workstationId() );
        TA_Base_Bus::RpcAgency::getInstance().subscribe( *this );
        MftProcessorPluginFactory::getInstance();

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ActivePlanAgency::init() ok");
    }

    void ActivePlanAgency::uninit()
    {
        cleanSteps();

        MftProcessorPluginFactory::destroyInstance();
        TA_Base_Bus::RpcAgency::getInstance().unsubscribe( *this );
        TA_Base_Bus::RpcAgency::destroyInstance();
        PlanAgentUpdateProcessor::destroyInstance();

        if( NULL != m_planAgent )
        {
            delete m_planAgent;
            m_planAgent = NULL;
        }

        RightsManager::destroyInstance();
    }

    TA_Base_Bus::PlanAgentLibrary* ActivePlanAgency::getPlanAgent() const
    {
        return m_planAgent;
    }

    void ActivePlanAgency::cleanSteps()
    {
        ACE_Guard<ACE_Thread_Mutex> gudLock( m_mtxStepsLock );

        for ( ActivePlanStepMapIt stepIt = m_activePlanStepMap.begin(); stepIt != m_activePlanStepMap.end(); ++stepIt )
        {
            if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == stepIt->second->getCurrentState() )
            {
                stepIt->second->terminateAndWait();
            }

            delete stepIt->second;
            stepIt->second = NULL;
        }

        m_activePlanStepMap.clear();
    }

    void ActivePlanAgency::processActivePlanUpdate( const TA_Base_Core::ActivePlanDetail& refPlanDetail )
    {
        if (( TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE == refPlanDetail.activePlanState ) ||
            ( TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE == refPlanDetail.activePlanState ) ||
            ( TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE == refPlanDetail.activePlanState ))
        {
            ACE_Guard<ACE_Thread_Mutex> gudLock( m_mtxStepsLock );

            for ( ActivePlanStepMapIt itStep = m_activePlanStepMap.begin(); itStep != m_activePlanStepMap.end(); )
            {
                if (( itStep->first.getNodeId() == refPlanDetail.plan.plan ) && ( itStep->first.getPlanInstance() == refPlanDetail.plan.instance ))
                {
                    if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == itStep->second->getCurrentState() )
                    {
                        itStep->second->terminateAndWait();
                    }

                    delete itStep->second;
                    itStep->second = NULL;
                    m_activePlanStepMap.erase( itStep++ );
                }
                else
                {
                    ++itStep;
                }
            }
        }
    }

    void ActivePlanAgency::handle( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
        const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
    {
        TA_Base_Bus::SortableActiveStepId srtStep( ulPlanId, ulInstanceNum, ulStepNum );

        ACE_Guard<ACE_Thread_Mutex> gudLock( m_mtxStepsLock );

        ActivePlanStepMapIt itExist = m_activePlanStepMap.find( srtStep );
        if ( m_activePlanStepMap.end() != itExist )
        {
            if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == itExist->second->getCurrentState() )
            {
                itExist->second->terminateAndWait();
            }

            delete itExist->second;
            itExist->second = NULL;
            m_activePlanStepMap.erase( itExist );
        }

        InteractiveStep* pNewStep = MftProcessorPluginFactory::getInstance().createExecutableInteractiveStep( unId, eType, ulLocation, ulPlanId, ulInstanceNum, ulStepNum, vecParameters );
        if ( NULL != pNewStep )
        {
            m_activePlanStepMap.insert( ActivePlanStepMap::value_type( srtStep, pNewStep ) );
            pNewStep->start();
            pNewStep = NULL;
        }
    }

    void ActivePlanAgency::acknowledgeRequest( const unsigned int unId )
    {
        TA_Base_Bus::RpcAgency::getInstance().asyncSendAck( RightsManager::getInstance().locationId(), unId );
    }

}