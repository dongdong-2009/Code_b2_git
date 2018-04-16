/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/security/rights_agent/src/main.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  <description>
  */
#include "RightsAgent.h"

#include "ace/OS.h"

using TA_Base_App::RightsAgentUser;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericAgentException;

int main( int argc, char* argv[] )
{
	int returnVal = 0;

    try
    {
        RightsAgentUser ra(argc,argv);

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Rights Agent starting..." );
        ra.run();
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

	LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Rights Agent shutting down..." );
    
    return( returnVal );
}
