/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_step_plugin/common_active/src/ActiveStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #9 $
  *
  *  Last modification: $DateTime: 2014/12/08 17:29:55 $
  *  Last modified by:  $Author: huangjian $
  *
  *  Implementation of .
  */

#include <sstream>
#include <limits>
#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/common/src/StringConstants.h"
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActiveStepMessage.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"


// namespace alias for Corba structures/types
const time_t NETWORK_DELAY = 3u;  //seconds allowance for a network delay -feel free to adjust if proven unsuitable
 
namespace TA_Base_App
{

ActiveStep::ActiveStep( BaseStepParameter& refParameter, IActivePlan& flowControl ) :
m_iPlan( flowControl ),
m_tmStartTime( 0u ),
m_delayStartTime( 0u ),
m_delayCompleted( 0u ),
m_lWaitingInterval(0u),
m_bRemarkChanged( false ),
m_bCustomizedRemark( false ),
m_bCompleted( false ),
m_bPendingPause( false ),
m_bPendingResume( false ),
m_bParallel( false ),
m_eState( eInitialState ),
m_refParameter( refParameter )
{
}

ActiveStep::~ActiveStep()
{
}

void ActiveStep::init()
{
    m_tmStartTime = 0u;
    m_delayStartTime = 0u;
    m_delayCompleted = 0u;
    m_lWaitingInterval = 100000;
    m_bRemarkChanged = false;
    m_bCustomizedRemark = false;
    m_bCompleted = false;
    m_bPendingPause = false;
    m_bPendingResume = false;
    m_eState = eInitialState;

    initImpl();
}

bool ActiveStep::customise( const TA_Base_Core::StepCustomisableDetail& newDetail )
{
    if ( !m_refParameter.basic.skippable && m_refParameter.basic.skip )
    {
        return false;
    }

    m_refParameter.basic.skip = newDetail.skip;
    m_refParameter.basic.delay = newDetail.delay;
    m_refParameter.basic.ignoreFailure = newDetail.ignoreFailure;

    return true;
}

unsigned int ActiveStep::id() const
{
    return m_refParameter.basic.id;
}

unsigned int ActiveStep::pos() const
{
    return m_refParameter.basic.position;
}

void ActiveStep::execute( bool& bStateChanged, bool& bRemarkChanged )
{
    switch ( m_eState )
    {
    case eInitialState:
        bStateChanged = initialize();
        break;
    case eDelaying:
        bStateChanged = delaying();
        break;
    case eValidating:
        bStateChanged = validate();
        break;
    case eExecuting:
        bStateChanged = executing();
        break;
    case eWaiting:
        bStateChanged = waiting();
        break;
    case eSucceed:
        bStateChanged = succeed();
        break;
    case eFailed:
        bStateChanged = failed();
        break;
    case eLoadPaused:
    case eDelayPaused:
    case eValidatePaused:
    case eFailedPaused:
        bStateChanged = paused();
        break;
    case eFailedCompleted:
    case eSucceedCompleted:
    case eSkipped:
        m_bCompleted = true;
        break;
    default:
        break;
    }

    bRemarkChanged = m_bRemarkChanged;
    m_bRemarkChanged = false;
}

std::string ActiveStep::dumpContext() const
{
	//std::ostringstream context;
	//context << "[P:" << m_iPlan.getPlan().plan << ",I:" << m_iPlan.id().instance << ",S:" << id() << "]";

	return "";
}

unsigned int ActiveStep::pkey() const
{
    return m_refParameter.basic.pkey;
}

void ActiveStep::refreshStartTime()
{
    m_tmStartTime = ACE_OS::time();
}

void ActiveStep::remark( const std::string& strRemark, const bool bFinal )
{
    if ( 0 == strRemark.compare( m_remark ))
    {
        return;
    }

    m_bRemarkChanged = true;
    m_bCustomizedRemark = bFinal;
    m_remark = strRemark.c_str();
}

std::list<ActiveStep*> ActiveStep::getNextStep()
{
    return m_lstOutgoingStep;
}

bool ActiveStep::completed()
{
    return m_bCompleted;
}

void ActiveStep::pushOutgoingStep( ActiveStep& refStep )
{
    m_lstOutgoingStep.push_back( &refStep );
}

void ActiveStep::pushIncomingStep( ActiveStep& refStep )
{
    m_lstIncomingStep.push_back( &refStep );
}

int ActiveStep::toExternalState( const StepState eState )
{
    int nExternalState = 0;

    switch ( eState )
    {
    case eInitialState:
        nExternalState = TA_Base_Core::UNEXECUTED_ACTIVE_STEP_STATE;
        break;
    case eDelaying:
        nExternalState = TA_Base_Core::DELAYED_ACTIVE_STEP_STATE;
        break;
    case eValidating:
    case eExecuting:
        nExternalState = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;
        break;
    case eWaiting:
        nExternalState = TA_Base_Core::WAITING_ACTIVE_STEP_STATE;
        break;
    case eLoadPaused:
    case eDelayPaused:
    case eValidatePaused:
    case eFailedPaused:
        nExternalState = TA_Base_Core::PAUSED_ACTIVE_STEP_STATE;
        break;
    case eFailed:
    case eFailedCompleted:
        nExternalState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        break;
    case eSucceed:
    case eSucceedCompleted:
        nExternalState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
        break;
    case eSkipped:
        nExternalState = TA_Base_Core::SKIPPED_ACTIVE_STEP_STATE;
        break;
    default:
        nExternalState = TA_Base_Core::UNDEFINED_ACTIVE_STEP_STATE;
        break;
    }

    return nExternalState;
}

ActiveStep::StepState ActiveStep::fromExternalState( const int nExtState )
{
    StepState eState = eInitialState;

    switch ( nExtState )
    {
    case TA_Base_Core::UNEXECUTED_ACTIVE_STEP_STATE:
        eState = eInitialState;
        break;
    case TA_Base_Core::LOADED_ACTIVE_STEP_STATE:
        eState = (eValidating == m_eState ) ? eExecuting : eValidating;
        break;
    case TA_Base_Core::WAITING_ACTIVE_STEP_STATE:
        eState = eWaiting;
        break;
    case TA_Base_Core::DELAYED_ACTIVE_STEP_STATE:
        eState = eDelaying;
        break;
    case TA_Base_Core::PAUSED_ACTIVE_STEP_STATE:
        eState = (eDelaying == m_eState) ? eDelayPaused : ((eValidating == m_eState) ? eValidatePaused : eLoadPaused);
        break;
    case TA_Base_Core::FAILED_ACTIVE_STEP_STATE:
        eState = eFailedCompleted;
        break;
    case TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE:
        eState = eSucceedCompleted;
        break;
    case TA_Base_Core::SKIPPED_ACTIVE_STEP_STATE:
        eState = eSkipped;
        break;
    default:
        eState = eInitialState;
        break;
    }

    return eState;
}

bool ActiveStep::initialize()
{
    bool bExternalStateChanged = false;

    if( m_refParameter.basic.skippable && m_refParameter.basic.skip )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s: skipped.", dumpContext().c_str() );

        m_remark = RemarkPlanStepSkipped;

        m_eState = eSkipped;
        bExternalStateChanged = true;
    }
    else if ( m_refParameter.basic.delay > 0 )
    {
        // Check if we are partway through a delay.
        if (m_delayCompleted != 0)
        {
            // Part way through a delay. Update the start time so that the total delay is correct.
            m_delayStartTime = ACE_OS::time() - m_delayCompleted;
        }
        else
        {
            // Keep a timestamp of the time the delay started.
            m_delayStartTime = ACE_OS::time();
        }

        // Update the remark to indicate we're in a delay
        std::ostringstream remark;
        remark << "In a " << m_refParameter.basic.delay << " second delay";
        m_remark = remark.str();

        m_eState = eDelaying;
        bExternalStateChanged = true;
    }
    else if ( m_bPendingPause )
    {
        m_remark = RemarkStepExecutionPaused;
        m_bPendingPause = false;
        m_eState = eLoadPaused;
        bExternalStateChanged = true;
    }
    else
    {
        ACE_Time_Value DefaultDelayTime( 0, 50000 ); // 50 millisecond
        ACE_OS::sleep( DefaultDelayTime );

        m_remark = RemarkExecutingStep;
        m_eState = eValidating;
        bExternalStateChanged = true;
    }

