/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlan.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #21 $
  *
  *  Last modification: $DateTime: 2015/06/03 18:11:32 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "ace/OS.h"
#include "ace/Guard_T.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DataConversion.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/GlobalConstant.h"
#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "app/response_plans/plan_agent/src/PlanAccessFactory.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/ActivePlanStepPausedMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanStepFailedMessage.h"
#include "app/response_plans/plan_agent/src/PlanDetail.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"
#include "app/response_plans/common/src/StringConstants.h"

// namespace alias for CORBA structures/types
unsigned long StartStepId = 1u;

namespace TA_Base_App
{
	ActivePlan::ActivePlan( const TA_Base_Core::ActivePlanId& plan, const bool bPending, const bool bStateSyncDone ) :
    // Initialize all member variables correctly
    m_plan(plan),
    m_engStepExecution( *this ),
    m_lstCurSteps(),
    m_strSession( "" ),
    m_strRemark(( bPending && bStateSyncDone ) ? RemarkPlanPending : "" ),
    m_isActive( true ),
    m_bPaused( bPending ),
    m_bStateSyncDone( bStateSyncDone )
	{
        PlanAccessFactory::instance()->getActiveStepList( m_plan.plan, *this, m_mapAllSteps );

        NumToActiveStepMap::iterator itStartStep = m_mapAllSteps.find( StartStepId );

        {
            ACE_Write_Guard<ACE_RW_Mutex> grdSteps( m_mtxCurStepsLock );
            m_lstCurSteps.clear();
            if ( m_mapAllSteps.end() != itStartStep )
            {
                m_lstCurSteps.push_back( itStartStep->second );
                m_engStepExecution.start( static_cast<int>(m_lstCurSteps.size()) );
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Fail to load plan %lu", m_plan.plan );
                PlanExecutor::instance()->complete( m_plan );
            }
        }
	}

	ActivePlan::~ActivePlan()
    {
        m_engStepExecution.stop();

        // Clear step map
        for ( std::map<unsigned long, ActiveStep*>::iterator itLoop = m_mapAllSteps.begin(); m_mapAllSteps.end() != itLoop; ++itLoop )
        {
            if ( NULL != itLoop->second )
            {
                delete itLoop->second;
                itLoop->second = NULL;
            }
        }

        m_mapAllSteps.clear();
	}

	int ActivePlan::execute()
	{
        return 0;
    }

    void ActivePlan::stepCompleted()
    {
        if ( !m_bStateSyncDone || !ActivePlanBulletinBoard::instance()->isControlMode() )
        {
            return;
        }

        return;

        ActiveStepList lstNextSteps;
        int nCompletedStep = 0;

        {
            ACE_Write_Guard<ACE_RW_Mutex> grdSteps( m_mtxCurStepsLock );

            for ( ActiveStepListIt itLoop = m_lstCurSteps.begin(); m_lstCurSteps.end() != itLoop; )
            {
                ActiveStep* pStep = *itLoop;

                if ( NULL == pStep )
                {
                    continue;
                }

                if ( pStep->completed() )
                {
                    ActiveStepList lstCurNextSteps = pStep->getNextStep();

                    ++nCompletedStep;

                    lstNextSteps.insert( lstNextSteps.begin(), lstCurNextSteps.begin(), lstCurNextSteps.end() );

                    itLoop = m_lstCurSteps.erase( itLoop );
                }
                else
                {
                    ++itLoop;
                }
            }

            m_lstCurSteps.insert( m_lstCurSteps.end(), lstNextSteps.begin(), lstNextSteps.end() );
        }

        if ( 0 < nCompletedStep )
        {
            m_engStepExecution.parallelAll( lstNextSteps, nCompletedStep );
        }
    }

