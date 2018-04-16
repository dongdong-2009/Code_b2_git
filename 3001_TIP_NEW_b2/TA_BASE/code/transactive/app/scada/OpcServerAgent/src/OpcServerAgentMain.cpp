/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcServerAgent/src/OpcServerAgentMain.cpp $
  * @author:	Dhanshri Mokashi
  * @version:	0.00
  *
  * Last modification:	$DateTime: 2016/06/22 16:15:14 $
  * Last modified by:	$Author: Dhanshri Mokashi $
  *
  * Description:
  *
  *	Main program for OpcServerAgent process
  *
  */

#include "OpcServerAgent.h"

#include "core/exceptions/src/GenericAgentException.h"
#include "core/utilities/src/DebugUtil.h"

int main( int argc, char * argv[] )
{
	try
	{
		TA_Base_App::OpcServerAgent objOpcServerAgent( argc, argv );
		objOpcServerAgent.startOpcServerAgent();
	}
    catch( const TA_Base_Core::GenericAgentException& gae )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "GenericAgentException", gae.what());
    }
	catch( const TA_Base_Core::TransactiveException & te)
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
	}
	catch( std::exception & e )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
	}
	catch( ... )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception" );
	}
	return 0;
}