    return bExternalStateChanged;
}

bool ActiveStep::delaying()
{
    bool bExternalStateChanged = false;

    if ( 0 == m_delayStartTime )
    {
        // The state is restored from monitor mode, simply re-wait
        m_delayStartTime = ACE_OS::time();
    }
    else
    {
        // Check that the delay hasn't completed yet.
        time_t currentTime = ACE_OS::time();
        if ( m_delayStartTime + static_cast<time_t>(m_refParameter.basic.delay) > currentTime )
        {
            // Delay not complete. Update the amount of the delay completed and return.
            m_delayCompleted = ACE_OS::time() - m_delayStartTime;
        }
        else
        {
            m_delayCompleted = 0;

            m_eState = eValidating;
        }
    }

    if ( m_bPendingPause )
    {
        m_remark = RemarkStepExecutionPaused;
        m_bPendingPause = false;
        m_eState = eDelayPaused;
        bExternalStateChanged = true;
    }

    return bExternalStateChanged;
}

bool ActiveStep::validate()
{
    bool bExternalStateChanged = false;

    if ( !checkImpl() )
    {
        if ( m_bPendingPause )
        {
            m_remark = RemarkStepExecutionPaused;
            m_bPendingPause = false;
            m_eState = eValidatePaused;
            bExternalStateChanged = true;
        }
        else
        {
            static const ACE_Time_Value atmSleep( 0, 100000 ); // 100 ms
            ACE_OS::sleep( atmSleep );
        }
    }
    else
    {
        m_remark = RemarkExecutingStep;
        m_eState = eExecuting;
        bExternalStateChanged = true;
    }

    return bExternalStateChanged;
}

