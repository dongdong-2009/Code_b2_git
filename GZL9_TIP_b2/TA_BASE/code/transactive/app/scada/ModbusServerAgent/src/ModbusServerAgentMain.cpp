/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/ModbusServerAgent/src/ModbusServerAgentMain.cpp $
  * @author:	Darren Sampson
  * @version:	0.00
  *
  * Last modification:	$DateTime: 2016/03/31 13:24:51 $
  * Last modified by:	$Author: dhanshri $
  *
  * Description:
  *
  *	Main program for ModbusServerAgent process
  *
  */

#include "app/scada/ModbusServerAgent/src/ModbusServerAgent.h"

#include "core/exceptions/src/GenericAgentException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/versioning/src/Version.h"

using namespace TA_Base_Core;

int main( int argc, char * argv[] )
{

	try
	{
        TA_Base_Core::Version::checkCommandLine(argc, argv);
		TA_Base_App::ModbusServerAgent objModbusServerAgent( argc, argv );
		objModbusServerAgent.startModbusServerAgent();
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
