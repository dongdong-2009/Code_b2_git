/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/FLZ1_TIP/FZL1/transactive/app/scada/PEnergyAgent/src/PEnergyAgentMain.cpp $
  * @author:	Liu Yong
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	Main program for PEnergyAgent process
  *
  */

#include "app/scada/PEnergyAgent/src/PEnergyAgent.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/GenericAgentException.h"

int main( int argc, char * argv[] )
{
	try
	{
		TA_Base_App::PEnergyAgent PEnergyAgent( argc, argv );
		PEnergyAgent.startPEnergyAgent();
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
