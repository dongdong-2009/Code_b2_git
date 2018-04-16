/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveParallelGateway.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2013/03/20 14:16:56 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include <limits>

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/common/src/StringConstants.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/plan_agent/src/ActiveParallelGateway.h"
 
namespace TA_Base_App
{

ActiveParallelGateway::ActiveParallelGateway( const BasicParameter& refDetail, IActivePlan& iPlan ) : 
ActiveStep( m_params, iPlan ),
m_bNextReturned( false ),
m_params( refDetail )
/*,
m_szJointTimes( 0 )*/
{
}

void ActiveParallelGateway::initImpl()
{
    // Ensure the last call would enable true next step(s) returned
    //++m_szJointTimes;
    m_params.basic.timeout = std::numeric_limits<time_t>::max();

    m_bParallel = true;
    m_lWaitingInterval = 500000;
}

bool ActiveParallelGateway::executeImpl()
{
    return true;
}

int ActiveParallelGateway::waitImpl()
{
    if ( m_bCompleted )
    {
        return eWaitSucceed;
    }

    size_t szCompletedCount = 0u;

    for (std::list<ActiveStep*>::iterator itLoop = m_lstIncomingStep.begin(); m_lstIncomingStep.end() != itLoop; ++itLoop)
    {
        if ( m_iPlan.stepCompleted( (*itLoop)->id() ))
        {
            ++szCompletedCount;
        }
    }

    int eWaitState = eAsyncWaitContinue;

    if ( szCompletedCount == m_lstIncomingStep.size() )
    {
        eWaitState = eWaitSucceed;
    }
    else if ( m_iPlan.allStepPaused( true ))
    {
        remark( RemarkSyncFailedByPausedSteps );
        eWaitState = eWaitFailed;
    }

    return eWaitState;
}

std::list<ActiveStep*> ActiveParallelGateway::getNextStep()
{
    // No lock needed since getNextStep is called by single thread only

    //if ( m_szJointTimes < m_lstIncomingStep.size() )
    if ( m_bNextReturned )
    {
        static std::list<ActiveStep*> EmptyList;
        return EmptyList;
    }

    m_bNextReturned = true;
    //m_szJointTimes = 0u;
    return m_lstOutgoingStep;
}

//bool ActiveParallelGateway::checkImpl()
//{
    //if ( m_bCompleted )
    //{
    //    return true;
    //}

    //bool bFullfill = true;

    //for ( std::list<ActiveStep*>::iterator itLoop = m_lstIncomingStep.begin(); m_lstIncomingStep.end() != itLoop; ++itLoop )
    //{
    //    if ( !m_iPlan.stepCompleted( (*itLoop)->id() ))
    //    {
    //        bFullfill = false;
    //        break;
    //    }
    //}

    //return bFullfill;
//}

} // TA_Base_App
