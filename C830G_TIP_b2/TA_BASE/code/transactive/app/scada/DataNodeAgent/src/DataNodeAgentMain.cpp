/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/4669_T01271350/TA_BASE/transactive/app/scada/DataNodeAgent/src/DataNodeAgentMain.cpp $
  * @author:	Darren Sampson
  * @version:	0.00
  *
  * Last modification:	$DateTime: 2008/11/28 16:26:01 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	Main program for DataNodeAgent process
  *
  */

#include "app/scada/DataNodeAgent/src/DataNodeAgent.h"

#include "core/exceptions/src/GenericAgentException.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

int main( int argc, char * argv[] )
{

	TA_Base_App::DataNodeAgent dataNodeAgent( argc, argv );
	dataNodeAgent.startDataNodeAgent();
		
	return 0;
}