    bool ActivePlan::stepCompleted( const unsigned long ulStepNum )
    {
        bool bCompleted = false;

        {
            ACE_Read_Guard<ACE_RW_Mutex> grdSteps( m_mtxCurStepsLock );

            NumToActiveStepMapIt itStep = m_mapAllSteps.find( ulStepNum );

            if ( m_mapAllSteps.end() != itStep )
            {
                bCompleted = itStep->second->completed();

                bCompleted &= ( m_mapExecutingSteps.find( ulStepNum ) == m_mapExecutingSteps.end() );
            }
        }

        return bCompleted;
    }

    void ActivePlan::stop()
	{
		FUNCTION_ENTRY( "ActivePlan::stop" );

        m_engStepExecution.stop();

        FUNCTION_EXIT
	}

    void ActivePlan::complete()
    {
        FUNCTION_ENTRY( "ActivePlan::complete" );

        m_engStepExecution.stop();

        FUNCTION_EXIT
    }

	bool ActivePlan::pause()
	{
        FUNCTION_ENTRY( "ActivePlan::pause" );

        bool bPauseSucceed = false;

        if ( !m_bPaused )
        {
            m_bPaused = true;
            bPauseSucceed = true;
        }

        FUNCTION_EXIT
        return bPauseSucceed;
	}

	void ActivePlan::resume()
    {
        FUNCTION_ENTRY( "ActivePlan::resume" );

        if ( m_bPaused )
        {
            m_bPaused = false;
        }

        {
            ACE_Read_Guard<ACE_RW_Mutex> grdSteps( m_mtxCurStepsLock );

            for ( NumToActiveStepMultiMapIt itLoop = m_mapExecutingSteps.begin(); m_mapExecutingSteps.end() != itLoop; ++itLoop )
            {
                if ( itLoop->second->isPaused() )
                {
                    itLoop->second->resume();
                }
            }
        }

        FUNCTION_EXIT
	}

	void ActivePlan::reassign( const std::string& sessionId )
	{
		FUNCTION_ENTRY( "ActivePlan::reassign" );

        m_strSession = sessionId.c_str();

        FUNCTION_EXIT
	}

	void ActivePlan::unassign( void )
	{
		FUNCTION_ENTRY( "ActivePlan::unassign" );
        FUNCTION_EXIT
	}

