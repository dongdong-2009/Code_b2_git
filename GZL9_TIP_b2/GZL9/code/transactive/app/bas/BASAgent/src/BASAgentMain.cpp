/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/BASAgentMain.cpp $
  * @author:	Raymond Pau
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2017/05/29 19:57:02 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *	Main program for BASAgent process
  *
  */

#include "app/bas/BASAgent/src/BASAgent.h"
#include "bus/scada/scada_agent/src/ScadaAgent.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/versioning/src/Version.h"

int main( int argc, char * argv[] )
{
	try
	{
        TA_Base_Core::Version::checkCommandLine(argc, argv);
		TA_IRS_App::BASAgent basAgent;
		basAgent.startBASAgent( argc, argv );
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
		
	return 1;
}
