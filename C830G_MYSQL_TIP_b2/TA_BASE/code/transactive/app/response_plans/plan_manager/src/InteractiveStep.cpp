//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/InteractiveStep.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2008/11/28 16:26:01 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "core/uuid/src/TAuuid.h"
#include "InteractiveStep.h"
#include "InteractiveStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// InteractiveStep

InteractiveStep::InteractiveStep(const TA_Base_Bus::IPlanManagerCorbaDef::CurrentPlanStepDetail &stepDetail) :
    m_stepDetail(stepDetail),
    m_launchEvent(FALSE, FALSE),
    m_launchError("")
{
    FUNCTION_ENTRY( "InteractiveStep" );
    FUNCTION_EXIT;
}


InteractiveStep::~InteractiveStep()
{
    FUNCTION_ENTRY( "~InteractiveStep" );

    m_launchEvent.Unlock();

    FUNCTION_EXIT;
}


TA_Base_Bus::IPlanManagerCorbaDef::CurrentPlanStepDetail InteractiveStep::getStepDetail() const
{
    FUNCTION_ENTRY( "getStepDetail" );
    FUNCTION_EXIT;
    return m_stepDetail;
}


void InteractiveStep::launchAndSignal()
{
    FUNCTION_ENTRY( "launchAndSignal" );

    try
    {
        launch();

        m_launchError = "";
    }
    catch (const InteractiveStepException& ex)
    {
        // This exception will not traverse thread boundaries, so trap the error
        // and return via waitUntilLaunched().
        m_launchError = ex.what();
    }

    m_launchEvent.SetEvent();

    FUNCTION_EXIT;
}


std::string InteractiveStep::waitUntilLaunched()
{
    FUNCTION_ENTRY( "waitUntilLaunched" );

    CSingleLock launchEventLock(&m_launchEvent);
    launchEventLock.Lock();

    FUNCTION_EXIT;
    return m_launchError;
}


