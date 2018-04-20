/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/main.cpp $
 * @author:  Ripple
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4503)
#endif

#include "app/trains/train_agent/src/TrainAgent.h"

//#include "bus/managed_agent/src/SystemControlledManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"

#include <ace/OS.h>

int main( int argc, char* argv[] )
{
    FUNCTION_ENTRY( "TrainAgent" );

    try
    {
/*        TA_Base_Bus::SystemControlledManager systemControlledManager( argc, argv );
        TA_IRS_App::TrainAgent agent;
        systemControlledManager.initialize( agent );
        systemControlledManager.runTheAgent();
*/
		TA_IRS_App::TrainAgent* agent = new TA_IRS_App::TrainAgent(argc, argv);
		agent->run();
		delete agent;
    }
    catch ( const TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

        FUNCTION_EXIT;
        return 1;
    }
    catch ( const std::exception& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );

        FUNCTION_EXIT;
        return 1;
    }
    catch ( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "...", "" );

        FUNCTION_EXIT;
        return 1;
    }

    FUNCTION_EXIT;

    return 0;
}
