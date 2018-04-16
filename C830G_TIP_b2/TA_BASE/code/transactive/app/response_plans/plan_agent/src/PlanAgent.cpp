/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/26 18:32:31 $
  * Last modified by:  $Author: builder $
  *
  */
#include "PlanAgent.h"

int main( int argc, char* argv[] )
{
	int returnVal = 0;

	ACE_Object_Manager* aceObjectManager = NULL;
	TA_Base_App::PlanAgentUser* pa = NULL;

	// All the various known (and the unknown) exception types
	// are caught here. This allows us to log the fact that the
	// app has not handled the exception properly rather than
	// just dissapearing. These try/catch pairs are placed around
	// the other two thread entry points: PlanExecutionEngine and PlanAgentCorbaDefImpl.
	try
	{
		aceObjectManager = new ACE_Object_Manager;
		aceObjectManager->init();

		pa = TA_Base_App::PlanAgentUser::getInstance();
		pa->initAgentUser(argc, argv);

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Plan Agent starting..." );
		pa->run();
	}
	catch( const std::exception& e )
	{
		LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "Unhandled." );
		returnVal = 128;
	}
	catch( const CORBA::Exception& e )
	{
		LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled." );
		returnVal = 128;
	}
    catch( ... )
	{
		LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled." );
		returnVal = 128;
	}

	LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Shutting down..." );
	
	delete pa;
	if( aceObjectManager != NULL )
	{
		aceObjectManager->fini();
	}
	delete aceObjectManager;

	LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Plan Agent done." );

	return( returnVal );
}

