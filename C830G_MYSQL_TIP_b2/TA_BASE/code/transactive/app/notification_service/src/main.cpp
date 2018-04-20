
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/notification_service/src/main.cpp $
 * @author:  <your name>
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * Main function for the NotificationServiceAgent
 */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <iostream>
#include "app/notification_service/src/NotificationAgent.h"

#include "ace/OS.h"

int main( int argc, char* argv[] )
{
	try
	{
		TA_Base_App::NotificationAgent* notifyAgent
			= new TA_Base_App::NotificationAgent( argc, argv );
		notifyAgent->startService();
		notifyAgent->deactivateAndDeleteServant();
		return 0;
	}
	catch( ... )
	{
		std::cout << "main(): Caught unknown exception" << std::endl;
	}

	return 1;
}
