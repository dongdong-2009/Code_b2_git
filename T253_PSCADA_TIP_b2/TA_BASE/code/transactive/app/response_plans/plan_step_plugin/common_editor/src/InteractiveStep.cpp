//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/InteractiveStep.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "core/utilities/src/DebugUtil.h"
#include "InteractiveStep.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// InteractiveStep

InteractiveStep::InteractiveStep(const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
								 const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters) :
    ActivePlanStep( unId )
{
    FUNCTION_ENTRY( "InteractiveStep" );
    FUNCTION_EXIT;
}

InteractiveStep::~InteractiveStep()
{
    FUNCTION_ENTRY( "~InteractiveStep" );
    
    FUNCTION_EXIT;
}

void InteractiveStep::execute()
{
    FUNCTION_ENTRY( "execute" );

    try
    {
        launch();
    }
    catch (const std::exception& ex)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, typeid(ex).name(), ex.what() );
    }

    FUNCTION_EXIT;
}


void InteractiveStep::stop()
{
    FUNCTION_ENTRY( "waitUntilLaunched" );

 	close();

    FUNCTION_EXIT;
}
}
