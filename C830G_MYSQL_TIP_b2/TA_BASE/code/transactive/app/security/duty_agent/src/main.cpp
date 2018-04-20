/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/security/duty_agent/src/main.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2011/06/06 11:32:52 $
  *  Last modified by:  $Author: shurui.liu $
  *
  *  <description>
  */
#include "DutyAgent.h"
#include "DutyAgentUser.h"

#include "ace/OS.h"

int main( int argc, char* argv[] )
{
	int returnVal = 0;

	ACE_Object_Manager* aceObjectManager = NULL;
	TA_Base_App::DutyAgentUser* daUser = NULL;

	// All the various known (and the unknown) exception types
	// are caught here. This allows us to log the fact that the
	// app has not handled the exception properly rather than
	// just dissapearing. These try/catch pairs are placed around
	// the other thread entry point: DutyAgentImpl.
	try
	{

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin new ACE_Object_Manager");
		aceObjectManager = new ACE_Object_Manager;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end new ACE_Object_Manager");
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin init ACE_Object_Manager");
		aceObjectManager->init();
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end init ACE_Object_Manager");
		daUser = new TA_Base_App::DutyAgentUser( argc, argv );

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Duty Agent starting..." );
		daUser->run();
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

	LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Duty Agent shutting down..." );
	
	delete daUser;

	if( aceObjectManager != NULL )
	{
		aceObjectManager->fini();
	}
	delete aceObjectManager;

	LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Duty Agent done." );

	return( returnVal );
}