bool ActiveStep::executing()
{
    // Default failure in the case that exception occur
    bool bStepExecSucceed = false;

    try
    {
        bStepExecSucceed = executeImpl();
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "unknown", dumpContext().c_str() );
    }

    if( bStepExecSucceed )
    {
        m_eState = ( 0u < m_refParameter.basic.timeout ) ? eWaiting : eSucceed;
    }
    else
    {
        m_eState = eFailed;
    }

    return false;
}

bool ActiveStep::waiting()
{
    bool bExternalStateChanged = false;

    if ( 0 == m_tmStartTime )
    {
        refreshStartTime();
    }

    // Default failure in the case that exception occur
    int nReply = eWaitUndefined;

    try
    {
        nReply = waitImpl();
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "unknown", dumpContext().c_str() );
    }

    if ( eWaitSucceed == nReply )
    {
        m_eState = eSucceed;
    }
    else if ( eWaitFailed == nReply )
    {
        if ( !m_bCustomizedRemark )
        {
            remark( RemarkStepExecutionTimeout );
        }

        m_eState = eFailed;
    }

    // Check whether timeout reached, if timeout is max time value, wait forever
    if (( m_refParameter.basic.timeout != std::numeric_limits<time_t>::max() ) && ( ACE_OS::time() >= ( m_tmStartTime + m_refParameter.basic.timeout + NETWORK_DELAY )))
    {
        remark( RemarkStepExecutionTimeout );
        m_eState = eFailed;
    }
    else
    {
        static const ACE_Time_Value atmSleep( 0, m_lWaitingInterval ); // default 100 ms
        ACE_OS::sleep( atmSleep );
    }

    return bExternalStateChanged;
}

