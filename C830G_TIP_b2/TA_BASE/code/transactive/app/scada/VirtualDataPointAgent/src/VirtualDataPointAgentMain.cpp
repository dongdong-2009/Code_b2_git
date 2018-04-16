/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/4669_T01271350/TA_BASE/transactive/app/scada/VirtualDataPointAgent/src/VirtualDataPointAgentMain.cpp $
  * @author:	HoaVu
  * @version:	0.00
  *
  * Last modification:	$DateTime: 2008/11/28 16:26:01 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	Main program for VirtualDataPointAgent process
  *
  */

#include "app/scada/VirtualDataPointAgent/src/VirtualDataPointAgent.h"

#include "core/exceptions/src/GenericAgentException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericAgentException;

int main( int argc, char * argv[] )
{
	try
	{
		VirtualDataPointAgent virtualDataPointAgent( argc, argv );
        virtualDataPointAgent.startVirtualDataPointAgent();
	}
    catch( const GenericAgentException& gae )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "GenericAgentException", gae.what());
    }
	catch( std::exception & e )
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, e.what() );
	}
	catch( ... )
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Caught unknown exception" );
	}

	return 1;
}