	bool ActivePlan::customiseStep( const TA_Base_Core::StepCustomisableDetail& newDetail )
	{
        FUNCTION_ENTRY( "ActivePlan::customiseStep" );

        bool bCustomized = false;

        NumToActiveStepMapIt itFound = m_mapAllSteps.find( newDetail.id );

        if ( m_mapAllSteps.end() != itFound )
        {
            bCustomized = itFound->second->customise( newDetail );
        }

        if ( !bCustomized )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "Failed to customise PlanStep [%lu, %lu, %lu]", m_plan.plan, m_plan.instance, newDetail.id );
        }

        FUNCTION_EXIT
        return bCustomized;
    }

    void ActivePlan::stepStateUpdated( const unsigned long ulStepNum, const int nState )
    {
        FUNCTION_ENTRY( "ActivePlan::stepStateUpdated" );

        NumToActiveStepMapIt itFound = m_mapAllSteps.find( ulStepNum );

        if ( m_mapAllSteps.end() != itFound )
        {
            itFound->second->stateUpdated( nState );
        }

        FUNCTION_EXIT
    }

    // Synchronize customise steps date from Control Agent to Monitor Agent
    void ActivePlan::customiseSteps( const TA_Base_Core::StepCustomisableDetails& newDetails )
    {
        for ( unsigned long nLoop = 0u; nLoop < newDetails.length(); ++nLoop )
        {
            const TA_Base_Core::StepCustomisableDetail& refDetail = newDetails[nLoop];

            NumToActiveStepMapIt itFound = m_mapAllSteps.find( refDetail.id );

            if ( m_mapAllSteps.end() == itFound )
            {
                continue;
            }

            itFound->second->customise( refDetail );
        }
    }

	std::string ActivePlan::dumpContext( void )
	{
		std::ostringstream context;

		context << "[P:" << m_plan.plan << ",I:" << m_plan.instance << "]";

		return context.str();
	}

    void ActivePlan::setStateSyncDone( bool bSyncDone )
    {
        m_bStateSyncDone = bSyncDone;
    }

    const TA_Base_Core::ActivePlanId& ActivePlan::id() const
    {
        return m_plan;
    }

    const std::string ActivePlan::name() const
    {
        return ActivePlanBulletinBoard::instance()->getActivePlanDetail( m_plan ).detail.path.in();
    }

    const std::string ActivePlan::session() const
    {
        return ActivePlanBulletinBoard::instance()->getActivePlanDetail( m_plan ).detail.session.in();
    }

    const unsigned int ActivePlan::userLocation() const
    {
        return ActivePlanBulletinBoard::instance()->getActivePlanDetail( m_plan ).userLocation;
    }

    const unsigned int ActivePlan::userWorkstation() const
    {
        return ActivePlanBulletinBoard::instance()->getActivePlanDetail( m_plan ).userWorkstation;
    }

    bool ActivePlan::isPausePending()
    {
        return m_bPaused;
    }

    bool ActivePlan::anyPlanStepFailed()
    {
        bool bResult = false;

        for ( NumToActiveStepMapIt itLoop = m_mapAllSteps.begin(); m_mapAllSteps.end() != itLoop; ++itLoop )
        {
            if ( itLoop->second->isFailed() )
            {
                bResult = true;
                break;
            }
        }

        return bResult;
    }

    int ActivePlan::runStep()
    {
        if ( !m_isActive )
        {
            return -1;
        }

        ActiveStep* pCurStep = NULL;

        {
            ACE_Write_Guard<ACE_RW_Mutex> grdSteps( m_mtxCurStepsLock );

            if ( !m_lstCurSteps.empty() )
            {
                pCurStep = m_lstCurSteps.front();
                m_lstCurSteps.pop_front();

                m_mapExecutingSteps.insert( NumToActiveStepMultiMap::value_type(pCurStep->id(), pCurStep ));
            }
        }

        if ( NULL == pCurStep )
        {
            return -1;
        }

        executeStep( *pCurStep );

        if ( !m_isActive )
        {
            return -1;
        }

        int nNewStepCount = 0;

        bool bPlanCompleted = false;

        {
            ACE_Write_Guard<ACE_RW_Mutex> grdSteps( m_mtxCurStepsLock );

            // No Control Monitor Mode data synchronization, avoid to use output / input data
            //std::map<std::string, std::string> mapExchangingData;

            //pCurStep->exchangeOutput( mapExchangingData );
            ActiveStepList lstCurNextSteps = pCurStep->getNextStep();

            for ( ActiveStepListIt itLoop = lstCurNextSteps.begin(); lstCurNextSteps.end() != itLoop; ++itLoop )
            {
                ActiveStep* pStep = *itLoop;
                if ( NULL == pStep )
                {
                    continue;
                }

                if ( m_mapExecutingSteps.find( pStep->id() ) != m_mapExecutingSteps.end() )
                {
                    continue;
                }

                if ( m_lstCurSteps.end() != std::find( m_lstCurSteps.begin(), m_lstCurSteps.end(), pStep ))
                {
                    continue;
                }

                ++nNewStepCount;
                //pStep->exchangeInput( mapExchangingData );
                m_lstCurSteps.push_back( pStep );
            }

            nNewStepCount -= 1;

            //m_lstCurSteps.insert( m_lstCurSteps.end(), lstCurNextSteps.begin(), lstCurNextSteps.end() );

            NumToActiveStepMultiMapIt itFound = m_mapExecutingSteps.find( pCurStep->id() );
            if ( itFound != m_mapExecutingSteps.end() )
            {
                m_mapExecutingSteps.erase( itFound );
            }

            //nNewStepCount = static_cast<int>(lstCurNextSteps.size()) - 1;

            bPlanCompleted = ( m_mapExecutingSteps.empty() && m_lstCurSteps.empty() );
        }

        if ( bPlanCompleted )
        {
            PlanExecutor::instance()->complete( m_plan );
        }
        else if ( m_bPaused && allStepPaused( false ))
        {
            planPaused();
        }

        return nNewStepCount;
    }

    void ActivePlan::executeStep( ActiveStep& refStep )
    {
        unsigned long ulStepId = refStep.id();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Plan Step [%lu, %lu, %lu] started", m_plan.plan, m_plan.instance, ulStepId );

        try
        {
            bool bExternalStateChanged = false;
            bool bRemarkChanged = false;

            // Initialization method would be called in both Control Mode and Monitor Mode
            refStep.init();
            ActivePlanBulletinBoard::instance()->initStep( m_plan, ulStepId, refStep.pos(), refStep.getName() );

            bool bCurStepPaused = false;

            while ( !refStep.completed() && m_isActive )
            {
                //The next step of a Plan is executed only if the PlanAgent is in Control mode
                // and not in Monitor mode. And Plan state synchronization from Control mode is done.
                if ( !m_bStateSyncDone || !ActivePlanBulletinBoard::instance()->isControlMode() )
                {
                    static const ACE_Time_Value CpuIdleInterval( 0, 100000 ); // 100 millisecond
                    ACE_OS::sleep( CpuIdleInterval );
                    continue;
                }

                if ( m_bPaused && !bCurStepPaused )
                {
                    bCurStepPaused = true;
                    refStep.pause();
                }

                refStep.execute( bExternalStateChanged, bRemarkChanged );

                if ( bExternalStateChanged )
                {
                    ActivePlanBulletinBoard::instance()->updateStepState( m_plan, ulStepId, refStep.getState(), refStep.getRemark() );

                    if ( refStep.isFailed() )
                    {
                        IActiveStepMessage* pErrorEvent = refStep.getErrorEvent();

                        IActivePlanMessage* pErrorMsg = ( NULL != pErrorEvent ) ? new ActivePlanStepFailedMessage( pErrorEvent ) :
                            new ActivePlanStepFailedMessage( refStep.getRemark(), ulStepId, refStep.isIgnoreFailure() );

                        PlanExecutor::instance()->sendMessage( m_plan, pErrorMsg );
                    }

                    if ( refStep.isPaused() )
                    {
                        PlanExecutor::instance()->sendMessage( m_plan, new ActivePlanStepPausedMessage( ulStepId ));
                    }
                    else if ( bCurStepPaused )
                    {
                        bCurStepPaused = false;
                    }
                }
                else if ( bRemarkChanged )
                {
                    ActivePlanBulletinBoard::instance()->updateStepRemark( m_plan, ulStepId, refStep.getRemark() );
                }
            }
        }
        catch( const std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when execute step" );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Plan Step [%lu, %lu, %lu] completed", m_plan.plan, m_plan.instance, ulStepId );
    }

    bool ActivePlan::allStepPaused( const bool bIgnoreParallels )
    {
        bool bAllPaused = true;

        ACE_Read_Guard<ACE_RW_Mutex> grdSteps( m_mtxCurStepsLock );

        if ( !m_lstCurSteps.empty() )
        {
            bAllPaused = false;
        }
        else if ( m_mapExecutingSteps.empty() )
        {
            bAllPaused = false;
        }
        else
        {
            size_t szParallelNum = 0u;

            for ( NumToActiveStepMultiMapIt itLoop = m_mapExecutingSteps.begin(); m_mapExecutingSteps.end() != itLoop; ++itLoop )
            {
                if ( bIgnoreParallels && itLoop->second->isParallel() )
                {
                    ++szParallelNum;
                    continue;
                }

                if ( !itLoop->second->isPaused() )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Plan Step [%lu, %lu, %lu] is not paused", m_plan.plan, m_plan.instance, itLoop->second->id() );
                    bAllPaused = false;
                    break;
                }
            }

            if ( bIgnoreParallels && ( szParallelNum == m_mapExecutingSteps.size() ))
            {
                bAllPaused = false;
            }
        }

        return bAllPaused;
    }

    void ActivePlan::planSharedData( const std::map<std::string, std::string>& mapData )
    {
        {
            ACE_Write_Guard<ACE_RW_Mutex> grdSteps( m_mtxSharedDataLock );

            for ( NameToValueMap::const_iterator itLoop = mapData.begin(); mapData.end() != itLoop; ++itLoop )
            {
                if ( itLoop->second.empty() )
                {
                    m_mapSharedData.erase( itLoop->first );
                }
                else
                {
                    m_mapSharedData[itLoop->first] = itLoop->second.c_str();
                }
            }
        }

        ActivePlanBulletinBoard::instance()->updatePlanSharedData( m_plan, mapData );
    }

    void ActivePlan::planSharedData( const std::string& strName, const std::string& strValue )
    {
        std::map<std::string, std::string> mapUpdatedValue;
        mapUpdatedValue.insert( std::map<std::string, std::string>::value_type( strName, strValue ));

        planSharedData( mapUpdatedValue );
    }

    const std::map<std::string, std::string> ActivePlan::planSharedData()
    {
        ACE_Read_Guard<ACE_RW_Mutex> grdSteps( m_mtxSharedDataLock );
        return m_mapSharedData;
    }

    const std::string ActivePlan::planSharedData( const std::string& strName )
    {
        ACE_Read_Guard<ACE_RW_Mutex> grdSteps( m_mtxSharedDataLock );

        NameToValueMap::const_iterator itFound = m_mapSharedData.find( strName );

        return ( m_mapSharedData.end() != itFound ) ? itFound->second : "";
    }

    void ActivePlan::planPaused()
    {
        std::string strRemark = "";

        if ( !m_strRemark.empty() )
        {
            strRemark = m_strRemark.c_str();
            m_strRemark.clear();
        }
        else
        {
            strRemark = m_bPaused ? RemarkPlanPausedByOperator : RemarkPlanPausedByFailure;
        }

        ActivePlanBulletinBoard::instance()->updatePlanState( m_plan, TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE, strRemark );
    }

    void ActivePlan::stepResponsed( const unsigned long ulStepNum, const int nResponse )
    {
        planSharedData( TA_Base_Core::DataConversion::toString( ulStepNum ), TA_Base_Core::DataConversion::toString( static_cast<long>(nResponse) ));
    }

    const int ActivePlan::stepResponsed( const unsigned long ulStepNum, const bool bErase )
    {
        const std::string strStep = TA_Base_Core::DataConversion::toString( ulStepNum );
        std::string strReponse = planSharedData( strStep );

        int nReponse = ActiveStep::eWaitUndefined;

        if ( !strReponse.empty() )
        {
            nReponse = static_cast<int>(TA_Base_Core::DataConversion::toInteger( strReponse ));

            if ( bErase )
            {
                strReponse.clear();
                planSharedData( strStep, strReponse );
            }
        }

        return nReponse;
    }

    void ActivePlan::stepData( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        ACE_Write_Guard<ACE_RW_Mutex> grdSteps( m_mtxSharedDataLock );

        if ( vecData.empty() )
        {
            m_mapStepData.erase( ulStepNum );
        }
        else
        {
            m_mapStepData[ulStepNum] = vecData;
        }
    }

    std::vector<std::string> ActivePlan::stepData( const unsigned long ulStepNum, const bool bErase )
    {
        std::vector<std::string> vecData;

        {
            ACE_Read_Guard<ACE_RW_Mutex> grdSteps( m_mtxSharedDataLock );

            StepToDataMapIt itFound = m_mapStepData.find( ulStepNum );

			if (itFound != m_mapStepData.end())
			{
				vecData = itFound->second;
			}
        }

        if ( !vecData.empty() && bErase )
        {
            ACE_Write_Guard<ACE_RW_Mutex> grdSteps( m_mtxSharedDataLock );

            m_mapStepData.erase( ulStepNum );
        }

        return vecData;
    }

    bool ActivePlan::getIsActive()
    {
        return m_isActive;
    }

    void ActivePlan::setIsActive( bool isActive )
    {
        m_isActive = isActive;
    }
} // TA_Base_App