bool ActiveStep::paused()
{
    bool bExternalStateChanged = false;
    static const ACE_Time_Value IdleCpuTime( 0, 200000 ); // 200 millisecond

    if ( m_bPendingResume )
    {
        m_bPendingResume = false;
        m_bCustomizedRemark = false;

        m_tmStartTime = 0u;

        m_eState = eInitialState;
        // No external state changed since eInitialState is intermediate state
    }
    else
    {
        // Sleep only if no resume
        ACE_OS::sleep( IdleCpuTime );
    }

    return bExternalStateChanged;
}

bool ActiveStep::succeed()
{
    if ( !m_bCustomizedRemark )
    {
        m_remark = RemarkStepExecutionSucceed;
    }

    m_eState = eSucceedCompleted;

    // Always true since eFailed is intermediate state
    return true;
}

bool ActiveStep::failed()
{
    if ( !m_bCustomizedRemark )
    {
        m_remark = RemarkStepExecutionFailed;
    }

    m_eState = m_refParameter.basic.ignoreFailure ? eFailedCompleted : eFailedPaused;

    // Always true since eFailed is intermediate state
    return true;
}

void ActiveStep::initImpl()
{
    // Default do nothing
}

int ActiveStep::waitImpl()
{
    return eWaitSucceed;
}

bool ActiveStep::isFailed()
{
    bool bFailed = false;

    switch ( m_eState )
    {
    case eFailedPaused:
    case eFailedCompleted:
        bFailed = true;
        break;
    default:
        bFailed = false;
        break;
    }

    return bFailed;
}

bool ActiveStep::isPaused()
{
    if ( m_bPendingResume )
    {
        return false;
    }

    bool bPaused = false;

    switch ( m_eState )
    {
    case eLoadPaused:
    case eDelayPaused:
    case eValidatePaused:
    case eFailedPaused:
        bPaused = true;
        break;
    default:
        bPaused = false;
        break;
    }

    return bPaused;
}

std::string ActiveStep::getRemark()
{
    return m_remark;
}

int ActiveStep::getState()
{
    return toExternalState( m_eState );
}

void ActiveStep::stateUpdated( const int nNewState )
{
    m_eState = fromExternalState( nNewState );

    // Check whether step completed in Agent Monitor Mode
    switch ( m_eState )
    {
    case eFailedCompleted:
    case eSucceedCompleted:
    case eSkipped:
        m_bCompleted = true;
        break;
    default:
        break;
    }
}

std::string ActiveStep::getName()
{
    return m_refParameter.basic.name;
}

IActiveStepMessage* ActiveStep::getErrorEvent()
{
    return NULL;
}

bool ActiveStep::pause()
{
    if ( m_bPendingPause )
    {
        return true;
    }

    switch ( m_eState )
    {
    case eInitialState:
    case eDelaying:
    case eValidating:
        m_bPendingPause = true;
        break;
    default:
        break;
    }

    return m_bPendingPause;
}

bool ActiveStep::resume()
{
    if ( m_bPendingResume )
    {
        return true;
    }

    if ( m_bPendingPause )
    {
        m_bPendingPause = false;
        return true;
    }

    switch ( m_eState )
    {
    case eLoadPaused:
    case eValidatePaused:
    case eDelayPaused:
    case eFailedPaused:
        m_bPendingResume = true;
        break;
    default:
        break;
    }

    return m_bPendingResume;
}

bool ActiveStep::hasNextStep()
{
    return (!m_lstOutgoingStep.empty());
}

bool ActiveStep::checkImpl()
{
    return true;
}

bool ActiveStep::isIgnoreFailure()
{
    return m_refParameter.basic.ignoreFailure;
}

void ActiveStep::populateParameter( std::vector<std::string>& vecParams )
{
    m_refParameter.deserialize( vecParams );
}

bool ActiveStep::isParallel()
{
    return m_bParallel;
}

} // TA_Base_App
