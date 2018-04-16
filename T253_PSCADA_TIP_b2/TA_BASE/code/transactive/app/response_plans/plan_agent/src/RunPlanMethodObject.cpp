/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/RunPlanMethodObject.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #7 $
  *
  *  Last modification: $DateTime: 2014/11/04 13:46:22 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of RunPlanMethodObject.
  */

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/PlanExecutionEngine.h"
#include "app/response_plans/plan_agent/src/RunPlanMethodObject.h"

namespace TA_Base_App
{
    RunPlanMethodObject::RunPlanMethodObject( PlanExecutionEngine& task, std::auto_ptr<ActivePlan> activePlan ) :
    m_engine(task),
    m_strSession( "" ), // Useless
    m_activePlan(activePlan),
    m_ulPlanId( 0u ), // Useless
    m_ulInstance( 0u ), // Useless
    m_ulLocation( 0u ), // Useless
    m_bPending( false ), // Useless
    m_bMaualStart( false ), // Useless
    m_bStartFromMonitorMode( false )
    {
    }

    RunPlanMethodObject::RunPlanMethodObject( PlanExecutionEngine& task, const std::string& strSession,
        const TA_Base_Core::ActivePlanId& refActiveId, const bool bPending, const bool bManualStart, const bool bStartFromMonitorMode ) :
    m_engine(task),
    m_strSession( strSession.c_str() ),
    m_activePlan( NULL ),
    m_ulPlanId( refActiveId.plan ),
    m_ulInstance( refActiveId.instance ),
    m_ulLocation( refActiveId.location ),
    m_bPending( bPending ),
    m_bMaualStart( bManualStart ),
    m_bStartFromMonitorMode( bStartFromMonitorMode )
    {
    }

    RunPlanMethodObject::~RunPlanMethodObject()
    {
    }

    int RunPlanMethodObject::call( void )
    {
        if ( NULL == m_activePlan.get() )
        {
            const TA_Base_Core::ActivePlanId stActivePlan = { m_ulPlanId, m_ulInstance, m_ulLocation };

            if ( !m_bStartFromMonitorMode )
            {
                m_engine.publishPlanStart( m_strSession, stActivePlan, m_bPending, m_bMaualStart );
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "-Msg-: Plan [%d, %d] started", m_ulPlanId, m_ulInstance );
            }

            try
            {
                // ActivePlan parameter bSyncDone = not start from monitor mode
                m_activePlan.reset( new ActivePlan( stActivePlan, m_bPending, !m_bStartFromMonitorMode ) );
            }
            catch( const std::exception& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled." );
            }
        }

        if ( NULL != m_activePlan.get() )
        {
            try
            {
                m_engine.runImpl( m_activePlan );
            }
            catch( const std::exception& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled." );
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create ActivePlan, skip execution with error" );
        }

        return 0;
    }
} // TA_Base_App
