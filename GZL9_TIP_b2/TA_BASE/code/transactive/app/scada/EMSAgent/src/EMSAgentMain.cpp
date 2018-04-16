/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/EMSAgent/src/EMSAgentMain.cpp $
  * @author:	Dhanshri
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	Main program for EMSAgent process
  *
  */

#include "app/scada/EMSAgent/src/EMSAgent.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/versioning/src/Version.h"

int main( int argc, char * argv[] )
{
	try
	{
        TA_Base_Core::Version::checkCommandLine(argc, argv);
		TA_IRS_App::EMSAgent EMSAgent( argc, argv );
		EMSAgent.startEMSAgent();
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

