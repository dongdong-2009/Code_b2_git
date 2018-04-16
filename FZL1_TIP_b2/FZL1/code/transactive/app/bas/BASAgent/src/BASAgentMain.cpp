/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/FZL1_TIP/FZL1/transactive/app/bas/BASAgent/src/BASAgentMain.cpp $
  * @author:	Raymond Pau
  * @version:	$Revision: #3 $
  *
  * Last modification:	$DateTime: 2017/05/29 19:08:02 $
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

int main( int argc, char * argv[] )
{
	try
	{
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
