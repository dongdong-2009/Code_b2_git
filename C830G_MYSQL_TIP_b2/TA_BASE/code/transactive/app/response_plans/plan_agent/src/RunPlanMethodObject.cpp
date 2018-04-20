/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Implementation of RunPlanMethodObject.
  */

#include "PlanAgent.h"

namespace TA_Base_App
{
    RunPlanMethodObject::RunPlanMethodObject( PlanExecutionEngine& task, std::auto_ptr<ActivePlan> activePlan ) :
        m_engine(task),
        m_strSession( "" ), // Useless
        m_strPmId( "" ), // Useless
        m_activePlan(activePlan),
        m_activePlanId(), // Useless but can't copy from activePlan which cause crash
        m_eInitialState( TA_Base_Core::UNDEFINED_ACTIVE_PLAN_STATE ), // Useless
        m_bMaualStart( false ), // Useless
        m_bStartFromMonitorMode( false )
    {
    }

    RunPlanMethodObject::RunPlanMethodObject( PlanExecutionEngine& task, 
        const std::string& strSession, const std::string& strPmId, const TA_Base_Core::ActivePlanId& refActiveId, 
        const TA_Base_Core::EActivePlanState& eInitialState, const bool bManualStart, const bool bStartFromMonitorMode ) :
        m_engine(task),
        m_strSession( strSession.c_str() ),
        m_strPmId( strPmId.c_str() ),
        m_activePlan( NULL ),
        m_activePlanId( refActiveId ),
        m_eInitialState( eInitialState ),
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
            if ( !m_bStartFromMonitorMode )
            {
                m_engine.publishPlanStart( m_strSession, m_strPmId, m_activePlanId, m_eInitialState, m_bMaualStart );
                LOG_GENERIC( SOURCE_INFO, INFO_LOG_LEVEL, "-Msg-: Plan [%d, %d] started", m_activePlanId.plan, m_activePlanId.instance );
            }

            try
            {
                // ActivePlan parameter bSyncDone = not start from monitor mode
                std::auto_ptr<ActivePlan> tmpPlan( new ActivePlan( m_activePlanId, m_eInitialState, !m_bStartFromMonitorMode ));
                m_activePlan = tmpPlan;
            }
            catch( const std::exception& e )
            {
                LOG_EXCEPTION_CATCH( SOURCE_INFO, typeid(e).name(), e.what() );
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH( SOURCE_INFO, "Unknown", "Unhandled." );
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
                LOG_EXCEPTION_CATCH( SOURCE_INFO, typeid(e).name(), e.what() );
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH( SOURCE_INFO, "Unknown", "Unhandled." );
            }
        }
        else
        {
            LOG_GENERIC( SOURCE_INFO, ERROR_LOG_LEVEL, "Failed to create ActivePlan, skip execution with error" );
        }

        return( 0 );
    }
} // TA_Base_App
